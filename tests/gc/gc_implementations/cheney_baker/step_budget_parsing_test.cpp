#include <gtest/gtest.h>

#include <bindings/gc.hpp>
#include <utils/config.hpp>

#include <cstdio>
#include <cstdlib>
#include <string>

using namespace hw::stella;

namespace {

constexpr char step_budget_env[] = "STELLA_GC_STEP_BUDGET";

void initialize_and_print_step_budget(const char *value) {
  const int env_result = value == nullptr ? ::unsetenv(step_budget_env)
                                          : ::setenv(step_budget_env, value, 1);

  if (env_result != 0) {
    std::exit(90);
  }

  auto config = gc::test::make_config();
  stella_gc_init(&config);
  stella_gc_print_statistics(stderr);
  stella_gc_shutdown();
  std::exit(EXIT_SUCCESS);
}

} // namespace

TEST(GcStepBudgetTest, UsesDefaultWhenEnvironmentIsUnset) {
  EXPECT_EXIT(
      { initialize_and_print_step_budget(nullptr); },
      ::testing::ExitedWithCode(EXIT_SUCCESS), "step budget: 1");
}

TEST(GcStepBudgetTest, AcceptsPositiveInteger) {
  EXPECT_EXIT(
      { initialize_and_print_step_budget("7"); },
      ::testing::ExitedWithCode(EXIT_SUCCESS), "step budget: 7");
}

TEST(GcStepBudgetTest, RejectsZero) {
  EXPECT_EXIT(
      { initialize_and_print_step_budget("0"); },
      ::testing::ExitedWithCode(EXIT_FAILURE), "ABI violation");
}

TEST(GcStepBudgetTest, RejectsEmptyValue) {
  EXPECT_EXIT(
      { initialize_and_print_step_budget(""); },
      ::testing::ExitedWithCode(EXIT_FAILURE), "ABI violation");
}

TEST(GcStepBudgetTest, RejectsNonNumericValue) {
  EXPECT_EXIT(
      { initialize_and_print_step_budget("abc"); },
      ::testing::ExitedWithCode(EXIT_FAILURE), "ABI violation");
}

TEST(GcStepBudgetTest, RejectsNegativeValue) {
  EXPECT_EXIT(
      { initialize_and_print_step_budget("-1"); },
      ::testing::ExitedWithCode(EXIT_FAILURE), "ABI violation");
}

TEST(GcStepBudgetTest, RejectsOverflow) {
  const std::string too_large(128, '9');

  EXPECT_EXIT(
      { initialize_and_print_step_budget(too_large.c_str()); },
      ::testing::ExitedWithCode(EXIT_FAILURE), "ABI violation");
}