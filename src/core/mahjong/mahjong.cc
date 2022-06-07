
#include "mahjong.hpp"
#include <algorithm>

namespace mj {
namespace {
using Pairs = s_Vector<Meld, 7>;
using Triples = s_Vector<Meld, 32>;
using Combos = s_Vector<Melds, 16>;

Pairs pairs(const Hand &hand) 
{
    hand.sort();
    Pairs pairs;
    for (Fast8 i = 0; i < hand.size() - 1; ++i)
    {
        if (hand[i] == hand[i + 1])
        {
            pairs.emplace_back(hand[i], hand[i + 1]);
            ++i;
        }
    }
    return pairs;
}

Triples triples(const Hand &hand)
{
    hand.sort();
    Fast8 j;
    Fast8 k;
    Triples triples;
    for (Fast8 i = 0; i < hand.size()-2; ++i)
    {
        if (hand[i].id7() == hand[i+1].id7() &&
            hand[i].id7() == hand[i+2].id7())
        {
            triples.emplace_back(hand[i], hand[i+1], hand[i+2]);
            j = i + 3;
        }
        else
            j = i + 1;
        
        if (hand[i].suit()==Suit::Wind || hand[i].suit()==Suit::Dragon ||
            hand[i].num1() > 7) continue;
        
        for (; j < hand.size()-1 && hand[i].suit()==hand[j].suit() && hand[j].num() - hand[i].num() <= 1; ++j)
        { 
            if (hand[j].num() == 1 + hand[i].num())
            {
                for (k = j + 1; k < hand.size() && hand[i].suit()==hand[k].suit() && hand[k].num() - hand[j].num() <= 1; ++k)
                {   
                    if (hand[k].num() == 1 + hand[j].num())
                    {
                        Meld m(hand[i], hand[j], hand[k]);
                        if (triples.empty() || m.ne7(triples.back()))
                            triples.push_back(m);
                        break;
                    }
                }
            }
        }
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


Fast16 traverse_tree(Hand &hand, Fast8 begin, Meld branch)
{
    if (hand.size() - begin < 3) return 0;

    for (; hand[begin] != branch.first(); ++begin)
        if (begin == hand.size() - 2) return 0;
    
    hand[begin++] = {};
    Fast8 i = begin;
    for (; hand[i] != branch.second(); ++i)
        if (i == hand.size() - 1) return 0;
    
    hand[i++] = {};
    for (; hand[i] != branch.third(); ++i)
        if (i == hand.size()) return 0;
    
    hand[i++] = {};

    return begin; // next time, only traverse from here (due to order)
}

std::unique_ptr<HandArray> dfs(Hand &tiles, const Meld *triples, Fast16 num_melds, Fast16 n)
{
    if (n == 0)
    {
        auto res = std::make_unique<HandArray>();
        res->count = 1;
        return res;
    }

    std::unique_ptr<HandArray> children = nullptr;
    for (Fast16 i = 0; i <= num_melds - n; ++i)
    {
        Hand tmp_hand = tiles.clean();
        Fast16 next = traverse_tree(tmp_hand, 0, triples[i]);
        if (next == 0) continue;

        auto found = dfs(tmp_hand, triples + i + 1, num_melds-i-1, n - 1);
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

Combos n_triples(Hand &hand, const Triples &triples, Fast8 n)
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

    // remove all wind and dragon tiles from the hand
    while (hand.size() && (
        hand[hand.size()-1].suit() == Suit::Wind || 
        hand[hand.size()-1].suit() == Suit::Dragon)
    )
        hand.pop_back();
    
    auto children = dfs(hand, triples.data(), triples.size(), n-perms.size());
    if (!children) return {};

    Combos results(children->count);
    if (perms.size() < n)
        add_arrays(children, results, 0);
    if (perms.size())
        add_perms(perms, results);
    
    return results;
}

void add_tenpai_tile(WaitingTiles &waiting, Hand hand_cpy, Suit suit, Fast8 num)
{
    hand_cpy.push_back(Tile(suit, num));
    hand_cpy.sort();
    auto wins = hand_cpy.agari();
    if (!wins.empty())
        waiting.emplace_back(suit, num);
}

} // anon namespace

Hand::Hand(const char *str)
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
            tiles_.emplace_back(Suit(cur_suit), *str - '1');
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
        tmp_hand.tiles_.erase(first, first+2);
        auto trips = triples(tmp_hand);
        if (trips.size() < closed_melds) continue;
        auto combos = n_triples(tmp_hand, trips, closed_melds);
        if (combos.empty()) continue;
        for (auto &combo : combos)
        {
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

    Fast8 idx = 0;
    WaitingTiles waiting;
    for (auto suit = Suit::Man;;++suit)
    {
        while((*this)[idx].suit() != suit)
        {
            if ((*this)[idx].suit() > suit)
                ++suit;
            if ((*this)[idx].suit() < suit)
                ++idx;
        }
        if (suit > Suit::Sou) break;

        for (int num = 0; num < 9; ++num)
        {
            while(((*this)[idx].suit() == suit && 
                (*this)[idx].num() < num-1) || 
                (*this)[idx].num() > num+1)
            {
                if ((*this)[idx].num() < num-1)
                    ++idx;
                if ((*this)[idx].num() > num+1)
                    ++num;
            }
            if ((*this)[idx].suit() != suit)
                break;
            
            add_tenpai_tile(waiting, *this, suit, num);
        }
    }

    for (int num = 0;; ++num)
    {
        while((*this)[idx].suit() == Suit::Wind &&
            (*this)[idx].num() != num)
        {
            if ((*this)[idx].num() < num)
                ++idx;
            if ((*this)[idx].num() > num)
                ++num;
        }
        if (num >= k_NumWinds || (*this)[idx].suit() != Suit::Wind)
            break;
        
        add_tenpai_tile(waiting, *this, Suit::Wind, num);
    }

    for (int num = 0;; ++num)
    {
        while((*this)[idx].suit() == Suit::Dragon &&
            (*this)[idx].num() != num)
        {
            if ((*this)[idx].num() < num)
                ++idx;
            if ((*this)[idx].num() > num)
                ++num;
        }
        if (num >= k_NumDragons || (*this)[idx].suit() != Suit::Dragon)
            break;
        
        add_tenpai_tile(waiting, *this, Suit::Dragon, num);
    }

    return waiting;
}

} // namespace mj
