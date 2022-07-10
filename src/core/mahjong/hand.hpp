
#pragma once
#include "base.hpp"
#include "core/adt/vector.hpp"

namespace mj {
/**
 * Compactified types
 */
using cPairs = vector<int, 7>;
using cMelds = vector<int, 4>;
using NormalWin = std::pair<cMelds, int>;
using Win = std::variant<bool, cPairs, NormalWin>;
using Hand4Hot = std::array<int, k_UniqueTiles>;

using Melds = vector<Meld, k_MaxNumMeld>;
using WaitingTiles = vector<Tile, 13>;
using Wins = vector<Win, 16>;
using HandDense = vector<Tile, k_MaxHandSize>;
using Discards = vector<Tile, k_MaxDiscards>;

/**
 * Calculate shanten count of a hand by using the algorithm described in
 * https://github.com/tomohxx/shanten-number. This algorithm also works for
 * hands with any number >1 tiles.
 *
 * @param h4 The 4hot representation of the hand. This should only include the
 * closed part of the hand.
 * @param n_melds The number of open melds in the hand.
 * @param mode The mode of win (k_ModeNormal, k_ModeChiitoi, k_ModeKokushi)
 * @return S8 The shanten number of the hand, 0 for tenpai, -1 for winning.
 */
S8 shanten(const Hand4Hot &h4, U8f n_melds, int mode);

namespace _hand_impl {
template <U8f N> class Hand
{
public:
    using HandDense = vector<Tile, N>;
    U64 flags{f_All64};
    Hand() : tiles4_(), tiles4m_() {}
    Hand(const char *, Dir = k_East);

    /**
     * @return If the hand is open or not.
     */
    constexpr bool open() const { return n_open_melds(); }

    /**
     * @return If the hand is closed or not.
     */
    constexpr bool closed() const { return !open(); }

    /**
     * @return S8 the shanten count of the hand. 0 for tenpai, and -1 for an
     * already winning hand.
     */
    S8 shanten() const;
    inline bool is_tenpai() const { return shanten() == 0; }
    inline bool is_agari() const { return shanten() == -1; }

    /**
     * Calculate the various ways a this hand can form winning combinations
     * (4 melds + pair, 7 pairs, or kokushi). For normal wins, (4 melds + pair),
     * only the closed parts of the win will be included, as the open part will
     * always be the same and can be accessed via the melds() method.
     *
     * @return Wins
     */
    Wins agari() const;

    /**
     * Find the tiles a player is waiting when the player is in tenpai.
     *
     * @return WaitingTiles vector of tiles the player is waiting for. Will be
     * empty if the player is not in tenpai.
     */
    WaitingTiles tenpai() const;

    constexpr void sort() const
    {
        if (sorted_)
            return;
        std::sort(tiles_.begin(), tiles_.end());
        sorted_ = true;
    }

    constexpr void sort_after_draw() const
    {
        if (sorted_)
            return;
        Tile t = tiles_.back();
        tiles_.pop_back();
        auto it = std::lower_bound(tiles_.begin(), tiles_.end(), t);
        tiles_.insert(it, t);
        sorted_ = true;
    }

    constexpr Dir player() const { return (*this)[0].player(); }

    /**
     * Flag operations
     */
    constexpr U64 check(U64 mask, U8f offset = 0) const noexcept
    {
        return (flags & mask) >> offset;
    }
    constexpr void set(U64 mask) noexcept { flags |= mask; }
    constexpr void clear(U64 mask) noexcept { flags &= ~mask; }

    /**
     * Accessor functions
     */
    constexpr Tile &operator[](U8f idx) noexcept
    {
        sorted_ = false;
        return tiles_[idx];
    }
    constexpr const Tile &operator[](U8f idx) const noexcept
    {
        return tiles_[idx];
    }
    constexpr const Meld &meld(U8f idx) const noexcept { return melds_[idx]; }
    constexpr Meld &meld(U8f idx) noexcept { return melds_[idx]; }

    constexpr void mark_sorted() const noexcept { sorted_ = true; }

    /**
     * Modifier functions
     */
    constexpr void push_back(const Tile &t)
    {
        ++tiles4_[t.id34()];
        ++tiles4m_[t.id34()];
        MJ_ASSERT(tiles4_[t.id34()] <= 4,
                  "More than 4 same tiles is not allowed");
        MJ_ASSERT(tiles4m_[t.id34()] <= 4,
                  "More than 4 same tiles is not allowed");
        tiles_.push_back(t);
        sorted_ = false;
    }

    template <typename... Args> constexpr void emplace_back(Args &&...args)
    {
        tiles_.emplace_back(std::forward<Args>(args)...);
        const auto &t = tiles_.back();
        ++tiles4_[t.id34()];
        ++tiles4m_[t.id34()];
        sorted_ = false;
        MJ_ASSERT(tiles4_[t.id34()] <= 4,
                  "More than 4 same tiles is not allowed");
        MJ_ASSERT(tiles4m_[t.id34()] <= 4,
                  "More than 4 same tiles is not allowed");
    }

    constexpr void pop_back() MJ_EXCEPT_CRIT
    {
        const auto &t = tiles_.back();
        MJ_ASSERT_CRIT(tiles4_[t.id34()] > 0,
                       "Something is wrong with 4hot tiles");
        MJ_ASSERT_CRIT(tiles4m_[t.id34()] > 0,
                       "Something is wrong with 4hot tiles");
        --tiles4_[t.id34()];
        --tiles4m_[t.id34()];
        tiles_.pop_back();
    }

    constexpr bool pong(const Tile &t) MJ_EXCEPT_CRIT
    {
        MJ_ASSERT(sorted_, "Hand is not sorted when calling pong");
        MJ_ASSERT(t.player() != player(), "Cannot pong own tile");
        if (tiles4_[t.id34()] < 2)
            return false;
        tiles4_[t.id34()] -= 2;
        tiles4m_[t.id34()]++;
        auto it = std::find_if(tiles_.begin(), tiles_.end(),
                               [t](const Tile &ht) { return ht.eq7(t); });
        MJ_ASSERT_CRIT(it != tiles_.end(), "Something wrong with tiles4_");
        melds_.emplace_back(t, *it, *(it + 1));
        tiles_.erase(it, it + 2);
        return true;
    }

    /**
     * Declare a CHII and absorb an opponent's tile into hand.
     *
     * @param t the tile to be absorbed.
     * @param chii_at the position of the CHII. -1 for left i.e. [1]23,
     * 0 for middle i.e. 1[2]3, 1 for right i.e. 12[3].
     * @return true if the chii is possible, false otherwise.
     */
    constexpr bool chii(const Tile &t, S8f chii_at) MJ_EXCEPT_CRIT
    {
        MJ_ASSERT(sorted_, "Hand is not sorted when calling chii");
        MJ_ASSERT(t.player() != player(), "Cannot chii own tile");
        if (next(t.player()) != player())
            return false;
        U8f tgt1{}, tgt2{};
        switch (chii_at)
        {
        case k_ChiiBelow:
            if (t.num1() >= 8)
                return false;
            tgt1 = t.id34() + 1;
            tgt2 = t.id34() + 2;
            break;
        case k_ChiiMiddle:
            if (t.num1() == 1 || t.num1() == 9)
                return false;
            tgt1 = t.id34() - 1;
            tgt2 = t.id34() + 1;
            break;
        case k_ChiiAbove:
            if (t.num1() <= 2)
                return false;
            tgt1 = t.id34() - 2;
            tgt2 = t.id34() - 1;
            break;
        }
        if (tiles4_[tgt1] < 1 || tiles4_[tgt2] < 1)
            return false;
        tiles4_[tgt1]--;
        tiles4_[tgt2]--;
        tiles4m_[t.id34()]++;

        auto it1 =
            std::find_if(tiles_.begin(), tiles_.end(),
                         [tgt1](const Tile &ht) { return ht.id34() == tgt1; });
        MJ_ASSERT_CRIT(it1 != tiles_.end(), "Something wrong with tiles4_");
        auto &&t1 = std::move(*it1);
        auto it2 =
            std::find_if(tiles_.erase(it1), tiles_.end(),
                         [tgt2](const Tile &ht) { return ht.id34() == tgt2; });
        MJ_ASSERT_CRIT(it2 != tiles_.end(), "Something wrong with tiles4_");
        auto &&t2 = std::move(*it2);
        tiles_.erase(it2);

        melds_.emplace_back(t, t1, t2);
        return true;
    }

    constexpr bool closed_kong(U8f t34) MJ_EXCEPT_CRIT
    {
        MJ_ASSERT(sorted_, "Hand is not sorted when calling closed_kong");
        if (tiles4_[t34] != 4)
            return false;
        tiles4_[t34] = 0;
        auto it =
            std::find_if(tiles_.begin(), tiles_.end(),
                         [t34](const Tile &ht) { return ht.id34() == t34; });
        MJ_ASSERT_CRIT(it != tiles_.end(), "Something wrong with tiles4_");
        melds_.emplace_back(*it, *(it + 1), *(it + 2), *(it + 3), true);
        tiles_.erase(it, it + 4);
        n_closed_kongs_++;
        return true;
    }

    constexpr bool self_kong(const Tile &t) MJ_EXCEPT_WARN
    {
        MJ_ASSERT(t.player() == player(), "Cannot self-kong other's tile");
        auto it =
            std::find_if(melds_.begin(), melds_.end(), [t](const Meld &m) {
                return m.is_set() && m.first().eq7(t);
            });
        if (it == melds_.end())
            return false;
        it->add_fourth(t);
        tiles4_[t.id34()]--;
        return true;
    }

    constexpr bool open_kong(const Tile &t) MJ_EXCEPT_CRIT
    {
        MJ_ASSERT(sorted_, "Hand is not sorted when calling open_kong");
        MJ_ASSERT(t.player() != tiles_[0].player(),
                  "Cannot open kong own tile");
        if (tiles4_[t.id34()] != 3)
            return false;
        tiles4_[t.id34()] = 0;
        tiles4m_[t.id34()] = 4;
        auto it = std::find_if(tiles_.begin(), tiles_.end(),
                               [t](const Tile &ht) { return ht.eq7(t); });
        MJ_ASSERT_CRIT(it != tiles_.end(), "Something wrong with tiles4_");
        melds_.emplace_back(t, *it, *(it + 1), *(it + 2));
        tiles_.erase(it, it + 3);
        return true;
    }

    constexpr void discard(const Tile &t) MJ_EXCEPT_CRIT
    {
        MJ_ASSERT_CRIT(tiles4_[t.id34()] > 0,
                       "Something is wrong with 4hot tiles");
        MJ_ASSERT_CRIT(tiles4m_[t.id34()] > 0,
                       "Something is wrong with 4hot tiles");
        --tiles4_[t.id34()];
        --tiles4m_[t.id34()];
        tiles_.erase(std::find(tiles_.begin(), tiles_.end(), t));
    }

    constexpr void discard(U8f idx) MJ_EXCEPT_CRIT
    {
        const auto &t = tiles_[idx];
        MJ_ASSERT_CRIT(tiles4_[t.id34()] > 0,
                       "Something is wrong with 4hot tiles");
        MJ_ASSERT_CRIT(tiles4m_[t.id34()] > 0,
                       "Something is wrong with 4hot tiles");
        --tiles4_[t.id34()];
        --tiles4m_[t.id34()];
        tiles_.erase(tiles_.begin() + idx);
    }

    /**
     * Size functions
     */
    constexpr U8f size() const noexcept { return tiles_.size(); }
    constexpr U8f n_melds() const noexcept { return melds_.size(); }
    constexpr U8f n_open_melds() const noexcept
    {
        return melds_.size() - n_closed_kongs_;
    }
    constexpr U8f n_closed_kongs() const noexcept { return n_closed_kongs_; }
    /**
     * Container data functions
     */
    constexpr HandDense &tiles() noexcept { return tiles_; }
    constexpr const HandDense &tiles() const noexcept { return tiles_; }
    constexpr Melds melds() noexcept { return melds_; }
    constexpr const Melds &melds() const noexcept { return melds_; }

    constexpr Hand4Hot &hand_4hot() noexcept { return tiles4_; }
    constexpr const Hand4Hot &hand_4hot() const noexcept { return tiles4_; }
    constexpr int &hand_4hot(U8f idx) noexcept { return tiles4_[idx]; }
    constexpr const int &hand_4hot(U8f idx) const noexcept
    {
        return tiles4_[idx];
    }

    constexpr Hand4Hot &hand_4hot_melds() noexcept { return tiles4m_; }
    constexpr const Hand4Hot &hand_4hot_melds() const noexcept
    {
        return tiles4m_;
    }
    constexpr int &hand_4hot_melds(U8f idx) noexcept { return tiles4m_[idx]; }
    constexpr const int &hand_4hot_melds(U8f idx) const noexcept
    {
        return tiles4m_[idx];
    }

    /**
     * Iterator functions
     */
    constexpr HandDense::iterator begin() noexcept { return tiles_.begin(); }
    constexpr HandDense::iterator end() noexcept { return tiles_.end(); }
    constexpr HandDense::const_iterator begin() const noexcept
    {
        return tiles_.begin();
    }
    constexpr HandDense::const_iterator end() const noexcept
    {
        return tiles_.end();
    }

private:
    mutable HandDense tiles_{};
    Hand4Hot tiles4_;
    Hand4Hot tiles4m_;
    mutable bool sorted_{};
    Melds melds_;
    U8f n_closed_kongs_{};

public:
    CONSTEXPR12 std::string to_string() const
    {
        std::string s;
        for (const auto &t : tiles_)
            s += t.to_string();
        s += "|";
        for (const auto &m : melds_)
            s += m.to_string();
        return s;
    }
};

} // namespace _hand_impl

using Hand = _hand_impl::Hand<k_MaxHandSize>;
using BigHand = _hand_impl::Hand<k_DeckSize>;

} // namespace mj
