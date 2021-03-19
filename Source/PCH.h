// Standard Library
#include <cstdint>
#include <cstring>

#include <algorithm>
#include <any>
#include <atomic>
#include <bitset>
#include <chrono>
#include <functional>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <thread>
#include <vector>

#include "glm/glm.hpp"

#include "App/Logger.h"

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using f32 = float;
using f64 = double;

static_assert(sizeof(uintptr_t) == 8, "Pebble only supports 64-bit compilers on 64-bit systems");
