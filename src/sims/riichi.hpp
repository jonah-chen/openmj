/**
 * Riichi simulator.
 */

#include "core/adt/count_map.hpp"
#include "core/mahjong/mahjong.hpp"
#include "core/mahjong/rng.hpp"

namespace mj {
namespace sim {
struct RiichiState
{
    constexpr static std::size_t k_MaxTurns =
        (k_DeckSize - k_DeadWallSize) / 4 + 1;

    // win on N turns
    using count_type = std::array<U32f, k_MaxTurns>;

    // key: score, value: wins with that score
    using score_type = count_map<U32f>;

    using detail_type = std::array<score_type, k_MaxTurns>;

    using yakus_type = count_map<U64>;

    using output_type =
        std::tuple<count_type, score_type, detail_type, yakus_type>;

    Hand hand;
    Dir prevailing_wind {k_East};
    Hand4Hot dead_tiles {};
    Hand4Hot my_discards {};
    // TileWeights discard_prob;
    // TileWeights hold_prob;
    bool furiten {true};
};

// TODO: as of now, this assumes opponents tsumogiri
template <typename RngType>
RiichiState::output_type iterative_riichi(RngType &rng, RiichiState state,
                                          U32f iters = 1, S8f tiles_left = -1)
{
    MJ_ASSERT(state.hand.check(scoring::f_Riichi), "riichi must be enabled");
    // subtract the hand and dead tiles from a full wallcontainer34
    Hand4Hot full_wall;
    std::fill(full_wall.begin(), full_wall.end(), 4);
    for (Tile tile : state.hand)
        full_wall[tile.id34()]--;
    for (auto [idx, num] : enumerate(state.dead_tiles))
        full_wall[idx] -= num;

    // count tiles in full_wall
    if (tiles_left == -1)
        tiles_left = std::accumulate(full_wall.begin(), full_wall.end(), 0) -
                     k_DeadWallSize;
    auto draws = tiles_left / 4;

    RiichiState::count_type counts{};
    RiichiState::score_type scores{};
    RiichiState::detail_type detail{};
    RiichiState::yakus_type yakus{};
    Dir m_player = state.hand.player();

    for (auto _ : range(iters))
    {
        random::Rng34 wall(rng, full_wall);
        bool won = false;

        for (auto i : range(draws))
        {
            Dir player = m_player;
            for (auto j : range(k_NumPlayers))
            {
                // make a copy of hand
                Hand hand = state.hand;
                // draw a tile from the wall
                Tile tile = convert34(wall());
                tile.set_dir(player);

                hand.emplace_back(tile);
                // check how high we can score
                auto wins = hand.agari();

                if (!wins.empty())
                {
                    // there is a win
                    auto [score, yaku] = scoring::score_hand(hand, tile);
                    score = scoring::points::win(score, m_player==k_East, m_player==player);


                    // add to the cumulative counts
                    counts[i]++;
                    auto &detail_i = detail[i];
                    scores.add(score);
                    detail_i.add(score);
                    yakus.add(yaku);
                    won = true;
                }

                if (won || state.furiten)
                    break;

                player = next(player);
            }

            if (won)
                break;
        }
    }

    return std::make_tuple(counts, scores, detail, yakus);
}

template <typename SeedType, typename RngType = std::mt19937>
RiichiState::output_type riichi(RiichiState state, U32f num_workers = 1,
                                U32f iters = 1, S8f tiles_left = -1,
                                SeedType *_seeds = nullptr)
{
    SeedType *seeds = _seeds ? _seeds : new SeedType[num_workers];
    if (_seeds == nullptr)
        for (auto i : range(num_workers))
            seeds[i] = std::random_device{}();

    std::vector<std::future<RiichiState::output_type>> futures;
    for (auto i : range(num_workers))
    {
        auto seed = seeds[i];
        futures.emplace_back(
            std::async(std::launch::async, [state, iters, tiles_left, seed]() {
                RngType rng(seed);
                return iterative_riichi(rng, state, iters, tiles_left);
            }));
    }

    RiichiState::count_type counts{};
    RiichiState::score_type scores{};
    RiichiState::detail_type detail{};
    RiichiState::yakus_type yakus{};

    for (auto &future : futures)
    {
        auto [c, s, d, y] = future.get();
        // add the 2 arrays c
        std::transform(c.begin(), c.end(), counts.begin(), counts.begin(),
                       std::plus<>());
        // add the 2 maps s
        scores += s;
        // add the 2 arrays d
        std::transform(d.begin(), d.end(), detail.begin(), detail.begin(),
                       std::plus<>());
        // add the 2 maps y
        yakus += y;
    }

    if (_seeds == nullptr)
        delete[] seeds;

    return std::make_tuple(counts, scores, detail, yakus);
}

} // namespace sim
} // namespace mj