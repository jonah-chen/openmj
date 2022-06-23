
#pragma once
#include "hand.hpp"
#include "compact_helpers.hpp"
#include "core/utils/logging.hpp"
#include <bitset>
#include <algorithm>
#include <numeric>
#include <sstream>

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


constexpr U64 k_YakuBegin = 1ull << 8;

constexpr U64 f_FuMask = 0xff;
constexpr U64 f_YakuMask = ~f_FuMask;

enum YakuFlags : U64 
{
    f_Riichi1       = k_YakuBegin,
    f_Riichi2       = k_YakuBegin << 1,
    f_Ippatsu       = k_YakuBegin << 2,
    f_MenTsumo      = k_YakuBegin << 3,
    f_Pinfu         = k_YakuBegin << 4,
    f_Ipeikou       = k_YakuBegin << 5,
    f_Haitei        = k_YakuBegin << 6,
    f_Houtei        = k_YakuBegin << 7,
    f_Rinshan       = k_YakuBegin << 8,
    f_Chankan       = k_YakuBegin << 9,
    f_Tanyao        = k_YakuBegin << 10,
    f_Hatsu         = k_YakuBegin << 11,
    f_Chun          = k_YakuBegin << 12,
    f_Haku          = k_YakuBegin << 13,
    f_EastP         = k_YakuBegin << 14,
    f_EastS         = k_YakuBegin << 15,
    f_SouthP        = k_YakuBegin << 16,
    f_SouthS        = k_YakuBegin << 17,
    f_WestS         = k_YakuBegin << 18,
    f_NorthS        = k_YakuBegin << 19,
    f_ChantaO       = k_YakuBegin << 20,
    f_ChantaC       = k_YakuBegin << 21,
    f_SanshokuSeqO  = k_YakuBegin << 22,
    f_SanshokuSeqC  = k_YakuBegin << 23,
    f_SanshokuSet   = k_YakuBegin << 24,
    f_IttsuO        = k_YakuBegin << 25,
    f_IttsuC        = k_YakuBegin << 26,
    f_Toitoi        = k_YakuBegin << 27,
    f_Sanankou      = k_YakuBegin << 28,
    f_Sankantsu     = k_YakuBegin << 29,
    f_Chitoitsu     = k_YakuBegin << 30,
    f_Honroutou     = k_YakuBegin << 31,
    f_Shousangen    = k_YakuBegin << 32,
    f_HonitsuO      = k_YakuBegin << 33,
    f_HonitsuC      = k_YakuBegin << 34,
    f_JunchanO      = k_YakuBegin << 35,
    f_JunchanC      = k_YakuBegin << 36,
    f_Ryanpeikou    = k_YakuBegin << 37,
    f_ChinitsuO     = k_YakuBegin << 38,
    f_ChinitsuC     = k_YakuBegin << 39,

    f_ChuurenPoutou = k_YakuBegin << 40,
    f_SuuAnkou      = k_YakuBegin << 41,
    f_SuuKantsu     = k_YakuBegin << 42,
    f_Daisangen     = k_YakuBegin << 43,
    f_Shosushi      = k_YakuBegin << 44,
    f_Daisushi      = k_YakuBegin << 45,
    f_Kokushi       = k_YakuBegin << 46,
    f_Ryuisou       = k_YakuBegin << 47,
    f_Tsuuiisou     = k_YakuBegin << 48,
    f_Chinroutou    = k_YakuBegin << 49,
    f_Tenhou        = k_YakuBegin << 50,
    f_Chihou        = k_YakuBegin << 51,
    f_DoubleYakuman = k_YakuBegin << 52,

    f_Renhou        = k_YakuBegin << 53,
    k_YakuEnd       = k_YakuBegin << 54
};

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

constexpr U64 f_HandIndependent =   f_Riichi |
                                    f_Ippatsu |
                                    f_Haitei |
                                    f_Houtei |
                                    f_Rinshan |
                                    f_Chankan |
                                    f_Tenhou |
                                    f_Chihou |
                                    f_Renhou;

constexpr U64 f_NormalPlay      = ~(f_Riichi |
                                    f_Ippatsu |
                                    f_Haitei | 
                                    f_Houtei |
                                    f_Rinshan |
                                    f_Chankan |
                                    f_Tenhou |
                                    f_Chihou |
                                    f_Renhou);

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
                                    f_SanshokuSet |
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
                                    f_JunchanC |
                                    f_Ryanpeikou;

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

using Doras = vector<Tile, 10>;

namespace _impl {
constexpr std::size_t bit_count(uint64_t v) noexcept
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

constexpr U16f basic_score(U64 yakus, U8f doras=0)
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

constexpr void filter_redundant_yaku(U64 &yakus)
{
    if (yakus & f_Chinitsu)
        yakus &= ~f_Honitsu;
    if (yakus & f_Junchan)
        yakus &= ~f_Chanta;
    if (yakus & f_Ryanpeikou)
        yakus &= ~f_Ipeikou;
}

/**
 * Evaluate a particular yaku. Implemented for all single 
 * 
 * @param hand The hand to evaluate.
 * @param win The win (set of sequences and pairs) to evaluate
 * @param agari_pai The tile that caused the win.
 * @return U8f The number of fan for yaku (0 to 6)
 */

constexpr U64 menTsumo(const Hand &hand, Tile agari_pai)
{
    if (hand.player()==agari_pai.player() && hand.closed())
        return f_MenTsumo;
    return 0;
}

constexpr U64 pinfu(const Hand &hand, const Win &win, Tile agari_pai)
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
            if (!(agari34==_int(meld) && 
                agari_pai.player()!=hand.player() && 
                hand.hand_4hot_melds(_int(meld))!=meld))
                triple_pts <<= 1;
            fu += triple_pts;            
        }
        else if ((agari34==_int(meld) && agari_pai.num() != c_num(7)) || 
                    (agari34==_int(meld)+2 && agari_pai.num() != c_num(3)))
            wait_fu = 0;
    }
    for (const auto &meld : hand.melds()) // open melds
    {
        if (meld.is_set())
        {
            U8f triple_pts = 2;
            if (meld.fourth())
            {
                triple_pts <<= 2;
                if (meld.closed_kong())
                    triple_pts <<= 1;
            }
            if (meld.first().is_19())
                triple_pts <<= 1;
            fu += triple_pts;
        }
    }
    fu += wait_fu;
    if (fu == k_BaseFu)
    {
        if (hand.open())
            return k_BaseFu + 10;
        else if (agari_pai.player() != hand.player())
            fu += 10;
        return fu | f_Pinfu;
    }
    if (agari_pai.player() == hand.player())
        fu += 2;
    else if (hand.closed())
        fu += 10;

    return 10 * ((fu + 9) / 10);
}

constexpr U64 ipeikou(const Win &win)
{
    auto win_ptr = std::get_if<NormalWin>(&win);
    if (win_ptr == nullptr)
        return 0;
    auto &[melds, pair] = *win_ptr;
    vector<int, 4> dups;
    for (const auto &meld : melds)
    {
        if (_is_run(meld))
        {
            if (std::find(dups.begin(), dups.end(), _int(meld)) != dups.end())
                return f_Ipeikou;
            dups.push_back(_int(meld));
        }
    }
    return 0;
}

constexpr U64 tanyao(const Hand &hand)
{
    for (int i : {0,8,9,17,18,26,27,28,29,30,31,32,33})
        if (hand.hand_4hot_melds(i) != 0)
            return 0;
    return f_Tanyao;
}

constexpr U64 yakuhai(const Hand &hand, U8f bonus_tile, U64 flag)
{
    return hand.hand_4hot_melds(bonus_tile) >= 3 ? flag : 0;
}

constexpr U64 chanta(const Hand &hand, const Win &win)
{
    // parse win
    auto win_ptr = std::get_if<NormalWin>(&win);
    if (win_ptr == nullptr)
        return 0;
    auto &[melds, pair] = *win_ptr;
    if (!_is_19(pair))
        return 0;
    for (const auto &meld : melds)
    {
        if (_is_set(meld))
        {
            if (!_is_19(_int(meld)))
                return 0;
        }
        else if (!_is_17(_int(meld)))
            return 0;
    }
    for (const auto &meld : hand.melds())
        if (!(meld.first().is_19() || meld.third().is_19()))
            return 0;
    return f_Chanta;
}

constexpr U64 sanshokuSeq(const Hand &hand, const Win &win)
{
    // parse win
    auto win_ptr = std::get_if<NormalWin>(&win);
    U8f seq[7] {{}};
    for (const auto &meld : hand.melds())
    {
        if (meld.is_set() || meld.first().is_honor())
            continue;
        seq[meld.first().num()] |= 1 << static_cast<U8f>(meld.first().suit());
    }
    for (const auto &meld : win_ptr->first)
    {
        if (_is_set(meld) || _is_honor(_int(meld)))
            continue;
        seq[_int(meld)%9] |= 1 << static_cast<U8f>(_int(meld)/9);
    }
    return std::find(seq, seq+7, 7) != seq+7 ? f_SanshokuSeq : 0;
}

constexpr U64 sanshokuSet(const Hand &hand, const Win &win)
{
    auto win_ptr = std::get_if<NormalWin>(&win);
    U8f set[9] {{}};
    for (const auto &meld : hand.melds())
    {
        if (!meld.is_set() || meld.first().is_honor())
            continue;
        set[meld.first().num()] |= 1 << static_cast<U8f>(meld.first().suit());
    }
    for (const auto &meld : win_ptr->first)
    {
        if (!_is_set(meld) || _is_honor(_int(meld)))
            continue;
        set[_int(meld)%9] |= 1 << static_cast<U8f>(_int(meld)/9);
    }
    return std::find(set, set+9, 7) != set+9 ? f_SanshokuSet : 0;
}

constexpr U64 ittsu(const Hand &hand, const Win &win)
{
    auto win_ptr = std::get_if<NormalWin>(&win);
    if (win_ptr == nullptr)
        return 0;

    U8f straights[3] {{}};
    for (const auto &meld : hand.melds())
    {
        if (meld.is_set() || meld.first().is_honor())
            continue;
        switch (meld.first().num())
        {
        case 0:
            straights[static_cast<U8f>(meld.first().suit())] |= 1 << 0;
            break;
        case 3:
            straights[static_cast<U8f>(meld.first().suit())] |= 1 << 1;
            break;
        case 6:
            straights[static_cast<U8f>(meld.first().suit())] |= 1 << 2;
            break;
        default:
            break;
        }
    }
    for (const auto &meld : win_ptr->first)
    {
        if (_is_set(meld) || _is_honor(_int(meld)))
            continue;
        switch (_int(meld)%9)
        {
        case 0:
            straights[static_cast<U8f>(_int(meld)/9)] |= 1 << 0;
            break;
        case 3:
            straights[static_cast<U8f>(_int(meld)/9)] |= 1 << 1;
            break;
        case 6:
            straights[static_cast<U8f>(_int(meld)/9)] |= 1 << 2;
            break;
        default:
            break;
        }
    }
    return std::find(straights, straights+3, 7) != straights+3 ? f_Ittsu : 0;
}

constexpr U64 toitoi(const Hand &hand, const Win &win)
{
    auto win_ptr = std::get_if<NormalWin>(&win);
    if (win_ptr == nullptr)
        return 0;
    auto &[melds, pair] = *win_ptr;
    for (const auto &meld : melds)
        if (_is_run(meld))
            return 0;
    for (const auto &meld : hand.melds())
        if (!meld.is_set())
            return 0;
    return f_Toitoi;
}

constexpr U64 sanankou(const Hand &hand, const Win &win, Tile agari_pai)
{
    auto win_ptr = std::get_if<NormalWin>(&win);
    if (win_ptr == nullptr)
        return 0;
    U8f ankou = hand.n_closed_kongs();
    for (const auto &meld : win_ptr->first)
        if (_is_set(meld) && !(_int(meld)==agari_pai.id34() && agari_pai.player()!=hand.player() && hand.hand_4hot(_int(meld))==3))
            ankou++;
    return ankou == 3 ? f_Sanankou : 0;
}

constexpr U64 sankantsu(const Hand &hand)
{
    U8f kantsu = std::count_if(hand.melds().begin(), hand.melds().end(), 
        [] (const Meld &meld)->bool { return meld.fourth(); });
    return kantsu == 3 ? f_Sankantsu : 0;
}

constexpr U64 chitoitsu(const Win &win)
{
    return std::get_if<cPairs>(&win) ? (25 | f_Chitoitsu) : 0;
}

constexpr U64 honroutou(const Hand &hand)
{
    for (auto i : {1,2,3,4,5,6,7,10,11,12,13,14,15,16,19,20,21,22,23,24,25})
        if (hand.hand_4hot_melds(i))
            return 0;
    return f_Honroutou;
}

constexpr U64 shousangen(const Hand &hand, const Win &win)
{
    auto win_ptr = std::get_if<NormalWin>(&win);
    if (win_ptr == nullptr || win_ptr->second < k_FirstHonorIdx + k_NumWinds)
        return 0;
    return  hand.hand_4hot_melds(k_Green34) + 
            hand.hand_4hot_melds(k_Red34) + 
            hand.hand_4hot_melds(k_White34) >= 8 ? f_Shousangen : 0;
}

constexpr U64 honitsu(const Hand &hand)
{
    U8f tsu = 0;
    for (Suit s = Suit::Man; s < Suit::Wind; ++s)
    {
        U8f s9 = 9*static_cast<U8f>(s);
        for (U8f n = s9; n < s9+8; ++n)
        {
            if (hand.hand_4hot_melds(n))
            {
                if (++tsu != 1)
                    return 0;
                break;
            }
        }
    }
    return f_Honitsu;
}

constexpr U64 junchan(const Hand &hand, const Win &win)
{
    auto win_ptr = std::get_if<NormalWin>(&win);
    if (win_ptr == nullptr)
        return 0;
    if (!_is_19(win_ptr->second) || _is_honor(win_ptr->second))
        return 0;
    for (const auto &meld : win_ptr->first)
    {
        if (_is_set(meld))
        {
            if (!_is_19(_int(meld)) || _is_honor(_int(meld)))
                return 0;
        }
        else if (!_is_17(_int(meld)))
            return 0;
    }
    for (const auto &meld : hand.melds())
        if (!((meld.first().is_19() && !meld.first().is_honor()) || 
                (meld.third().is_19() && !meld.third().is_honor())))
            return 0;
    return f_Junchan;
}

constexpr U64 ryanpeikou(const Win &win)
{
    auto win_ptr = std::get_if<NormalWin>(&win);
    if (win_ptr == nullptr || win_ptr->first.size()!=4)
        return 0;
    for (const auto &meld : win_ptr->first)
        if (_is_set(meld))
            return 0;
    return  (win_ptr->first[0]==win_ptr->first[1] && win_ptr->first[2]==win_ptr->first[3]) ||
            (win_ptr->first[0]==win_ptr->first[2] && win_ptr->first[1]==win_ptr->first[3]) ||
            (win_ptr->first[0]==win_ptr->first[3] && win_ptr->first[1]==win_ptr->first[2]) 
            ? f_Ryanpeikou : 0;
}

constexpr U64 chinitsu(const Hand &hand)
{
    if (!(honitsu(hand) & f_Honitsu))
        return 0;
    for (U8f i = k_FirstHonorIdx; i < k_UniqueTiles; ++i)
        if (hand.hand_4hot_melds(i))
            return 0;
    return f_Chinitsu;
}

constexpr U64 chuurenPoutou(const Hand &hand, Tile agari_pai)
{
    constexpr U8f counts[9] = {3,1,1,1,1,1,1,1,3};
    if (hand.open())
        return 0;
    for (Suit s = Suit::Man; s < Suit::Wind; ++s)
    {
        U8f s9 = 9*static_cast<U8f>(s);
        if (hand.hand_4hot(s9))
        {
            if (hand.hand_4hot(s9) < counts[0])
                return 0;
        }
        else continue;

        for (U8f n = 1; n < 9; ++n)
            if (hand.hand_4hot(s9+n) < counts[n])
                return 0;

        if (hand.hand_4hot(agari_pai.id34()) > counts[agari_pai.num()])
            return f_DoubleYakuman | f_ChuurenPoutou;
        return f_ChuurenPoutou;
    }
    return 0;
}

constexpr U64 suuAnkou(const Hand &hand, const Win &win, Tile agari_pai)
{
    if (hand.open())
        return 0;
    auto win_ptr = std::get_if<NormalWin>(&win);
    if (win_ptr == nullptr)
        return 0;
    for (const auto &meld : win_ptr->first)
        if (_is_run(meld))
            return 0;
    if (agari_pai.id34() == win_ptr->second)
        return f_DoubleYakuman | f_SuuAnkou;
    else if (agari_pai.player() == hand.player())
        return f_SuuAnkou;
    return 0;
}

constexpr U64 suuKantsu(const Hand &hand)
{
    if (hand.n_melds() != 4)
        return 0;
    for (const auto &meld : hand.melds())
        if (!meld.fourth())
            return 0;
    return f_SuuKantsu;
}

constexpr U64 daisangen(const Hand &hand)
{
    if (hand.hand_4hot_melds(k_Green34)>=3 && 
        hand.hand_4hot_melds(k_White34)>=3 && 
        hand.hand_4hot_melds(k_Red34)>=3)
        return f_Daisangen;
    return 0;
}

constexpr U64 shosushi(const Hand &hand, const Win &win)
{
    auto win_ptr = std::get_if<NormalWin>(&win);
    if (win_ptr == nullptr || win_ptr->second < k_FirstHonorIdx ||
        win_ptr->second >= k_FirstHonorIdx + k_NumWinds)
        return 0;   
    U8f shi = 1;
    for (auto i : {k_East34, k_South34, k_West34, k_North34})
        if (hand.hand_4hot_melds(i)>=3)
            shi++;
    return shi == 4 ? f_Shosushi : 0;
}

constexpr U64 daisushi(const Hand &hand)
{
    if (hand.hand_4hot_melds(k_East34)>=3 && 
        hand.hand_4hot_melds(k_South34)>=3 && 
        hand.hand_4hot_melds(k_West34)>=3 &&
        hand.hand_4hot_melds(k_North34)>=3)
        return f_DoubleYakuman | f_Daisushi;
    return 0;
}

constexpr U64 kokushi(const Hand &hand, const Win &win, Tile agari_pai)
{
    auto win_ptr = std::get_if<bool>(&win);
    if (win_ptr == nullptr || !*win_ptr)
        return 0;
    if (hand.hand_4hot(agari_pai.id34()) > 1)
        return f_DoubleYakuman | f_Kokushi;
    return f_Kokushi;
}

constexpr U64 ryuisou(const Hand &hand)
{
    for (auto i : {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,22,24,26,27,28,29,30,32,33})
        if (hand.hand_4hot_melds(i))
            return 0;
    return f_Ryuisou;
}

constexpr U64 tsuuiisou(const Hand &hand)
{
    for (auto i = 0; i < k_FirstHonorIdx; ++i)
        if (hand.hand_4hot_melds(i))
            return 0;
    return f_Tsuuiisou;
}

constexpr U64 chinroutou(const Hand &hand)
{
    for (auto i = k_FirstHonorIdx; i < k_UniqueTiles; ++i)
        if (hand.hand_4hot_melds(i))
            return 0;
    return (honroutou(hand) & f_Honroutou) ? f_Chinroutou : 0;
}

/*******************************************************************************
 * Hybrid Scoring for optimized performance
 ******************************************************************************/
constexpr U64 chinitsu_honitsu(const Hand &hand)
{
    if (!(honitsu(hand) & f_Honitsu))
        return 0;

    for (U8f i = k_FirstHonorIdx; i < k_UniqueTiles; ++i)
        if (hand.hand_4hot_melds(i))
            return f_Honitsu;
    
    return f_Honitsu | f_Chinitsu;
}

constexpr U64 junchan_chanta(const Hand &hand, const Win &win)
{
    auto win_ptr = std::get_if<NormalWin>(&win);
    if (win_ptr == nullptr)
        return 0;
    
    U64 ret = f_Chanta | f_Junchan;
    if (!_is_19(win_ptr->second))
        return 0;
    if (_is_honor(win_ptr->second))
        ret &= ~f_Junchan;
    for (const auto &meld : win_ptr->first)
    {
        if (_is_set(meld))
        {
            if (!_is_19(_int(meld)))
                return 0;
            if (_is_honor(_int(meld)))
                ret &= ~f_Junchan;
        }
        else if (!_is_17(_int(meld)))
            return 0;
    }
    for (const auto &meld : hand.melds())
    {
        if (!(meld.first().is_19() || meld.third().is_19()))
            return 0;
        if (meld.first().is_honor())
            ret &= ~f_Junchan;
    }
    return ret;
}

} // namespace _impl

template<U64 yaku>
constexpr U64 eval(const Hand &hand, const Win &win, Tile agari_pai)
{
    U64 ret = 0;
    if constexpr (yaku & f_HandIndependent)
        ret |= (yaku & f_HandIndependent);

    if constexpr (yaku & f_East)
        ret |= (_impl::yakuhai(hand, k_East34, f_East) & yaku);
    if constexpr (yaku & f_South)
        ret |= (_impl::yakuhai(hand, k_South34, f_West) & yaku);
    if constexpr (yaku & f_West)
        ret |= (_impl::yakuhai(hand, k_West34, f_West) & yaku);
    if constexpr (yaku & f_North)
        ret |= (_impl::yakuhai(hand, k_North34, f_North) & yaku);
    if constexpr (yaku & f_Hatsu)
        ret |= _impl::yakuhai(hand, k_Green34, f_Hatsu);
    if constexpr (yaku & f_Chun)
        ret |= _impl::yakuhai(hand, k_Red34, f_Chun);
    if constexpr (yaku & f_Haku)
        ret |= _impl::yakuhai(hand, k_White34, f_Haku);
    
    if constexpr (yaku & f_Chinitsu && yaku & f_Honitsu)
        ret |= (_impl::chinitsu_honitsu(hand) & yaku);
    else if constexpr (yaku & f_Chinitsu)
        ret |= (_impl::chinitsu(hand) & yaku);
    else if constexpr (yaku & f_Honitsu)
        ret |= (_impl::honitsu(hand) & yaku);
    if constexpr (yaku & f_Chanta && yaku & f_Junchan)
        ret |= (_impl::junchan_chanta(hand, win) & yaku);
    else if constexpr (yaku & f_Chanta)
        ret |= (_impl::chanta(hand, win) & yaku);
    else if constexpr (yaku & f_Junchan)
        ret |= (_impl::junchan(hand, win) & yaku);

    if constexpr (yaku & f_MenTsumo)
        ret |= _impl::menTsumo(hand, agari_pai);
    if constexpr (yaku & f_Pinfu)
        ret |= _impl::pinfu(hand, win, agari_pai);
    if constexpr (yaku & f_Tanyao)
        ret |= _impl::tanyao(hand);
    if constexpr (yaku & f_Ipeikou)
        ret |= _impl::ipeikou(win);
    if constexpr (yaku & f_Ryanpeikou)
        ret |= _impl::ryanpeikou(win);
    if constexpr (yaku & f_Chitoitsu)
        ret |= _impl::chitoitsu(win);
    if constexpr (yaku & f_Honroutou)
        ret |= _impl::honroutou(hand);
    if constexpr (yaku & f_SanshokuSeq)
        ret |= (_impl::sanshokuSeq(hand, win) & yaku);
    if constexpr (yaku & f_SanshokuSet)
        ret |= (_impl::sanshokuSet(hand, win) & yaku);
    if constexpr (yaku & f_Sanankou)
        ret |= _impl::sanankou(hand, win, agari_pai);
    if constexpr (yaku & f_Sankantsu)
        ret |= _impl::sankantsu(hand);
    if constexpr (yaku & f_Ittsu)
        ret |= (_impl::ittsu(hand, win) & yaku);
    if constexpr (yaku & f_Toitoi)
        ret |= _impl::toitoi(hand, win);
    if constexpr (yaku & f_Shousangen)
        ret |= _impl::shousangen(hand, win);
    
    if constexpr (yaku & f_ChuurenPoutou)
        ret |= _impl::chuurenPoutou(hand, agari_pai);
    if constexpr (yaku & f_SuuAnkou)
        ret |= _impl::suuAnkou(hand, win, agari_pai);
    if constexpr (yaku & f_SuuKantsu)
        ret |= _impl::suuKantsu(hand);
    if constexpr (yaku & f_Daisangen)
        ret |= _impl::daisangen(hand);
    if constexpr (yaku & f_Shosushi)
        ret |= _impl::shosushi(hand, win);
    if constexpr (yaku & f_Daisushi)
        ret |= _impl::daisushi(hand);
    if constexpr (yaku & f_Kokushi)
        ret |= _impl::kokushi(hand, win, agari_pai);
    if constexpr (yaku & f_Ryuisou)
        ret |= _impl::ryuisou(hand);
    if constexpr (yaku & f_Tsuuiisou)
        ret |= _impl::tsuuiisou(hand);
    if constexpr (yaku & f_Chinroutou)
        ret |= _impl::chinroutou(hand);

    return ret;    
}

constexpr std::pair<U32f, U64> score_hand(const Hand &hand, const Win &win, Tile agari_pai, U8f doras=0)
{
    U64 yakus = eval<f_YakumanMask>(hand, win, agari_pai);
    if (hand.flags & yakus)
    {
        return {_impl::basic_score(hand.flags & yakus), hand.flags & yakus};
    }
    yakus |= eval<f_HandIndependent>(hand, win, agari_pai);
    yakus |= eval<f_MenTsumo>(hand, win, agari_pai);

    { // evaluate chiitoi
        auto chitoi = eval<f_Chitoitsu>(hand, win, agari_pai);
        if (chitoi)
        {
            yakus |= 25 | f_Chitoitsu;
            yakus |= eval<f_Chinitsu | f_Honitsu>(hand, win, agari_pai);
            if (!(yakus & f_Chinitsu) && eval<f_Honroutou>(hand, win, agari_pai))
                yakus |= f_Honroutou;
            else 
                yakus |= eval<f_Tanyao>(hand, win, agari_pai);
            _impl::filter_redundant_yaku(yakus);
            return {_impl::basic_score(hand.flags & yakus, doras), hand.flags & yakus};
        }
    }

    yakus |= eval<f_Pinfu>(hand, win, agari_pai);
    {
        auto yakuhai = eval<f_Yakuhai>(hand, win, agari_pai);
        yakus |= yakuhai;
        if (!yakuhai)
        {
            yakus |= eval<f_Tanyao>(hand, win, agari_pai);
            yakus |= eval<f_Ryanpeikou>(hand, win, agari_pai);
        }
        else
        {
            yakus |= eval<f_Shousangen>(hand, win, agari_pai);
        }
    }
    {
        auto itsu = eval<f_Chinitsu | f_Honitsu>(hand, win, agari_pai);
        yakus |= itsu;
        if (!itsu)
        {
            yakus |= eval<f_SanshokuSeq>(hand, win, agari_pai);
            yakus |= eval<f_SanshokuSet>(hand, win, agari_pai);
        }
    }
    {
        auto honrotou = eval<f_Honroutou>(hand, win, agari_pai);
        yakus |= honrotou;
        if (!honrotou)
        {
            auto y19 = eval<f_Junchan | f_Chanta>(hand, win, agari_pai);
            if (y19)
                yakus |= y19;
            else
                yakus |= eval<f_Ittsu>(hand, win, agari_pai);
        }
    }

    yakus |= eval<f_Ipeikou>(hand, win, agari_pai);
    if (!(yakus & f_Pinfu))
    { 
        yakus |= eval<f_Sanankou>(hand, win, agari_pai);
        yakus |= eval<f_Sankantsu>(hand, win, agari_pai);
        yakus |= eval<f_Toitoi>(hand, win, agari_pai);
    }
    _impl::filter_redundant_yaku(yakus);
    yakus &= hand.flags;
    if (yakus & f_YakuMask) return {_impl::basic_score(yakus, doras), yakus};
    else return {0, yakus};
}

inline std::pair<U32f, U64> score_hand(const Hand &hand, Tile agari_pai)
{
    auto wins = hand.agari();
    U32f best_score = 0;
    U64 best_yaku = 0;
    for (const auto &win : wins)
    {
        auto [score, yakus] = score_hand(hand, win, agari_pai);
        if (score > best_score)
        {
            best_score = score;
            best_yaku = yakus;
        }
    }
    return {best_score, best_yaku};
}

inline std::string yaku_str(U64 yaku)
{
    std::stringstream ss;
    ss << (yaku & f_FuMask) << " fu\n";
    if (yaku & f_Riichi1)
        ss << "Riichi\n";
    if (yaku & f_Riichi2)
        ss << "Riichi\n";
    if (yaku & f_Ippatsu)
        ss << "Ippatsu\n";
    if (yaku & f_MenTsumo)
        ss << "MenTsumo\n";
    if (yaku & f_Pinfu)
        ss << "Pinfu\n";
    if (yaku & f_Ipeikou)
        ss << "Ipeikou\n";
    if (yaku & f_Haitei)
        ss << "Haitei\n";
    if (yaku & f_Houtei)
        ss << "Houtei\n";
    if (yaku & f_Rinshan)
        ss << "Rinshan\n";
    if (yaku & f_Chankan)
        ss << "Chankan\n";
    if (yaku & f_Tanyao)
        ss << "Tanyao\n";
    if (yaku & f_Hatsu)
        ss << "Hatsu\n";
    if (yaku & f_Chun)
        ss << "Chun\n";
    if (yaku & f_Haku)
        ss << "Haku\n";
    if (yaku & f_EastP)
        ss << "EastP\n";
    if (yaku & f_EastS)
        ss << "EastS\n";
    if (yaku & f_SouthP)
        ss << "SouthP\n";
    if (yaku & f_SouthS)
        ss << "SouthS\n";
    if (yaku & f_WestS)
        ss << "WestS\n";
    if (yaku & f_NorthS)
        ss << "NorthS\n";
    if (yaku & f_ChantaO)
        ss << "ChantaO\n";
    if (yaku & f_ChantaC)
        ss << "ChantaC\n";
    if (yaku & f_SanshokuSeqO)
        ss << "SanshokuSeqO\n";
    if (yaku & f_SanshokuSeqC)
        ss << "SanshokuSeqC\n";
    if (yaku & f_SanshokuSet)
        ss << "SanshokuSet\n";
    if (yaku & f_IttsuO)
        ss << "IttsuO\n";
    if (yaku & f_IttsuC)
        ss << "IttsuC\n";
    if (yaku & f_Toitoi)
        ss << "Toitoi\n";
    if (yaku & f_Sanankou)
        ss << "Sanankou\n";
    if (yaku & f_Sankantsu)
        ss << "Sankantsu\n";
    if (yaku & f_Chitoitsu)
        ss << "Chitoitsu\n";
    if (yaku & f_Honroutou)
        ss << "Honroutou\n";
    if (yaku & f_Shousangen)
        ss << "Shousangen\n";
    if (yaku & f_HonitsuO)
        ss << "HonitsuO\n";
    if (yaku & f_HonitsuC)
        ss << "HonitsuC\n";
    if (yaku & f_JunchanO)
        ss << "JunchanO\n";
    if (yaku & f_JunchanC)
        ss << "JunchanC\n";
    if (yaku & f_Ryanpeikou)
        ss << "Ryanpeikou\n";
    if (yaku & f_ChinitsuO)
        ss << "ChinitsuO\n";
    if (yaku & f_ChinitsuC)
        ss << "ChinitsuC\n";
    if (yaku & f_ChuurenPoutou)
        ss << "ChuurenPoutou\n";
    if (yaku & f_SuuAnkou)
        ss << "SuuAnkou\n";
    if (yaku & f_SuuKantsu)
        ss << "SuuKantsu\n";
    if (yaku & f_Daisangen)
        ss << "Daisangen\n";
    if (yaku & f_Shosushi)
        ss << "Shosushi\n";
    if (yaku & f_Daisushi)
        ss << "Daisushi\n";
    if (yaku & f_Kokushi)
        ss << "Kokushi\n";
    if (yaku & f_Ryuisou)
        ss << "Ryuisou\n";
    if (yaku & f_Tsuuiisou)
        ss << "Tsuuiisou\n";
    if (yaku & f_Chinroutou)
        ss << "Chinroutou\n";
    if (yaku & f_Tenhou)
        ss << "Tenhou\n";
    if (yaku & f_Chihou)
        ss << "Chihou\n";
    if (yaku & f_DoubleYakuman)
        ss << "DoubleYakuman\n";
    if (yaku & f_Renhou)
        ss << "Renhou\n";
    return ss.str();
}

} // namespace scoring
} // namespace mj
