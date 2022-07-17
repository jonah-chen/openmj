
#pragma once
#include "pch/pch.hpp"

namespace mj {
using U8f = uint_fast8_t;
using U16f = uint_fast16_t;
using U32f = uint_fast32_t;
using U64f = uint_fast64_t;
using S8f = int_fast8_t;
using S16f = int_fast16_t;
using S32f = int_fast32_t;
using S64f = int_fast64_t;
using U8 = uint8_t;
using U16 = uint16_t;
using U32 = uint32_t;
using U64 = uint64_t;
using S8 = int8_t;
using S16 = int16_t;
using S32 = int32_t;
using S64 = int64_t;
using F32 = float;
using F64 = double;
using F64 = double;

using MJ_BOOL = S8f; // template safe boolean
constexpr MJ_BOOL MJ_TRUE = 1;
constexpr MJ_BOOL MJ_FALSE = 0;
constexpr MJ_BOOL MJ_MAYBE = -1;

constexpr U8 f_All8 = (U8)-1;
constexpr U16 f_All16 = (U16)-1;
constexpr U32 f_All32 = (U32)-1;
constexpr U64 f_All64 = (U64)-1;

enum Dir : U8f
{
    k_East,
    k_South,
    k_West,
    k_North
};

constexpr U8f k_FirstHonorIdx = 27;
constexpr U8f k_East34 = k_FirstHonorIdx + k_East;
constexpr U8f k_South34 = k_FirstHonorIdx + k_South;
constexpr U8f k_West34 = k_FirstHonorIdx + k_West;
constexpr U8f k_North34 = k_FirstHonorIdx + k_North;
constexpr U8f k_NumWinds = 4;
constexpr U8f k_NumPlayers = 4;

constexpr U8f k_Green = 0;
constexpr U8f k_Red = 1;
constexpr U8f k_White = 2;
constexpr U8f k_Green34 = k_FirstHonorIdx + k_NumWinds + k_Green;
constexpr U8f k_Red34 = k_FirstHonorIdx + k_NumWinds + k_Red;
constexpr U8f k_White34 = k_FirstHonorIdx + k_NumWinds + k_White;
constexpr U8f k_NumDragons = 3;

constexpr U16f k_UniqueTiles = 34;
constexpr U16f k_DeckSize = 136;
constexpr U16f k_DeadWallSize = 14;
constexpr U16f k_MaxHandSize = 14;
constexpr U16f k_MaxNumMeld = 4;
constexpr U16f k_MaxDiscards = 24;

constexpr int k_ModeNormal = 1;
constexpr int k_ModeChiitoi = 2;
constexpr int k_ModeKokushi = 4;
constexpr int k_ModeAll = k_ModeNormal | k_ModeChiitoi | k_ModeKokushi;

constexpr S8f k_ChiiBelow = -1;
constexpr S8f k_ChiiMiddle = 0;
constexpr S8f k_ChiiAbove = 1;

constexpr U16f k_BaseFu = 20;

/**
 * fedcba9876543210
 * sssnnnnppfffffff
 */
namespace tilelayout {
constexpr U8f k_PlayerPos = 7;
constexpr U8f k_NumPos = 9;
constexpr U8f k_SuitPos = 13;
} // namespace tilelayout

constexpr const char *k_DefaultFont = "../assets/fonts/DroidSans.ttf";
constexpr float k_DefaultFontSize = 24.0f;

constexpr bool k_LittleEndian
#ifdef __BYTE_ORDER__
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        = true;
    #else
        = false;
    #endif
#else
    = (std::bit_cast<U16>("\x12") == 0x12);
#endif

constexpr bool k_BigEndian
#ifdef __BYTE_ORDER__
    #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        = true;
    #else
        = false;
    #endif
#else
    = (std::bit_cast<U16>("\x12") == 0x1200);
#endif

template<U8f N>
constexpr U64 k_BitMask()
{
    if constexpr (N == 0)
        return 0;
    if constexpr (k_LittleEndian && N <= 64)
        return (U64)-1 >> (64 - N);
    if constexpr (k_BigEndian && N <= 64)
        return (U64)-1 << (64 - N);
    return 0;
}

} // namespace mj
