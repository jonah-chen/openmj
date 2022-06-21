
#include "test/test.hpp"
#include "core/mahjong/mahjong.hpp"

void t_ClosedHand(const char *hand, mj::Tile agari_pai, mj::U32f score, mj::U64 yakus, mj::U64 flags=mj::scoring::f_NormalPlay & mj::scoring::f_ClosedHandMask)
{
    mj::Hand h(hand);
    h.flags = flags;
    auto [calc_score, calc_yaku] = mj::scoring::score_hand(h, agari_pai);
    assert(calc_score == score, "Hand: " + (hand + (" score of " + 
        std::to_string(score))) + " expected, got " + std::to_string(calc_score));
    assert(calc_yaku == yakus, "Hand: " + (hand + (" yaku of:\n" + 
        mj::scoring::yaku_str(yakus))) + " expected, got:\n" + 
        mj::scoring::yaku_str(calc_yaku));
}

int main()
{
    t_ClosedHand("m123456789s33d222", mj::Tile("2d"), mj::scoring::k_Mangan,
        40 | mj::scoring::f_IttsuC | mj::scoring::f_MenTsumo | mj::scoring::f_Chun);
    t_ClosedHand("m123456789s333d22", mj::Tile("2d"), 960, 
        30 | mj::scoring::f_IttsuC | mj::scoring::f_MenTsumo);
    t_ClosedHand("p11123456789999", mj::Tile("9p"), mj::scoring::k_Yakuman*2,
        mj::scoring::f_ChuurenPoutou | mj::scoring::f_DoubleYakuman);
    t_ClosedHand("p11123456789999", mj::Tile("8p"), mj::scoring::k_Yakuman,
        mj::scoring::f_ChuurenPoutou);
    t_ClosedHand("w111222333444d22", mj::Tile("2d"), mj::scoring::k_Yakuman*2,
        mj::scoring::f_Daisushi | mj::scoring::f_Tsuuiisou | mj::scoring::f_SuuAnkou | mj::scoring::f_DoubleYakuman);
    t_ClosedHand("m123p234s34566789", mj::Tile("9s"), 320,
        20 | mj::scoring::f_Pinfu | mj::scoring::f_MenTsumo);
    
    return g_FailureCount;
}
