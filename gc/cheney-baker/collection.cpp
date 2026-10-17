#include "collection.hpp"

#include <limits>

#include "copying.hpp"
#include "gc_state.hpp"

namespace hw::stella::gc::collection {

void BeginCollectCycle() {
  auto &gc_state = get_gc_state();
  auto &heap = gc_state.GetHeap();
  auto &roots = gc_state.GetRoots();

  // clean to space before start copying
  heap.ToSpace().Reset();

  // Reset per-cycle statistics before Forward starts
  // copying reachable objects.
  gc_state.GetStats().BeginCycle();

  // begin collecting
  gc_state.SetCollectionState(CollectionPhase::Collecting);

  // roots traversal
  for (StellaValue *root : roots.Dynamic()) {
    *root = copying::Forward(*root);
  }

  // permanent roots traversal
  for (StellaValue *root : roots.Permanent()) {
    *root = copying::Forward(*root);
  }

  // Cheney scan
  std::byte *scan = heap.ToSpace().Begin();
  gc_state.SetScan(scan);
}

void Step(size_t budget) {
  auto &gc_state = get_gc_state();

  if (gc_state.CollectionState() != CollectionPhase::Collecting) {
    return;
  }

  auto &heap = gc_state.GetHeap();

  std::size_t processed = 0;
  std::byte *scan = gc_state.Scan();

  while (scan < heap.ToSpace().Top() && processed < budget) {
    scan = copying::ScanOne(scan);
    ++processed;
  }

  gc_state.SetScan(scan);

  if (scan == heap.ToSpace().Top()) {
    FinishCycle();
  }
}

void FinishCycle() {
  auto &state = get_gc_state();
  auto &heap = state.GetHeap();

  heap.SwapSpaces();

  state.GetStats().FinishCycle();

  state.SetScan(nullptr);

  state.SetCollectionState(CollectionPhase::Idle);
}

void IncrementalStep() {
  auto &gc_state = get_gc_state();

  if (gc_state.CollectionState() != CollectionPhase::Collecting) {
    return;
  }

  ++gc_state.GetStats().normal_steps;

  Step(gc_state.StepBudget());
}

void Collect() {

  auto &gc_state = get_gc_state();

  if (gc_state.CollectionState() == CollectionPhase::Idle) {
    BeginCollectCycle();
  }

  while (get_gc_state().CollectionState() == CollectionPhase::Collecting) {
    collection::Step(std::numeric_limits<std::size_t>::max());
  }
}

} // namespace hw::stella::gc::collection