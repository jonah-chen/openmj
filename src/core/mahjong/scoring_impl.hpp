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
inline constexpr U64 eval(const Hand &hand, const Win &win, Tile agari_pai)
{
    return yaku;
}

template<>
inline constexpr U64 eval<f_MenTsumo>(const Hand &hand, const Win &win, Tile agari_pai)
{
    if (hand.player()==agari_pai.player() && hand.closed())
        return f_MenTsumo;
    return 0;
}

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
inline constexpr U64 eval<f_Ipeikou>(const Hand &hand, const Win &win, Tile agari_pai)
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
inline constexpr U64 eval<f_Tanyao>(const Hand &hand, const Win &win, Tile agari_pai)
{
    for (int i : {0,8,9,17,18,26,27,28,29,30,31,32,33})
        if (hand.hand_4hot_melds(i) != 0)
            return 0;
    return f_Tanyao;
}

template<>
inline constexpr U64 eval<f_Hatsu>(const Hand &hand, const Win &win, Tile agari_pai)
{
    constexpr U8f k_Hatsu34 = k_FirstHonorIdx + k_NumWinds + k_Green;
    return hand.hand_4hot_melds(k_Hatsu34) >= 3;
}

template<>
inline constexpr U64 eval<f_Chun>(const Hand &hand, const Win &win, Tile agari_pai)
{
    constexpr U8f k_Chun34 = k_FirstHonorIdx + k_NumWinds + k_Red;
    return hand.hand_4hot_melds(k_Chun34) >= 3;
}

template<>
inline constexpr U64 eval<f_Haku>(const Hand &hand, const Win &win, Tile agari_pai)
{
    constexpr U8f k_Haku34 = k_FirstHonorIdx + k_NumWinds + k_White;
    return hand.hand_4hot_melds(k_Haku34) >= 3;
}

template<>
inline constexpr U64 eval<f_East>(const Hand &hand, const Win &win, Tile agari_pai)
{
    constexpr U8f k_East34 = k_FirstHonorIdx + k_East;
    return hand.hand_4hot_melds(k_East34) >= 3;
}

template<>
inline constexpr U64 eval<f_South>(const Hand &hand, const Win &win, Tile agari_pai)
{
    constexpr U8f k_South34 = k_FirstHonorIdx + k_South;
    return hand.hand_4hot_melds(k_South34) >= 3;
}

template<>
inline constexpr U64 eval<f_West>(const Hand &hand, const Win &win, Tile agari_pai)
{
    constexpr U8f k_West34 = k_FirstHonorIdx + k_West;
    return hand.hand_4hot_melds(k_West34) >= 3;
}

template<>
inline constexpr U64 eval<f_North>(const Hand &hand, const Win &win, Tile agari_pai)
{
    constexpr U8f k_North34 = k_FirstHonorIdx + k_North;
    return hand.hand_4hot_melds(k_North34) >= 3;
}

template<>
inline constexpr U64 eval<f_Chanta>(const Hand &hand, const Win &win, Tile agari_pai)
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
inline constexpr U64 eval<f_SanshokuSeq>(const Hand &hand, const Win &win, Tile agari_pai)
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
inline constexpr U64 eval<f_SanshokuSet>(const Hand &hand, const Win &win, Tile agari_pai)
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
inline constexpr U64 eval<f_Ittsu>(const Hand &hand, const Win &win, Tile agari_pai)
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
inline constexpr U64 eval<f_Sanankou>(const Hand &hand, const Win &win, Tile agari_pai)
{
    auto win_ptr = std::get_if<NormalWin>(&win);
    if (win_ptr == nullptr)
        return 0;
    U8f ankou = 0;
    for (const auto &meld : win_ptr->first)
        if (_is_set(meld) && !(_int(meld)==agari_pai.id34() && agari_pai.player()!=hand.player() && hand.hand_4hot(_int(meld))==3))
            ankou++;
    return ankou == 3 ? f_Sanankou : 0;
}

template<>
inline constexpr U64 eval<f_Sankantsu>(const Hand &hand, const Win &win, Tile agari_pai)
{
    auto win_ptr = std::get_if<NormalWin>(&win);
    return 0;
}

} // namespace scoring
} // namespace mj
