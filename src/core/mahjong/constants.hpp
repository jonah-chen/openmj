
#pragma once

#if __GNUC__ >= 12
#define CONSTEXPR12 constexpr
#else
#define CONSTEXPR12 inline
#endif

#include <cstdint>

namespace mj {
using Fast8 = uint_fast8_t;
using Fast16 = uint_fast16_t;
using Fast32 = uint_fast32_t;
using Fast64 = uint_fast64_t;
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
constexpr U8 f_All8 = (U8)-1;
constexpr U16 f_All16 = (U16)-1;
constexpr U32 f_All32 = (U32)-1;
constexpr U64 f_All64 = (U64)-1;

constexpr Fast8 k_NumWinds = 4;
constexpr Fast8 k_NumPlayers = 4;

constexpr Fast8 k_Green = 0;
constexpr Fast8 k_Red = 1;
constexpr Fast8 k_White = 2;
constexpr Fast8 k_NumDragons = 3;

constexpr Fast16 k_UniqueTiles = 34;
constexpr Fast16 k_DeckSize = 136;
constexpr Fast16 k_DeadWallSize = 14;
constexpr Fast16 k_MaxHandSize = 14;
constexpr Fast16 k_MaxNumMeld = 4;
constexpr Fast16 k_MaxDiscards = 24;

constexpr int k_ModeNormal = 1;
constexpr int k_ModeChiitoi = 2;
constexpr int k_ModeKokushi = 4;
constexpr int k_ModeAll = k_ModeNormal | k_ModeChiitoi | k_ModeKokushi;

constexpr Fast16 k_BaseFu = 20;

/**
 * fedcba9876543210
 * sssnnnnppfffffff
 */
namespace tilelayout
{
    constexpr Fast8 k_PlayerPos = 7;
    constexpr Fast8 k_NumPos = 9;
    constexpr Fast8 k_SuitPos = 13;
} // namespace tilelayout

} // namespace mj