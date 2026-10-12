#pragma once

#include <cstddef>

namespace hw::stella::gc::collection {

// incremental collection
void BeginCollectCycle();
void Step(size_t budget);
void FinishCycle();

void IncrementalStep();

// full collection
void Collect();

} // namespace hw::stella::gc::collection