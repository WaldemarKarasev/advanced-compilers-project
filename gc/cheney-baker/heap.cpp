#include "heap.hpp"
#include <cstdint>
#include <cstdlib>

namespace hw::stella::gc::heap {

bool SemiSpace::Init(size_t capacity) noexcept {
  Deinit();

  auto *memory = static_cast<std::byte *>(std::malloc(capacity));

  if (memory == nullptr) {
    return false;
  }

  storage_.reset(memory);
  top_ = memory;
  capacity_ = capacity;
  limit_ = top_ + capacity_;

  return true;
}

void SemiSpace::Deinit() noexcept {
  storage_.reset();
  top_ = nullptr;
  limit_ = nullptr;
  capacity_ = 0;
}

std::byte *SemiSpace::Allocate(size_t bytes, size_t alignment) noexcept {
  if (storage_ == nullptr) {
    return nullptr;
  }

  const auto current = reinterpret_cast<std::uintptr_t>(top_);

  const auto remainder = current % alignment;

  const auto padding = remainder == 0 ? 0 : alignment - remainder;

  if (padding > Free()) {
    return nullptr;
  }

  if (bytes > Free() - padding) {
    return nullptr;
  }

  top_ += padding;

  auto *result = top_;
  top_ += bytes;

  return result;
}

std::byte *SemiSpace::AllocateHigh(size_t bytes, size_t alignment) noexcept {

  if (storage_ == nullptr) {
    return nullptr;
  }

  if (bytes > Free()) {
    return nullptr;
  }

  auto candidate = reinterpret_cast<std::uintptr_t>(limit_) - bytes;

  const auto remainder = candidate % alignment;

  candidate -= remainder;

  auto *new_limit = reinterpret_cast<std::byte *>(candidate);

  if (new_limit < top_) {
    return nullptr;
  }

  limit_ = new_limit;

  return limit_;
}

void SemiSpace::Reset() noexcept {
  top_ = storage_.get();

  limit_ = storage_ == nullptr ? nullptr : top_ + capacity_;
}

bool SemiSpace::Contains(const void *ptr) const noexcept {
  if (ptr == nullptr || storage_ == nullptr) {
    return false;
  }

  const auto address = reinterpret_cast<std::uintptr_t>(ptr);

  const auto begin = reinterpret_cast<std::uintptr_t>(Begin());
  const auto end = reinterpret_cast<std::uintptr_t>(End());

  return address >= begin && address < end;
}

std::byte *SemiSpace::Top() noexcept { return top_; }
const std::byte *SemiSpace::Top() const noexcept { return top_; }

std::byte *SemiSpace::Begin() noexcept { return storage_.get(); }

const std::byte *SemiSpace::Begin() const noexcept { return storage_.get(); }
std::byte *SemiSpace::End() noexcept {
  return storage_ == nullptr ? nullptr : storage_.get() + capacity_;
}
const std::byte *SemiSpace::End() const noexcept {
  return storage_ == nullptr ? nullptr : storage_.get() + capacity_;
}

size_t SemiSpace::Capacity() const noexcept { return capacity_; }

size_t SemiSpace::Used() const noexcept {
  if (storage_ == nullptr) {
    return 0;
  }

  const size_t low = static_cast<size_t>(top_ - Begin());

  const size_t high = static_cast<size_t>(End() - limit_);

  return low + high;
}

size_t SemiSpace::Free() const noexcept {
  if (storage_ == nullptr) {
    return 0;
  }

  return static_cast<size_t>(limit_ - top_);
}

void SemiSpace::FreeDeleter::operator()(std::byte *ptr) const noexcept {
  std::free(ptr);
};

bool Heap::Init(size_t semispace_size) noexcept {
  Deinit();

  if (not space_a_.Init(semispace_size)) {
    return false;
  }

  if (not space_b_.Init(semispace_size)) {
    space_a_.Deinit();
    return false;
  }

  from_ = &space_a_;
  to_ = &space_b_;

  return true;
}
void Heap::Deinit() noexcept {
  space_a_.Deinit();
  space_b_.Deinit();

  from_ = &space_a_;
  to_ = &space_b_;
}

SemiSpace &Heap::FromSpace() noexcept { return *from_; }

SemiSpace &Heap::ToSpace() noexcept { return *to_; }

const SemiSpace &Heap::FromSpace() const noexcept { return *from_; }
const SemiSpace &Heap::ToSpace() const noexcept { return *to_; }

bool Heap::Owns(const void *ptr) const noexcept {
  return space_a_.Contains(ptr) || space_b_.Contains(ptr);
}

void Heap::SwapSpaces() noexcept {
  std::swap(from_, to_);

  to_->Reset();
}

} // namespace hw::stella::gc::heap