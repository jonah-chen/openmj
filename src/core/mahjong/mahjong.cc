
#include "mahjong.hpp"
#include "extern/shanten-number/calsht.hpp"
#include <algorithm>

namespace mj {
namespace {
using Pairs = s_Vector<Meld, 7>;
using Triples = s_Vector<Meld, 32>;
using Combos = s_Vector<Melds, 32>;


Pairs pairs(const Hand &hand) 
{
    hand.sort();
    Pairs pairs;
    for (Fast8 i = 0; i < hand.size() - 1; ++i)
    {
        if (hand[i] == hand[i+1])
        {
            pairs.emplace_back(hand[i], hand[i+1]);
            ++i;
        }
        if (hand[i] == hand[i+1])
            ++i;
    }
    return pairs;
}

Triples triples(const Hand &hand)
{
    Triples triples;
    Fast8 idx = 0;
    const auto &h4 = hand.hand_4hot();
    // test for runs
    for (Suit s = Suit::Man; s < Suit::Wind; ++s)
    {
        const Fast8 s9 = static_cast<Fast8>(s)*9;
        for (Fast8 n = 0; n < 7; ++n)
        {
            Fast8 p = s9 + n;
            if (h4[p] && h4[p+1] && h4[p+2])
                triples.emplace_back(
                    hand[idx], hand[idx+h4[p]], hand[idx+h4[p]+h4[p+1]]);
            idx += h4[p];
        }
        idx += h4[s9+7] + h4[s9+8];
    }
    idx = 0;
    // test for sets
    for (Fast8 i = 0; i < k_UniqueTiles; ++i)
    {
        if (h4[i] >= 3)
            triples.emplace_back(hand[idx], hand[idx+1], hand[idx+2]);
        idx += h4[i];
    }
    return triples;
}

struct HandArray;

struct Node
{
    Meld meld;
    std::unique_ptr<HandArray> child;
    Node(const Meld &meld, std::unique_ptr<HandArray> &&child)
        : meld(meld), child(std::move(child))
    {}
};   

struct HandArray
{
    std::vector<Node> arr;
    Fast16 count {};
    CONSTEXPR12 bool empty() { return arr.empty(); }
    CONSTEXPR12 std::size_t size() const noexcept { return arr.size(); }
    void insert(const Meld &meld, std::unique_ptr<HandArray> &&child)
    {
        count += child->count;
        arr.emplace_back(meld, std::move(child));
    }
};


int_fast8_t traverse_tree(Hand4Hot &hand, Fast8 size, Meld branch)
{
    auto &first = hand[branch.first().id34()];
    auto &second = hand[branch.second().id34()];
    auto &third = hand[branch.third().id34()];
    if (size >= 3 && first && second && third)
    {
        --first;
        --second;
        --third;
        return size - 3;
    }
    return -1;
}

std::unique_ptr<HandArray> dfs(Hand4Hot &tiles, Fast8 size, const Meld *triples, Fast16 num_melds, Fast16 n)
{
    if (n == 0)
    {
        auto res = std::make_unique<HandArray>();
        res->count = 1;
        return res;
    }

    if (num_melds < n)
        return nullptr;

    std::unique_ptr<HandArray> children = nullptr;
    for (Fast16 i = 0; i <= num_melds - n; ++i)
    {
        Hand4Hot tmp_hand = tiles;
        int_fast8_t next = traverse_tree(tmp_hand, size, triples[i]);
        if (next == -1) continue;
        auto found = dfs(tmp_hand, next, triples+i+1, num_melds-i-1, n-1);
        if (!found) continue;
        children = std::make_unique<HandArray>();
        children->insert(triples[i], std::move(found));
    }

    return children;
}

void add_perms(const Melds &perms, Combos &results)
{
    for (const auto &perm : perms)
        for (Melds &result : results)
            result.push_back(perm);
}

void add_arrays(const std::unique_ptr<HandArray> &root, Combos &result, std::size_t offset)
{
    if (root->empty()) return;

    std::size_t cur_offset = offset;
    for (const auto &node : root->arr)
    {
        for (Fast16 j = 0; j < node.child->count; ++j)
            result[cur_offset+j].push_back(node.meld);
        add_arrays(node.child, result, cur_offset);
        cur_offset += node.child->count;
    }
}

Combos n_triples(Hand &&hand, const Triples &triples, Fast8 n)
{
    if (triples.size() < n) return {};

    // temporary id to ensure no 4-of-a-kinds are counted as two triples
    Fast8 tmp_id = Tile().id7();

    Melds perms; // We start from the end of the list because the triples are
                 // weakly ordered. We want to detect triples for the wind and
                 // dragon tiles then remove them to optimize performence.
    for (auto it = triples.rbegin(); it != triples.rend(); ++it)
    {
        const auto &tile = it->first();
        if (tile.suit() == Suit::Wind || tile.suit() == Suit::Dragon)
        {
            if (tile.id7() == tmp_id) continue;
            tmp_id = tile.id7();
            perms.push_back(*it);
        }
        else break;
    }

    auto children = dfs(hand.hand_4hot(), hand.size(), triples.data(), triples.size(), n-perms.size());
    if (!children) return {};

    Combos results(children->count);
    if (perms.size() < n)
        add_arrays(children, results, 0);
    if (perms.size())
        add_perms(perms, results);
    
    return results;
}

void tenpai_win_impl(Fast8 offset, WaitingTiles &res, Hand4Hot &h4, 
                     Fast8 n_melds, Suit suit, Fast8 num, bool kokushi_possible)
{
    using namespace ext::tomohxx;
    if (++h4[offset] <= 4 && -1==shanten(h4, n_melds, k_ModeNormal | k_ModeChiitoi))
        res.emplace_back(suit, num);
    else if (kokushi_possible && -1==shanten(h4, n_melds, k_ModeKokushi))
        res.emplace_back(suit, num);
    h4[offset]--;
}

} // anon namespace

S8 shanten(const Hand4Hot &h4, Fast8 n_melds, int mode)
{
    static ext::tomohxx::Calsht sht("../assets/tables");
    auto [num, _] = sht(h4, k_MaxNumMeld - n_melds, mode);
    return num - 1;
}

Hand::Hand(const char *str) : tiles4_(k_UniqueTiles, 0)
{
    const char *suits = "mpswd";
    Fast8 cur_suit = 0;
    for (; *str && size() < k_MaxHandSize; ++str)
    {
        if (*str < '1' || *str > '9')
        {
            while (suits[cur_suit] != *str)
                if (++cur_suit == 5)
                    return;
        }
        else
            emplace_back(Suit(cur_suit), *str - '1');
    }
}

void Hand::sort() const
{
    if (sorted_)
        return;
    std::sort(tiles_.begin(), tiles_.end());
    sorted_ = true;
}

Hand Hand::clean() const
{
    Hand output;
    for (const auto &tile : tiles_)
        if (tile)
            output.push_back(tile);
    if (sorted_)
        output.mark_sorted();
    return output;
}

Wins Hand::agari() const
{
    if (!is_agari()) return {};
    auto p = pairs(*this);
    if (p.empty()) return {};

    Wins wins;
    const Fast8 closed_melds = k_MaxNumMeld - melds();
    if (closed_melds == 0)
    {
        for (const auto &pair : p)
            wins.emplace_back(melds_, pair, flags_);
        return wins;
    }

    for (const auto &pair : p)
    {
        auto tmp_hand = *this;
        auto first = std::find(tmp_hand.tiles_.begin(), tmp_hand.tiles_.end(), pair.first());
        tmp_hand.hand_4hot()[pair.first().id34()]-=2;
        auto trips = triples(tmp_hand);
        tmp_hand.tiles_.erase(first, first+2);
        if (trips.size() < closed_melds) continue;
        auto combos = n_triples(std::move(tmp_hand), trips, closed_melds);
        if (combos.empty()) continue;
        for (auto &combo : combos)
        {
            if (!melds_.empty())
                combo.insert(combo.end(), melds_.begin(), melds_.end());
            if (wins.empty())
            {
                wins.emplace_back(combo, pair, flags_);
                continue;
            }

            const auto &prev = wins.back();
            for (size_t i = 0; i < combo.size(); ++i)
            {
                if (combo[i].ne7(prev.melds[i]))
                {
                    wins.emplace_back(combo, pair, flags_);
                    break;
                }
            }
        }
    }
    return wins;
}

WaitingTiles Hand::tenpai() const
{
    if (size() + 3*melds() != k_MaxHandSize - 1) return {};
    if (!is_tenpai()) return {};
    WaitingTiles waiting;
    Hand4Hot h4 (hand_4hot());
    bool kokushi_possible = melds();
    // Fast8 chitoi_possible = melds() ? 0 : 2;
    // check normal tile waits first
    for (Suit s = Suit::Man; s < Suit::Wind; ++s)
    {
        Fast8 s9 = static_cast<Fast8>(s)*9;

        // deal with n=0
        if (h4[s9] || h4[s9+1])
            tenpai_win_impl(s9, waiting, h4, melds(), s, 0, kokushi_possible);

        for (int n = 1; n < 8; ++n)
        {
            if (h4[s9+n-1] || h4[s9+n] || h4[s9+n+1])
            {
                using namespace ext::tomohxx;
                if (h4[s9+n]++)
                    kokushi_possible = false;
                if (h4[s9+n] <= 4 && -1==mj::shanten(h4, melds(), k_ModeNormal | k_ModeChiitoi))
                    waiting.emplace_back(s, n);
                h4[s9+n]--;
            }
        }
        if (h4[s9+7] || h4[s9+8])
            tenpai_win_impl(s9+8, waiting, h4, melds(), s, 8, kokushi_possible);
    }
    // check wind waits
    constexpr Fast8 k_WindOffset = Tile::k_Offsets[static_cast<Fast8>(Suit::Wind)];
    constexpr Fast8 k_DragonOffset = Tile::k_Offsets[static_cast<Fast8>(Suit::Dragon)];
    for (int n = 0; n < k_NumWinds; ++n)
    {
        Fast8 offset = k_WindOffset+n;
        if (h4[offset])
            tenpai_win_impl(offset, waiting, h4, melds(), Suit::Wind, n, kokushi_possible);
    }
    // check dragon waits
    for (int n = 0; n < k_NumDragons; ++n)
    {
        Fast8 offset = k_DragonOffset+n;
        if (h4[offset])
            tenpai_win_impl(offset, waiting, h4, melds(), Suit::Dragon, n, kokushi_possible);
    }

    return waiting;
}

S8 Hand::shanten() const
{ 
    return mj::shanten(hand_4hot(), melds(), ext::tomohxx::k_ModeAll); 
};



} // namespace mj
