#pragma once
#include <vector>
#include <string>

namespace beaver {

#define NOMINMAX
#define INLINE inline constexpr
#define ALIGN64 alignas(64)

using i8  = char;
using i16 = short;
using i32 = int;
using i64 = long long;

using u8  = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

using MB = u32; // megabytes
using MS = i64; // milliseconds

using Strings = std::vector<std::string>;

}
