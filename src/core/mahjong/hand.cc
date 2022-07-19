
#include "hand.hpp"
#include "compact_helpers.hpp"
#include "extern/shanten-number/calsht.hpp"

namespace mj {
namespace {
using Pair_s = std::variant<int, cPairs>;
using Runs4Hot = std::array<U8f, k_FirstHonorIdx>;
using Sets4Hot = std::array<bool, 34>;
using Triples = std::pair<Runs4Hot, Sets4Hot>;
using cMeldsI = std::pair<cMelds, Triples>;
using TempTrip = vector<Meld, 4>;

constexpr Pair_s pairs(const Hand4Hot &h4)
{
    cPairs pairs;
    for (Suit s = Suit::Man; s < Suit::Wind; ++s)
    {
        const U8f s9 = static_cast<U8f>(s) * 9;
        // for 1s
        if (h4[s9] == 2 && h4[s9 + 1] == 0)
            return s9;
        else if (h4[s9] >= 2)
            pairs.push_back(s9);
        for (U8f i = 1; i < 8; ++i)
        {
            if (h4[s9 + i] == 2 && h4[s9 + i - 1] == 0 && h4[s9 + i + 1] == 0)
                return s9 + i;
            else if (h4[s9 + i] >= 2)
                pairs.push_back(s9 + i);
        }
        if (h4[s9 + 8] == 2 && h4[s9 + 7] == 0)
            return s9 + 8;
        else if (h4[s9 + 8] >= 2)
            pairs.push_back(s9 + 8);
    }
    for (U8f t = k_FirstHonorIdx; t < k_UniqueTiles; ++t)
    {
        if (h4[t] == 2)
            return t;
        else if (h4[t] > 2)
            pairs.push_back(t);
    }
    if (pairs.size() == 1)
        return pairs.back();
    return pairs;
}

std::optional<cPairs> seven_pairs(const Hand4Hot &h4)
{
    cPairs p;
    for (U8f t = 0; t < k_UniqueTiles; ++t)
    {
        if (h4[t] == 2)
            p.push_back(t);
        else if (h4[t])
            return std::nullopt;
    }
    return p;
}

constexpr Triples triples(const Hand4Hot &h4)
{
    // test for sets
    Sets4Hot sets{};
    for (U8f i = 0; i < k_UniqueTiles; ++i)
        if (h4[i] >= 3)
            sets[i] = true;
    // test for runs
    Runs4Hot runs{};
    for (Suit s = Suit::Man; s < Suit::Wind; ++s)
    {
        const U8f s9 = static_cast<U8f>(s) * 9;
        for (U8f n = 0; n < 7; ++n)
        {
            U8f p = s9 + n;
            runs[p] = std::min({h4[p], h4[p + 1], h4[p + 2]});
        }
    }
    return {runs, sets};
}

std::variant<cMelds, cMeldsI> perms(Hand4Hot &h4, U8f N)
{
    cMelds all_perms;
    while (all_perms.size() < N)
    {
        cMelds new_perms;
        auto [runs, sets] = triples(h4);
        for (Suit s = Suit::Man; s < Suit::Wind; ++s)
        {
            const U8f s9 = static_cast<U8f>(s) * 9;
            if (runs[s9] && h4[s9] != 3) // 123 run.
                                         // Only not perm when there's 3 1's
                new_perms.push_back(_run(s9));

            for (int i = 1; i < 6; ++i) // 234 ... 678 runs
                if (runs[s9 + i] && ((h4[s9 + i - 1] == 0 && h4[s9 + i] != 3) ||
                                     (h4[s9 + i + 3] == 0 &&
                                      h4[s9 + i + 2] != 3))) // check both sides
                    new_perms.push_back(_run(s9 + i));

            if (runs[s9 + 6] &&
                h4[s9 + 8] != 3) // 789 run.
                                 // Only not perm when there's 3 9's
                new_perms.push_back(_run(s9 + 6));

            if (sets[s9] && runs[s9] < 3) // 1 set
                new_perms.push_back(_set(s9));
            if (sets[s9 + 1] &&
                (h4[s9 + 2] == 0 || (h4[s9] == 0 && runs[s9 + 1] < 3))) // 2
                new_perms.push_back(_set(s9 + 1));
            for (int i = 2; i < 7; ++i) // 34567
            {
                bool upper = h4[s9 + i - 1] == 0 && runs[s9 + i] < 3;
                bool lower = h4[s9 + i + 1] == 0 && runs[s9 + i - 2] < 3;
                if (sets[s9 + i] && (upper || lower))
                    new_perms.push_back(_set(s9 + i));
            }
            if (sets[s9 + 7] &&
                (h4[s9 + 6] == 0 || (h4[s9 + 8] == 0 && runs[s9 + 5] < 3))) // 8
                new_perms.push_back(_set(s9 + 7));
            if (sets[s9 + 8] && runs[s9 + 6] < 3) // 9
                new_perms.push_back(_set(s9 + 8));
        }
        // sets of honors only have to be searched once
        if (all_perms.empty())
            for (U8f t = k_FirstHonorIdx; t < k_UniqueTiles; ++t)
                if (sets[t])
                    new_perms.push_back(_set(t));

        // If there are no new perms, there's nothing we can do so just return.
        if (new_perms.empty())
            return cMeldsI(all_perms, {runs, sets});

        // subtract the new perms from the hand, add them to all_perms, try
        // again.
        for (int perm : new_perms)
        {
            all_perms.push_back(perm);
            if (_is_set(perm))
                h4[_int(perm)] -= 3;
            else
                for (int i = _int(perm); i < _int(perm) + 3; ++i)
                    h4[i] -= 1;
        }
    }
    return all_perms;
}

void normal_win(Hand4Hot h4, int pair, U8f n_melds, Wins &wins)
{
    h4[pair] -= 2;
    // check if it is a win at all by adding a dummy honor pair
    for (int t = k_UniqueTiles - 1; t >= k_FirstHonorIdx; --t)
    {
        if (h4[t] == 0)
        {
            h4[t] = 2;
            if (mj::shanten(h4, n_melds, k_ModeNormal) == -1)
            {
                h4[t] = 0;
                break;
            }
            else
                return;
        }
    }
    U8f N = k_MaxNumMeld - n_melds;
    auto p = perms(h4, N);
    auto *ptr = std::get_if<cMelds>(&p);
    if (ptr)
    {
        wins.push_back(std::move(NormalWin(std::move(*ptr), pair)));
    }
    else
    {
        auto [melds, trips] = std::get<cMeldsI>(p);
        auto [runs, sets] = trips;
        if (N - melds.size() == 3)
        {
            auto pos = std::distance(runs.begin(),
                                     std::find(runs.begin(), runs.end(), 3));
            MJ_ASSERT(pos != runs.size(), "no 3-run found should never happen");

            auto cpy_melds = melds;
            for (int i = 0; i < 3; ++i)
            {
                cpy_melds.push_back(_run(pos));
                melds.push_back(_set(pos + i));
            }
            wins.emplace_back(NormalWin(cpy_melds, pair));
            wins.emplace_back(NormalWin(melds, pair));
        }
        else if (N - melds.size() == 4)
        {
            auto it4 = std::find(runs.begin(), runs.end(), 4);
            if (it4 != runs.end())
            {
                auto pos = std::distance(runs.begin(), it4);
                melds.push_back(_run(pos));
                auto cpy_melds = melds;
                for (int i = 0; i < 3; ++i)
                {
                    cpy_melds.push_back(_set(pos + i));
                    melds.push_back(_run(pos));
                }
                wins.emplace_back(NormalWin(melds, pair));
                wins.emplace_back(NormalWin(cpy_melds, pair));
            }
            else
            {
                auto pos = std::distance(
                    runs.begin(), std::find(runs.begin(), runs.end(), 3));
                MJ_ASSERT(pos != runs.size(),
                          "no 3-run found should never happen after 4-run");
                auto melds_b = melds;
                auto melds_e = melds;
                melds_b.push_back(_set(pos));
                melds_e.push_back(_set(pos + 1));
                for (int i = 0; i < 3; ++i)
                {
                    melds.push_back(_set(pos + i));
                    melds_b.push_back(_run(pos + 1));
                    melds_e.push_back(_run(pos));
                }
                melds.push_back(_set(pos + 3));
                wins.emplace_back(NormalWin(melds, pair));
                wins.emplace_back(NormalWin(melds_b, pair));
                wins.emplace_back(NormalWin(melds_e, pair));
            }
        }
        else
            MJ_ALWAYS_THROW(true, std::runtime_error,
                            "unexpected number of unidentified melds" +
                                std::to_string(N - melds.size()));
    }
}

void tenpai_win_impl(U8f offset, WaitingTiles &res, Hand4Hot &h4, U8f n_melds,
                     Suit suit, U8f num, bool kokushi_possible)
{
    using namespace ext::tomohxx;
    if (++h4[offset] <= 4 &&
        -1 == shanten(h4, n_melds, k_ModeNormal | k_ModeChiitoi))
        res.emplace_back(suit, num);
    else if (kokushi_possible && -1 == shanten(h4, n_melds, k_ModeKokushi))
        res.emplace_back(suit, num);
    h4[offset]--;
}

} // namespace

S8 shanten(const Hand4Hot &h4, U8f n_melds, int mode)
{
    static ext::tomohxx::Calsht sht("../assets/tables");
    auto [num, _] = sht(h4, k_MaxNumMeld - n_melds, mode);
    return num - 1;
}

namespace _hand_impl {
template <U8f N>
Hand<N>::Hand(const char *str, Dir dir) : tiles4_(), tiles4m_()
{
    constexpr const char *suits = "mpswd";
    U8f cur_suit = 0;
    for (; *str; ++str)
    {
        if (*str < '1' || *str > '9')
        {
            while (suits[cur_suit] != *str)
                if (++cur_suit == 5)
                    return;
        }
        else
        {
            MJ_ASSERT(size() < N, "too many tiles in hand. use a bigger varient"
                                  " of hand like BigHand instead");
            emplace_back(Suit(cur_suit), *str - '1', dir);
        }
    }
    sorted_ = true;
}

template <U8f N>
Wins Hand<N>::agari() const
{
    Wins wins;
    if (n_melds() == 0)
    {
        if (mj::shanten(hand_4hot(), 0, k_ModeKokushi) == -1)
        {
            wins.emplace_back(true);
            return wins;
        }
        auto sp = seven_pairs(hand_4hot());
        if (sp)
            wins.emplace_back(sp.value());
    }
    Pair_s p = pairs(hand_4hot());
    int *pair_ptr = std::get_if<int>(&p);
    if (pair_ptr)
        normal_win(hand_4hot(), *pair_ptr, n_melds(), wins);
    else
        for (const auto &pair : std::get<cPairs>(p))
            normal_win(hand_4hot(), pair, n_melds(), wins);
    return wins;
}

template <U8f N>
WaitingTiles Hand<N>::tenpai() const
{
    if (size() + 3u * n_melds() < k_MaxHandSize - 1u)
        return {};
    if (!is_tenpai())
        return {};
    WaitingTiles waiting;
    Hand4Hot h4(hand_4hot());
    bool kokushi_possible = n_melds();
    // U8f chitoi_possible = n_melds() ? 0 : 2;
    // check normal tile waits first
    for (Suit s = Suit::Man; s < Suit::Wind; ++s)
    {
        U8f s9 = static_cast<U8f>(s) * 9;

        // deal with n=0
        if (h4[s9] || h4[s9 + 1])
            tenpai_win_impl(s9, waiting, h4, n_melds(), s, 0, kokushi_possible);

        for (int n = 1; n < 8; ++n)
        {
            if (h4[s9 + n - 1] || h4[s9 + n] || h4[s9 + n + 1])
            {
                using namespace ext::tomohxx;
                if (h4[s9 + n]++)
                    kokushi_possible = false;
                if (h4[s9 + n] <= 4 &&
                    -1 == mj::shanten(h4, n_melds(),
                                      k_ModeNormal | k_ModeChiitoi))
                    waiting.emplace_back(s, n);
                h4[s9 + n]--;
            }
        }
        if (h4[s9 + 7] || h4[s9 + 8])
            tenpai_win_impl(s9 + 8, waiting, h4, n_melds(), s, 8,
                            kokushi_possible);
    }
    // check wind waits
    constexpr U8f k_WindOffset = Tile::k_Offsets[static_cast<U8f>(Suit::Wind)];
    constexpr U8f k_DragonOffset =
        Tile::k_Offsets[static_cast<U8f>(Suit::Dragon)];
    for (int n = 0; n < k_NumWinds; ++n)
    {
        U8f offset = k_WindOffset + n;
        if (h4[offset])
            tenpai_win_impl(offset, waiting, h4, n_melds(), Suit::Wind, n,
                            kokushi_possible);
    }
    // check dragon waits
    for (int n = 0; n < k_NumDragons; ++n)
    {
        U8f offset = k_DragonOffset + n;
        if (h4[offset])
            tenpai_win_impl(offset, waiting, h4, n_melds(), Suit::Dragon, n,
                            kokushi_possible);
    }

    return waiting;
}

template <U8f N>
S8 Hand<N>::shanten() const
{
    return mj::shanten(hand_4hot(), n_melds(), k_ModeAll);
};

template class Hand<k_MaxHandSize>;
template class Hand<k_DeckSize>;

} // namespace _hand_impl
} // namespace mj
