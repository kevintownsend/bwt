#include "benchmark/rand_int_benchmark.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <vector>

TEST(CreateInts, Empty) {
  ASSERT_EQ(std::vector<uint32_t>(), create_pseudo_random_ints(0));
}

TEST(CreateInts, DeterministicForSeed) {
  const std::vector<uint32_t> a = create_pseudo_random_ints(16, 123);
  const std::vector<uint32_t> b = create_pseudo_random_ints(16, 123);
  const std::vector<uint32_t> c = create_pseudo_random_ints(16, 456);
  ASSERT_EQ(a.size(), 16u);
  ASSERT_EQ(a, b);
  ASSERT_NE(a, c);
}

TEST(RandIntBenchmark, EmptyAndVariedSizes) {
  EXPECT_TRUE(rand_int_benchmark(0));
  EXPECT_TRUE(rand_int_benchmark(3));
  EXPECT_TRUE(rand_int_benchmark(1000));
}

TEST(BenchmarkWorkloads, VerifiesRoundTripsAndRunClustering) {
  const BenchmarkResult text_res =
      run_char_benchmark("text", create_repetitive_text(4096));
  EXPECT_TRUE(text_res.verified);
  EXPECT_EQ(text_res.elements, 4096u);
  EXPECT_EQ(text_res.bytes, 4096u);
  EXPECT_GE(text_res.encode_ms, 0.0);
  EXPECT_GE(text_res.decode_ms, 0.0);
  EXPECT_GT(text_res.avg_run_after, text_res.avg_run_before);

  const BenchmarkResult dna_res =
      run_char_benchmark("dna", create_repetitive_dna(4096));
  EXPECT_TRUE(dna_res.verified);
  EXPECT_GT(dna_res.avg_run_after, dna_res.avg_run_before);

  const BenchmarkResult bin_res =
      run_uint8_benchmark("low_bin", create_low_entropy_binary(4096));
  EXPECT_TRUE(bin_res.verified);

  const BenchmarkResult u32_res =
      run_uint32_benchmark("rand_u32", create_pseudo_random_ints(1024));
  EXPECT_TRUE(u32_res.verified);
  EXPECT_EQ(u32_res.bytes, 1024u * sizeof(uint32_t));
}
