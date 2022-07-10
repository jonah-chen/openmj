
#pragma once
#include "constants.hpp"
#include "core/utils/logging.hpp"

namespace mj {
/**
 * Functions to deal with compactified types
 */
constexpr bool _is_honor(int x) MJ_EXCEPT_WARN
{
    MJ_ASSERT(x < k_UniqueTiles, "_is_honor() called on invalid tile");
    return x >= k_FirstHonorIdx;
}
constexpr bool _is_19(int x) MJ_EXCEPT_WARN
{
    MJ_ASSERT(x < k_UniqueTiles, "_is_19() called on invalid tile");
    return x % 9 == 0 || x % 9 == 8 || _is_honor(x);
}
constexpr bool _is_17(int x) MJ_EXCEPT_WARN
{
    MJ_ASSERT(x < k_UniqueTiles, "_is_17() called on invalid tile");
    MJ_ASSERT(!_is_honor(x), "_is_17() called on honor tile");
    return x % 9 == 0 || x % 9 == 6;
}

constexpr bool _is_set(int x) noexcept { return x & 0x100; }
constexpr bool _is_run(int x) noexcept { return !(x & 0x100); }
constexpr int _set(int x) noexcept { return x | 0x100; }
constexpr int _run(int x) noexcept { return x; }
constexpr int _int(int x) noexcept { return x & 0xff; }
constexpr int _2nd(int x) noexcept { return _int(x) + 1; }
constexpr int _3rd(int x) noexcept { return _int(x) + 2; }

} // namespace mj
