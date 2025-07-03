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

//TODO(kevintownsend): This test fails with segfault.
// TEST(RandIntBenchmark, Empty) {
//  rand_int_benchmark(0);
//}
