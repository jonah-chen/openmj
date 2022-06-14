
#pragma once
#include "mahjong.hpp"
#include "core/utils/logging.hpp"

namespace mj {
namespace scoring {
constexpr Fast16 k_BonusScore = 100;
constexpr Fast16 k_RiichiDeposit = 1000;
constexpr Fast16 k_BaseFu = 20;
constexpr Fast16 k_Mangan = 2000;
constexpr Fast16 k_Haneman = 3000;
constexpr Fast16 k_Baiman = 4000;
constexpr Fast16 k_Sanbaiman = 6000;
constexpr Fast16 k_Yakuman = 8000;

constexpr Fast8 k_NumYaku = 30;
constexpr Fast8 k_NumYakuman = 11;

constexpr U16 f_DoubleYakuman = 0x0001;

using Doras = s_Vector<Tile, 10>;

enum class Yaku : Fast64
{
    invalid = 0,
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
    suu_kantsu,
    daisangen,
    shosushi,
    daisushi,
    kokushi,
    ryuisou,
    chinroutou,
    tenhou,
    chihou
};

constexpr Yaku operator|(Yaku a, Yaku b) 
{
    return static_cast<Yaku>(static_cast<Fast64>(a) << 8 | static_cast<Fast64>(b));
}

struct ScoringCombo
{
    U16 flags {};
    std::array<Fast8, k_NumYaku> yaku {};
    std::array<Fast8, k_NumYakuman> yakuman {};
    Doras *doras { nullptr };
    Fast16 fu {};
    Dir round {};

    CONSTEXPR12 Fast8 &operator[](Yaku y) MJ_EXCEPT_CRIT
    {   MJ_ASSERT_CRIT(static_cast<Fast64>(y) <= k_NumYaku,
        "Cannot index multiple yaku at once.");
        return yaku[static_cast<Fast64>(y)-1]; }
    CONSTEXPR12 const Fast8 &operator[](Yaku y) const MJ_EXCEPT_CRIT
    {   MJ_ASSERT_CRIT(static_cast<Fast64>(y) <= k_NumYaku,
        "Cannot index multiple yaku at once.");
        return yaku[static_cast<Fast64>(y)-1]; }
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
 * @param hand The hand to evaluate.
 * @param win The win (set of sequences and pairs) to evaluate
 * @param agari_pai The tile that caused the win.
 * @return Fast8 The number of fan for yaku (0 to 6)
 */
template<Yaku yaku>
Fast8 eval(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai);

/**
 * Evaluate a particular yakuman
 * 
 * @tparam yakuman The yakuman to evaluate
 * @param combo The scoring combo reference to aggregate the result
 * @param hand The hand to evaluate.
 * @param win The win (set of sequences and pairs) to evaluate
 * @param agari_pai The tile that caused the win.
 * @return Fast8 The number of counts for the yakuman (0, 1, or 2)
 */
template<Yakuman yakuman>
Fast8 eval(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai);

Fast32 basic_score(Fast16 fu, Fast8 fan);

Fast32 score_hand(const Hand &hand, const Win &win, Tile agari_pai);

} // namespace scoring
} // namespace mj
