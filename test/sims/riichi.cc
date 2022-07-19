
#include "sims/riichi.hpp"
#include "test/test.hpp"

constexpr mj::U32f iters = 1009;

static mj::random::WallContainer34 find_order(mj::sim::RiichiState state)
{
    std::mt19937 _rng{10};
    mj::Hand4Hot full_wall;
    std::fill(full_wall.begin(), full_wall.end(), 4);
    full_wall -= state.hand.hand_4hot_melds();
    mj::random::Rng34 rng(_rng, full_wall);
    // we will draw away some tiles
    auto num_tiles = std::accumulate(state.other_hand_size.begin(),
                                     state.other_hand_size.end(), 0);
    rng(num_tiles);
    // this is not a good way to do things
    // because the order of the sampling from RNG may not be random
}

static auto sanity_test(mj::sim::RiichiState cfg)
{
    std::vector<mj::sim::RiichiState::seed_type> seeds;
    for (auto i : mj::range(std::thread::hardware_concurrency()))
        seeds.push_back(i + 100);

    using namespace mj::sim;
    auto out = riichi(cfg, -1, iters, -1, seeds.data());
    auto [counts, scores, detail, yakus] = out;
    // test that detail contains same scores as score
    for (auto i : mj::range(counts.size()))
    {
        mj::U32f total_occurence = 0;
        for (const auto &[score, occurence] : detail[i])
        {
            total_occurence += occurence;
            // check score is a key of scores
            assert(scores.find(score) != scores.end(),
                   "score must be a key of scores");
        }
        assert(total_occurence == counts[i], "total occurence should be equal");
    }
    return out;
}

TEST t_RiichiBasic()
{
    std::minstd_rand _rng{10};
    mj::sim::RiichiState state("m111222333s3w222", mj::k_North);
    state.prevailing_wind = mj::k_South;
    state.furiten = false;
    mj::sim::iterative_riichi(_rng, state);
}

TEST t_RiichiTsumo()
{
    const auto iters = 10007;
    std::vector<mj::sim::RiichiState::seed_type> seeds;
    for (auto i : mj::range(std::thread::hardware_concurrency()))
        seeds.push_back(i + 100);

    mj::sim::RiichiState cfg("m12345678s345w33", mj::k_North);
    {
        auto [counts, scores, detail, yakus] = sanity_test(cfg);
        using namespace mj::scoring;
        std::array<mj::U64, 2> gt_yakus = {
            20 | f_Riichi1 | f_Pinfu | f_MenTsumo,
            20 | f_Riichi1 | f_Pinfu | f_IttsuC | f_MenTsumo};
        for (auto y : gt_yakus)
            assert(yakus.find(y) != yakus.end(),
                   "yaku " + yaku_str(y) + " not found.");
        assert_eq(yakus.size(), gt_yakus.size());
    }
    {
        cfg.furiten = false;
        auto [counts, scores, detail, yakus] = sanity_test(cfg);
        using namespace mj::scoring;
        std::array<mj::U64, 4> gt_yakus = {
            30 | f_Riichi1 | f_Pinfu, 30 | f_Riichi1 | f_Pinfu | f_IttsuC,
            20 | f_Riichi1 | f_Pinfu | f_MenTsumo,
            20 | f_Riichi1 | f_Pinfu | f_IttsuC | f_MenTsumo};
        for (auto y : gt_yakus)
            assert(yakus.find(y) != yakus.end(),
                   "yaku " + yaku_str(y) + " not found.");
        assert_eq(yakus.size(), gt_yakus.size());
    }
}

int main()
{
    t_RiichiTsumo();
    return g_FailureCount;
}