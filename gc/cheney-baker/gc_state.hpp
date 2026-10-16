#pragma once

#include <cstddef>

#include "gc_stats.hpp"
#include "heap.hpp"
#include "roots.hpp"

namespace hw::stella::gc {

enum class Lifecycle {
  NotInit,
  Running,
  Shutdown,
};

enum class CollectionPhase {
  Idle,
  Collecting,
};

static constexpr size_t kDefaultStepBudget = 1;

class GcState {
public:
  GcState() = default;

  bool Init(size_t max_heap_bytes) & {
    if (not heap_.Init(max_heap_bytes)) {
      return false;
    }

    lifecycle_ = Lifecycle::Running;
    max_heap_bytes_ = max_heap_bytes;

    stats_.Reset();

    collection_phase_ = CollectionPhase::Idle;
    scan_ = nullptr;
    step_budget_ = kDefaultStepBudget;

    return true;
  }

  void Deinit() & {
    roots_.Clear();
    heap_.Deinit();

    lifecycle_ = Lifecycle::Shutdown;
    max_heap_bytes_ = 0;

    collection_phase_ = CollectionPhase::Idle;
    scan_ = nullptr;
    step_budget_ = kDefaultStepBudget;
  }

  Lifecycle State() const & { return lifecycle_; }
  size_t MaxHeapSize() const & { return max_heap_bytes_; }

  // heap
  heap::Heap &GetHeap() & { return heap_; }
  const heap::Heap &GetHeap() const & { return heap_; }

  // roots
  roots::Roots &GetRoots() & { return roots_; }
  const roots::Roots &GetRoots() const & { return roots_; }

  // stats
  Statistics &GetStats() { return stats_; }
  const Statistics &GetStats() const { return stats_; }

  CollectionPhase CollectionState() const noexcept { return collection_phase_; }

  void SetCollectionState(CollectionPhase phase) noexcept {
    collection_phase_ = phase;
  }

  std::byte *Scan() const noexcept { return scan_; }

  void SetScan(std::byte *scan) noexcept { scan_ = scan; }

  size_t StepBudget() const noexcept { return step_budget_; }

  void SetStepBudget(size_t value) noexcept { step_budget_ = value; }

private:
  Lifecycle lifecycle_ = Lifecycle::NotInit;
  size_t max_heap_bytes_ = 0;

  heap::Heap heap_;
  roots::Roots roots_;

  CollectionPhase collection_phase_ = CollectionPhase::Idle;

  std::byte *scan_ = nullptr;

  size_t step_budget_ = kDefaultStepBudget;

  Statistics stats_;
};

void require_running();

GcState &get_gc_state();

} // namespace hw::stella::gc