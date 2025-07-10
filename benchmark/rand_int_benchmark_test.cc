#include <gtest/gtest.h>

#include <vector>

#include "rand_int_benchmark.h"

TEST(CreateInts, Empty) {
  ASSERT_EQ(std::vector<uint32_t>(), create_pseudo_random_ints(0));
}

TEST(CreateInts, Small) {
  std::vector<uint32_t> random_ints = create_pseudo_random_ints(3);
  ASSERT_EQ(random_ints[0], 282475248);
}

TEST(RandIntBenchmark, Empty) {
  ASSERT_TRUE(rand_int_benchmark(0));
}

TEST(RandIntBenchmark, Small) {
  ASSERT_TRUE(rand_int_benchmark(3));
}
