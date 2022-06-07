
#include "test/test.hpp"
#include "core/mahjong/mahjong.hpp"

TEST t_FalsyTile()
{
    mj::Tile t1;
    assert(!t1, "Truthy invalid tile");
}

TEST t_1Man()
{
    constexpr mj::Tile t2(mj::Suit::Man, mj::c_num(1), mj::k_East);
    constexpr mj::Tile t3(mj::Suit::Man, mj::c_num(1), mj::k_South);
    constexpr mj::Tile t4(mj::Suit::Man, mj::c_num(1), mj::k_East, mj::tilelayout::f_Transparent);

    assert(t2, "Falsy valid 1 man");
    assert(t2.suit() == mj::Suit::Man, "suit() should be Man");
    assert(t2.num() == 0, "num() should be 0 for 1man");
    assert(t2.num1() == 1, "num1() should be 1 for 1man");
    assert(t2.player() == mj::k_East, "player() should be East");
    
    assert(t2 != t3, "t2 should not be equal to t3");
    assert(t2 != t4, "t2 should not be equal to t4");
    assert(t2.eq7(t3) && t3.eq7(t2), "t2 should be 7-bit equal to t3");
    assert(t2.eq7(t4) && t4.eq7(t2), "t2 should be 7-bit equal to t4");

    assert(t2 < t4, "1 man should come before 2 man");
    assert(t4 > t2, "2 man should come after 1 man");
}

TEST t_NorthWind()
{
    constexpr mj::Tile t5(mj::Suit::Wind, mj::k_North, mj::k_North);
    assert(t5.suit() == mj::Suit::Wind, "suit() should be Wind");
}

int main()
{
    t_FalsyTile();
    t_1Man();
    t_NorthWind();

    return g_FailureCount;
}