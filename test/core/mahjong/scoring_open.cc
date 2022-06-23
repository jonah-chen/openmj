#define OPEN_HAND(X) (new _openHand(#X))
#include "test/test.hpp"
#include "core/mahjong/mahjong.hpp"

struct _openHand
{
    mj::Hand h;
    const char *hand;
    _openHand(const char *hand) : h(hand), hand(hand) {}
    _openHand *PONG(mj::Tile t)
    {
        assert(h.pong(t), "Pong not possible");
        return this;
    }
    _openHand *CHII(mj::Tile t, mj::S8f chii_at)
    {
        assert(h.chii(t, chii_at), "Failed to chii " + std::to_string(t) + " at " + std::to_string(chii_at));
        return this;
    }
    _openHand *DISCARD(mj::Tile t)
    {
        h.discard(t);
        return this;
    }
    _openHand *KONG(mj::Tile t)
    {
        assert(h.open_kong(t), "Kong not possible");
        return this;
    }
    _openHand *KONG(mj::U8f t34)
    {
        h.push_back(mj::convert34(t34).set_dir(h.player()));
        h.sort_after_draw();
        assert(h.closed_kong(t34), "Kong not possible");
        return this;
    }
    void SCORE(mj::Tile agari_pai, mj::U32f score, mj::U64 yakus, mj::U64 flags=mj::scoring::f_OpenHandMask & mj::scoring::f_NormalPlay)
    {
        h.push_back(agari_pai);
        h.flags = flags;
        auto [calc_score, calc_yaku] = mj::scoring::score_hand(h, agari_pai);
        assert(calc_score == score, "Hand: " + (hand + (" score of " + 
            std::to_string(score))) + " expected, got " + std::to_string(calc_score));
        assert(calc_yaku == yakus, "Hand: " + (hand + (" yaku of:\n" + 
            mj::scoring::yaku_str(yakus))) + " expected, got:\n" + 
            mj::scoring::yaku_str(calc_yaku));
        delete this;
    }
};


int main()
{
    OPEN_HAND(m113366s448)
        ->PONG(mj::Tile("1m", mj::k_South))
        ->PONG(mj::Tile("3m", mj::k_South))
        ->PONG(mj::Tile("6m", mj::k_South))
        ->PONG(mj::Tile("4s", mj::k_South))
        ->SCORE(mj::Tile("8s"), 640, 
            40 | mj::scoring::f_Toitoi);
    OPEN_HAND(m1999p11s11d11)
        ->PONG(mj::Tile("1p", mj::k_South))
        ->PONG(mj::Tile("1s", mj::k_South))
        ->PONG(mj::Tile("1d", mj::k_South))
        ->SCORE(mj::Tile("1m"), mj::scoring::k_Mangan, 
            50 | mj::scoring::f_Honroutou | mj::scoring::f_Toitoi | mj::scoring::f_Hatsu);
    OPEN_HAND(m11133366s4488)
        ->PONG(mj::Tile("6m", mj::k_North))
        ->SCORE(mj::Tile("4s", mj::k_South), 640, 
            40 | mj::scoring::f_Toitoi);
    OPEN_HAND(m11133366s4488)
        ->PONG(mj::Tile("6m", mj::k_North))
        ->SCORE(mj::Tile("4s", mj::k_East), 
            mj::scoring::k_Mangan, 50 | mj::scoring::f_Toitoi | mj::scoring::f_Sanankou);
    OPEN_HAND(m111333666s1288)
        ->KONG(mj::Tile("1m").id34())
        ->KONG(mj::Tile("3m").id34())
        ->KONG(mj::Tile("6m").id34()) // closed ron +10
        ->SCORE(mj::Tile("3s", mj::k_South), mj::scoring::k_Mangan, 
            100 | mj::scoring::f_Sankantsu | mj::scoring::f_Sanankou, 
            mj::scoring::f_NormalPlay & mj::scoring::f_ClosedHandMask);
    OPEN_HAND(m111333666s1288)
        ->KONG(mj::Tile("1m", mj::k_South))
        ->KONG(mj::Tile("3m", mj::k_North))
        ->KONG(mj::Tile("6m", mj::k_West))
        ->SCORE(mj::Tile("3s", mj::k_South), 960, 
            60 | mj::scoring::f_Sankantsu);
    OPEN_HAND(m111333666w1113)
        ->KONG(mj::Tile("1m", mj::k_South))
        ->KONG(mj::Tile("3m", mj::k_North))
        ->KONG(mj::Tile("6m", mj::k_West))
        ->KONG(mj::Tile("1w").id34())
        ->SCORE(mj::Tile("3w", mj::k_South), mj::scoring::k_Yakuman,
            mj::scoring::f_SuuKantsu);
    OPEN_HAND(m111999p111999s1)
        ->KONG(mj::Tile("1p", mj::k_South))
        ->KONG(mj::Tile("1m", mj::k_South))
        ->KONG(mj::Tile("9m", mj::k_South))
        ->KONG(mj::Tile("9p", mj::k_South))
        ->SCORE(mj::Tile("1s", mj::k_South), mj::scoring::k_Yakuman,
            mj::scoring::f_SuuKantsu | mj::scoring::f_Chinroutou);
    OPEN_HAND(m13p13s13d1112)
        ->CHII(mj::Tile("2p", mj::k_North), mj::k_ChiiMiddle)
        ->CHII(mj::Tile("2s", mj::k_North), mj::k_ChiiMiddle)
        ->CHII(mj::Tile("2m", mj::k_North), mj::k_ChiiMiddle)
        ->SCORE(mj::Tile("2d", mj::k_South), 960,
            30 | mj::scoring::f_ChantaO | mj::scoring::f_SanshokuSeqO | mj::scoring::f_Hatsu);
    OPEN_HAND(p12233557889)
        ->CHII(mj::Tile("1p", mj::k_North), mj::k_ChiiBelow)
        ->PONG(mj::Tile("5p", mj::k_North))
        ->SCORE(mj::Tile("8p"), mj::scoring::k_Mangan,
            30 | mj::scoring::f_ChinitsuO);
    OPEN_HAND(p3355s23568)
        ->PONG(mj::Tile("3p", mj::k_South))
        ->CHII(mj::Tile("4s", mj::k_North), mj::k_ChiiAbove)
        ->PONG(mj::Tile("5p", mj::k_South))
        ->CHII(mj::Tile("4s", mj::k_North), mj::k_ChiiBelow)
        ->SCORE(mj::Tile("8s", mj::k_South), 240,
            30 | mj::scoring::f_Tanyao);
    OPEN_HAND(p3355s23568)
        ->PONG(mj::Tile("3p", mj::k_South))
        ->CHII(mj::Tile("4s", mj::k_North), mj::k_ChiiAbove)
        ->PONG(mj::Tile("5p", mj::k_South))
        ->CHII(mj::Tile("4s", mj::k_North), mj::k_ChiiBelow)
        ->SCORE(mj::Tile("8s", mj::k_South),0,0,mj::scoring::f_NoOpenTanyaoMask & mj::scoring::f_OpenHandMask & mj::scoring::f_NormalPlay);

    return g_FailureCount;
}