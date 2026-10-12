#pragma once

#include <cstddef>

namespace hw::stella::gc::collection {

void BeginCollectCycle();
void Step(size_t budget);
void FinishCycle();

void Collect();

} // namespace hw::stella::gc::collection