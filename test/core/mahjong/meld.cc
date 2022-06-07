
#include "test/test.hpp"
#include "core/mahjong/mahjong.hpp"

TEST t_EmptyMeld()
{
    using namespace mj;
    constexpr Meld empty;
    assert(empty.first() == Tile() && 
            empty.second() == Tile() &&
            empty.third() == Tile() &&
            empty.fourth() == Tile(), "empty meld should be all empty");
    
    assert(!empty, "empty meld should be falsy");
}

TEST t_BasicMeld()
{
    using namespace mj;
    constexpr Tile t1(Suit::Man, c_num(1));
    constexpr Tile t2(Suit::Man, c_num(2));
    constexpr Tile t3(Suit::Man, c_num(3));
    constexpr Tile t4(Suit::Dragon, k_Green);
    constexpr Tile t5(Suit::Dragon, k_Green, tilelayout::f_Transparent);

    constexpr Meld pair(t1, t2);
    constexpr Meld triple(t1, t2, t3);
    constexpr Meld quad(t1, t2, t3, t4);

    assert(pair.first() == t1, "pair.first() should be t1");
    assert(pair.second() == t2, "pair.second() should be t2");
    assert(triple.first() == t1, "triple.first() should be t1");
    assert(triple.second() == t2, "triple.second() should be t2");
    assert(triple.third() == t3, "triple.third() should be t3");
    assert(quad.first() == t1, "quad.first() should be t1");
    assert(quad.second() == t2, "quad.second() should be t2");
    assert(quad.third() == t3, "quad.third() should be t3");
    assert(quad.fourth() == t4, "quad.fourth() should be t4");

    assert(pair, "non-empty should be truthy");

    constexpr Meld pairOpq(t4, t4);
    constexpr Meld pairTrans(t4, t5);
    assert(pairOpq != pairTrans, "differing flags should cause inequality");
    assert(pairOpq.eq7(pairTrans), "differing flags should be 7-bit equal");
}

int main()
{
    t_EmptyMeld();
    t_BasicMeld();
    return g_FailureCount;
}
