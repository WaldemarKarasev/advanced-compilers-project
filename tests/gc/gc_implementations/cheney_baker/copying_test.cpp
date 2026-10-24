#include <gtest/gtest.h>

#include "utils/descriptor.hpp"
#include "utils/gc_fixture.hpp"

#include <copying.hpp>

using namespace hw::stella;

using CheneyBakerCopyingTest = gc::test::GcFixture<>;

TEST_F(CheneyBakerCopyingTest, ForwardKeepsNull) {
  EXPECT_EQ(gc::copying::Forward(nullptr), nullptr);
}

TEST_F(CheneyBakerCopyingTest, ForwardCopiesObjectToToSpace) {
  auto descriptor = gc::test::make_descriptor(1, 1, 2);

  StellaValue old_object = stella_gc_alloc(&descriptor);

  stella_object_init_primitive(old_object, 0, 42);

  StellaValue new_object = gc::copying::Forward(old_object);

  ASSERT_NE(new_object, nullptr);
  EXPECT_NE(new_object, old_object);

  EXPECT_EQ(new_object->descriptor, &descriptor);

  ASSERT_NE(new_object->fields, nullptr);

  EXPECT_EQ(new_object->fields, reinterpret_cast<StellaSlot *>(new_object + 1));

  EXPECT_EQ(new_object->fields[1].primitive, 42u);
}

TEST_F(CheneyBakerCopyingTest, ForwardReturnsSameCopyOnRepeatedVisit) {
  auto descriptor = gc::test::make_descriptor();

  StellaValue old_object = stella_gc_alloc(&descriptor);

  StellaValue first = gc::copying::Forward(old_object);

  StellaValue second = gc::copying::Forward(old_object);

  EXPECT_EQ(first, second);
  EXPECT_NE(first, old_object);
}

TEST_F(CheneyBakerCopyingTest, ScanPreservesSharing) {
  auto leaf_descriptor = gc::test::make_descriptor();

  auto owner_descriptor = gc::test::make_descriptor(2, 0, 2);

  StellaValue leaf = stella_gc_alloc(&leaf_descriptor);

  StellaValue owner = stella_gc_alloc(&owner_descriptor);

  stella_object_init_managed(owner, 0, leaf);
  stella_object_init_managed(owner, 1, leaf);

  StellaValue owner_copy = gc::copying::Forward(owner);

  gc::copying::ScanOne(reinterpret_cast<std::byte *>(owner_copy));

  ASSERT_NE(owner_copy->fields[0].managed, nullptr);

  EXPECT_EQ(owner_copy->fields[0].managed, owner_copy->fields[1].managed);

  EXPECT_NE(owner_copy->fields[0].managed, leaf);
}

TEST_F(CheneyBakerCopyingTest, ScanDoesNotTouchPrimitiveFields) {
  auto descriptor = gc::test::make_descriptor(0, 1, 1);

  StellaValue object = stella_gc_alloc(&descriptor);

  constexpr auto value = static_cast<StellaPrimitive>(0x12345678);

  stella_object_init_primitive(object, 0, value);

  StellaValue copy = gc::copying::Forward(object);

  gc::copying::ScanOne(reinterpret_cast<std::byte *>(copy));

  EXPECT_EQ(copy->fields[0].primitive, value);
}

#include <stella/scalar.h>

TEST_F(CheneyBakerCopyingTest, ForwardDoesNotMoveStaticObject) {
  StellaValue value = &stella_bool_true;

  EXPECT_EQ(gc::copying::Forward(value), value);
}