
#pragma once
#include "mahjong.hpp"

namespace mj {
namespace scoring {
constexpr Fast16 k_BonusScore = 100;
constexpr Fast16 k_RiichiDeposit = 1000;
constexpr Fast16 k_BaseFu = 20;
constexpr Fast16 k_Yakuman = 13;
constexpr Fast16 k_Mangan = 2000;
constexpr Fast16 k_Haneman = 3000;
constexpr Fast16 k_Baiman = 4000;
constexpr Fast16 k_Sanbaiman = 6000;

constexpr Fast8 k_NumYaku = 30;
constexpr Fast8 k_NumYakuman = 10;

enum class Yaku : Fast8
{
    riichi,
    ippatsu,
    men_tsumo,
    pinfu,
    ipeikou,
    haitei,
    houtei,
    rinshan,
    chankan,
    tanyao,
    hatsu,
    chun,
    haku,
    prevailing_wind,
    seat_wind,
    chanta,
    sanshoku_seq,
    sanshoku_set,
    ittsu,
    toitoi,
    sanankou,
    sankantsu,
    chitoitsu,
    honroutou,
    shousangen,
    honitsu,
    junchan,
    ryanpeikou,
    chinitsu,
    dora
};

enum class Yakuman : Fast8
{
    chuuren_poutou,
    suu_ankou,
    daisangen,
    shosushi,
    daisushi,
    kokushi,
    ryuisou,
    chinroutou,
    tenhou,
    chihou
};

struct ScoringCombo
{
    U16 flags;
    std::array<Fast8, k_NumYaku> yaku;
    std::array<Fast8, k_NumYakuman> yakuman;

    CONSTEXPR12 Fast8 &operator[](Yaku y) noexcept
    { return yaku[static_cast<Fast8>(y)]; }
    CONSTEXPR12 const Fast8 &operator[](Yaku y) const noexcept
    { return yaku[static_cast<Fast8>(y)]; }
    CONSTEXPR12 Fast8 &operator[](Yakuman y) noexcept
    { return yakuman[static_cast<Fast8>(y)]; }
    CONSTEXPR12 const Fast8 &operator[](Yakuman y) const noexcept
    { return yakuman[static_cast<Fast8>(y)]; }
};

/**
 * Evaluate a particular yaku 
 * 
 * @tparam yaku The yaku to evaluate
 * @param combo The scoring combo reference to aggregate the result
 * @param win The win (set of sequences and pairs) to evaluate
 * @return Fast8 The number of fan for yaku (0 to 6)
 */
template<Yaku yaku>
Fast8 eval(ScoringCombo &combo, const Win &win);

/**
 * Evaluate a particular yakuman
 * 
 * @tparam yakuman The yakuman to evaluate
 * @param combo The scoring combo reference to aggregate the result
 * @param win The win (set of sequences and pairs) to evaluate
 * @return Fast8 The number of counts for the yakuman (0, 1, or 2)
 */
template<Yakuman yakuman>
Fast8 eval(ScoringCombo &combo, const Win &win);

} // namespace scoring
} // namespace mj
