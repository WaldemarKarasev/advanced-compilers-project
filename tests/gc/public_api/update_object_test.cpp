#include <gtest/gtest.h>

#include <bindings/gc.hpp>
#include <utils/descriptor.hpp>
#include <utils/gc_fixture.hpp>

using namespace hw::stella;

using GcUpdateObjectTest = gc::test::GcFixture<>;

TEST_F(GcUpdateObjectTest, UpdatesManagedAndPrimitiveFields) {
  auto old_descriptor =
      gc::test::make_descriptor(1, 1, 3, STELLA_DESCRIPTOR_UPDATABLE);
  auto new_descriptor = gc::test::make_descriptor(1, 1, 3);
  auto leaf_descriptor = gc::test::make_descriptor();

  StellaValue object = nullptr;
  StellaValue replacement = nullptr;

  stella_gc_push_root(&object);
  stella_gc_push_root(&replacement);

  object = stella_gc_alloc(&old_descriptor);
  replacement = stella_gc_alloc(&leaf_descriptor);

  StellaValue *new_managed[] = {&replacement};
  const StellaPrimitive new_primitives[] = {42};

  stella_gc_update_object(&object, &new_descriptor, new_managed,
                          new_primitives);

  auto *updated_descriptor = object->descriptor;
  StellaValue updated_managed = stella_gc_read_managed(&object, 0);
  StellaPrimitive updated_primitive = stella_object_read_primitive(object, 0);

  stella_gc_pop_root(&replacement);
  stella_gc_pop_root(&object);

  EXPECT_EQ(updated_descriptor, &new_descriptor);
  EXPECT_EQ(updated_managed, replacement);
  EXPECT_EQ(updated_primitive, 42u);
}

TEST_F(GcUpdateObjectTest, ClearsInactiveSlotsAfterShrinking) {
  auto old_descriptor =
      gc::test::make_descriptor(3, 0, 4, STELLA_DESCRIPTOR_UPDATABLE);
  auto new_descriptor = gc::test::make_descriptor(1, 1, 4);
  auto leaf_descriptor = gc::test::make_descriptor();

  StellaValue object = nullptr;
  StellaValue old_child = nullptr;
  StellaValue replacement = nullptr;

  stella_gc_push_root(&object);
  stella_gc_push_root(&old_child);
  stella_gc_push_root(&replacement);

  object = stella_gc_alloc(&old_descriptor);
  old_child = stella_gc_alloc(&leaf_descriptor);
  replacement = stella_gc_alloc(&leaf_descriptor);

  stella_object_init_managed(object, 2, old_child);

  StellaValue *new_managed[] = {&replacement};
  const StellaPrimitive new_primitives[] = {42};

  stella_gc_update_object(&object, &new_descriptor, new_managed,
                          new_primitives);

  StellaValue inactive_managed = object->fields[2].managed;
  StellaPrimitive inactive_primitive = object->fields[3].primitive;

  stella_gc_pop_root(&replacement);
  stella_gc_pop_root(&old_child);
  stella_gc_pop_root(&object);

  EXPECT_EQ(inactive_managed, nullptr);
  EXPECT_EQ(inactive_primitive, 0u);

  StellaValue active_managed = stella_gc_read_managed(&object, 0);
  StellaPrimitive active_primitive = stella_object_read_primitive(object, 0);

  EXPECT_EQ(active_managed, replacement);
  EXPECT_EQ(active_primitive, 42u);
}

TEST_F(GcUpdateObjectTest, RejectsUpdateWithoutUpdatableFlag) {
  auto old_descriptor = gc::test::make_descriptor(0, 0, 0);
  auto new_descriptor = gc::test::make_descriptor(0, 0, 0);

  StellaValue object = nullptr;
  stella_gc_push_root(&object);
  object = stella_gc_alloc(&old_descriptor);

  EXPECT_EXIT(
      { stella_gc_update_object(&object, &new_descriptor, nullptr, nullptr); },
      ::testing::ExitedWithCode(EXIT_FAILURE), "ABI violation");

  stella_gc_pop_root(&object);
}

TEST_F(GcUpdateObjectTest, RejectsStaticNewDescriptor) {
  auto old_descriptor =
      gc::test::make_descriptor(0, 0, 0, STELLA_DESCRIPTOR_UPDATABLE);
  auto new_descriptor =
      gc::test::make_descriptor(0, 0, 0, STELLA_DESCRIPTOR_STATIC);

  StellaValue object = nullptr;
  stella_gc_push_root(&object);
  object = stella_gc_alloc(&old_descriptor);

  EXPECT_EXIT(
      { stella_gc_update_object(&object, &new_descriptor, nullptr, nullptr); },
      ::testing::ExitedWithCode(EXIT_FAILURE), "ABI violation");

  stella_gc_pop_root(&object);
}

TEST_F(GcUpdateObjectTest, RejectsNewDescriptorWithDifferentSlotCapacity) {
  auto old_descriptor =
      gc::test::make_descriptor(0, 0, 1, STELLA_DESCRIPTOR_UPDATABLE);
  auto new_descriptor = gc::test::make_descriptor(0, 2, 2);

  StellaValue object = nullptr;
  stella_gc_push_root(&object);
  object = stella_gc_alloc(&old_descriptor);

  const StellaPrimitive new_primitives[] = {11, 22};

  EXPECT_EXIT(
      {
        stella_gc_update_object(&object, &new_descriptor, nullptr,
                                new_primitives);
      },
      ::testing::ExitedWithCode(EXIT_FAILURE), "ABI violation");

  stella_gc_pop_root(&object);
}

TEST_F(GcUpdateObjectTest, RejectsNullManagedRoots) {
  auto old_descriptor =
      gc::test::make_descriptor(0, 0, 1, STELLA_DESCRIPTOR_UPDATABLE);
  auto new_descriptor = gc::test::make_descriptor(1, 0, 1);

  StellaValue object = nullptr;
  stella_gc_push_root(&object);
  object = stella_gc_alloc(&old_descriptor);

  EXPECT_EXIT(
      { stella_gc_update_object(&object, &new_descriptor, nullptr, nullptr); },
      ::testing::ExitedWithCode(EXIT_FAILURE), "ABI violation");

  stella_gc_pop_root(&object);
}

TEST_F(GcUpdateObjectTest, RejectsNullPrimitiveValues) {
  auto old_descriptor =
      gc::test::make_descriptor(0, 0, 1, STELLA_DESCRIPTOR_UPDATABLE);
  auto new_descriptor = gc::test::make_descriptor(0, 1, 1);

  StellaValue object = nullptr;
  stella_gc_push_root(&object);
  object = stella_gc_alloc(&old_descriptor);

  EXPECT_EXIT(
      { stella_gc_update_object(&object, &new_descriptor, nullptr, nullptr); },
      ::testing::ExitedWithCode(EXIT_FAILURE), "ABI violation");

  stella_gc_pop_root(&object);
}

TEST_F(GcUpdateObjectTest, RejectsNullManagedRootEntry) {
  auto old_descriptor =
      gc::test::make_descriptor(0, 0, 1, STELLA_DESCRIPTOR_UPDATABLE);
  auto new_descriptor = gc::test::make_descriptor(1, 0, 1);
  StellaValue *new_managed[] = {nullptr};

  StellaValue object = nullptr;
  stella_gc_push_root(&object);
  object = stella_gc_alloc(&old_descriptor);

  EXPECT_EXIT(
      {
        stella_gc_update_object(&object, &new_descriptor, new_managed, nullptr);
      },
      ::testing::ExitedWithCode(EXIT_FAILURE), "ABI violation");

  stella_gc_pop_root(&object);
}