#pragma once

#include <cstddef>
#include <memory>

namespace hw::stella::gc::heap {

class SemiSpace {
public:
  SemiSpace() = default;

  SemiSpace(const SemiSpace &) = delete;
  SemiSpace &operator=(const SemiSpace &) = delete;

  bool Init(size_t capacity) noexcept;
  void Deinit() noexcept;

  std::byte *Allocate(size_t bytes, size_t alignment) noexcept;
  std::byte *AllocateHigh(size_t bytes, size_t alignment) noexcept;

  void Reset() noexcept;

  bool Contains(const void *ptr) const noexcept;

  std::byte *Top() noexcept;
  const std::byte *Top() const noexcept;

  std::byte *Begin() noexcept;
  const std::byte *Begin() const noexcept;
  std::byte *End() noexcept;
  const std::byte *End() const noexcept;

  size_t Capacity() const noexcept;
  size_t Used() const noexcept;
  size_t Free() const noexcept;

  std::byte *Limit() noexcept { return limit_; }

  const std::byte *Limit() const noexcept { return limit_; }

private:
  struct FreeDeleter {
    void operator()(std::byte *ptr) const noexcept;
  };

  std::unique_ptr<std::byte, FreeDeleter> storage_;
  std::byte *top_ = nullptr;
  std::byte *limit_ = nullptr;
  size_t capacity_ = 0;
};

class Heap {
public:
  bool Init(size_t semispace_size) noexcept;
  void Deinit() noexcept;

  SemiSpace &FromSpace() noexcept;
  SemiSpace &ToSpace() noexcept;

  const SemiSpace &FromSpace() const noexcept;
  const SemiSpace &ToSpace() const noexcept;

  bool Owns(const void *ptr) const noexcept;

  void SwapSpaces() noexcept;

private:
  SemiSpace space_a_;
  SemiSpace space_b_;

  SemiSpace *from_ = &space_a_;
  SemiSpace *to_ = &space_b_;
};

} // namespace hw::stella::gc::heap