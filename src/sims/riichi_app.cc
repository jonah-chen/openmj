
#include "riichi.hpp"

int main()
{
    // create a random number generator
    std::mt19937 rng{10};
    mj::sim::RiichiState state;
    state.hand = mj::Hand("m123p456s1134567", mj::k_West);
    state.hand.flags =
        (mj::scoring::f_NormalPlay & mj::scoring::f_ClosedHandMask) |
        mj::scoring::f_Riichi1 | mj::scoring::f_EastP | mj::scoring::f_WestS;
    state.defense_prob = {1.0f, 1.0f, 0.5f};
    // state.set_defense(2.5f / 3.0f);
    state.furiten = false;
    // run the simulation
    auto [a, b, c, d] = mj::sim::riichi(state, -1, 1000000u / 8, -1);

    auto amax = *std::max_element(a.begin(), a.end());
    if (amax > 0)
    {
        for (auto [idx, count] : mj::enumerate(a))
        {
            std::cout << std::setw(2) << idx << ": ";
            for ([[maybe_unused]] auto i : mj::range(20 * count / amax))
                std::cout << "*";
            std::cout << "\n";
        }
    }
    std::cout << "\nScores\n";
    auto max_for_map = std::max_element(b.begin(), b.end(), [](auto &a, auto &b) {
        return a.second < b.second;
    })->second;
    for (auto [score, count] : b)
    {
        std::cout << std::setw(5) << score << " (" << std::setw(6) << count << "): ";
        for ([[maybe_unused]] auto i : mj::range(20 * count / max_for_map))
            std::cout << "*";
        std::cout << "\n";
    }
    return 0;
}
