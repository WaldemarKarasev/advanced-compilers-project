#pragma once

#include <cstddef>

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
  void Init(Lifecycle lifecycle, size_t max_heap_bytes) & {
    lifecycle_ = lifecycle;
    max_heap_bytes_ = max_heap_bytes;
  }

  void Deinit() & {
    lifecycle_ = Lifecycle::Shutdown;
    max_heap_bytes_ = 0;
  }

private:
  Lifecycle lifecycle_ = Lifecycle::NotInit;
  size_t max_heap_bytes_ = 0;
};

void require_running();

GcState &get_gc_state();

} // namespace hw::stella::gc