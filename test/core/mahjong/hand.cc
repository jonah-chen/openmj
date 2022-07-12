
#include "core/mahjong/mahjong.hpp"
#include "test/test.hpp"

TEST t_Closed()
{
    mj::Hand h1("m123345567p333sw22d");
    mj::Hand h2("m123345567p333w22");
    mj::Hand h3("s33344455566677");
    mj::BigHand h4("m12334567p333w22d123");
    mj::Hand h5("w1112223344d222");
    mj::Hand h6("m1112223334689");
    mj::Hand h7("m111222333p147w1");

    assert_crit(h1.size() == 14, "h1 should have 14 tiles", 255);
    assert_crit(h2.size() == 14, "h2 should have 14 tiles", 255);
    for (int i = 0; i < 14; ++i)
        assert(h1[i] == h2[i],
               "h1 and h2 should be equal at idx " + std::to_string(i));

    assert(!h1.agari().empty(), "h1 can win at least once");
    assert(!h3.agari().empty(), "h3 can win at least once");
    std::cerr << h3.agari().size() << std::endl;
    assert(h4.agari().empty(), "h4 cannot win at least once");
    assert(h4.tenpai().size() == 3, "h4 is in 3 sided wait but got " +
                                        std::to_string(h4.tenpai().size()));
    assert(h5.tenpai().size() == 2, "h5 is in 2 sided wait but got " +
                                        std::to_string(h5.tenpai().size()));
    assert(h6.tenpai().empty(), "h6 is not in tenpai");

    assert(h1.is_agari(), "h1 can win");
    assert(h3.is_agari(), "h3 can win");
    assert(!h4.is_agari(), "h4 cannot win");
    assert(h4.is_tenpai(), "h4 is in tenpai");
    assert(h5.is_tenpai(), "h5 is in tenpai");
    assert(!h6.is_tenpai(), "h6 is not in tenpai");
    assert(h6.shanten() == 1,
           "h6 should have shanten 1, but got " + std::to_string(h6.shanten()));
    assert(h7.shanten() == 2,
           "h7 should have shanten 2, but got " + std::to_string(h7.shanten()));
}

TEST t_Pong() {}

int main()
{
    t_Closed();

    return g_FailureCount;
}
