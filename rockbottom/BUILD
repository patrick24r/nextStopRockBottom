load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility=["//visibility:public"])

cc_library(
    name="printer",
    hdrs=["printer.h"],
    srcs=["printer.cc"],
    deps=[":fetcher",
          ":rgbmatrix",
          ],
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
               "rpi-rgb/lib/**/*.h",
               ]),
    hdrs=glob(["rpi-rgb/include/**/*.h",
               "rpi-rgb/lib/**/*.h"]),
    includes=["rpi-rgb/include",
              "rpi-rgb/lib",
              "rpi-rgb/lib/rpi",
              "rpi-rgb/lib/rp1/rp1_pio_vendor/piolib/include",
              "rpi-rgb/lib/rp1/rp1_pio_vendor/include",],
    visibility=["//visibility:private"],
    linkopts=["-lpthread", "-lrt"],
    copts=["-O3", "-Wall"],
)
