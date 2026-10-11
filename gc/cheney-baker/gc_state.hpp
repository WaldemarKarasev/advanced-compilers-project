#pragma once

#include <cstddef>

#include "heap.hpp"
#include "roots.hpp"

namespace hw::stella::gc {

enum class Lifecycle {
  NotInit,
  Running,
  Shutdown,
};

class GcState {
public:
  GcState() = default;

  Lifecycle State() const & { return lifecycle_; }
  size_t MaxHeapSize() const & { return max_heap_bytes_; }
  
  // heap
  heap::Heap &GetHeap() & { return heap_; }
  const heap::Heap &GetHeap() const & { return heap_; }

  // roots
  roots::Roots& GetRoots() & { return roots_; }
  const roots::Roots& GetRoots() const & { return roots_; }


  bool Init(size_t max_heap_bytes) & {
    if (not heap_.Init(max_heap_bytes)) {
      return false;
    }

    lifecycle_ = Lifecycle::Running;
    max_heap_bytes_ = max_heap_bytes;

    return true;
  }

  void Deinit() & {
    roots_.Clear();
    heap_.Deinit();

    lifecycle_ = Lifecycle::Shutdown;
    max_heap_bytes_ = 0;
  }

private:
  Lifecycle lifecycle_ = Lifecycle::NotInit;
  size_t max_heap_bytes_ = 0;

  heap::Heap heap_;
  roots::Roots roots_;
};

void require_running();

GcState &get_gc_state();

} // namespace hw::stella::gc