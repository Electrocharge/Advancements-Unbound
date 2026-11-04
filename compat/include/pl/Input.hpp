#pragma once
#include <functional>
#include <cstdint>
#include "pl/Export.hpp"
namespace pl::input {
struct TouchEvent {
  int action{};
  float x{};
  float y{};
  float pressure{};
  int pointerId{};
};
using TouchCallback = std::function<bool(const TouchEvent&)>;
PL_EXPORT bool registerTouchCallback(TouchCallback callback);
PL_EXPORT void unregisterTouchCallbacks();
}
