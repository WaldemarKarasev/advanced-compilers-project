#include "utils/config.hpp"
#include "utils/descriptor.hpp"

#include <gtest/gtest.h>

#include <bindings/gc.hpp>

using namespace hw::stella;

class GcAllocationTest : public ::testing::Test {
protected:
  static void SetUpTestSuite() {
    auto config = gc::test::make_config(64 * 1024);
    stella_gc_init(&config);
  }

  static void TearDownTestSuite() { stella_gc_shutdown(); }
};

TEST_F(GcAllocationTest, AllocatesObjectWithoutFields) {
  auto descriptor = gc::test::make_descriptor();

  StellaValue object = stella_gc_alloc(&descriptor);

  ASSERT_NE(object, nullptr);

  EXPECT_EQ(object->descriptor, &descriptor);
  EXPECT_EQ(object->gc_word, 0u);
  EXPECT_EQ(object->fields, nullptr);
}

TEST_F(GcAllocationTest, InitializesObjectLayout) {
  auto descriptor = gc::test::make_descriptor(2, // managed
                                              1, // primitive
                                              4  // capacity
  );

  StellaValue object = stella_gc_alloc(&descriptor);

  ASSERT_NE(object, nullptr);
  ASSERT_NE(object->fields, nullptr);

  EXPECT_EQ(object->fields, reinterpret_cast<StellaSlot *>(object + 1));

  EXPECT_EQ(object->fields[0].managed, nullptr);
  EXPECT_EQ(object->fields[1].managed, nullptr);

  EXPECT_EQ(object->fields[2].primitive, 0u);

  // inactive
  EXPECT_EQ(object->fields[3].primitive, 0u);
}

TEST_F(GcAllocationTest, InitializesManagedAndPrimitiveFields) {
  auto child_descriptor = gc::test::make_descriptor();

  auto owner_descriptor = gc::test::make_descriptor(2, 1, 3);

  StellaValue child = stella_gc_alloc(&child_descriptor);

  StellaValue owner = stella_gc_alloc(&owner_descriptor);

  stella_object_init_managed(owner, 1, child);

  stella_object_init_primitive(owner, 0, 12345);

  EXPECT_EQ(owner->fields[1].managed, child);

  EXPECT_EQ(owner->fields[2].primitive, 12345u);
}

TEST_F(GcAllocationTest, RejectsNullDescriptor) {
  EXPECT_EXIT(
      { stella_gc_alloc(nullptr); }, ::testing::ExitedWithCode(EXIT_FAILURE),
      "ABI violation");
}

TEST_F(GcAllocationTest, RejectsStaticDescriptor) {
  EXPECT_EXIT(
      {
        auto descriptor =
            gc::test::make_descriptor(0, 0, 0, STELLA_DESCRIPTOR_STATIC);

        stella_gc_alloc(&descriptor);
      },
      ::testing::ExitedWithCode(EXIT_FAILURE), "ABI violation");
}

TEST_F(GcAllocationTest, RejectsTooManyActiveFields) {
  EXPECT_EXIT(
      {
        auto descriptor = gc::test::make_descriptor(2, 2, 3);

        stella_gc_alloc(&descriptor);
      },
      ::testing::ExitedWithCode(EXIT_FAILURE), "ABI violation");
}

TEST_F(GcAllocationTest, RejectsInvalidDescriptorAbi) {
  EXPECT_EXIT(
      {
        auto descriptor = gc::test::make_descriptor();

        descriptor.abi_version = STELLA_C_GC_ABI_VERSION + 1;

        stella_gc_alloc(&descriptor);
      },
      ::testing::ExitedWithCode(EXIT_FAILURE), "ABI violation");
}