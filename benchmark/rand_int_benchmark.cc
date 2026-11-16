#include "benchmark/rand_int_benchmark.h"

#include <chrono>
#include <cstdint>
#include <limits>
#include <optional>
#include <random>
#include <string>
#include <string_view>
#include <vector>

#include "bwt.hpp"

namespace {

template <typename T>
BenchmarkResult run_benchmark_impl(const std::string &name,
                                   const std::vector<T> &data) {
  BenchmarkResult result;
  result.name = name;
  result.elements = data.size();
  result.bytes = data.size() * sizeof(T);
  result.avg_run_before = average_run_length(data);

  const auto t0 = std::chrono::steady_clock::now();
  const bwt::Encoded<T> encoded = bwt::encode(data);
  const auto t1 = std::chrono::steady_clock::now();
  const std::optional<std::vector<T>> decoded = bwt::decode(encoded);
  const auto t2 = std::chrono::steady_clock::now();

  result.encode_ms =
      std::chrono::duration<double, std::milli>(t1 - t0).count();
  result.decode_ms =
      std::chrono::duration<double, std::milli>(t2 - t1).count();
  result.encode_mb_per_s = (result.encode_ms > 0.0)
                               ? (static_cast<double>(result.bytes) /
                                  (result.encode_ms * 1e3))
                               : 0.0;
  result.decode_mb_per_s = (result.decode_ms > 0.0)
                               ? (static_cast<double>(result.bytes) /
                                  (result.decode_ms * 1e3))
                               : 0.0;
  result.avg_run_after = average_run_length(encoded.data);
  result.verified = decoded.has_value() && (*decoded == data);
  return result;
}

} // namespace

std::vector<uint32_t> create_pseudo_random_ints(std::size_t n, uint32_t seed) {
  std::vector<uint32_t> values(n);
  std::mt19937 generator(seed);
  std::uniform_int_distribution<uint32_t> distribution(
      0, std::numeric_limits<uint32_t>::max());
  for (std::size_t i = 0; i < n; ++i) {
    values[i] = distribution(generator);
  }
  return values;
}

std::vector<char> create_repetitive_dna(std::size_t n, uint32_t seed) {
  static constexpr std::string_view kMotifs[] = {
      "GATTACA", "ATATCGAT", "CGCGTACG", "TTGACAA",
  };
  static constexpr char kBases[] = "ACGT";
  std::mt19937 rng(seed);
  std::vector<char> dna;
  dna.reserve(n);
  while (dna.size() < n) {
    if ((rng() % 4) != 0) {
      const std::string_view motif = kMotifs[rng() % 4];
      const std::size_t take = std::min(motif.size(), n - dna.size());
      dna.insert(dna.end(), motif.begin(), motif.begin() + take);
    } else {
      dna.push_back(kBases[rng() % 4]);
    }
  }
  return dna;
}

std::vector<char> create_repetitive_text(std::size_t n) {
  static constexpr std::string_view kCorpus =
      "The Burrows-Wheeler transform rearranges a character sequence into runs "
      "of similar symbols by sorting all cyclic contexts lexicographically. ";
  std::vector<char> text;
  text.reserve(n);
  std::size_t block = 0;
  while (text.size() < n) {
    const std::size_t copy_len = std::min(kCorpus.size(), n - text.size());
    text.insert(text.end(), kCorpus.begin(), kCorpus.begin() + copy_len);
    if (text.size() < n) {
      const std::string tag = "[" + std::to_string(block++ % 16) + "] ";
      const std::size_t tag_len = std::min(tag.size(), n - text.size());
      text.insert(text.end(), tag.begin(), tag.begin() + tag_len);
    }
  }
  return text;
}

std::vector<uint8_t> create_low_entropy_binary(std::size_t n, uint32_t seed) {
  std::mt19937 rng(seed);
  std::vector<uint8_t> data(n);
  for (std::size_t i = 0; i < n; ++i) {
    const uint32_t r = rng() % 100;
    if (r < 85) {
      data[i] = static_cast<uint8_t>(i % 4 == 0 ? 0x01 : 0x00);
    } else if (r < 95) {
      data[i] = 0x7F;
    } else {
      data[i] = static_cast<uint8_t>(0x80 + (rng() & 0x7Fu));
    }
  }
  return data;
}

BenchmarkResult run_char_benchmark(const std::string &name,
                                   const std::vector<char> &data) {
  return run_benchmark_impl(name, data);
}

BenchmarkResult run_uint8_benchmark(const std::string &name,
                                    const std::vector<uint8_t> &data) {
  return run_benchmark_impl(name, data);
}

BenchmarkResult run_uint32_benchmark(const std::string &name,
                                     const std::vector<uint32_t> &data) {
  return run_benchmark_impl(name, data);
}

bool rand_int_benchmark(std::size_t n) {
  return run_uint32_benchmark("rand_u32", create_pseudo_random_ints(n)).verified;
}
