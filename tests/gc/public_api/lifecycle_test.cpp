#include <utils/config.hpp>

#include <gtest/gtest.h>

#include <cstdio>
#include <cstdlib>


using namespace hw::stella;

TEST(GcLifecycleTest, RejectsNullConfig) {
    EXPECT_EXIT(
        {
            stella_gc_init(nullptr);
        },
        ::testing::ExitedWithCode(EXIT_FAILURE),
        "ABI violation");
}

TEST(GcLifecycleTest, RejectsInvalidAbiVersion) {
    EXPECT_EXIT(
        {
            auto config = gc::test::make_config(1024, STELLA_C_GC_ABI_VERSION + 1);

            stella_gc_init(&config);
        },
        ::testing::ExitedWithCode(EXIT_FAILURE),
        "ABI violation");
}

TEST(GcLifecycleTest, RejectsZeroHeapSize) {
    EXPECT_EXIT(
        {
            auto config = gc::test::make_config(0);
            stella_gc_init(&config);
        },
        ::testing::ExitedWithCode(EXIT_FAILURE),
        "ABI violation");
}

TEST(GcLifecycleTest, RejectsRepeatedInitialization) {
    EXPECT_EXIT(
        {
            auto config = gc::test::make_config();

            stella_gc_init(&config);
            stella_gc_init(&config);
        },
        ::testing::ExitedWithCode(EXIT_FAILURE),
        "ABI violation");
}

TEST(GcLifecycleTest, RejectsOperationBeforeInitialization) {
    EXPECT_EXIT(
        {
            stella_gc_print_statistics(stderr);
        },
        ::testing::ExitedWithCode(EXIT_FAILURE),
        "ABI violation");
}

TEST(GcLifecycleTest, RejectsOperationAfterShutdown) {
    EXPECT_EXIT(
        {
            auto config = gc::test::make_config();

            stella_gc_init(&config);
            stella_gc_shutdown();

            stella_gc_print_statistics(stderr);
        },
        ::testing::ExitedWithCode(EXIT_FAILURE),
        "ABI violation");
}

TEST(GcLifecycleTest, RejectsInitializationAfterShutdown) {
    EXPECT_EXIT(
        {
            auto config = gc::test::make_config();

            stella_gc_init(&config);
            stella_gc_shutdown();

            stella_gc_init(&config);
        },
        ::testing::ExitedWithCode(EXIT_FAILURE),
        "ABI violation");
}