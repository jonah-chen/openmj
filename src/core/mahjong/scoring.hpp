
#pragma once
#include "mahjong.hpp"
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

constexpr U64 f_FuMask = 0xff;
constexpr U64 f_YakuMask = ~f_FuMask;

constexpr U64 f_Riichi1         = 0x0000000000000100; // single or double
constexpr U64 f_Riichi2         = 0x0000000000000200; // double only
constexpr U64 f_Riichi          = f_Riichi1 | f_Riichi2;
constexpr U64 f_Ippatsu         = 0x0000000000000400;
constexpr U64 f_MenTsumo        = 0x0000000000000800;
constexpr U64 f_Pinfu           = 0x0000000000001000;
constexpr U64 f_Ipeikou         = 0x0000000000002000;
constexpr U64 f_Haitei          = 0x0000000000004000;
constexpr U64 f_Houtei          = 0x0000000000008000;
constexpr U64 f_Rinshan         = 0x0000000000010000;
constexpr U64 f_Chankan         = 0x0000000000020000;
constexpr U64 f_Tanyao          = 0x0000000000040000;

constexpr U64 f_Hatsu           = 0x0000000000080000;
constexpr U64 f_Chun            = 0x0000000000100000;
constexpr U64 f_Haku            = 0x0000000000200000;
constexpr U64 f_EastP           = 0x0000000000400000;
constexpr U64 f_EastS           = 0x0000000000800000;
constexpr U64 f_East            = f_EastP | f_EastS;
constexpr U64 f_SouthP          = 0x0000000001000000;
constexpr U64 f_SouthS          = 0x0000000002000000;
constexpr U64 f_South           = f_SouthP | f_SouthS;
constexpr U64 f_WestS           = 0x0000000004000000;
constexpr U64 f_West            = f_WestS;
constexpr U64 f_NorthS          = 0x0000000008000000;
constexpr U64 f_North           = f_NorthS;

constexpr U64 f_ChantaO         = 0x0000000010000000;
constexpr U64 f_ChantaC         = 0x0000000020000000;
constexpr U64 f_Chanta          = f_ChantaO | f_ChantaC;
constexpr U64 f_SanshokuSeqO    = 0x0000000040000000;
constexpr U64 f_SanshokuSeqC    = 0x0000000080000000;
constexpr U64 f_SanshokuSeq     = f_SanshokuSeqO | f_SanshokuSeqC;
constexpr U64 f_SanshokuSet     = 0x0000000100000000;
constexpr U64 f_IttsuO          = 0x0000000200000000;
constexpr U64 f_IttsuC          = 0x0000000400000000;
constexpr U64 f_Ittsu           = 0x0000000600000000;
constexpr U64 f_Toitoi          = 0x0000000800000000;
constexpr U64 f_Sanankou        = 0x0000001000000000;
constexpr U64 f_Sankantsu       = 0x0000002000000000;
constexpr U64 f_Chitoitsu       = 0x0000004000000000;
constexpr U64 f_Honroutou       = 0x0000008000000000;
constexpr U64 f_Shousangen      = 0x0000010000000000;
constexpr U64 f_HonitsuO        = 0x0000020000000000;
constexpr U64 f_HonitsuC        = 0x0000040000000000;
constexpr U64 f_Honitsu         = f_HonitsuO | f_HonitsuC;
constexpr U64 f_JunchanO        = 0x0000080000000000;
constexpr U64 f_JunchanC        = 0x0000100000000000;
constexpr U64 f_Junchan         = f_JunchanO | f_JunchanC;
constexpr U64 f_Ryanpeikou      = 0x0000200000000000;
constexpr U64 f_ChinitsuO       = 0x0000400000000000;
constexpr U64 f_ChinitsuC       = 0x0000800000000000;
constexpr U64 f_Chinitsu        = f_ChinitsuO | f_ChinitsuC;

constexpr U64 f_ChuurenPoutou   = 0x0004000000000000;
constexpr U64 f_SuuAnkou        = 0x0008000000000000;
constexpr U64 f_SuuKantsu       = 0x0010000000000000;
constexpr U64 f_Daisangen       = 0x0020000000000000;
constexpr U64 f_Shosushi        = 0x0040000000000000;
constexpr U64 f_Daisushi        = 0x0080000000000000;
constexpr U64 f_Kokushi         = 0x0100000000000000;
constexpr U64 f_Ryuisou         = 0x0200000000000000;
constexpr U64 f_Chinroutou      = 0x0400000000000000;
constexpr U64 f_Tenhou          = 0x0800000000000000;
constexpr U64 f_Chihou          = 0x1000000000000000;
constexpr U64 f_DoubleYakuman   = 0x2000000000000000;
constexpr U64 f_YakumanMask     = 4610560118520545280ULL;

constexpr U64 f_Renhou          = 0x4000000000000000;

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
 * Evaluate a particular yaku 
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

template<>
inline constexpr U64 eval<f_Pinfu>(const Hand &hand, const Win &win, Tile agari_pai)
{
    auto win_ptr = std::get_if<NormalWin>(&win);
    if (win_ptr == nullptr)
        return 25;
    auto &[melds, pair] = *win_ptr;
    U8f agari34 = agari_pai.id34();
    U8f fu = k_BaseFu;
    U8f wait_fu = 2;
    for (const auto &meld : melds) // closed or semi-closed melds
    {
        if (_is_set(meld))
        {
            U8f triple_pts = 2;
            if (_is_19(_int(meld)))
                triple_pts <<= 1;

            // meld must be closed unless the third tile is won from ron
            if (agari34==_int(meld) && agari_pai.player()!=hand[0].player() && hand.hand_4hot_melds()[_int(meld)]!=meld)
            ;
            else
                triple_pts <<= 1;
            fu += triple_pts;            
        }
        else if ((agari34==_int(meld) && agari_pai.num() != c_num(7)) || agari34==_int(meld)+2 && agari_pai.num() != c_num(3))
            wait_fu = 0;
    }
    for (const auto &meld : hand.melds()) // open melds
    {
        if (meld.is_set())
        {
            U8f triple_pts = 2;
            if (meld.fourth())
                triple_pts <<= 2;
            if (meld.first().is_19())
                triple_pts <<= 1;
            fu += triple_pts;
        }
    }

    fu += wait_fu;
    if (fu == k_BaseFu)
    {
        if (hand.check(Hand::f_Open))
            return fu + 10;
        else if (agari_pai.player() != hand[0].player())
            fu += 10;
        return fu | f_Pinfu;
    }
    if (agari_pai.player() == hand[0].player())
        fu += 2;
    if (!hand.check(Hand::f_Open))
        fu += 10;

    return 10 * ((fu + 9) / 10);
}

template<>
inline constexpr U64 eval<f_Ipeikou>(const Hand &hand, const Win &win, Tile agari_pai)
{
    auto win_ptr = std::get_if<NormalWin>(&win);
    if (win_ptr == nullptr)
        return 0;
    auto &[melds, pair] = *win_ptr;
    for (const auto &meld : melds)
    {
        if (_is_run(meld))
        {
            
        }
    }
}



U32f score_hand(const Hand &hand, const Win &win, Tile agari_pai);

} // namespace scoring
} // namespace mj
