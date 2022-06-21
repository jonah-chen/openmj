
#include "test/test.hpp"
#include "core/mahjong/mahjong.hpp"

void t_Hand(const char *hand, mj::Tile agari_pai, mj::U32f score, mj::U64 yakus, mj::U64 flags=mj::scoring::f_NormalPlay & mj::scoring::f_ClosedHandMask)
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
    t_Hand("m123456789s33d222", mj::Tile("2d"), mj::scoring::k_Mangan,
        40 | mj::scoring::f_IttsuC | mj::scoring::f_MenTsumo | mj::scoring::f_Chun,
        mj::scoring::f_NormalPlay & mj::scoring::f_ClosedHandMask);
    t_Hand("m123456789s333d22", mj::Tile("2d"), 960, 
        30 | mj::scoring::f_IttsuC | mj::scoring::f_MenTsumo);
    return g_FailureCount;
}
