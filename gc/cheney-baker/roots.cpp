#include "roots.hpp"

#include <algorithm>
#include <new>

namespace hw::stella::gc::roots {

bool Roots::IsRegistered(StellaValue *root) const noexcept {
  const auto in_dynamic =
      std::find(dynamic_.begin(), dynamic_.end(), root) != dynamic_.end();

  if (in_dynamic) {
    return true;
  }

  return std::find(permanent_.begin(), permanent_.end(), root) !=
         permanent_.end();
}

bool Roots::PushDynamic(StellaValue *root) noexcept {
  try {
    dynamic_.push_back(root);
    return true;
  } catch (const std::bad_alloc &) {
    return false;
  }
}

void Roots::PopDynamic() noexcept { dynamic_.pop_back(); }

bool Roots::RegisterPermanent(StellaValue *root) noexcept {
  try {
    permanent_.push_back(root);
    return true;
  } catch (const std::bad_alloc &) {
    return false;
  }
}

void Roots::Clear() noexcept {
  dynamic_.clear();
  permanent_.clear();
}

} // namespace hw::stella::gc::roots