load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")

cc_library(
    name = "bwt",
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
