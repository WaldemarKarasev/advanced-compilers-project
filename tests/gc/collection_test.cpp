#include "utils/config.hpp"
#include "utils/descriptor.hpp"

#include <bindings/gc.hpp>
#include <gtest/gtest.h>

using namespace hw::stella;

TEST(GcCollectionTest, ReclaimsGarbageAndPreservesCycle) {
  constexpr std::size_t object_size = sizeof(StellaObject) + sizeof(StellaSlot);

  auto config = gc::test::make_config(4 * object_size);

  stella_gc_init(&config);

  auto descriptor = gc::test::make_descriptor(1, 0, 1);

  // unreachable garbage
  stella_gc_alloc(&descriptor);

  StellaValue root = nullptr;
  stella_gc_push_root(&root);

  root = stella_gc_alloc(&descriptor);

  StellaValue child = nullptr;
  stella_gc_push_root(&child);

  child = stella_gc_alloc(&descriptor);

  // A -> B
  stella_object_init_managed(root, 0, child);

  // B -> A
  stella_object_init_managed(child, 0, root);

  stella_gc_pop_root(&child);

  stella_gc_alloc(&descriptor);

  StellaValue old_root = root;
  StellaValue old_child = child;

  // no space => collection
  StellaValue fresh = stella_gc_alloc(&descriptor);

  ASSERT_NE(fresh, nullptr);

  // check that root should be updated
  EXPECT_NE(root, old_root);

  StellaValue new_child = stella_gc_read_managed(&root, 0);
  ASSERT_NE(new_child, nullptr);
  stella_gc_push_root(&new_child);

  EXPECT_NE(new_child, old_child);

  StellaValue back_edge = stella_gc_read_managed(&new_child, 0);
  EXPECT_EQ(back_edge, root);

  stella_gc_pop_root(&new_child);
}