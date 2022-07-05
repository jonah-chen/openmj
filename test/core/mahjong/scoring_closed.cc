
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
    t_ClosedHand("p11123456789999", mj::Tile("9p"), mj::scoring::k_Yakuman*2,
        mj::scoring::f_ChuurenPoutou | mj::scoring::f_DoubleYakuman);
    t_ClosedHand("p11123456789999", mj::Tile("8p"), mj::scoring::k_Yakuman,
        mj::scoring::f_ChuurenPoutou);
    t_ClosedHand("w111222333444d22", mj::Tile("2d"), mj::scoring::k_Yakuman*2,
        mj::scoring::f_Daisushi | mj::scoring::f_Tsuuiisou | mj::scoring::f_SuuAnkou | mj::scoring::f_DoubleYakuman);
    t_ClosedHand("s22334466688d111", mj::Tile("3s"), mj::scoring::k_Yakuman,
        mj::scoring::f_Ryuisou);
    t_ClosedHand("m123w11122333444", mj::Tile("3m"), mj::scoring::k_Yakuman,
        mj::scoring::f_Shosushi);
    t_ClosedHand("m19p19s19w1234d1233", mj::Tile("3d"), mj::scoring::k_Yakuman*2,
        mj::scoring::f_Kokushi | mj::scoring::f_DoubleYakuman);
    t_ClosedHand("m19p119s19w1234d123", mj::Tile("3d"), mj::scoring::k_Yakuman,
        mj::scoring::f_Kokushi);
    t_ClosedHand("m111999p11s111999", mj::Tile("1s"), mj::scoring::k_Yakuman,
        mj::scoring::f_Chinroutou | mj::scoring::f_SuuAnkou);
    t_ClosedHand("m111999p11s111999", mj::Tile("1p"), mj::scoring::k_Yakuman*2,
        mj::scoring::f_Chinroutou | mj::scoring::f_SuuAnkou | mj::scoring::f_DoubleYakuman);
    t_ClosedHand("m12344d111222333", mj::Tile("4m"), mj::scoring::k_Yakuman,
        mj::scoring::f_Daisangen);
    
    t_ClosedHand("m123456s44455666", mj::Tile("5m"), 320,
        40 | mj::scoring::f_MenTsumo);
    t_ClosedHand("m112233s44455666", mj::Tile("2m"), 640,
        40 | mj::scoring::f_Ipeikou | mj::scoring::f_MenTsumo);
    t_ClosedHand("m111789s123999d22", mj::Tile("2d"), 1280,
        40 | mj::scoring::f_ChantaC | mj::scoring::f_MenTsumo);
    t_ClosedHand("m345p345s34588899", mj::Tile("9s"), 960,
        30 | mj::scoring::f_SanshokuSeqC | mj::scoring::f_MenTsumo);
    t_ClosedHand("m222234p222s222d22", mj::Tile("2d"), mj::scoring::k_Mangan,
        40 | mj::scoring::f_SanshokuSet | mj::scoring::f_Sanankou | mj::scoring::f_MenTsumo);
    t_ClosedHand("m333444555s234d11", mj::Tile("1d"), 1280,
        40 | mj::scoring::f_Sanankou | mj::scoring::f_MenTsumo);
    t_ClosedHand("m11223344557788", mj::Tile("1m"), mj::scoring::k_Baiman,
        25 | mj::scoring::f_Chitoitsu | mj::scoring::f_ChinitsuC | mj::scoring::f_MenTsumo);
    t_ClosedHand("m1199s1199w112233", mj::Tile("3w"), mj::scoring::k_Mangan,
        25 | mj::scoring::f_Honroutou | mj::scoring::f_Chitoitsu | mj::scoring::f_MenTsumo);
    t_ClosedHand("m1122334455d1122", mj::Tile("2d"), mj::scoring::k_Haneman,
        25 | mj::scoring::f_Chitoitsu | mj::scoring::f_HonitsuC | mj::scoring::f_MenTsumo);
    t_ClosedHand("m123p234d11122233", mj::Tile("2d"), mj::scoring::k_Mangan,
        40 | mj::scoring::f_Shousangen | mj::scoring::f_Hatsu | mj::scoring::f_Chun | mj::scoring::f_MenTsumo);
    t_ClosedHand("m123p123s11123999", mj::Tile("9s"), mj::scoring::k_Haneman,
        40 | mj::scoring::f_JunchanC | mj::scoring::f_SanshokuSeqC | mj::scoring::f_MenTsumo);
    t_ClosedHand("m123456789s33d222", mj::Tile("2d"), mj::scoring::k_Mangan,
        40 | mj::scoring::f_IttsuC | mj::scoring::f_MenTsumo | mj::scoring::f_Chun);
    t_ClosedHand("m123456789s333d22", mj::Tile("2d"), 960, 
        30 | mj::scoring::f_IttsuC | mj::scoring::f_MenTsumo);
    t_ClosedHand("m123p234s34566789", mj::Tile("9s"), 320,
        20 | mj::scoring::f_Pinfu | mj::scoring::f_MenTsumo);
    t_ClosedHand("p22334488s334455", mj::Tile("2p"), mj::scoring::k_Haneman,
        20 | mj::scoring::f_Pinfu | mj::scoring::f_Ryanpeikou | mj::scoring::f_Tanyao | mj::scoring::f_MenTsumo);
    t_ClosedHand("p22334488s334455", mj::Tile("4s"), mj::scoring::k_Mangan,
        30 | mj::scoring::f_Ryanpeikou | mj::scoring::f_Tanyao | mj::scoring::f_MenTsumo);
    t_ClosedHand("p22334488s334455", mj::Tile("8p"), mj::scoring::k_Mangan,
        30 | mj::scoring::f_Ryanpeikou | mj::scoring::f_Tanyao | mj::scoring::f_MenTsumo);
    t_ClosedHand("p22334488d222333", mj::Tile("8d"), mj::scoring::k_Haneman,
        40 | mj::scoring::f_HonitsuC | mj::scoring::f_Ipeikou | mj::scoring::f_Chun | mj::scoring::f_Haku | mj::scoring::f_MenTsumo);
    t_ClosedHand("s11223355778899", mj::Tile("9s"), mj::scoring::k_Sanbaiman,
        20 | mj::scoring::f_Ryanpeikou | mj::scoring::f_ChinitsuC | mj::scoring::f_Pinfu | mj::scoring::f_MenTsumo);

    return g_FailureCount;
}
