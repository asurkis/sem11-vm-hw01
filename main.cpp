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
  for (size_t i = 0; i < 1024 * 1024; ++i) {
    size_t pos = idx_distrib(rng);
    ptr[pos]++;
  }
}

void measure(size_t kb) {
  std::vector<int> vec(1024 * kb / sizeof(int), '\0');
  size_t len = vec.size();
  volatile int *ptr = &vec[0];

  std::minstd_rand rng;
  std::uniform_int_distribution<size_t> idx_distrib(0, len - 1);

  // Preload cache
  do_random_access(ptr, len);
  auto t1 = std::chrono::high_resolution_clock::now();
  do_random_access(ptr, len);
  auto t2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> duration = t2 - t1;

  std::cout << "Size: " << std::setw(6) << kb
            << " KB; Access time: " << duration.count() << " ms\n";
}

int main() {
  size_t step = 4;
  for (size_t kb = 1; kb <= 1024; kb *= step) {
    for (size_t i = 1; i < step; ++i) {
      measure(i * kb);
    }
  }
  return 0;
}
