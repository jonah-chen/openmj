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
    Hand hand;
    Dir prevailing_wind{k_East};
    Hand4Hot dead_tiles{};
    Hand4Hot my_discards{}; // these tiles must be included in dead_tiles

    std::array<Hand4Hot, k_NumPlayers - 1>
        other_hand{}; // these tiles must be in dead_tiles
    std::array<float, k_NumPlayers - 1>
        defense_prob{}; // these tiles must be included in dead_tiles
    std::array<U8, k_NumPlayers - 1> other_hand_size{};
    // TileWeights discard_prob;
    // TileWeights hold_prob;
    bool furiten{true};
    bool other_hand_known{false};


    using rng_type = std::minstd_rand;
    using seed_type = rng_type::result_type;

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

    RiichiState()
    {
        std::fill(other_hand_size.begin(), other_hand_size.end(),
                  k_MaxHandSize - 1);
    }
    explicit RiichiState(const char *hand_str, Dir player = k_East)
        : hand(hand_str, player)
    {
        if (hand.ec)
            return;
        std::fill(other_hand_size.begin(), other_hand_size.end(),
                  k_MaxHandSize - 1);
        hand.flags = mj::scoring::f_NormalPlay & mj::scoring::f_ClosedHandMask;
        hand.set(scoring::f_Riichi1);
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

};

RiichiState::output_type iterative_riichi(RiichiState::rng_type &rng,
                                          RiichiState cfg, U32f iters = 1,
                                          S8f tiles_left = -1);

RiichiState::output_type riichi(const RiichiState &cfg, S32f num_workers = -1,
                                U32f iters = 1, S8f tiles_left = -1,
                                RiichiState::seed_type *_seeds = nullptr);

} // namespace sim
} // namespace mj
