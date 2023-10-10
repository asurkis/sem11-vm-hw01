// Source article:
// http://embedded.cs.uni-saarland.de/literature/AndreasAbelMastersThesis.pdf

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <ratio>
#include <vector>

void do_random_access(volatile int *ptr, size_t len) {
  std::minstd_rand rng;
  std::uniform_int_distribution<size_t> idx_distrib(0, len - 1);
  for (uint64_t i = 0; i < 1024 * 1024; ++i) {
    size_t pos = idx_distrib(rng);
    ptr[pos]++;
  }
}

void measure_size(size_t kb) {
  std::vector<int> vec(1024 * kb / sizeof(int), 0);
  size_t len = vec.size();
  volatile int *ptr = vec.data();

  // Preload cache
  do_random_access(ptr, len);
  auto t1 = std::chrono::high_resolution_clock::now();
  do_random_access(ptr, len);
  auto t2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> duration = t2 - t1;

  std::cout << "Size: " << std::setw(6) << kb
            << " KB; Access time: " << duration.count() << " ms\n";
}

void do_steps(volatile int *ptr, size_t step, size_t len) {
  for (uint64_t iter = 0; iter < 1024; ++iter) {
    for (size_t j = 0; j < step; ++j) {
      for (size_t i = j; i < len; i += step) {
        ptr[i]++;
      }
    }
  }
}

void measure_assoc(size_t step) {
  std::vector<int> vec(32 * 1024, 0);
  size_t len = vec.size();
  volatile int *ptr = vec.data();

  // Preload cache
  do_steps(ptr, step, len);
  auto t1 = std::chrono::high_resolution_clock::now();
  do_steps(ptr, step, len);
  auto t2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> duration = t2 - t1;

  std::cout << "Step: " << std::setw(6) << step
            << "; Access time: " << duration.count() << " ms\n";
}

int main() {
  for (size_t step = 1; step <= 1024; step *= 2) {
    measure_assoc(step);
  }
  return 0;
}
