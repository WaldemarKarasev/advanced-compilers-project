#include "collection.hpp"

#include "copying.hpp"
#include "gc_state.hpp"
#include "stella/runtime.h"

namespace hw::stella::gc::collection {

void Collect() {
    auto& gc_state = get_gc_state();
    auto& heap = gc_state.GetHeap();
    auto& roots = gc_state.GetRoots();

    // clean to space before start copying
    heap.ToSpace().Reset();

    // roots traversal
    for (StellaValue* root : roots.Dynamic()) {
        *root = copying::Forward(*root);
    }

    // permanent roots traversal
    for (StellaValue* root : roots.Permanent()) {
        *root = copying::Forward(*root);
    }

    // Cheney scan
    std::byte* scan = heap.ToSpace().Begin();

    while (scan < heap.ToSpace().Top()) {
        scan = copying::ScanOne(scan);
    }

    // swapping spaces
    heap.SwapSpaces();
}

} // namespace hw::stella::gc::collection