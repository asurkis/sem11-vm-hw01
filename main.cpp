// Source article:
// http://embedded.cs.uni-saarland.de/literature/AndreasAbelMastersThesis.pdf

#include <algorithm>
#include <chrono>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <random>
#include <ratio>

int buffer[16 * 1024 * 1024];

int do_random_access(int *ptr, size_t len) {
  int pos = 0;
  for (int i = 0; i < 8 * 1024 * 1024; ++i) {
    int dx = ptr[pos];
    pos = (pos + dx + 1) % len;
  }
  return pos;
}

void measure_size(size_t kb) {
  size_t len = 1024 * kb / sizeof(int);
  for (size_t i = 0; i < len; ++i) {
    buffer[i] = i;
  }
  std::minstd_rand rng;
  std::shuffle(buffer, buffer + len, rng);
  int *ptr = buffer;

  // Preload cache
  int garbage = do_random_access(ptr, len);
  auto t1 = std::chrono::high_resolution_clock::now();
  garbage += do_random_access(ptr, len);
  auto t2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> duration = t2 - t1;

  std::cout << "Size: " << std::setw(6) << kb
            << " KB; Access time: " << duration.count() << " ms; Garbage: " << garbage << "\n";
}

void do_steps(int *ptr, size_t step, size_t len) {
  for (uint64_t iter = 0; iter < 1024; ++iter) {
    for (size_t j = 0; j < step; ++j) {
      for (size_t i = j; i < len; i += step) {
        ptr[i]++;
      }
    }
  }
}

void measure_assoc(size_t step) {
  size_t len = 32 * 1024;
  memset(buffer, 0, sizeof(buffer[0]) * len);
  int *ptr = buffer;

  // Preload cache
  do_steps(ptr, step, len);
  auto t1 = std::chrono::high_resolution_clock::now();
  do_steps(ptr, step, len);
  auto t2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> duration = t2 - t1;

  std::cout << "Step: " << std::setw(6) << step
            << "; Access time: " << duration.count() << " ms\n";
}

void do_ws(int *ptr, size_t ws, size_t assoc) {
  for (size_t iter = 0; iter < 1024; ++iter) {
    for (size_t i = 0; i < assoc; ++i) {
      ptr[ws * i]++;
    }
  }
}

void measure_3() {
  double prev_ms = 0.0;
  size_t assoc_old = 0;
  bool is_first = true;
  int *ptr = buffer;
  for (size_t ws = 1024; ws > 0; ws /= 2) {
    for (size_t assoc = 1; assoc * ws <= sizeof(buffer) / sizeof(buffer[0]);
         ++assoc) {
      size_t len = ws * assoc;

      do_ws(ptr, ws, assoc);
      auto t1 = std::chrono::high_resolution_clock::now();
      do_ws(ptr, ws, assoc);
      auto t2 = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double, std::milli> duration = t2 - t1;

      double ms = duration.count() / assoc;

      if (is_first) {
        prev_ms = ms;
        is_first = false;
        continue;
      }

      double eps = ms / prev_ms - 1.0;
      prev_ms = ms;
      if (std::abs(eps) >= 0.25) {
        if (assoc - 1 == 2 * assoc_old) {
          std::cout << "ws: " << ws << "; assoc: " << assoc
                    << "; duration: " << duration.count() << " ms\n";
        } else {
          assoc_old = assoc - 1;
          break;
        }
      }
    }
  }
}

int main() {
  for (size_t kb = 1; kb <= 4096; kb *= 2) {
    measure_size(kb);
  }
  // measure_size(sizeof(buffer) / 1024);
  for (size_t step = 1; step <= 4096; step *= 2) {
    measure_assoc(step);
  }
  // measure_3();
  return 0;
}
