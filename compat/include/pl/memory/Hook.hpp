#pragma once
#include <cstdint>
#include "pl/Export.hpp"
namespace pl::memory {
class HookHandle {
public:
  HookHandle() = default;
  HookHandle(void* target, void* replacement, void** original) noexcept;
  HookHandle(const HookHandle&) = delete;
  HookHandle& operator=(const HookHandle&) = delete;
  HookHandle(HookHandle&& other) noexcept : mTarget(other.mTarget) { other.mTarget = nullptr; }
  HookHandle& operator=(HookHandle&& other) noexcept { if (this != &other) { reset(); mTarget = other.mTarget; other.mTarget = nullptr; } return *this; }
  ~HookHandle() { reset(); }
  void reset() noexcept;
  explicit operator bool() const noexcept { return mTarget != nullptr; }
private:
  void* mTarget{};
};
}
