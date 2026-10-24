#include <gtest/gtest.h>

#include <bindings/gc.hpp>
#include <utils/descriptor.hpp>
#include <utils/gc_fixture.hpp>

#include <cstddef>

using namespace hw::stella;

constexpr std::size_t one_slot_object_size =
    sizeof(StellaObject) + sizeof(StellaSlot);
constexpr std::size_t tiny_heap_size = 4 * one_slot_object_size;

using GcSmallHeapStressTest = gc::test::GcFixture<tiny_heap_size>;

TEST_F(GcSmallHeapStressTest, PreservesRootedCycleWhileReclaimingGarbage) {
  auto descriptor = gc::test::make_descriptor(1, 0, 1);

  StellaValue root = nullptr;
  stella_gc_push_root(&root);

  root = stella_gc_alloc(&descriptor);
  stella_gc_write_managed(&root, 0, &root);

  for (std::size_t i = 0; i < 64; ++i) {
    static_cast<void>(stella_gc_alloc(&descriptor));

    if (i % 8 == 7) {
      StellaValue self = stella_gc_read_managed(&root, 0);
      EXPECT_EQ(self, root);
    }
  }

  StellaValue self = stella_gc_read_managed(&root, 0);
  EXPECT_EQ(self, root);

  stella_gc_pop_root(&root);
}