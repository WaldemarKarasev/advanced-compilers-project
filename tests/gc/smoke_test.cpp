#include <bindings/gc.hpp>

#include <gtest/gtest.h>

TEST(GcSmokeTest, InitAndShutdownCanBeLinkedAndCalled) {
  StellaGcConfig config{
      .abi_version = 1,
      .max_heap_bytes = 1024,
  };

  stella_gc_init(&config);
  stella_gc_shutdown();

  SUCCEED();
}