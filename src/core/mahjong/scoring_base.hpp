
#pragma once
#include "mahjong_hand.hpp"
#include "compact_helpers.hpp"
#include "core/utils/logging.hpp"
#include <bitset>
#include <algorithm>
#include <numeric>

namespace mj {
namespace scoring {
constexpr U16f k_BonusScore = 100;
constexpr U16f k_RiichiDeposit = 1000;
constexpr U16f k_BaseFu = 20;
constexpr U16f k_Mangan = 2000;
constexpr U16f k_Haneman = 3000;
constexpr U16f k_Baiman = 4000;
constexpr U16f k_Sanbaiman = 6000;
constexpr U16f k_Yakuman = 8000;

enum YakuFlags : U64 
{
    f_Riichi1 = 1ull << 8,
    f_Riichi2 = 1ull << 9,
    f_Ippatsu = 1ull << 10,
    f_MenTsumo = 1ull << 11,
    f_Pinfu = 1ull << 12,
    f_Ipeikou = 1ull << 13,
    f_Haitei = 1ull << 14,
    f_Houtei = 1ull << 15,
    f_Rinshan = 1ull << 16,
    f_Chankan = 1ull << 17,
    f_Tanyao = 1ull << 18,
    f_Hatsu = 1ull << 19,
    f_Chun = 1ull << 20,
    f_Haku = 1ull << 21,
    f_EastP = 1ull << 22,
    f_EastS = 1ull << 23,
    f_SouthP = 1ull << 24,
    f_SouthS = 1ull << 25,
    f_WestS = 1ull << 26,
    f_NorthS = 1ull << 27,
    f_ChantaO = 1ull << 28,
    f_ChantaC = 1ull << 29,
    f_SanshokuSeqO = 1ull << 30,
    f_SanshokuSeqC = 1ull << 31,
    f_SanshokuSet = 1ull << 32,
    f_IttsuO = 1ull << 33,
    f_IttsuC = 1ull << 34,
    f_Toitoi = 1ull << 35,
    f_Sanankou = 1ull << 36,
    f_Sankantsu = 1ull << 37,
    f_Chitoitsu = 1ull << 38,
    f_Honroutou = 1ull << 39,
    f_Shousangen = 1ull << 40,
    f_HonitsuO = 1ull << 41,
    f_HonitsuC = 1ull << 42,
    f_JunchanO = 1ull << 43,
    f_JunchanC = 1ull << 44,
    f_Ryanpeikou = 1ull << 45,
    f_ChinitsuO = 1ull << 46,
    f_ChinitsuC = 1ull << 47,

    f_ChuurenPoutou = 1ull << 48,
    f_SuuAnkou = 1ull << 49,
    f_SuuKantsu = 1ull << 50,
    f_Daisangen = 1ull << 51,
    f_Shosushi = 1ull << 52,
    f_Daisushi = 1ull << 53,
    f_Kokushi = 1ull << 54,
    f_Ryuisou = 1ull << 55,
    f_Tsuuiisou = 1ull << 56,
    f_Chinroutou = 1ull << 57,
    f_Tenhou = 1ull << 58,
    f_Chihou = 1ull << 59,
    f_DoubleYakuman = 1ull << 60,

    f_Renhou = 1ull << 61
};


constexpr U64 f_FuMask = 0xff;
constexpr U64 f_YakuMask = ~f_FuMask;

constexpr U64 f_Riichi          = f_Riichi1 | f_Riichi2;
constexpr U64 f_East            = f_EastP | f_EastS;
constexpr U64 f_South           = f_SouthP | f_SouthS;
constexpr U64 f_West            = f_WestS;
constexpr U64 f_North           = f_NorthS;
constexpr U64 f_Chanta          = f_ChantaO | f_ChantaC;
constexpr U64 f_SanshokuSeq     = f_SanshokuSeqO | f_SanshokuSeqC;
constexpr U64 f_Ittsu           = f_IttsuO | f_IttsuC;
constexpr U64 f_Honitsu         = f_HonitsuO | f_HonitsuC;
constexpr U64 f_Junchan         = f_JunchanO | f_JunchanC;
constexpr U64 f_Chinitsu        = f_ChinitsuO | f_ChinitsuC;

constexpr U64 f_OpenHandMask    = ~(f_Riichi | 
                                    f_Ippatsu | 
                                    f_MenTsumo | 
                                    f_Pinfu | 
                                    f_Ipeikou |
                                    f_ChantaC |
                                    f_SanshokuSeqC |
                                    f_IttsuC |
                                    f_HonitsuC |
                                    f_JunchanC |
                                    f_ChinitsuC);

constexpr U64 f_NoOpenTanyaoMask=  ~f_Tanyao & f_OpenHandMask;

constexpr U64 f_ClosedHandMask  = ~(f_ChantaO |
                                    f_SanshokuSeqO |
                                    f_IttsuO |
                                    f_HonitsuO |
                                    f_JunchanO |
                                    f_ChinitsuO);

constexpr U64 f_Yakuhai         =   f_East | f_South | f_West | f_North | 
                                    f_Hatsu | f_Chun | f_Haku;

constexpr U64 f_Yaku1Fan        =   f_Riichi |
                                    f_Ippatsu |
                                    f_MenTsumo |
                                    f_Pinfu |
                                    f_Ipeikou |
                                    f_Haitei |
                                    f_Houtei |
                                    f_Rinshan |
                                    f_Chankan |
                                    f_Tanyao |
                                    f_Yakuhai |
                                    f_ChantaO |
                                    f_SanshokuSeqO |
                                    f_IttsuO;

constexpr U64 f_Yaku2Fan        =   f_ChantaC |
                                    f_SanshokuSeqC |
                                    f_IttsuC |
                                    f_Toitoi |
                                    f_Sanankou |
                                    f_Sankantsu |
                                    f_Chitoitsu |
                                    f_Honroutou |
                                    f_Shousangen |
                                    f_HonitsuO |
                                    f_JunchanO;

constexpr U64 f_Yaku3Fan        =   f_HonitsuC |
                                    f_JunchanC;

constexpr U64 f_Yaku5Fan        =   f_ChinitsuO;
constexpr U64 f_Yaku6Fan        =   f_ChinitsuC;

constexpr U64 f_YakumanMask     =   f_ChuurenPoutou |
                                    f_SuuAnkou |
                                    f_SuuKantsu |
                                    f_Daisangen |
                                    f_Shosushi |
                                    f_Daisushi |
                                    f_Kokushi |
                                    f_Ryuisou |
                                    f_Tsuuiisou |
                                    f_Chinroutou |
                                    f_Tenhou |
                                    f_Chihou;

constexpr std::array<U64, 6> k_YakuVal 
{ f_Yaku1Fan, f_Yaku2Fan, f_Yaku3Fan, 0, f_Yaku5Fan, f_Yaku6Fan };

using Doras = s_Vector<Tile, 10>;

inline constexpr std::size_t bit_count(uint64_t v) noexcept
{
    if (std::is_constant_evaluated()) 
    {
        std::size_t n = 0;
        while (v) 
        {
            v &= v - 1; // clear right-most 1-bit
            ++n;
        }
        return n;
    }
    return std::bitset<64>(v).count();
}

inline constexpr U16f basic_score(U64 yakus, U8f doras=0)
{
    // check for yakuman
    if (yakus & f_DoubleYakuman)
        return 2 * k_Yakuman;
    if (yakus & f_YakumanMask)
        return k_Yakuman;
    U8f fu = yakus & f_FuMask;
    U8f fan = doras;
    for (U8f i = 0; i < k_YakuVal.size(); ++i)
        fan += (i+1)*bit_count(yakus & k_YakuVal[i]);
    if (fan >= 11)
        return k_Sanbaiman;
    if (fan >= 8)
        return k_Baiman;
    if (fan >= 6)
        return k_Haneman;
    if (fan == 5 || (fu >= 40 && fan == 4) || (fu >= 70 && fan == 3))
        return k_Mangan;
    
    return fu << (2 + fan);
}

/**
 * Evaluate a particular yaku. Implemented for all single 
 * 
 * @tparam yaku The yaku to evaluate
 * @param combo The scoring combo reference to aggregate the result
 * @param hand The hand to evaluate.
 * @param win The win (set of sequences and pairs) to evaluate
 * @param agari_pai The tile that caused the win.
 * @return U8f The number of fan for yaku (0 to 6)
 */
template<U64 yaku>
inline constexpr U64 eval(const Hand &hand, const Win &win, Tile agari_pai);

inline constexpr std::pair<U32f, U64> score_hand(const Hand &hand, const Win &win, Tile agari_pai)
{
    
    return {0, 0};
}

inline constexpr U32f score_hand(const Hand &hand, Tile agari_pai)
{
    auto wins = hand.agari();
    U32f best_score = 0;
    for (const auto &win : wins)
    {
        auto [score, yakus] = score_hand(hand, win, agari_pai);
        if (score > best_score)
            best_score = score;
    }
    return best_score;
}

} // namespace scoring
} // namespace mj
