#include "riichi.hpp"

namespace mj {
namespace sim {
RiichiState::output_type iterative_riichi(RiichiState::rng_type &rng,
                                          RiichiState cfg, U32f iters,
                                          S8f tiles_left)
{
    MJ_ASSERT(cfg.hand.check(scoring::f_Riichi), "riichi must be enabled");
    MJ_ASSERT_CRIT(*std::max_element(cfg.defense_prob.begin(),
                                     cfg.defense_prob.end()) <= 1.0f,
                   "defense probability must be <= 1.0");
    MJ_ASSERT_CRIT(*std::min_element(cfg.defense_prob.begin(),
                                     cfg.defense_prob.end()) >= 0.0f,
                   "defense probability must be >= 0.0");
    Hand4Hot full_wall;
    std::fill(full_wall.begin(), full_wall.end(), 4);

    full_wall -= cfg.hand.hand_4hot_melds();
    full_wall -= cfg.dead_tiles;

    if (tiles_left == -1)
        tiles_left = full_wall.n_tiles() - k_DeadWallSize;
    auto draws = (tiles_left - 3 * (k_MaxHandSize - 1)) / 4;

    RiichiState::count_type counts{};
    RiichiState::score_type scores{};
    RiichiState::detail_type detail{};
    RiichiState::yakus_type yakus{};
    Dir m_player = cfg.hand.player();

    for ([[maybe_unused]] auto _ : range(iters))
    {
        random::Rng34 wall(rng, full_wall);
        std::uniform_real_distribution<F32> dist(0.0f, 1.0f);
        if (!cfg.other_hand_known && !cfg.furiten)
            for (auto &hand : cfg.other_hand)
                wall(k_MaxHandSize - 1, hand);

        bool won = false;

        for (auto i : range(draws))
        {
            Dir player = m_player;
            for ([[maybe_unused]] auto j : range(k_NumPlayers))
            {
                Hand hand = cfg.hand;
                Tile tile = convert34(wall());
                tile.set_dir(player);

                bool check_win = true;
                auto d = diff(player, m_player);
                if (d != 0 && dist(rng) < cfg.defense_prob[d - 1])
                {
                    auto &other = cfg.other_hand[d - 1];
                    other[tile]++; // draw
                    for (auto t : range(k_UniqueTiles))
                    {
                        if (other[t] && cfg.my_discards[t])
                        {
                            check_win = false;
                            other[t]--; // discard safe tile (if any)
                            break;
                        }
                    }
                    other[tile]--; // discard drawn tile (no safe tiles)
                }

                if (check_win)
                {
                    hand.emplace_back(tile);
                    // check how high we can score
                    auto wins = hand.agari();

                    if (wins.empty())
                    {
                        cfg.my_discards[tile.id34()]++;
                    }
                    else
                    {
                        auto [score, yaku] = scoring::score_hand(hand, tile);
                        score = scoring::points::win(score, m_player == k_East,
                                                     m_player == player);

                        // add to the cumulative counts
                        counts[i]++;
                        auto &detail_i = detail[i];
                        scores.add(score);
                        detail_i.add(score);
                        yakus.add(yaku);
                        won = true;
                    }
                }

                if (won || cfg.furiten)
                    break;

                player = next(player);
            }

            if (won)
                break;
        }
    }

    return std::make_tuple(counts, scores, detail, yakus);
}

RiichiState::output_type riichi(const RiichiState &cfg, S32f num_workers,
                                U32f iters, S8f tiles_left,
                                RiichiState::seed_type *_seeds)
{
    MJ_ASSERT_CRIT(cfg.hand.check(scoring::f_Riichi), "riichi must be enabled");
    MJ_ASSERT_CRIT(*std::max_element(cfg.defense_prob.begin(),
                                     cfg.defense_prob.end()) <= 1.0f,
                   "defense probability must be <= 1.0");
    MJ_ASSERT_CRIT(*std::min_element(cfg.defense_prob.begin(),
                                     cfg.defense_prob.end()) >= 0.0f,
                   "defense probability must be >= 0.0");

    auto max_workers = std::thread::hardware_concurrency();
    if (num_workers > max_workers)
        num_workers = max_workers;
    else if (num_workers == -1)
        num_workers = max_workers;

    auto *seeds = _seeds ? _seeds : new RiichiState::seed_type[num_workers];
    if (_seeds == nullptr)
        for (auto i : range(num_workers))
            seeds[i] = std::random_device{}();

    std::vector<std::future<RiichiState::output_type>> futures;
    for (auto i : range(num_workers))
    {
        auto seed = seeds[i];
        futures.emplace_back(
            std::async(std::launch::async, [seed, cfg, iters, tiles_left]() {
                RiichiState::rng_type rng(seed);
                return iterative_riichi(rng, cfg, iters, tiles_left);
            }));
    }

    RiichiState::count_type counts{};
    RiichiState::score_type scores{};
    RiichiState::detail_type detail{};
    RiichiState::yakus_type yakus{};

    for (auto &future : futures)
    {
        auto &&[c, s, d, y] = future.get();
        counts += c;
        scores += s;
        detail += d;
        yakus += y;
    }

    if (_seeds == nullptr)
        delete[] seeds;

    return std::make_tuple(counts, scores, detail, yakus);
}

} // namespace sim
} // namespace mj
