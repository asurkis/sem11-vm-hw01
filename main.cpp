// Source article:
// http://embedded.cs.uni-saarland.de/literature/AndreasAbelMastersThesis.pdf

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <ratio>
#include <vector>

constexpr size_t MIN_LINE_SIZE = 16;

void measure(size_t sz) {
  std::vector<unsigned char> vec(1024 * sz, '\0');
  size_t len = vec.size();

  std::vector<size_t> idx;
  std::default_random_engine rng;
  std::uniform_int_distribution<size_t> dist(0, MIN_LINE_SIZE - 1);
  for (size_t i = 0; i < len / MIN_LINE_SIZE; ++i) {
    idx.push_back(MIN_LINE_SIZE * i + dist(rng));
  }
  // auto mid_iter = idx.begin() + sz * 1024 / 2 / MIN_LINE_SIZE;
  // std::shuffle(idx.begin(), mid_iter, rng);
  // std::shuffle(mid_iter, idx.end(), rng);
  // for (size_t i = 1; i < idx.size() / 2; i += 2) {
  //   std::swap(idx[i], idx[i + idx.size() / 2]);
  // }
  std::shuffle(idx.begin(), idx.end(), rng);

  volatile unsigned char *ptr = &vec[0];

  auto t1 = std::chrono::high_resolution_clock::now();
  for (int benchmark_iter = 0; benchmark_iter < 1024; ++benchmark_iter) {
    for (size_t i : idx) {
      ptr[i]++;
    }
  }
  auto t2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<int64_t, std::nano> duration = t2 - t1;

  std::cout << "Size " << std::setw(6) << sz
            << " KB; Average access time: " << std::setw(6)
            << duration.count() / len << " nanos\n";
}

int main() {
  for (size_t sz = 1; sz < 4096; sz *= 2) {
    measure(sz);
  }
  return 0;
}
