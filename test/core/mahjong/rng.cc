
#include "test/test.hpp"
#include "core/mahjong/rng.hpp"
#include "core/adt/itertools.hpp"

TEST reproduce()
{
    std::mt19937 rng1 { 10 }, rng2 { 10 };
    mj::random::Rng34 rng34_1 { rng1 };
    mj::random::Rng34 rng34_2 { rng2 };

    for ([[maybe_unused]] auto i : mj::range(mj::k_DeckSize))
    {
        assert(rng34_1() == rng34_2(), "RNGs should be reproducible");
    }
}

TEST count_std()
{
    std::mt19937 rng { 20 };
    mj::random::RngDeck rng_deck { rng };
    for (auto i : mj::range1(mj::k_DeckSize - mj::k_DeadWallSize))
        assert(rng_deck(mj::k_East), "Deck should have more than " + std::to_string(i) + " tiles");
    assert(!rng_deck(mj::k_East), "Deck should have less than " + std::to_string(mj::k_DeckSize - mj::k_DeadWallSize + 1) + " tiles");

    for (auto i : mj::range1(mj::k_DeadWallSize))
        assert(rng_deck.dora(), "Deck should have more than " + std::to_string(i) + " dora");
    assert(!rng_deck.dora(), "Deck should have less than " + std::to_string(mj::k_DeadWallSize + 1) + " dora");

    mj::random::RngDeck rng_deck_2 { rng };
    for (auto i : mj::range1(mj::k_DeadWallSize))
        assert(rng_deck_2.dora(), "Deck should have more than " + std::to_string(i) + " dora when drawn first");
    assert(!rng_deck_2.dora(), "Deck should have less than " + std::to_string(mj::k_DeadWallSize + 1) + " dora when drawn first");
    for (auto i : mj::range1(mj::k_DeckSize - mj::k_DeadWallSize))
        assert(rng_deck_2(mj::k_East), "Deck should have more than " + std::to_string(i) + " tiles");
    assert(!rng_deck_2(mj::k_East), "Deck should have less than " + std::to_string(mj::k_DeckSize - mj::k_DeadWallSize + 1) + " tiles");
}

TEST count_custom_wall()
{
    std::mt19937 rng { 30 };
    mj::Hand4Hot tiles, check{};
    std::fill_n(tiles.begin(), mj::k_UniqueTiles, 2);
    tiles[0] = 0;
    mj::random::Rng34 rng34 { rng, tiles };
    for ([[maybe_unused]] auto i : mj::range(mj::k_UniqueTiles * 2 - 2))
        check[rng34()]++;
    for (const auto &[idx, i] : mj::enumerate(check))
    {
        if (idx == 0)
            assert(i == 0, "For this, tile 0 should have 0 tiles.");
        else
            assert(i == 2, "For this, tile " + std::to_string(idx) + " should have 2 tiles.");
    }
}

int main()
{
    reproduce();
    count_std();
    count_custom_wall();

    return g_FailureCount;
}