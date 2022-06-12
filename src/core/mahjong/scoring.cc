
#include "scoring.hpp"
#include "core/utils/logging.hpp"
#include <algorithm>
#include <numeric>

namespace mj {
namespace scoring {
template<>
Fast8 eval<Yaku::invalid>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
{
    MJ_ALWAYS_THROW(1, std::logic_error, "Invalid yaku");
}

template<>
Fast8 eval<Yaku::riichi>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
{
    MJ_ASSERT(combo[Yaku::riichi] == 0, "Cannot evaluate a riichi twice.");
    return combo[Yaku::riichi] = hand.check(Hand::f_Riichi, Hand::fp_Riichi);
}

template<>
Fast8 eval<Yaku::ippatsu>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
{
    MJ_ASSERT(combo[Yaku::ippatsu] == 0, "Cannot evaluate an ippatsu twice.");
    return combo[Yaku::ippatsu] = (bool)hand.check(Hand::f_Ippatsu);
}

template<>
Fast8 eval<Yaku::men_tsumo>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
{
    MJ_ASSERT(combo[Yaku::men_tsumo] == 0, "Cannot evaluate men_tsumo twice.");
    MJ_ASSERT(hand.size(), "Empty Hand is not allowed");
    return combo[Yaku::men_tsumo] == hand.check(Hand::f_Open) && agari_pai.player() == hand[0].player();
}

// template<>
// Fast8 eval<Yaku::pinfu>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     MJ_ASSERT(combo[Yaku::pinfu] == 0, "Cannot evaluate a pinfu twice.");
//     auto win_ptr = std::get_if<NormalWin>(&win);
//     auto &[melds, pair] = *win_ptr;

//     if (combo.fu)
//         return combo[Yaku::pinfu] = (combo.fu == k_BaseFu) || 
//         (agari_pai.player()!=hand[0].player() && combo.fu == k_BaseFu + 10 && !hand.check(Hand::f_Open));
    
//     Fast16 &fu = combo.fu; 
//     fu = k_BaseFu;

//     Fast16 wait_fu = 2;

//     for (const auto &meld : melds)
//     {
//         if (_is_set(meld))
//         {
//             Fast16 triple_pts = 2;
//             if (meld.fourth())
//                 triple_pts <<= 2;
            
//             if (meld.first().is_19())
//                 triple_pts <<= 1;
//             if (meld.second() == meld.called())
//                 triple_pts <<= 1;
//             fu += triple_pts;
//         }
//         else if ((meld.first() == agari_pai && agari_pai.num() != c_num(7)) || (meld.third() == agari_pai && agari_pai.num() != c_num(3)))
//             wait_fu = 0;
//     }

//     fu += wait_fu;
    
//     if (win.pair.first().suit() == Suit::Dragon || // dragon
//         win.pair.first().eq7({Suit::Wind, hand[0].player()}) || // seat wind
//         win.pair.first().eq7({Suit::Wind, combo.round})) // prevailing wind
//         fu += 2;
    
//     if (fu == k_BaseFu)
//     {
//         if (hand.check(Hand::f_Open))
//         {
//             fu += 10;
//             return combo[Yaku::pinfu] = 0;
//         }
//         if (agari_pai.player() != hand[0].player())
//             fu += 10;
//         return combo[Yaku::pinfu] = 1;
//     }
//     if (agari_pai.player() == hand[0].player())
//         fu += 2;
//     if (!hand.check(Hand::f_Open))
//         fu += 10;

//     fu = 10 * ((fu + 9) / 10);
//     return combo[Yaku::pinfu] = 0;
// }

// template<>
// Fast8 eval<Yaku::ipeikou>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     MJ_ASSERT(combo[Yaku::ipeikou] == 0, "Cannot evaluate an ipeikou twice.");
//     if (combo[Yaku::ryanpeikou])
//         return 0;
//     if (hand.check(Hand::f_Open))
//         return combo[Yaku::ipeikou] = 0;
//     for (std::size_t i = 0; i < win.melds.size() - 1; i++)
//     {
//         if (win.melds[i].eq7(win.melds[i + 1]))
//             return combo[Yaku::ipeikou] = 1;
//     }
//     return combo[Yaku::ipeikou] = 0;
// }

// template<>
// Fast8 eval<Yaku::haitei>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     MJ_ASSERT(combo[Yaku::haitei] == 0, "Cannot evaluate a haitei twice.");
//     return combo[Yaku::haitei] = agari_pai & Tile::f_LastTile && agari_pai.player() == hand[0].player();
// }

// template<>
// Fast8 eval<Yaku::houtei>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     MJ_ASSERT(combo[Yaku::houtei] == 0, "Cannot evaluate a houtei twice.");
//     return combo[Yaku::houtei] = agari_pai & Tile::f_LastTile && agari_pai.player() != hand[0].player();
// }

// template<>
// Fast8 eval<Yaku::rinshan>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     MJ_ASSERT(combo[Yaku::rinshan] == 0, "Cannot evaluate a rinshan twice.");
//     return combo[Yaku::rinshan] = agari_pai & Tile::f_Rinshan && agari_pai.player() == hand[0].player();
// }

// template<>
// Fast8 eval<Yaku::chankan>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     MJ_ASSERT(combo[Yaku::chankan] == 0, "Cannot evaluate a chankan twice.");
//     MJ_WARN("chankan is not implemented");
//     return combo[Yaku::chankan] = 0;
// }

// template<>
// Fast8 eval<Yaku::tanyao>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     MJ_ASSERT(combo[Yaku::tanyao] == 0, "Cannot evaluate a tanyao twice.");
//     for (int i : {0,8,9,17,18,26,27,28,29,30,31,32,33})
//         if (hand.hand_4hot()[i])
//             return combo[Yaku::tanyao] = 0;
//     return combo[Yaku::tanyao] = 1;
// }

// template<>
// Fast8 eval<Yaku::hatsu>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     MJ_ASSERT(combo[Yaku::hatsu] == 0, "Cannot evaluate a hatsu twice.");
//     return combo[Yaku::hatsu] = hand.hand_4hot()[31] >= 3;
// }

// template<>
// Fast8 eval<Yaku::chun>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     MJ_ASSERT(combo[Yaku::chun] == 0, "Cannot evaluate a chun twice.");
//     return combo[Yaku::chun] = hand.hand_4hot()[32] >= 3;
// }

// template<>
// Fast8 eval<Yaku::haku>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     MJ_ASSERT(combo[Yaku::haku] == 0, "Cannot evaluate a haku twice.");
//     return combo[Yaku::haku] = hand.hand_4hot()[33] >= 3;
// }

// template<>
// Fast8 eval<Yaku::prevailing_wind>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     MJ_ASSERT(combo[Yaku::prevailing_wind] == 0, "Cannot evaluate a prevailing wind twice.");
//     return combo[Yaku::prevailing_wind] = hand.hand_4hot()[27+combo.round] >= 3;
// }

// template<>
// Fast8 eval<Yaku::seat_wind>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     MJ_ASSERT(combo[Yaku::seat_wind] == 0, "Cannot evaluate a seat wind twice.");
//     return combo[Yaku::seat_wind] = hand.hand_4hot()[27+hand[0].player()] >= 3;
// }

// template<>
// Fast8 eval<Yaku::chanta>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     MJ_ASSERT(combo[Yaku::chanta] == 0, "Cannot evaluate a chanta twice.");
//     if (combo[Yaku::junchan])
//         return 0;
//     if (!win.pair.first().is_19())
//         return combo[Yaku::chanta] = 0;
//     for (const auto &meld : win.melds)
//         if (!(meld.first().is_19() || meld.third().is_19()))
//             return combo[Yaku::chanta] = 0;
//     return combo[Yaku::chanta] = hand.check(Hand::f_Open) ? 1 : 2;
// }

// template<>
// Fast8 eval<Yaku::sanshoku_seq>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     MJ_ASSERT(combo[Yaku::sanshoku_seq] == 0, "Cannot evaluate a sanshoku seq twice.");
//     std::array<Fast8, 9> seq {{}};
//     for (const auto &meld : win.melds)
//     {
//         if (meld.is_set() || meld.first().is_honor())
//             continue;
//         seq[meld.first().num()] |= 1 << static_cast<Fast8>(meld.first().suit()); 
//     }
//     return combo[Yaku::sanshoku_seq] = (std::find(seq.begin(), seq.end(), 7) != seq.end())*(hand.check(Hand::f_Open) ? 1 : 2);
// }

// template<>
// Fast8 eval<Yaku::sanshoku_set>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     MJ_ASSERT(combo[Yaku::sanshoku_set] == 0, "Cannot evaluate a sanshoku set twice.");
//     std::array<Fast8, 9> set {{}};
//     for (const auto &meld : win.melds)
//     {
//         if (!meld.is_set() || meld.first().is_honor())
//             continue;
//         set[meld.first().num()] |= 1 << static_cast<Fast8>(meld.first().suit()); 
//     }
//     return combo[Yaku::sanshoku_seq] = (std::find(set.begin(), set.end(), 7) != set.end())*2;
// }

// template<>
// Fast8 eval<Yaku::ittsu>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     MJ_ASSERT(combo[Yaku::ittsu] == 0, "Cannot evaluate a ittsu twice.");
//     std::array<Fast8, 3> straights {{}};
//     for (const auto &meld : win.melds)
//     {
//         if (meld.is_set() || meld.first().is_honor())
//             continue;
//         switch (meld.first().num())
//         {
//         case 0:
//             straights[static_cast<Fast8>(meld.first().suit())] |= 1 << 0;
//             break;
//         case 3:
//             straights[static_cast<Fast8>(meld.first().suit())] |= 1 << 1;
//             break;
//         case 6:
//             straights[static_cast<Fast8>(meld.first().suit())] |= 1 << 2;
//             break;
//         default:
//             break;
//         }
//     }
//     return combo[Yaku::ittsu] = (std::find(straights.begin(), straights.end(), 7) != straights.end()) * (hand.check(Hand::f_Open) ? 1 : 2);
// }

// template<>
// Fast8 eval<Yaku::toitoi>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     MJ_ASSERT(combo[Yaku::toitoi] == 0, "Cannot evaluate a toitoi twice.");
//     for (const auto &meld : win.melds)
//         if (!meld.is_set())
//             return combo[Yaku::toitoi] = 0;
//     return combo[Yaku::toitoi] = 2;
// }

// template<>
// Fast8 eval<Yaku::sanankou>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     MJ_ASSERT(combo[Yaku::sanankou] == 0, "Cannot evaluate a sanankou twice.");
//     Fast8 ankou = 0;
//     for (const auto &meld : win.melds)
//         if (meld.is_set() && meld.called().player() == meld.third().player())
//             ankou++;
//     return combo[Yaku::sanankou] = 2*(ankou == 3);
// }

// template<>
// Fast8 eval<Yaku::sankantsu>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     MJ_ASSERT(combo[Yaku::sankantsu] == 0, "Cannot evaluate a sankantsu twice.");
//     Fast8 kantsu = 0;
//     for (const auto &meld : win.melds)
//         if (meld.fourth())
//             kantsu++;
//     return combo[Yaku::sankantsu] = 2*(kantsu == 3);
// }

// template<>
// Fast8 eval<Yaku::chitoitsu>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     MJ_ASSERT(combo[Yaku::chitoitsu] == 0, "Cannot evaluate a chitoitsu twice.");
//     if (shanten(hand.hand_4hot(), 0, k_ModeChiitoi) == -1)
//     {
//         combo.fu = 25;
//         return combo[Yaku::chitoitsu] = 2;
//     }
//     return combo[Yaku::chitoitsu] = 0;
// }

// template<>
// Fast8 eval<Yaku::honroutou>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     MJ_ASSERT(combo[Yaku::honroutou] == 0, "Cannot evaluate a honroutou twice.");
//     for (int i : {1,2,3,4,5,6,7,10,11,12,13,14,15,16,19,20,21,22,23,24,25})
//         if (hand.hand_4hot()[i])
//             return combo[Yaku::honroutou] = 0;
//     return combo[Yaku::honroutou] = 2;
// }

// template<>
// Fast8 eval<Yaku::shousangen>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     MJ_ASSERT(combo[Yaku::shousangen] == 0, "Cannot evaluate a shousangen twice.");
//     if (win.pair.first().suit() != Suit::Dragon)
//         return combo[Yaku::shousangen] = 0;
//     Fast8 gen = 1;
//     for (const auto &meld : win.melds)
//         if (meld.first().suit() == Suit::Dragon)
//             if (++gen == 3)
//                 return combo[Yaku::shousangen] = 2;
//     return combo[Yaku::shousangen] = 0;
// }

// template<>
// Fast8 eval<Yaku::honitsu>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     MJ_ASSERT(combo[Yaku::honitsu] == 0, "Cannot evaluate a honitsu twice.");
//     if (combo[Yaku::chinitsu])
//         return 0;

//     Fast8 tsu = 0;
//     for (Suit s = Suit::Man; s < Suit::Wind; ++s)
//     {
//         Fast8 s9 = 9*static_cast<Fast8>(s);
//         for (Fast8 n = s9; n < s9+8; ++n)
//         {
//             if (hand.hand_4hot()[n])
//             {
//                 tsu++;
//                 break;
//             }
//         }
//     }
//     return combo[Yaku::honitsu] = (tsu==1)*(hand.check(Hand::f_Open) ? 2 : 3);
// }

// template<>
// Fast8 eval<Yaku::junchan>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     MJ_ASSERT(combo[Yaku::junchan] == 0, "Cannot evaluate a junchan twice.");
//     if (!win.pair.first().is_19())
//         return combo[Yaku::junchan] = 0;
//     for (const auto &meld : win.melds)
//         if (!((meld.first().is_19() && !meld.first().is_honor()) || 
//                 (meld.third().is_19() && !meld.third().is_honor())))
//             return combo[Yaku::junchan] = 0;

//     combo[Yaku::chanta] = 0;
//     return combo[Yaku::chanta] = hand.check(Hand::f_Open) ? 2 : 3;
// }

// template<>
// Fast8 eval<Yaku::ryanpeikou>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     MJ_ASSERT(combo[Yaku::ryanpeikou] == 0, "Cannot evaluate a ryanpeikou twice.");
//     if (hand.check(Hand::f_Open))
//         return combo[Yaku::ryanpeikou] = 0;
//     if ((combo[Yaku::ryanpeikou] = 3*(win.melds[0].eq7(win.melds[1]) && win.melds[2].eq7(win.melds[3]))))
//         combo[Yaku::ipeikou] = 0;
//     return combo[Yaku::ryanpeikou];
// }

// template<>
// Fast8 eval<Yaku::chinitsu>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     MJ_ASSERT(combo[Yaku::chinitsu] == 0, "Cannot evaluate a chinitsu twice.");
//     Fast8 tsu = 0;
//     for (Suit s = Suit::Man; s < Suit::Wind; ++s)
//     {
//         Fast8 s9 = 9*static_cast<Fast8>(s);
//         for (Fast8 n = s9; n < s9+8; ++n)
//         {
//             if (hand.hand_4hot()[n])
//             {
//                 tsu++;
//                 break;
//             }
//         }
//     }
//     for (int i = 27; i < 34; ++i)
//         if (hand.hand_4hot()[i])
//             return combo[Yaku::chinitsu] = 0;
    
//     combo[Yaku::honitsu] = 0;
//     return combo[Yaku::chinitsu] = (tsu==1)*(hand.check(Hand::f_Open) ? 5 : 6);
// }

// template<>
// Fast8 eval<Yaku::dora>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     MJ_ASSERT(combo[Yaku::dora] == 0, "Cannot evaluate a dora twice.");
//     const auto &h4 = hand.hand_4hot();
//     combo[Yaku::dora] = std::count_if(hand.begin(), hand.end(), [](Tile t) { return t & Tile::f_Red; });

//     if (combo.doras)
//         combo[Yaku::dora] = std::accumulate(combo.doras->begin(), combo.doras->end(), combo[Yaku::dora], 
//         [&h4](Fast8 acc, Tile t) { return acc + h4[t.id34()]; });

//     return combo[Yaku::dora];
// }

// template<>
// Fast8 eval<Yakuman::chuuren_poutou>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     constexpr std::array<Fast8, 9> counts = {3,1,1,1,1,1,1,1,3};
//     for (Suit s = Suit::Man; s < Suit::Wind; ++s)
//     {
//         Fast8 s9 = 9*static_cast<Fast8>(s);
//         for (Fast8 n = 0; n < 9; ++n)
//             if (hand.hand_4hot()[s9+n] <= counts[n])
//                 goto FAILURE;
//     // SUCCESS
//         if (hand.hand_4hot()[agari_pai.id34()] > counts[agari_pai.num()])
//             return combo[Yakuman::chuuren_poutou] = 2;
//         return combo[Yakuman::chuuren_poutou] = 1;
//     FAILURE:
//         ;
//     }
//     return combo[Yakuman::chuuren_poutou] = 0;
// }

// template<>
// Fast8 eval<Yakuman::suu_ankou>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     for (const auto &meld : win.melds)
//         if (!(meld.is_set() && meld.first().player() == meld.third().player()))
//             return combo[Yakuman::suu_ankou] = 0;
//     if (agari_pai.eq7(win.pair.first()))
//         return combo[Yakuman::suu_ankou] = 2;
//     return combo[Yakuman::suu_ankou] = 1;
// }

// template<>
// Fast8 eval<Yakuman::suu_kantsu>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     for (const auto &meld : win.melds)
//         if (!meld.fourth())
//             return combo[Yakuman::suu_kantsu] = 0;
//     return combo[Yakuman::suu_kantsu] = 1;
// }

// template<>
// Fast8 eval<Yakuman::daisangen>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     Fast8 gen = 0;
//     for (const auto &meld : win.melds)
//         if (meld.first().suit() == Suit::Dragon)
//             if (++gen == 3)
//                 return combo[Yakuman::daisangen] = 1;
//     return combo[Yakuman::daisangen] = 0;
    
// }

// template<>
// Fast8 eval<Yakuman::shosushi>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     if (win.pair.first().suit() != Suit::Wind)
//         return combo[Yakuman::shosushi] = 0;
    
//     Fast8 shi = 1;
//     for (const auto &meld : win.melds)
//         if (meld.first().suit() == Suit::Wind)
//             if (++shi == 4)
//                 return combo[Yakuman::shosushi] = 1;
//     return combo[Yakuman::shosushi] = 0;
// }

// template<>
// Fast8 eval<Yakuman::daisushi>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     for (const auto &meld : win.melds)
//         if (meld.first().suit() != Suit::Wind)
//             return combo[Yakuman::daisushi] = 0;
//     return combo[Yakuman::daisushi] = 2;
// }

// template<>
// Fast8 eval<Yakuman::kokushi>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     if (shanten(hand.hand_4hot(), 0, k_ModeKokushi) != -1)
//         return combo[Yakuman::kokushi] = 0;
//     return combo[Yakuman::kokushi] = hand.hand_4hot()[agari_pai.id34()];
// }

// template<>
// Fast8 eval<Yakuman::ryuisou>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     const auto &h4= hand.hand_4hot();
//     return combo[Yakuman::ryuisou] = 14 <= h4[19]+h4[20]+h4[21]+h4[23]+h4[25]+h4[31];
// }

// template<>
// Fast8 eval<Yakuman::chinroutou>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     const auto &h4= hand.hand_4hot();
//     return combo[Yakuman::chinroutou] = 14 <= h4[0]+h4[8]+h4[9]+h4[17]+h4[18]+h4[26]; 
// }

// template<>
// Fast8 eval<Yakuman::tenhou>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     return combo[Yakuman::tenhou] = agari_pai & Tile::f_FirstTurn && hand[0].player() == agari_pai.player() && agari_pai.player() == k_East;
// }

// template<>
// Fast8 eval<Yakuman::chihou>(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     return combo[Yakuman::chihou] = agari_pai & Tile::f_FirstTurn && hand[0].player() == agari_pai.player() && agari_pai.player() != k_West;
// }

// /*******************************************************************************
//  * Hybrid Scoring for optimized performance
//  ******************************************************************************/
// template<>
// Fast8 eval
// <Yaku::chinitsu | Yaku::honitsu>
// (ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     Fast8 tsu = 0;
//     for (Suit s = Suit::Man; s < Suit::Wind; ++s)
//     {
//         Fast8 s9 = 9*static_cast<Fast8>(s);
//         for (Fast8 n = s9; n < s9+8; ++n)
//         {
//             if (hand.hand_4hot()[n])
//             {
//                 tsu++;
//                 break;
//             }
//         }
//     }
//     if (tsu != 1)
//         return 0;
//     for (int i = 27; i < 34; ++i)
//         if (hand.hand_4hot()[i])
//             return combo[Yaku::honitsu] = hand.check(Hand::f_Open) ? 2 : 3;
//     return combo[Yaku::chinitsu] = hand.check(Hand::f_Open) ? 5 : 6;    
// }

// template<>
// Fast8 eval
// <Yaku::sanshoku_seq | Yaku::sanshoku_set>
// (ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     MJ_ASSERT(combo[Yaku::sanshoku_seq] == 0, "sanshoku_seq already set");
//     MJ_ASSERT(combo[Yaku::sanshoku_set] == 0, "sanshoku_set already set");
//     std::array<Fast8, 9> seq {{}}, set {{}};
//     for (const auto &meld : win.melds)
//     {
//         if (meld.first().is_honor())
//             continue;
//         (meld.is_set() ? set : seq)[meld.first().num()] |= 1 << static_cast<Fast8>(meld.first().suit()); 
//     }
//     combo[Yaku::sanshoku_seq] = (std::find(seq.begin(), seq.end(), 7) != seq.end())*(hand.check(Hand::f_Open) ? 1 : 2);
//     combo[Yaku::sanshoku_set] = (std::find(set.begin(), set.end(), 7) != set.end())*2;
    
//     return combo[Yaku::sanshoku_seq] + combo[Yaku::sanshoku_set];
// }

// template<>
// Fast8 eval
// <Yaku::sanshoku_set | Yaku::sanshoku_seq>
// (ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     return eval<Yaku::sanshoku_seq | Yaku::sanshoku_set>(combo, hand, win, agari_pai);
// }

// template<>
// Fast8 eval
// <Yaku::junchan | Yaku::chanta>
// (ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     MJ_ASSERT(combo[Yaku::chanta] == 0, "Cannot evaluate a chanta twice.");
//     MJ_ASSERT(combo[Yaku::junchan] == 0, "Cannot evaluate a junchan twice.");
//     if (!win.pair.first().is_19())
//         return 0;

//     combo[Yaku::junchan] = hand.check(Hand::f_Open) ? 2 : 3;

//     for (const auto &meld : win.melds)
//     {
//         if (!(meld.first().is_19() || meld.third().is_19()))
//             return combo[Yaku::junchan] = 0;
//         else if (meld.first().is_honor())
//             combo[Yaku::junchan] = 0;
//     }
//     if (win.pair.first().is_honor())
//         combo[Yaku::junchan] = 0;
//     if (combo[Yaku::junchan])
//         return combo[Yaku::junchan];
//     return combo[Yaku::chanta] = hand.check(Hand::f_Open) ? 1 : 2;
// }

// Fast32 basic_score(Fast16 fu, Fast8 fan)
// {
//     if (fan >= 13)
//         return k_Yakuman;
//     if (fan >= 11)
//         return k_Sanbaiman;
//     if (fan >= 8)
//         return k_Baiman;
//     if (fan >= 6)
//         return k_Haneman;
//     if (fan == 5 || (fu >= 40 && fan == 4) || (fu >= 70 && fan == 3))
//         return k_Mangan;
//     return fu << (2 + fan);
// }

// Fast32 score_hand(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai)
// {
//     eval<Yakuman::chuuren_poutou>(combo, hand, win, agari_pai);
//     eval<Yakuman::suu_ankou>(combo, hand, win, agari_pai);
//     eval<Yakuman::suu_kantsu>(combo, hand, win, agari_pai);
//     eval<Yakuman::daisangen>(combo, hand, win, agari_pai);
//     eval<Yakuman::shosushi>(combo, hand, win, agari_pai);
//     eval<Yakuman::daisushi>(combo, hand, win, agari_pai);
//     eval<Yakuman::kokushi>(combo, hand, win, agari_pai);
//     eval<Yakuman::ryuisou>(combo, hand, win, agari_pai);
//     eval<Yakuman::chinroutou>(combo, hand, win, agari_pai);
//     eval<Yakuman::tenhou>(combo, hand, win, agari_pai);
//     eval<Yakuman::chihou>(combo, hand, win, agari_pai);
    
//     Fast8 yakumans = std::accumulate(combo.yakuman.begin(), combo.yakuman.end(), 0);
//     if (yakumans >= 1)
//         return k_Yakuman*((combo.flags & f_DoubleYakuman) ? yakumans : 1);
    
//     if (eval<Yaku::chitoitsu>(combo, hand, win, agari_pai))
//     {
//     if(eval<Yaku::chinitsu | Yaku::honitsu>(combo, hand, win, agari_pai)){}
//     else if (eval<Yaku::honroutou>(combo, hand, win, agari_pai)){}
//     else eval<Yaku::tanyao>(combo, hand, win, agari_pai);
//     }
//     else
//     {
//     eval<Yaku::pinfu>(combo, hand, win, agari_pai);
//     eval<Yaku::riichi>(combo, hand, win, agari_pai);
//     eval<Yaku::ippatsu>(combo, hand, win, agari_pai);
//     eval<Yaku::men_tsumo>(combo, hand, win, agari_pai);
//     eval<Yaku::haitei>(combo, hand, win, agari_pai);
//     eval<Yaku::houtei>(combo, hand, win, agari_pai);
//     eval<Yaku::rinshan>(combo, hand, win, agari_pai);
//     eval<Yaku::chankan>(combo, hand, win, agari_pai);
//     if (
//     eval<Yaku::prevailing_wind>(combo, hand, win, agari_pai) |
//     eval<Yaku::seat_wind>(combo, hand, win, agari_pai) |
//     eval<Yaku::hatsu>(combo, hand, win, agari_pai) |
//     eval<Yaku::chun>(combo, hand, win, agari_pai) |
//     eval<Yaku::haku>(combo, hand, win, agari_pai) 
//     ){}
//     else
//     {
//     eval<Yaku::tanyao>(combo, hand, win, agari_pai);
//     eval<Yaku::ryanpeikou>(combo, hand, win, agari_pai);
//     }
//     if(
//     eval<Yaku::chinitsu | Yaku::honitsu>(combo, hand, win, agari_pai)
//     ){}
//     else
//     {
//     eval<Yaku::sanshoku_seq | Yaku::sanshoku_set>(combo, hand, win, agari_pai);
//     }
//     if(
//     eval<Yaku::honroutou>(combo, hand, win, agari_pai)
//     ){}
//     else if(
//     eval<Yaku::junchan | Yaku::chanta>(combo, hand, win, agari_pai)
//     ){}
//     else
//     {
//     eval<Yaku::ittsu>(combo, hand, win, agari_pai);
//     }
//     eval<Yaku::ipeikou>(combo, hand, win, agari_pai);
//     eval<Yaku::shousangen>(combo, hand, win, agari_pai);
//     eval<Yaku::toitoi>(combo, hand, win, agari_pai);
//     eval<Yaku::sanankou>(combo, hand, win, agari_pai);
//     eval<Yaku::sankantsu>(combo, hand, win, agari_pai);
//     }

//     Fast8 fan = std::accumulate(combo.yaku.begin(), combo.yaku.end(), 0);
//     return basic_score(combo.fu, fan);
// }

} // namespace scoring
} // namespace mj

#include "rng.hpp"
