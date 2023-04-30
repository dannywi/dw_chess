test_files = glob(["test*.cpp"])

[
    cc_test(
        name = "test_runner_" + test_file,
        srcs = [test_file],
        deps = [
            "//src/framework",
            "@googletest//:gtest_main",
        ],
    )
    for test_file in test_files
]
