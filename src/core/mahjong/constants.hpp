
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

constexpr Fast8 k_East = 0;
constexpr Fast8 k_South = 1;
constexpr Fast8 k_West = 2;
constexpr Fast8 k_North = 3;
constexpr Fast8 k_NumWinds = 4;

constexpr Fast8 k_Green = 0;
constexpr Fast8 k_Red = 1;
constexpr Fast8 k_White = 2;
constexpr Fast8 k_NumDragons = 3;

constexpr Fast16 k_UniqueTiles = 34;
constexpr Fast16 k_DeckSize = 136;
constexpr Fast16 k_DeadWallSize = 14;
constexpr Fast16 k_MaxHandSize = 14;
constexpr Fast16 k_MaxNumMeld = 4;

namespace tilelayout
{
    constexpr Fast8 k_PlayerPos = 6;
    constexpr Fast8 k_NumPos = 8;
    constexpr Fast8 k_SuitPos = 12;

    constexpr U16 f_Transparent = 0x0001;
    constexpr U16 f_Red = 0x0002;
    constexpr U16 f_Open = 0x0004;
} // namespace tilelayout


constexpr Fast8 fp_Riichi = 62;
constexpr U64 f_SingleRiichi = 1ull << fp_Riichi;
constexpr U64 f_DoubleRiichi = 2ull << fp_Riichi;
constexpr U64 f_Riichi = 3ull << fp_Riichi;

constexpr Fast8 fp_Ippatsu = 61;
constexpr U64 f_Ippatsu = 1ull << fp_Ippatsu;


} // namespace mj