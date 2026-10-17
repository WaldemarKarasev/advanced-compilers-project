#pragma once

#include <cinttypes>
#include <cstddef>
#include <cstdio>

#include "gc_state.hpp"
#include "object.hpp"

namespace hw::stella::gc {

inline StellaValue finish_allocation(std::byte *storage,
                                     const StellaObjectDescriptor *descriptor,
                                     std::size_t size) {

  auto *result = reinterpret_cast<StellaObject *>(storage);

  object::initialize_object(result, descriptor);

  auto &state = get_gc_state();

  const bool during_collection =
      state.CollectionState() == CollectionPhase::Collecting;

  state.GetStats().RecordAllocation(size, during_collection);

  return result;
}

inline void print_object(FILE *output, StellaValue object) {
  const StellaObjectDescriptor *descriptor = object->descriptor;

  std::fprintf(output,
               "    object=%p name=%s "
               "m=%zu p=%zu c=%zu "
               "gc_word=%" PRIuPTR "\n",
               static_cast<void *>(object),
               descriptor->debug_name != nullptr ? descriptor->debug_name
                                                 : "(unnamed)",
               descriptor->managed_count, descriptor->primitive_count,
               descriptor->slot_capacity, object->gc_word);

  for (std::size_t i = 0; i < descriptor->managed_count; ++i) {

    std::fprintf(output, "      managed[%zu]=%p\n", i,
                 static_cast<void *>(object->fields[i].managed));
  }

  for (std::size_t i = 0; i < descriptor->primitive_count; ++i) {

    const std::size_t physical_index = descriptor->managed_count + i;

    std::fprintf(output, "      primitive[%zu]=%" PRIuPTR "\n", i,
                 object->fields[physical_index].primitive);
  }

  for (std::size_t i = descriptor->managed_count + descriptor->primitive_count;
       i < descriptor->slot_capacity; ++i) {

    std::fprintf(output, "      inactive[%zu]=%" PRIuPTR "\n", i,
                 object->fields[i].primitive);
  }
}

inline void print_object_range(FILE *output, std::byte *begin, std::byte *end) {

  std::byte *cursor = begin;

  while (cursor < end) {
    auto *object = reinterpret_cast<StellaObject *>(cursor);

    print_object(output, object);

    cursor += object::object_size(object->descriptor);
  }
}

inline void print_space(FILE *output, const char *name, heap::SemiSpace &space) {

  std::fprintf(output,
               "%s:\n"
               "  begin=%p\n"
               "  top=%p\n"
               "  limit=%p\n"
               "  end=%p\n"
               "  capacity=%zu\n"
               "  used=%zu\n"
               "  free=%zu\n",
               name, static_cast<void *>(space.Begin()),
               static_cast<void *>(space.Top()),
               static_cast<void *>(space.Limit()),
               static_cast<void *>(space.End()), space.Capacity(), space.Used(),
               space.Free());

  std::fprintf(output, "  low objects:\n");

  print_object_range(output, space.Begin(), space.Top());

  std::fprintf(output, "  high objects:\n");

  print_object_range(output, space.Limit(), space.End());
}

} // namespace hw::stella::gc
