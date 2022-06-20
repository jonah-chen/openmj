/**
 * @file scoring_impl.hpp
 * @author Jonah Chen
 * @brief Implementations for scoring all the yakus in the game.
 * @version 0.1
 * @date 2022-06-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once
#include "scoring_base.hpp"

namespace mj {
namespace scoring {

template<U64 yaku>
concept HandIndependent = static_cast<bool>(yaku & 
    (f_Riichi | f_Ippatsu | f_Haitei | f_Houtei | f_Rinshan | f_Chankan | f_Tenhou | f_Chihou | f_Renhou));

template<U64 yaku>
requires HandIndependent<yaku>
inline constexpr U64 eval
(const Hand &hand, const Win &win, Tile agari_pai)
{ return yaku; }

template<>
inline constexpr U64 eval
<f_MenTsumo>
(const Hand &hand, const Win &win, Tile agari_pai)
{
    if (hand.player()==agari_pai.player() && hand.closed())
        return f_MenTsumo;
    return 0;
}

template<>
inline constexpr U64 eval
<f_Pinfu>
(const Hand &hand, const Win &win, Tile agari_pai)
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
                    agari34==_int(meld)+2 && agari_pai.num() != c_num(3))
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
        if (hand.open())
            return fu + 10;
        else if (agari_pai.player() != hand.player())
            fu += 10;
        return fu | f_Pinfu;
    }
    if (agari_pai.player() == hand.player())
        fu += 2;
    if (hand.closed())
        fu += 10;

    return 10 * ((fu + 9) / 10);
}

template<>
inline constexpr U64 eval
<f_Ipeikou>
(const Hand &hand, const Win &win, Tile agari_pai)
{
    auto win_ptr = std::get_if<NormalWin>(&win);
    if (win_ptr == nullptr)
        return 0;
    auto &[melds, pair] = *win_ptr;
    s_Vector<int, 4> dups;
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

template<>
inline constexpr U64 eval
<f_Tanyao>
(const Hand &hand, const Win &win, Tile agari_pai)
{
    for (int i : {0,8,9,17,18,26,27,28,29,30,31,32,33})
        if (hand.hand_4hot_melds(i) != 0)
            return 0;
    return f_Tanyao;
}

template<>
inline constexpr U64 eval
<f_Hatsu>
(const Hand &hand, const Win &win, Tile agari_pai)
{
    return hand.hand_4hot_melds(k_Green34) >= 3;
}

template<>
inline constexpr U64 eval
<f_Chun>
(const Hand &hand, const Win &win, Tile agari_pai)
{
    return hand.hand_4hot_melds(k_Red34) >= 3;
}

template<>
inline constexpr U64 eval
<f_Haku>
(const Hand &hand, const Win &win, Tile agari_pai)
{
    return hand.hand_4hot_melds(k_White34) >= 3;
}

template<>
inline constexpr U64 eval
<f_East>
(const Hand &hand, const Win &win, Tile agari_pai)
{
    return hand.hand_4hot_melds(k_East34) >= 3;
}

template<>
inline constexpr U64 eval
<f_South>
(const Hand &hand, const Win &win, Tile agari_pai)
{
    return hand.hand_4hot_melds(k_South34) >= 3;
}

template<>
inline constexpr U64 eval
<f_West>
(const Hand &hand, const Win &win, Tile agari_pai)
{
    return hand.hand_4hot_melds(k_West34) >= 3;
}

template<>
inline constexpr U64 eval
<f_North>
(const Hand &hand, const Win &win, Tile agari_pai)
{
    return hand.hand_4hot_melds(k_North34) >= 3;
}

template<>
inline constexpr U64 eval
<f_Chanta>
(const Hand &hand, const Win &win, Tile agari_pai)
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
    return f_Chanta;    
}

template<>
inline constexpr U64 eval
<f_SanshokuSeq>
(const Hand &hand, const Win &win, Tile agari_pai)
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

template<>
inline constexpr U64 eval
<f_SanshokuSet>
(const Hand &hand, const Win &win, Tile agari_pai)
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
    return std::find(set, set+9, 9) != set+9 ? f_SanshokuSet : 0;
}

template<>
inline constexpr U64 eval
<f_Ittsu>
(const Hand &hand, const Win &win, Tile agari_pai)
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

template<>
inline constexpr U64 eval
<f_Sanankou>
(const Hand &hand, const Win &win, Tile agari_pai)
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

template<>
inline constexpr U64 eval
<f_Sankantsu>
(const Hand &hand, const Win &win, Tile agari_pai)
{
    auto win_ptr = std::get_if<NormalWin>(&win);
    if (win_ptr == nullptr)
        return 0;
    U8f kantsu = std::count_if(hand.melds().begin(), hand.melds().end(), 
        [] (const Meld &meld)->bool { return meld.fourth(); });
    return kantsu == 3 ? f_Sankantsu : 0;
}

template<>
inline constexpr U64 eval<f_Chitoitsu>(const Hand &hand, const Win &win, Tile agari_pai)
{
    auto win_ptr = std::get_if<cPairs>(&win);
    return win_ptr ? f_Chitoitsu : 0;
}

template<>
inline constexpr U64 eval
<f_Honroutou>
(const Hand &hand, const Win &win, Tile agari_pai)
{
    for (auto i : {1,2,3,4,5,6,7,10,11,12,13,14,15,16,19,20,21,22,23,24,25})
        if (hand.hand_4hot_melds(i))
            return 0;
    return f_Honroutou;
}

template<>
inline constexpr U64 eval<f_Shousangen>(const Hand &hand, const Win &win, Tile agari_pai)
{
    auto win_ptr = std::get_if<NormalWin>(&win);
    if (win_ptr == nullptr)
        return 0;
    if (win_ptr->second < k_FirstHonorIdx + k_NumWinds)
        return 0;
    U8f gen = 1;
    for (const auto &meld : win_ptr->first)
        if (_int(meld) >= k_FirstHonorIdx + k_NumWinds && ++gen==3)
            return f_Shousangen;
    for (const auto &meld : hand.melds())
        if (meld.first().suit()==Suit::Dragon && ++gen==3)
                return f_Shousangen;
    return 0;
}

template<>
inline constexpr U64 eval
<f_Honitsu>
(const Hand &hand, const Win &win, Tile agari_pai)
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

template<>
inline constexpr U64 eval
<f_Junchan>
(const Hand &hand, const Win &win, Tile agari_pai)
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

template<> 
inline constexpr U64 eval
<f_Ryanpeikou>
(const Hand &hand, const Win &win, Tile agari_pai)
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

template<>
inline constexpr U64 eval
<f_Chinitsu>
(const Hand &hand, const Win &win, Tile agari_pai)
{
    if (!(eval<f_Honitsu>(hand, win, agari_pai) & f_Honitsu))
        return 0;
    for (int i = k_FirstHonorIdx; i < k_UniqueTiles; ++i)
        if (hand.hand_4hot_melds(i))
            return 0;
    return f_Chinitsu;
}

template<>
inline constexpr U64 eval
<f_ChuurenPoutou>
(const Hand &hand, const Win &win, Tile agari_pai)
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
            if (hand.hand_4hot(s9+n) <= counts[n])
                return 0;

        if (hand.hand_4hot(agari_pai.id34()) > counts[agari_pai.num()])
            return f_DoubleYakuman | f_ChuurenPoutou;
        return f_ChuurenPoutou;
    }
    return 0;
}

template<>
inline constexpr U64 eval
<f_SuuAnkou>
(const Hand &hand, const Win &win, Tile agari_pai)
{
    if (hand.open())
        return 0;
    auto win_ptr = std::get_if<NormalWin>(&win);
    for (const auto &meld : win_ptr->first)
        if (_is_run(meld))
            return 0;
    if (agari_pai.id34() == win_ptr->second)
        return f_DoubleYakuman | f_SuuAnkou;
    else if (agari_pai.player() == hand.player())
        return f_SuuAnkou;
    return 0;
}

template<>
inline constexpr U64 eval
<f_SuuKantsu>
(const Hand &hand, const Win &win, Tile agari_pai)
{
    if (hand.n_melds() != 4)
        return 0;
    for (const auto &meld : hand.melds())
        if (!meld.fourth())
            return 0;
    return f_SuuKantsu;
}

template<>
inline constexpr U64 eval
<f_Daisangen>
(const Hand &hand, const Win &win, Tile agari_pai)
{
    if (hand.hand_4hot_melds(k_Green34)>=3 && 
        hand.hand_4hot_melds(k_White34)>=3 && 
        hand.hand_4hot_melds(k_Red34)>=3)
        return f_Daisangen;
    return 0;
}

template<>
inline constexpr U64 eval
<f_Shosushi>
(const Hand &hand, const Win &win, Tile agari_pai)
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

template<>
inline constexpr U64 eval
<f_Daisushi>
(const Hand &hand, const Win &win, Tile agari_pai)
{
    if (hand.hand_4hot_melds(k_East34)>=3 && 
        hand.hand_4hot_melds(k_South34)>=3 && 
        hand.hand_4hot_melds(k_West34)>=3 &&
        hand.hand_4hot_melds(k_North34)>=3)
        return f_DoubleYakuman | f_Daisushi;
    return 0;
}

template<>
inline constexpr U64 eval
<f_Kokushi>
(const Hand &hand, const Win &win, Tile agari_pai)
{
    auto win_ptr = std::get_if<bool>(&win);
    if (win_ptr == nullptr || !*win_ptr)
        return 0;
    if (hand.hand_4hot(agari_pai.id34()))
        return f_DoubleYakuman | f_Kokushi;
    return f_Kokushi;
}

template<>
inline constexpr U64 eval
<f_Ryuisou>
(const Hand &hand, const Win &win, Tile agari_pai)
{
    for (auto i : {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,22,24,26,27,28,29,30,32,33})
        if (hand.hand_4hot_melds(i))
            return 0;
    return f_Ryuisou;
}

template<>
inline constexpr U64 eval
<f_Tsuuiisou>
(const Hand &hand, const Win &win, Tile agari_pai)
{
    for (auto i = 0; i < k_FirstHonorIdx; ++i)
        if (hand.hand_4hot_melds(i))
            return 0;
    return f_Tsuuiisou;
}

template<>
inline constexpr U64 eval
<f_Chinroutou>
(const Hand &hand, const Win &win, Tile agari_pai)
{
    for (auto i = k_FirstHonorIdx; i < k_UniqueTiles; ++i)
        if (hand.hand_4hot_melds(i))
            return 0;
    return (eval<f_Honroutou>(hand, win, agari_pai) & f_Honroutou) ? f_Chinroutou : 0;
}

/*******************************************************************************
 * Hybrid Scoring for optimized performance
 ******************************************************************************/
template<>
inline constexpr U64 eval
<f_Chinitsu | f_Honitsu>
(const Hand &hand, const Win &win, Tile agari_pai)
{
    if (!(eval<f_Honitsu>(hand, win, agari_pai) & f_Honitsu))
        return 0;

    for (int i = k_FirstHonorIdx; i < k_UniqueTiles; ++i)
        if (hand.hand_4hot_melds(i))
            return f_Honitsu;
    
    return f_Honitsu | f_Chinitsu;
}

template<>
inline constexpr U64 eval
<f_Junchan | f_Chanta>
(const Hand &hand, const Win &win, Tile agari_pai)
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
        if (!meld.first().is_19() || !meld.third().is_19())
            return 0;
        if (meld.first().is_honor())
            ret &= ~f_Junchan;
    }
    return ret;
}

/*******************************************************************************
 * Helper Functions
 ******************************************************************************/
template<>
inline constexpr U64 eval
<f_YakumanMask>
(const Hand &hand, const Win &win, Tile agari_pai)
{
    return f_YakumanMask;
}

} // namespace scoring
} // namespace mj
