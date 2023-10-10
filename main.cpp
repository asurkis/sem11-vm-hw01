// Source article:
// http://embedded.cs.uni-saarland.de/literature/AndreasAbelMastersThesis.pdf

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <ratio>
#include <vector>

void measure(size_t sz) {
  std::vector<unsigned char> vec(1024 * sz, '\0');
  size_t len = vec.size();
  volatile unsigned char *ptr = &vec[0];

  std::default_random_engine rng;
  std::uniform_int_distribution<size_t> idx_distrib(0, len - 1);

  auto t1 = std::chrono::high_resolution_clock::now();
  for (size_t i = 0; i < 1024 * 1024; ++i) {
    size_t pos = idx_distrib(rng);
    ptr[pos]++;
  }
  auto t2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> duration = t2 - t1;

  std::cout << "Size " << std::setw(6) << sz
            << " KB; Access time: " << std::setw(6) << duration.count()
            << " ms\n";
}

int main() {
  for (size_t sz = 1; sz <= 1024 * 1024; sz *= 2) {
    measure(sz);
  }
  return 0;
}
