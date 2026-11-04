#pragma once
#include <functional>
#include "pl/Export.hpp"
namespace ll::render {
using PostClearCallback = std::function<void(int, int)>;
PL_EXPORT void registerPostClearCallback(PostClearCallback callback);
}
