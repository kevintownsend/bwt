cc_library(
  name = "rand_int_benchmark",
  srcs = ["rand_int_benchmark.cc"],
  hdrs = ["rand_int_benchmark.h"],
  deps = ["//:bwt"],
)

cc_test(
  name = "rand_int_benchmark_test",
  srcs = ["rand_int_benchmark_test.cc"],
  deps = [
    ":rand_int_benchmark",
    "@googletest//:gtest",
    "@googletest//:gtest_main",
  ]
)

cc_binary(
  name = "rand_int_benchmark_bin",
  srcs = ["rand_int_benchmark_bin.cc"],
  deps = [":rand_int_benchmark"],
)

