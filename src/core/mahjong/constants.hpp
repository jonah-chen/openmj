
#pragma once

#if __GNUC__ >= 12
#define CONSTEXPR12 constexpr
#else
#define CONSTEXPR12 inline
#endif

#include <cstdint>

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
constexpr U8 f_All8 = (U8)-1;
constexpr U16 f_All16 = (U16)-1;
constexpr U32 f_All32 = (U32)-1;
constexpr U64 f_All64 = (U64)-1;

constexpr U8f k_NumWinds = 4;
constexpr U8f k_NumPlayers = 4;

constexpr U8f k_Green = 0;
constexpr U8f k_Red = 1;
constexpr U8f k_White = 2;
constexpr U8f k_NumDragons = 3;
constexpr U8f k_FirstHonorIdx = 27;

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

constexpr U16f k_BaseFu = 20;

/**
 * fedcba9876543210
 * sssnnnnppfffffff
 */
namespace tilelayout
{
    constexpr U8f k_PlayerPos = 7;
    constexpr U8f k_NumPos = 9;
    constexpr U8f k_SuitPos = 13;
} // namespace tilelayout

} // namespace mj