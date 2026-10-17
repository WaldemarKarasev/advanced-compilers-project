#include "copying.hpp"

#include "gc_state.hpp"
#include "object.hpp"
#include "stella/runtime.h"

#include <cstring>

namespace hw::stella::gc::copying {

StellaValue Forward(StellaValue value) {
  if (value == nullptr) {
    return nullptr;
  }

  auto &gc_state = get_gc_state();
  auto &heap = gc_state.GetHeap();

  // if value already copied to to-space
  if (!heap.FromSpace().Contains(value)) {
    return value;
  }

  // gc_word used as a storage for forwarded pointer
  if (value->gc_word != 0) {
    return reinterpret_cast<StellaValue>(value->gc_word);
  }

  const std::size_t size = object::object_size(value->descriptor);

  // allocate space for object to be copied
  std::byte *storage = heap.ToSpace().Allocate(size, alignof(StellaObject));

  if (storage == nullptr) {
    stella_gc_out_of_memory(size);
  }

  std::memcpy(storage, value, size);

  auto *copy = reinterpret_cast<StellaObject *>(storage);

  copy->fields = copy->descriptor->slot_capacity == 0
                     ? nullptr
                     : reinterpret_cast<StellaSlot *>(copy + 1);

  copy->gc_word = 0;

  value->gc_word = reinterpret_cast<std::uintptr_t>(copy);

  if (gc_state.CollectionState() == CollectionPhase::Collecting) {
    gc_state.GetStats().RecordCopied(size);
  }

  return copy;
}

std::byte *ScanOne(std::byte *scan) {
  auto *current = reinterpret_cast<StellaObject *>(scan);

  const StellaObjectDescriptor *descriptor = current->descriptor;

  for (std::size_t i = 0; i < descriptor->managed_count; ++i) {

    current->fields[i].managed = Forward(current->fields[i].managed);
  }

  const std::size_t size = object::object_size(descriptor);

  return scan + size;
}

} // namespace hw::stella::gc::copying