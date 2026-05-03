load("@rules_cc//cc:cc_library.bzl", "cc_library")

cc_library(
  name = "printer",
  hdrs = ["printer.h"],
  srcs = ["printer.cc"],
  visibility = ["//:__pkg__"],
)

cc_library(
  name = "fetcher",
  hdrs = ["fetcher.h"],
  srcs = ["fetcher.cc"],
  deps = [
    "@nlohmann_json//:json",
    "@curl//:curl",
  ],
  copts = ["-std=c++20"],
  visibility = ["//:__pkg__"],
)