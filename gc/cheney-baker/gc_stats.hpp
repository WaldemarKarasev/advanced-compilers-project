#pragma once

#include <algorithm>
#include <cstddef>

namespace hw::stella::gc {

struct Statistics {
  size_t allocated_bytes = 0;
  size_t allocated_objects = 0;

  size_t completed_cycles = 0;
  size_t normal_steps = 0;
  size_t alloc_budget_exceeded = 0;

  size_t occupied_bytes = 0;
  size_t peak_occupied_bytes = 0;

  size_t managed_reads = 0;
  size_t managed_writes = 0;
  size_t primitive_reads = 0;
  size_t primitive_writes = 0;

  size_t read_barrier_activations = 0;
  size_t write_barrier_activations = 0;

  size_t peak_dynamic_root_depth = 0;

  size_t cycle_copied_bytes = 0;
  size_t cycle_new_bytes = 0;

  void Reset() noexcept { *this = {}; }

  void RecordAllocation(size_t bytes, bool during_collection) noexcept {
    allocated_bytes += bytes;
    ++allocated_objects;

    occupied_bytes += bytes;

    if (during_collection) {
      cycle_new_bytes += bytes;
    }

    peak_occupied_bytes = std::max(peak_occupied_bytes, occupied_bytes);
  }

  void BeginCycle() noexcept {
    cycle_copied_bytes = 0;
    cycle_new_bytes = 0;
  }

  void RecordCopied(size_t bytes) noexcept { cycle_copied_bytes += bytes; }

  void FinishCycle() noexcept {
    occupied_bytes = cycle_copied_bytes + cycle_new_bytes;

    ++completed_cycles;
  }

  void UpdatePeakDynRootDepth(size_t depth) {
    if (depth > peak_dynamic_root_depth) {
      peak_dynamic_root_depth = depth;
    }
  }
};

} // namespace hw::stella::gc