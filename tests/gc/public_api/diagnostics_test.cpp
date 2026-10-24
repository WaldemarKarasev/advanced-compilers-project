#include <gtest/gtest.h>

#include <bindings/gc.hpp>
#include <utils/gc_fixture.hpp>

#include <cstdio>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>

using namespace hw::stella;

using GcDiagnosticsTest = gc::test::GcFixture<>;

namespace {

StellaValue permanent_root = nullptr;

struct FileCloser {
  void operator()(FILE *file) const noexcept {
    if (file != nullptr) {
      std::fclose(file);
    }
  }
};

using TempFile = std::unique_ptr<FILE, FileCloser>;

TempFile make_temp_file() { return TempFile(std::tmpfile()); }

std::string read_output(FILE *file) {
  std::fflush(file);
  std::rewind(file);

  std::string output;
  char buffer[256];

  while (std::fgets(buffer, sizeof(buffer), file) != nullptr) {
    output += buffer;
  }

  return output;
}

void expect_contains(const std::string &output, std::string_view fragment) {
  EXPECT_NE(output.find(fragment), std::string::npos) << "Output:\n" << output;
}

} // namespace

TEST_F(GcDiagnosticsTest, PrintsStatistics) {
  auto file = make_temp_file();
  ASSERT_NE(file, nullptr);

  stella_gc_print_statistics(file.get());

  const auto output = read_output(file.get());

  expect_contains(output, "allocated bytes:");
  expect_contains(output, "allocated objects:");
  expect_contains(output, "completed cycles:");
  expect_contains(output, "occupied bytes:");
  expect_contains(output, "managed reads:");
  expect_contains(output, "primitive writes:");
  expect_contains(output, "step budget:");

  const auto budget_position = output.find("step budget: ");
  ASSERT_NE(budget_position, std::string::npos);

  std::istringstream budget_stream(
      output.substr(budget_position + std::string("step budget: ").size()));

  size_t step_budget = 0;
  budget_stream >> step_budget;

  EXPECT_GT(step_budget, 0u);
}

TEST_F(GcDiagnosticsTest, PrintsDynamicAndPermanentRoots) {
  auto file = make_temp_file();
  ASSERT_NE(file, nullptr);

  StellaValue dynamic_root = nullptr;
  stella_gc_push_root(&dynamic_root);
  stella_gc_register_permanent_root(&permanent_root);

  stella_gc_print_roots(file.get());
  const auto output = read_output(file.get());

  stella_gc_pop_root(&dynamic_root);

  expect_contains(output, "dynamic roots: 1");
  expect_contains(output, "dynamic[0]: slot=");
  expect_contains(output, "permanent roots: 1");
  expect_contains(output, "permanent[0]: slot=");
}

TEST_F(GcDiagnosticsTest, PrintsHeapState) {
  auto file = make_temp_file();
  ASSERT_NE(file, nullptr);

  stella_gc_print_state(file.get());

  const auto output = read_output(file.get());

  expect_contains(output, "phase: idle");
  expect_contains(output, "max heap bytes:");
  expect_contains(output, "step budget:");
  expect_contains(output, "from-space");
  expect_contains(output, "to-space");
}

TEST_F(GcDiagnosticsTest, RejectsNullStatisticsOutput) {
  EXPECT_EXIT(
      { stella_gc_print_statistics(nullptr); },
      ::testing::ExitedWithCode(EXIT_FAILURE), "ABI violation");
}

TEST_F(GcDiagnosticsTest, RejectsNullRootsOutput) {
  EXPECT_EXIT(
      { stella_gc_print_roots(nullptr); },
      ::testing::ExitedWithCode(EXIT_FAILURE), "ABI violation");
}

TEST_F(GcDiagnosticsTest, RejectsNullStateOutput) {
  EXPECT_EXIT(
      { stella_gc_print_state(nullptr); },
      ::testing::ExitedWithCode(EXIT_FAILURE), "ABI violation");
}