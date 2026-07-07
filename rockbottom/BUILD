load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility=["//visibility:public"])

cc_library(
    name="printer",
    hdrs=["printer.h"],
    srcs=["printer.cc"],
    deps=[":fetcher",
          ":rgbmatrix",
          "@bazel_tools//tools/cpp/runfiles",
          ],
    data=[":fonts"],
    visibility=["//:__pkg__"],
)

cc_library(
    name="fetcher",
    hdrs=["fetcher.h"],
    srcs=["fetcher.cc"],
    deps=[
        "@nlohmann_json//:json",
        "@curl//:curl",
    ],
    copts=["-std=c++20"],
)

cc_library(
    name="rgbmatrix",
    srcs=glob(["rpi-rgb/lib/**/*.cc",
               "rpi-rgb/lib/**/*.c",
               ]),
    hdrs=glob(["rpi-rgb/include/**/*.h",
               "rpi-rgb/lib/**/*.h",
               "rpi-rgb/lib/rp1/**/*.c",
               ]),
    includes=["rpi-rgb/include",
              "rpi-rgb/lib",
              "rpi-rgb/lib/rp1",
              "rpi-rgb/lib/rp1/rp1_pio_vendor/piolib/include",
              "rpi-rgb/lib/rp1/rp1_pio_vendor/include",],
    visibility=["//visibility:private"],
    linkopts=["-lpthread", "-lrt"],
    copts=["-O3", "-Wall"],
)

filegroup(
    name="fonts",
    srcs=glob(["rpi-rgb/fonts/*.bdf"]),
)
