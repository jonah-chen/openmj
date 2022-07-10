
#include "riichi.hpp"

int main()
{
    // create a random number generator
    std::mt19937 rng{10};
    mj::sim::RiichiState state;
    state.hand = mj::Hand("m123p23s12355w222");
    state.hand.flags =
        (mj::scoring::f_NormalPlay & mj::scoring::f_ClosedHandMask) |
        mj::scoring::f_Riichi1;
    state.furiten = true;
    // run the simulation
    auto [a, b, c, d] = mj::sim::riichi<mj::U32f>(state, 8u, 100u);

    auto amax = *std::max_element(a.begin(), a.end());
    if (amax > 0)
    {
        for (auto [idx, count] : mj::enumerate(a))
        {
            std::cout << std::setw(2) << idx << ": ";
            for (int i = 0; i < 20 * count / amax; ++i)
                std::cout << "*";
            std::cout << "\n";
        }
    }
    for (auto [score, count] : b)
        std::cout << score << ": " << count << std::endl;
    return 0;
}
