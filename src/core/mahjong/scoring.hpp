
#pragma once
#include "mahjong.hpp"
#include "core/utils/logging.hpp"

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

constexpr U8f k_NumYaku = 30;
constexpr U8f k_NumYakuman = 11;

constexpr U16 f_DoubleYakuman = 0x0001;

using Doras = s_Vector<Tile, 10>;

enum class Yaku : U64f
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

enum class Yakuman : U8f
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
    return static_cast<Yaku>(static_cast<U64f>(a) << 8 | static_cast<U64f>(b));
}

struct ScoringCombo
{
    U16 flags {};
    std::array<U8f, k_NumYaku> yaku {};
    std::array<U8f, k_NumYakuman> yakuman {};
    Doras *doras { nullptr };
    U16f fu {};
    Dir round {};

    CONSTEXPR12 U8f &operator[](Yaku y) MJ_EXCEPT_CRIT
    {   MJ_ASSERT_CRIT(static_cast<U64f>(y) <= k_NumYaku,
        "Cannot index multiple yaku at once.");
        return yaku[static_cast<U64f>(y)-1]; }
    CONSTEXPR12 const U8f &operator[](Yaku y) const MJ_EXCEPT_CRIT
    {   MJ_ASSERT_CRIT(static_cast<U64f>(y) <= k_NumYaku,
        "Cannot index multiple yaku at once.");
        return yaku[static_cast<U64f>(y)-1]; }
    CONSTEXPR12 U8f &operator[](Yakuman y) noexcept
    { return yakuman[static_cast<U8f>(y)]; }
    CONSTEXPR12 const U8f &operator[](Yakuman y) const noexcept
    { return yakuman[static_cast<U8f>(y)]; }
};

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
template<Yaku yaku>
U8f eval(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai);

/**
 * Evaluate a particular yakuman
 * 
 * @tparam yakuman The yakuman to evaluate
 * @param combo The scoring combo reference to aggregate the result
 * @param hand The hand to evaluate.
 * @param win The win (set of sequences and pairs) to evaluate
 * @param agari_pai The tile that caused the win.
 * @return U8f The number of counts for the yakuman (0, 1, or 2)
 */
template<Yakuman yakuman>
U8f eval(ScoringCombo &combo, const Hand &hand, const Win &win, Tile agari_pai);

U32f basic_score(U16f fu, U8f fan);

U32f score_hand(const Hand &hand, const Win &win, Tile agari_pai);

} // namespace scoring
} // namespace mj
