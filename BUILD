cc_library(
  name = "bwt",
  srcs = ["bwt.cc"],
  hdrs = ["bwt.hpp"],
  visibility = ["//visibility:public"],
)

cc_test(
  name = "bwt_test",
  srcs = ["bwt_test.cc"],
  deps = [
    ":bwt",
    "@googletest//:gtest",
    "@googletest//:gtest_main",
  ],
)
