// gtest
#include <gtest/gtest.h>

#include <utils/config.hpp>
#include <utils/descriptor.hpp>
#include <utils/gc_fixture.hpp>

#include <bindings/gc.hpp>

#include <cstdlib>

using namespace hw::stella;

using GcRootTest = gc::test::GcFixture<>;

TEST_F(GcRootTest, PushesAndPopsRoot) {
  StellaValue root = nullptr;

  stella_gc_push_root(&root);
  stella_gc_pop_root(&root);

  SUCCEED();
}

TEST_F(GcRootTest, RootValueCanChangeWhileRegistered) {
  StellaValue root = nullptr;

  auto descriptor = gc::test::make_descriptor();

  stella_gc_push_root(&root);

  root = stella_gc_alloc(&descriptor);

  ASSERT_NE(root, nullptr);

  stella_gc_pop_root(&root);
}

TEST_F(GcRootTest, SupportsLifoRootStack) {
  StellaValue first = nullptr;
  StellaValue second = nullptr;

  stella_gc_push_root(&first);
  stella_gc_push_root(&second);

  stella_gc_pop_root(&second);
  stella_gc_pop_root(&first);
}

TEST_F(GcRootTest, RejectsPopOutOfOrder) {
  EXPECT_EXIT(
      {
        StellaValue first = nullptr;
        StellaValue second = nullptr;

        stella_gc_push_root(&first);
        stella_gc_push_root(&second);

        stella_gc_pop_root(&first);
      },
      ::testing::ExitedWithCode(EXIT_FAILURE), "ABI violation");
}

TEST_F(GcRootTest, RejectsDuplicateDynamicRoot) {
  EXPECT_EXIT(
      {
        StellaValue root = nullptr;

        stella_gc_push_root(&root);
        stella_gc_push_root(&root);
      },
      ::testing::ExitedWithCode(EXIT_FAILURE), "ABI violation");
}

TEST_F(GcRootTest, RejectsRootRegisteredAsDynamicAndPermanent) {
  EXPECT_EXIT(
      {
        StellaValue root = nullptr;

        stella_gc_push_root(&root);
        stella_gc_register_permanent_root(&root);
      },
      ::testing::ExitedWithCode(EXIT_FAILURE), "ABI violation");
}

TEST_F(GcRootTest, AllowsDifferentRootsForSameObject) {
  auto descriptor = gc::test::make_descriptor();

  StellaValue object = stella_gc_alloc(&descriptor);

  StellaValue first = object;
  StellaValue second = object;

  stella_gc_push_root(&first);
  stella_gc_push_root(&second);

  EXPECT_EQ(first, second);

  stella_gc_pop_root(&second);
  stella_gc_pop_root(&first);
}

TEST(GcRootDeathTest, RejectsShutdownWithDynamicRoots) {
  EXPECT_EXIT(
      {
        auto config = gc::test::make_config();

        stella_gc_init(&config);

        StellaValue root = nullptr;
        stella_gc_push_root(&root);

        stella_gc_shutdown();
      },
      ::testing::ExitedWithCode(EXIT_FAILURE), "ABI violation");
}

TEST(GcRootDeathTest, AllowsShutdownWithPermanentRoot) {
  EXPECT_EXIT(
      {
        auto config = gc::test::make_config();

        stella_gc_init(&config);

        StellaValue root = nullptr;
        stella_gc_register_permanent_root(&root);

        stella_gc_shutdown();

        std::exit(EXIT_SUCCESS);
      },
      ::testing::ExitedWithCode(EXIT_SUCCESS), "");
}