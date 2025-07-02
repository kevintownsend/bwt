# TODO: Add rand_int_benchmark
cc_library(
  name = "rand_int_benchmark",
  srcs = ["rand_int_benchmark.cc"],
  hdrs = ["rand_int_benchmark.h"],
)

# rand_int_benchmark_test
cc_test(
  name = "rand_int_benchmark_test",
  srcs = ["rand_int_benchmark_test.cc"],
  deps = [
    ":rand_int_benchmark",
    "@googletest//:gtest",
    "@googletest//:gtest_main",
  ]
)

# rand_int_benchmark_bin
cc_binary(
  name = "rand_int_benchmark_bin",
  srcs = ["rand_int_benchmark_bin.cc"],
  deps = [":rand_int_benchmark"],
)

