#pragma once

#include <cstddef>

#include <bindings/gc.hpp>

namespace hw::stella::gc::copying {

StellaValue Forward(StellaValue value);

std::byte *ScanOne(std::byte *scan);

} // namespace hw::stella::gc::copying