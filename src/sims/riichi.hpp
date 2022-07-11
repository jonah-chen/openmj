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
        (k_DeckSize - k_DeadWallSize - (k_MaxHandSize - 1) * 4) / 4 + 1;

    // win on N turns
    using count_type = mod_array<U32f, k_MaxTurns>;

    // key: score, value: wins with that score
    using score_type = count_map<U32f>;

    using detail_type = mod_array<score_type, k_MaxTurns>;

    using yakus_type = count_map<U64>;

    using output_type =
        std::tuple<count_type, score_type, detail_type, yakus_type>;

    RiichiState() = default;
    explicit RiichiState(const char *hand_str, Dir player = k_East)
        : hand(hand_str, player)
    {
    }

    /**
     * @brief Set the defense probability for all players defending.
     * 
     * @param prob the probability of defending.
     */
    constexpr void set_defense(float prob) MJ_EXCEPT_CRIT
    {
        MJ_ASSERT_CRIT(prob >= 0.0f && prob <= 1.0f, "Invalid probability");
        std::fill(defense_prob.begin(), defense_prob.end(), prob);
    }

    Hand hand;
    Dir prevailing_wind{k_East};
    Hand4Hot dead_tiles{};
    Hand4Hot my_discards{}; // these tiles must be included in dead_tiles

    std::array<Hand4Hot, k_NumPlayers - 1>
        other_hand{}; // these tiles must be in dead_tiles
    std::array<float, k_NumPlayers - 1>
        defense_prob{}; // these tiles must be included in dead_tiles

    // TileWeights discard_prob;
    // TileWeights hold_prob;
    bool furiten{true};
    bool other_hand_known{false};
};

// TODO: as of now, this assumes opponents tsumogiri
template <typename RngType>
RiichiState::output_type iterative_riichi(RngType &rng, RiichiState cfg,
                                          U32f iters = 1, S8f tiles_left = -1)
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

template <typename SeedType, typename RngType = std::mt19937>
RiichiState::output_type riichi(const RiichiState &cfg, U32f num_workers = 1,
                                U32f iters = 1, S8f tiles_left = -1,
                                SeedType *_seeds = nullptr)
{
    MJ_ASSERT_CRIT(cfg.hand.check(scoring::f_Riichi), "riichi must be enabled");
    MJ_ASSERT_CRIT(*std::max_element(cfg.defense_prob.begin(),
                                cfg.defense_prob.end()) <= 1.0f,
              "defense probability must be <= 1.0");
    MJ_ASSERT_CRIT(*std::min_element(cfg.defense_prob.begin(),
                                cfg.defense_prob.end()) >= 0.0f,
              "defense probability must be >= 0.0");


    SeedType *seeds = _seeds ? _seeds : new SeedType[num_workers];
    if (_seeds == nullptr)
        for (auto i : range(num_workers))
            seeds[i] = std::random_device{}();

    std::vector<std::future<RiichiState::output_type>> futures;
    for (auto i : range(num_workers))
    {
        auto seed = seeds[i];
        futures.emplace_back(std::async(
            std::launch::async, [seed, cfg, iters, tiles_left]() {
                RngType rng(seed);
                return iterative_riichi(rng, cfg, iters, tiles_left);
            }));
    }

    RiichiState::count_type counts{};
    RiichiState::score_type scores{};
    RiichiState::detail_type detail{};
    RiichiState::yakus_type yakus{};

    for (auto &future : futures)
    {
        auto [c, s, d, y] = future.get();
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
