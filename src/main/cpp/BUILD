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
    name = "main",
    srcs = ["main.cpp"],
    deps = [
        ":http",
        "@third_party-glog//:glog",
    ],
)

cc_binary(
    name = "test_thread",
    srcs = ["tests/test_thread.cpp"],
    linkopts = ["-lpthread"],
    deps = [
        ":thread",
        "@third_party-glog//:glog",
    ],
)

cc_binary(
    name = "test_server",
    srcs = ["tests/test_server.cpp"],
    linkopts = ["-lpthread"],
    deps = [
        ":http",
        "@third_party-glog//:glog",
    ],
)
