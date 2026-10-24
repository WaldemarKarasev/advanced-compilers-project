#pragma once

// gtest
#include <cstddef>
#include <gtest/gtest.h>

#include "config.hpp"
#include <bindings/gc.hpp>


namespace hw::stella::gc::test {

template <size_t HeapSize = 64 * 1024>
class GcFixture : public ::testing::Test {
protected:
  static void SetUpTestSuite() {
    auto config = gc::test::make_config(HeapSize);
    stella_gc_init(&config);
  }

  static void TearDownTestSuite() { stella_gc_shutdown(); }
};

} // namespace hw::stella::gc::test