#ifndef BWT_BENCHMARK_RAND_INT_BENCHMARK_H_
#define BWT_BENCHMARK_RAND_INT_BENCHMARK_H_

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

struct BenchmarkResult {
  std::string name;
  std::size_t elements = 0;
  std::size_t bytes = 0;
  double encode_ms = 0.0;
  double decode_ms = 0.0;
  double encode_mb_per_s = 0.0;
  double decode_mb_per_s = 0.0;
  double avg_run_before = 0.0;
  double avg_run_after = 0.0;
  bool verified = false;
};

template <typename T>
double average_run_length(const std::vector<T>& data) {
  if (data.empty()) {
    return 0.0;
  }
  std::size_t runs = 1;
  for (std::size_t i = 1; i < data.size(); ++i) {
    if (!(data[i] == data[i - 1])) {
      ++runs;
    }
  }
  return static_cast<double>(data.size()) / static_cast<double>(runs);
}

std::vector<uint32_t> create_pseudo_random_ints(std::size_t n,
                                                uint32_t seed = 42);
std::vector<char> create_repetitive_dna(std::size_t n, uint32_t seed = 42);
std::vector<char> create_repetitive_text(std::size_t n);
std::vector<uint8_t> create_low_entropy_binary(std::size_t n,
                                               uint32_t seed = 42);

BenchmarkResult run_char_benchmark(const std::string& name,
                                   const std::vector<char>& data);
BenchmarkResult run_uint8_benchmark(const std::string& name,
                                    const std::vector<uint8_t>& data);
BenchmarkResult run_uint32_benchmark(const std::string& name,
                                     const std::vector<uint32_t>& data);

bool rand_int_benchmark(std::size_t n);

#endif  // BWT_BENCHMARK_RAND_INT_BENCHMARK_H_
