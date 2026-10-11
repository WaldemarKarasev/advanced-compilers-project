#pragma once

#include <vector>

#include <bindings/gc.hpp>

namespace hw::stella::gc::roots {

class Roots {
public:
  bool IsRegistered(StellaValue *root) const noexcept;

  bool PushDynamic(StellaValue *root) noexcept;
  void PopDynamic() noexcept;

  bool RegisterPermanent(StellaValue *root) noexcept;

  bool DynamicEmpty() const noexcept { return dynamic_.empty(); }

  StellaValue *DynamicTop() const noexcept {
    return dynamic_.empty() ? nullptr : dynamic_.back();
  }

  const std::vector<StellaValue *> &Dynamic() const noexcept {
    return dynamic_;
  }

  const std::vector<StellaValue *> &Permanent() const noexcept {
    return permanent_;
  }

  void Clear() noexcept;

private:
  std::vector<StellaValue *> dynamic_;
  std::vector<StellaValue *> permanent_;
};

} // namespace hw::stella::gc::roots