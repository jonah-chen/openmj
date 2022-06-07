
#include "test/test.hpp"
#include "core/mahjong/mahjong.hpp"

TEST t_Construct()
{
    mj::Hand h1("m123345567p333sw22d");
    mj::Hand h2("m123345567p333w22");
    mj::Hand h3("s33344455566677");
    mj::Hand h4("m12334567p333w22");
    mj::Hand h5("w1112223344d222");
    mj::Hand h6("m1112223334689");

    assert_crit(h1.size() == 14, "h1 should have 14 tiles", 255);
    assert_crit(h2.size() == 14, "h2 should have 14 tiles", 255);
    for (int i = 0; i < 14; ++i)
        assert(h1[i] == h2[i], "h1 and h2 should be equal at idx " + std::to_string(i));
    
    assert(!h1.agari().empty(), "h1 can win");
    assert(!h3.agari().empty(), "h3 can win");
    assert(h4.agari().empty(), "h4 cannot win");
    assert(!h4.tenpai().empty(), "h4 is in tenpai");
    assert(!h5.tenpai().empty(), "h5 is in tenpai");
    assert(h6.tenpai().empty(), "h6 is not in tenpai");
}

int main()
{
    t_Construct();
    return g_FailureCount;
}
