#include <utils/descriptor.hpp>
#include <utils/gc_fixture.hpp>

#include <bindings/gc.hpp>
#include <gtest/gtest.h>

using namespace hw::stella;

using GcFieldAccessTest = gc::test::GcFixture<>;

TEST_F(GcFieldAccessTest, ReadsAndWritesManagedAndPrimitiveFields) {
  auto object_descriptor = gc::test::make_descriptor(1, // managed fields
                                                     2, // primitive fields
                                                     3  // total capacity
  );
  auto leaf_descriptor = gc::test::make_descriptor();

  StellaValue object = nullptr;
  StellaValue first_value = nullptr;
  StellaValue replacement = nullptr;

  stella_gc_push_root(&object);
  stella_gc_push_root(&first_value);
  stella_gc_push_root(&replacement);

  object = stella_gc_alloc(&object_descriptor);
  first_value = stella_gc_alloc(&leaf_descriptor);
  replacement = stella_gc_alloc(&leaf_descriptor);

  stella_gc_write_managed(&object, 0, &first_value);
  EXPECT_EQ(stella_gc_read_managed(&object, 0), first_value);

  stella_gc_write_managed(&object, 0, &replacement);
  EXPECT_EQ(stella_gc_read_managed(&object, 0), replacement);

  constexpr StellaPrimitive first_primitive = 0x1234;
  constexpr StellaPrimitive second_primitive = 0x5678;

  stella_object_write_primitive(object, 0, first_primitive);
  stella_object_write_primitive(object, 1, second_primitive);

  EXPECT_EQ(stella_object_read_primitive(object, 0), first_primitive);
  EXPECT_EQ(stella_object_read_primitive(object, 1), second_primitive);

  // Primitive writes must not overwrite the managed field.
  EXPECT_EQ(stella_gc_read_managed(&object, 0), replacement);

  stella_gc_pop_root(&replacement);
  stella_gc_pop_root(&first_value);
  stella_gc_pop_root(&object);
}

TEST_F(GcFieldAccessTest, RejectsManagedReadOutOfRange) {
  auto descriptor = gc::test::make_descriptor(1, 0, 1);
  StellaValue object = nullptr;

  stella_gc_push_root(&object);
  object = stella_gc_alloc(&descriptor);

  EXPECT_EXIT(
      { static_cast<void>(stella_gc_read_managed(&object, 1)); },
      ::testing::ExitedWithCode(EXIT_FAILURE), "ABI violation");

  stella_gc_pop_root(&object);
}

TEST_F(GcFieldAccessTest, RejectsManagedWriteOutOfRange) {
  auto descriptor = gc::test::make_descriptor(1, 0, 1);
  StellaValue object = nullptr;
  StellaValue value = nullptr;

  stella_gc_push_root(&object);
  object = stella_gc_alloc(&descriptor);

  EXPECT_EXIT(
      { stella_gc_write_managed(&object, 1, &value); },
      ::testing::ExitedWithCode(EXIT_FAILURE), "ABI violation");

  stella_gc_pop_root(&object);
}

TEST_F(GcFieldAccessTest, RejectsPrimitiveReadOutOfRange) {
  auto descriptor = gc::test::make_descriptor(0, 1, 1);
  StellaValue object = nullptr;

  stella_gc_push_root(&object);
  object = stella_gc_alloc(&descriptor);

  EXPECT_EXIT(
      { static_cast<void>(stella_object_read_primitive(object, 1)); },
      ::testing::ExitedWithCode(EXIT_FAILURE), "ABI violation");

  stella_gc_pop_root(&object);
}

TEST_F(GcFieldAccessTest, RejectsPrimitiveWriteOutOfRange) {
  auto descriptor = gc::test::make_descriptor(0, 1, 1);
  StellaValue object = nullptr;

  stella_gc_push_root(&object);
  object = stella_gc_alloc(&descriptor);

  EXPECT_EXIT(
      { stella_object_write_primitive(object, 1, 42); },
      ::testing::ExitedWithCode(EXIT_FAILURE), "ABI violation");

  stella_gc_pop_root(&object);
}

TEST_F(GcFieldAccessTest, RejectsManagedWriteToStaticObject) {
  auto descriptor =
      gc::test::make_descriptor(1, 0, 1, STELLA_DESCRIPTOR_STATIC);

  // static object initialization
  StellaSlot fields[1]{};
  StellaObject object_storage{&descriptor, 0, fields};

  StellaValue object = &object_storage;
  StellaValue value = nullptr;

  stella_gc_push_root(&object);
  stella_gc_push_root(&value);

  EXPECT_EXIT(
      { stella_gc_write_managed(&object, 0, &value); },
      ::testing::ExitedWithCode(EXIT_FAILURE), "ABI violation");

  stella_gc_pop_root(&value);
  stella_gc_pop_root(&object);
}

TEST_F(GcFieldAccessTest, RejectsPrimitiveWriteToStaticObject) {
  auto descriptor =
      gc::test::make_descriptor(0, 1, 1, STELLA_DESCRIPTOR_STATIC);

  // static object initialization
  StellaSlot fields[1]{};
  StellaObject object_storage{&descriptor, 0, fields};

  StellaValue object = &object_storage;

  stella_gc_push_root(&object);

  EXPECT_EXIT(
      { stella_object_write_primitive(object, 0, 42); },
      ::testing::ExitedWithCode(EXIT_FAILURE), "ABI violation");

  stella_gc_pop_root(&object);
}