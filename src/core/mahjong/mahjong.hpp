
#pragma once

#include <algorithm>
#include <array>
#include <variant>
#include <vector>

#include "constants.hpp"
#include "core/utils/stack_allocator.hpp"
#include "core/utils/logging.hpp"

namespace mj {
/**
 * Convert computer-readable tile number to human-readable tile number.
 * 
 * @param c_num computer-readable tile number
 * @return constexpr Fast8 human-readable tile number
 * @warning Does not check for invalid tile number
 */
constexpr Fast8 h_num(Fast8 c_num) { return c_num + 1; }

/**
 * Convert human-readable tile number to computer-readable tile number.
 * 
 * @param h_num human-readable tile number 
 * @return constexpr Fast8 computer-readable tile number
 * @warning Does not check for invalid tile number
 */
constexpr Fast8 c_num(Fast8 h_num) { return h_num - 1; }

enum class Suit : U16
{
    Man, Pin, Sou, Wind, Dragon
};

constexpr Suit &operator++(Suit &suit) MJ_EXCEPT_WARN
{
    MJ_ASSERT(suit < Suit::Dragon, "Suit overflows");
    return suit = static_cast<Suit>(static_cast<U16>(suit) + 1);
}

constexpr bool operator==(Suit lhs, Suit rhs) noexcept
{
    return static_cast<U16>(lhs) == static_cast<U16>(rhs);
}

constexpr bool operator<(Suit lhs, Suit rhs) noexcept
{
    return static_cast<U16>(lhs) < static_cast<U16>(rhs);
}

constexpr bool operator>(Suit lhs, Suit rhs) noexcept
{
    return static_cast<U16>(lhs) > static_cast<U16>(rhs);
}

enum Dir : Fast8
{
    k_East, k_South, k_West, k_North
};

/**
 * Increment direction in the same way turns pass in mahjong, including 
 * wrapping around
 * 
 * @param dir Direction to increment
 * @return constexpr Dir& Incremented direction
 */
constexpr Dir &operator++(Dir &dir) noexcept
{
    dir = static_cast<Dir>((dir + 1) & 3);
    return dir;
}
/**
 * Increment direction in the same way turns pass in mahjong, including 
 * wrapping around
 * 
 * @param dir Direction to increment
 * @return constexpr Dir Incremented direction
 */
constexpr Dir operator++(Dir &dir, int) noexcept
{
    Dir tmp = dir;
    ++dir;
    return tmp;
}

/**
 * The contents of a mahjong tile is represented as a 16-bit integer. The 7 high
 * bits represent the tile itself, and the 9 low bits are reserved for flags.
 */
class Tile
{
public:
    constexpr static std::array<int, 5> k_Offsets
    { 0, 9, 18, 27, 31 };

    constexpr static U16 f_Transparent  = 0x0001;
    constexpr static U16 f_Red          = 0x0002;
    constexpr static U16 f_Tsumogiri    = 0x0004;
    constexpr static U16 f_LastTile     = 0x0008;
    constexpr static U16 f_FirstTurn    = 0x0010;
    constexpr static U16 f_Rinshan      = 0x0020;
public:
    constexpr Tile() noexcept : id_(f_All16) {}
    constexpr explicit Tile(U16 id) : id_(id) {}
    constexpr Tile(Suit suit, Fast8 num, Fast8 player=k_East, U16 flags=0)
    noexcept : id_((U16)suit << tilelayout::k_SuitPos | 
                        num << tilelayout::k_NumPos | 
                        player << tilelayout::k_PlayerPos | flags) {}
    constexpr void set(U16 flag) { id_ |= flag; }
    constexpr void set_dir(Dir dir) { id_ |= (U16)dir << tilelayout::k_PlayerPos; }
    constexpr bool check(U16 flag) const { return id_ & flag; }

private:
    U16 id_;

public:
    constexpr Suit suit() const noexcept
    { return static_cast<Suit>((id_ >> tilelayout::k_SuitPos) & 7); }

    constexpr Fast8 num() const noexcept
    { return (id_ >> tilelayout::k_NumPos) & 15; }
    constexpr Fast8 num1() const noexcept
    { return num() + 1; }

    constexpr Fast8 player() const noexcept
    { return (id_ >> tilelayout::k_PlayerPos) & 3; }

    constexpr operator bool() const noexcept
    { return id_ != f_All16; }
    constexpr U16 id() const noexcept
    { return id_; }

    /**
     * @return constexpr Fast8 The tile content ignoring the flags. 
     */
    constexpr Fast8 id7() const noexcept
    { return (id_ >> tilelayout::k_NumPos) & 127; }

    /** 
     * @return constexpr Fast8 The tile in the order 0 to 33.
     */
    constexpr Fast8 id34() const noexcept
    { return k_Offsets[static_cast<Fast8>(suit())] + num(); }

    /**
     * Equality ignoring flags
     */
    constexpr bool eq7(const Tile& rhs) const noexcept
    { return id7() == rhs.id7(); }

    /**
     * Non-equality ignoring flags
     */
    constexpr bool ne7(const Tile& rhs) const noexcept
    { return id7() != rhs.id7(); }

    constexpr bool is_honor() const MJ_EXCEPT_WARN
    {   MJ_ASSERT(*this, "is_honor() called on invalid tile");
        return suit() >= Suit::Wind; }

    constexpr bool is_19() const MJ_EXCEPT_WARN
    {   MJ_ASSERT(*this, "is_19() called on invalid tile");
        return num() == c_num(1) || num() == c_num(9) || is_honor(); }

    constexpr bool operator==(const Tile &rhs) const
    { return id_ == rhs.id_; }
    constexpr bool operator!=(const Tile &rhs) const
    { return id_ != rhs.id_; }
    constexpr bool operator<(const Tile &rhs) const
    { return id_ < rhs.id_; }
    constexpr bool operator>(const Tile &rhs) const
    { return id_ > rhs.id_; }
    constexpr bool operator<=(const Tile &rhs) const
    { return id_ <= rhs.id_; }
    constexpr bool operator>=(const Tile &rhs) const
    { return id_ >= rhs.id_; }

public:
    [[gnu::noinline]] std::string to_string() const
    {
        constexpr std::array<char, 5> suits =
        { 'm', 'p', 's', 'w', 'd' };
        if (*this)
            return std::to_string(num1()) + suits[static_cast<int>(suit())];
        return "??";
    }
};

constexpr Tile convert34(int n)
{
    U16 suit = std::count_if(Tile::k_Offsets.begin(), Tile::k_Offsets.end(), 
        [n](int offset) { return n >= offset; })-1;
    U16 num = n - Tile::k_Offsets[suit];
    return Tile(suit << tilelayout::k_SuitPos | num << tilelayout::k_NumPos);
}


/**
 * Melds, which represent all sets of tiles possible in the game, including
 * pairs, triples, and quads.
 */
class Meld
{
    constexpr static U64 mask7 = 127ul << tilelayout::k_NumPos;
    constexpr static U64 Mask7 = mask7 | mask7 << 16 | mask7 << 32 | mask7 << 48;
public:
    constexpr Meld() noexcept : id_(f_All64) {}
    constexpr Meld(Tile called, Tile t1, Tile t2 = {}, Tile t3 = {})
        : id_((static_cast<U64>(called.id())<< 48) |
              (static_cast<U64>(t1.id()) << 32) |
              (static_cast<U64>(t2.id()) << 16) |
               static_cast<U64>(t3.id())) {}

    /** 
     * @return constexpr Tile The first tile in the meld.
     */
    constexpr Tile first() const noexcept
    { return Tile(id_ >> 48); }
    constexpr Tile called() const noexcept
    { return first(); }
    /** 
     * @return constexpr Tile The second tile in the meld.
     */
    constexpr Tile second() const noexcept
    { return Tile((id_ >> 32) & 0xffff); }
    /** 
     * @return constexpr Tile The third tile in the meld.
     */
    constexpr Tile third() const noexcept
    { return Tile((id_ >> 16) & 0xffff); }
    /** 
     * @return constexpr Tile The fourth tile in the meld.
     */
    constexpr Tile fourth() const noexcept
    { return Tile(id_ & 0xffff); }

    constexpr operator bool() const noexcept
    { return id_ != f_All64; }
    
    constexpr bool operator==(const Meld &rhs) const
    { return id_ == rhs.id_; }
    constexpr bool operator!=(const Meld &rhs) const
    { return id_ != rhs.id_; }

    constexpr bool operator<(const Meld &rhs) const
    { return id_ < rhs.id_; }

    /**
     * Equality ignoring flags
     */
    constexpr bool eq7(const Meld& rhs) const noexcept
    { return (id_ & Mask7) == (rhs.id_ & Mask7); }
    /**
     * Non-equality ignoring flags
     */
    constexpr bool ne7(const Meld& rhs) const noexcept
    { return (id_ & Mask7) != (rhs.id_ & Mask7); }

    constexpr bool is_set() const noexcept 
    { return first().id7() == third().id7();  }

private:
    U64 id_;

public:
    [[gnu::noinline]] std::string to_string() const
    {
        return first().to_string() + second().to_string() +
            third().to_string() + fourth().to_string();
    }
};
/**
 * Compactified types
 */
using cPairs = s_Vector<int, 7>;
using cMelds = s_Vector<int, 4>;
using NormalWin = std::pair<cMelds, int>;
using Win = std::variant<bool, cPairs, NormalWin>;
using Hand4Hot = std::array<int, k_UniqueTiles>;

/**
 * Functions to deal with compactified types
 */
constexpr bool _is_honor(int x) MJ_EXCEPT_WARN
{   MJ_ASSERT(x < k_UniqueTiles, "_is_honor() called on invalid tile");
    return x >= k_FirstHonorIdx; }
constexpr bool _is_19(int x) MJ_EXCEPT_WARN
{   MJ_ASSERT(x < k_UniqueTiles, "_is_19() called on invalid tile");
    return x % 9 == c_num(1) || x % 9 == c_num(9) || _is_honor(x); }
constexpr bool _is_17(int x) MJ_EXCEPT_WARN
{   MJ_ASSERT(x < k_UniqueTiles, "_is_17() called on invalid tile");
    MJ_ASSERT(!_is_honor(x), "_is_17() called on honor tile");
    return x % 9 == c_num(1) || x % 9 == c_num(7); }

constexpr bool _is_set(int x) noexcept { return x & 0x100; }
constexpr bool _is_run(int x) noexcept { return !(x & 0x100); }
constexpr int _set(int x) noexcept { return x | 0x100; }
constexpr int _run(int x) noexcept { return x; }
constexpr int _int(int x) noexcept { return x & 0xff; }
constexpr int _2nd(int x) noexcept { return _int(x)+1; }
constexpr int _3rd(int x) noexcept { return _int(x)+2; }


using Melds = s_Vector<Meld, k_MaxNumMeld>;
using WaitingTiles = s_Vector<Tile, 13>;
using Wins = s_Vector<Win, 16>;
using HandDense = s_Vector<Tile, k_MaxHandSize>;
using Discards = s_Vector<Tile, k_MaxDiscards>;

S8 shanten(const Hand4Hot &h4, Fast8 n_melds, int mode);

class Hand
{
public:
    constexpr static Fast8 fp_Riichi = 62;
    constexpr static Fast8 fp_Ippatsu = 61;
    constexpr static Fast8 fp_Open = 60;
    constexpr static Fast8 fp_Haitei = 59;
    constexpr static Fast8 fp_FirstTurn = 58;

    constexpr static U64 f_SingleRiichi = 1ull << fp_Riichi;
    constexpr static U64 f_DoubleRiichi = 2ull << fp_Riichi;
    constexpr static U64 f_Riichi = f_SingleRiichi | f_DoubleRiichi;
    constexpr static U64 f_Ippatsu = 1ull << fp_Ippatsu;
    constexpr static U64 f_Open = 1ull << fp_Open;
public:
    Hand() : tiles4_() {}
    Hand(const char *);

    S8 shanten() const;
    inline bool is_tenpai() const { return shanten() == 0; }
    inline bool is_agari() const { return shanten() == -1; }

    Wins agari() const;
    WaitingTiles tenpai() const;

    void sort() const;
    Hand clean() const;

    constexpr U64 check(U64 mask, Fast8 offset=0) const noexcept
    { return (flags_ & mask) >> offset; }
    constexpr void set(U64 mask) noexcept
    { flags_ |= mask; }
    constexpr void clear(U64 mask) noexcept
    { flags_ &= ~mask; }

    CONSTEXPR12 HandDense &tiles() noexcept { return tiles_; }
    CONSTEXPR12 const HandDense &tiles() const noexcept { return tiles_; }

    CONSTEXPR12 Fast8 size() const noexcept
    { return tiles_.size(); }

    CONSTEXPR12 Tile &operator[](Fast8 idx) noexcept
    { sorted_ = false; return tiles_[idx]; }
    CONSTEXPR12 const Tile &operator[](Fast8 idx) const noexcept
    { return tiles_[idx]; }
 
    constexpr void mark_sorted() const noexcept { sorted_ = true; }

    CONSTEXPR12 void push_back(const Tile &t)
    {
        ++tiles4_[t.id34()];
        MJ_ASSERT(tiles4_[t.id34()] <= 4, "More than 4 same tiles is not allowed");
        tiles_.push_back(t);
        sorted_ = false; 
    }

    template<typename... Args>
    CONSTEXPR12 void emplace_back(Args&&... args)
    {
        tiles_.emplace_back(args...);
        const auto &t = tiles_.back();
        ++tiles4_[t.id34()];
        MJ_ASSERT(tiles4_[t.id34()] <= 4, "More than 4 same tiles is not allowed");
    }

    CONSTEXPR12 void pop_back() MJ_EXCEPT_CRIT
    { 
        const auto &t = tiles_.back();
        MJ_ASSERT_CRIT(tiles4_[t.id34()] > 0, "Something is wrong with 4hot tiles");
        --tiles4_[t.id34()];
        tiles_.pop_back(); 

    }

    CONSTEXPR12 Fast8 melds() const noexcept
    { return melds_.size(); }
    CONSTEXPR12 const Meld &meld(Fast8 idx) const noexcept
    { return melds_[idx]; }
    CONSTEXPR12 Meld &meld(Fast8 idx) noexcept
    { return melds_[idx]; }

    CONSTEXPR12 Hand4Hot &hand_4hot() noexcept { return tiles4_; }
    CONSTEXPR12 const Hand4Hot &hand_4hot() const noexcept { return tiles4_; }

    CONSTEXPR12 HandDense::iterator begin() noexcept
    { return tiles_.begin(); }
    CONSTEXPR12 HandDense::iterator end() noexcept
    { return tiles_.end(); }
    CONSTEXPR12 HandDense::const_iterator begin() const noexcept
    { return tiles_.begin(); }
    CONSTEXPR12 HandDense::const_iterator end() const noexcept
    { return tiles_.end(); }

private:
    U64 flags_{};

    mutable HandDense tiles_{};
    Hand4Hot tiles4_;
    mutable bool sorted_{};
    Melds melds_;

public:
    std::string to_string() const
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

} // namespace mj
