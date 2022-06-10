
#include "test/test.hpp"
#include "core/mahjong/scoring.hpp"

int main()
{
    mj::scoring::ScoringCombo sc;
    mj::scoring::eval<mj::scoring::Yaku::riichi>(sc, mj::Hand(), mj::Win());
    return g_FailureCount;
}
