package(default_visibility = ["//visibility:public"])

cc_library(
    name = "http",
    srcs = glob([
        "http/*.cpp",
    ]),
    hdrs = glob([
        "http/*.h",
    ]),
    includes = ["."],
    copts = ["-std=c++1z"],
    deps = [
        "@third_party-event//:event",
        "@third_party-glog//:glog",
    ]
)

cc_binary(
    name = "server",
    srcs = ["main.cpp"],
    linkopts = ["-lpthread"],
    copts = ["-std=c++1z"],
    deps = [
        ":http",
        "@third_party-glog//:glog",
        "@third_party-gflags//:gflags",
    ],
)
