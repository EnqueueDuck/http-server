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
        ":thread",
        "@third_party-glog//:glog",
    ]
)

cc_library(
    name = "thread",
    srcs = glob([
        "thread/*.cpp",
    ]),
    hdrs = glob([
        "thread/*.h",
    ]),
    includes = ["."],
    deps = [
        "@third_party-glog//:glog",
    ]
)

cc_binary(
    name = "server",
    srcs = ["main.cpp"],
    linkopts = ["-lpthread"],
    deps = [
        ":http",
        "@third_party-glog//:glog",
        "@third_party-gflags//:gflags",
    ],
)
