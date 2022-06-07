
#pragma once

#if __GNUC__ >= 12
#define CONSTEXPR12 constexpr
#else
#define CONSTEXPR12 inline
#endif

#include <array>
#include <vector>
#include <cstdint>
#include "core/utils/stack_allocator.hpp"

namespace mj {
using Fast8 = uint_fast8_t;
using Fast16 = uint_fast16_t;
using Fast32 = uint_fast32_t;
using Fast64 = uint_fast64_t;
using U8 = uint8_t;
using U16 = uint16_t;
using U32 = uint32_t;
using U64 = uint64_t;
using S8 = int8_t;
using S16 = int16_t;
using S32 = int32_t;
using S64 = int64_t;
using F32 = float;
using F64 = double;
using F64 = double;
constexpr U8 f_All8 = (U8)-1;
constexpr U16 f_All16 = (U16)-1;
constexpr U32 f_All32 = (U32)-1;
constexpr U64 f_All64 = (U64)-1;

constexpr Fast8 k_East = 0;
constexpr Fast8 k_South = 1;
constexpr Fast8 k_West = 2;
constexpr Fast8 k_North = 3;
constexpr Fast8 k_NumWinds = 4;

constexpr Fast8 k_Green = 0;
constexpr Fast8 k_Red = 1;
constexpr Fast8 k_White = 2;
constexpr Fast8 k_NumDragons = 3;

constexpr Fast16 k_UniqueTiles = 34;
constexpr Fast16 k_DeckSize = 136;
constexpr Fast16 k_DeadWallSize = 14;
constexpr Fast16 k_MaxHandSize = 14;
constexpr Fast16 k_MaxNumMeld = 4;

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

namespace tilelayout
{
    constexpr Fast8 k_PlayerPos = 6;
    constexpr Fast8 k_NumPos = 8;
    constexpr Fast8 k_SuitPos = 12;

    constexpr U16 f_Transparent = 0x0001;
    constexpr U16 f_Red = 0x0002;
    constexpr U16 f_Open = 0x0004;
} // namespace tilelayout

enum class Suit : U16
{
    Man, Pin, Sou, Wind, Dragon
};

constexpr Suit &operator++(Suit &suit)
{
    return suit = static_cast<Suit>(static_cast<U16>(suit) + 1);
}

constexpr bool operator==(Suit lhs, Suit rhs)
{
    return static_cast<U16>(lhs) == static_cast<U16>(rhs);
}

constexpr bool operator<(Suit lhs, Suit rhs)
{
    return static_cast<U16>(lhs) < static_cast<U16>(rhs);
}

constexpr bool operator>(Suit lhs, Suit rhs)
{
    return static_cast<U16>(lhs) > static_cast<U16>(rhs);
}

enum class Dir : Fast8
{
    East = k_East,
    South = k_South,
    West = k_West,
    North = k_North
};

/**
 * Increment direction in the same way turns pass in mahjong, including 
 * wrapping around
 * 
 * @param dir Direction to increment
 * @return constexpr Dir& Incremented direction
 */
constexpr Dir &operator++(Dir &dir)
{
    dir = static_cast<Dir>((static_cast<Fast8>(dir) + 1) & 3);
    return dir;
}
/**
 * Increment direction in the same way turns pass in mahjong, including 
 * wrapping around
 * 
 * @param dir Direction to increment
 * @return constexpr Dir Incremented direction
 */
constexpr Dir operator++(Dir &dir, int)
{
    Dir tmp = static_cast<Dir>((static_cast<Fast8>(dir) + 1) & 3);
    dir++;
    return tmp;
}

/**
 * The contents of a mahjong tile is represented as a 16-bit integer. The 7 high
 * bits represent the tile itself, and the 9 low bits are reserved for flags.
 */
class Tile
{
public:
    constexpr Tile() noexcept : id_(-1) {}
    constexpr Tile(U16 id) noexcept : id_(id) {}
    constexpr Tile(Suit suit, Fast8 num, Fast8 player=k_East, U16 flags=0)
    noexcept : id_((U16)suit << tilelayout::k_SuitPos | 
                        num << tilelayout::k_NumPos | 
                        player << tilelayout::k_PlayerPos | flags) {}

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
    { return id_ != (U16)-1; }
    constexpr U16 id() const noexcept
    { return id_; }

    /**
     * @return constexpr Fast8 The tile content ignoring the flags. 
     */
    constexpr Fast8 id7() const noexcept
    { return (id_ >> tilelayout::k_NumPos) & 127; }

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
};

/**
 * Melds, which represent all sets of tiles possible in the game, including
 * pairs, triples, and quads.
 */
class Meld
{
public:
    constexpr Meld() noexcept : id_(-1) {}
    constexpr Meld(Tile called, Tile t1, Tile t2 = {}, Tile t3 = {})
        : id_(((U64)called.id()<< 48) |
              ((U64)t1.id() << 32) |
              ((U64)t2.id() << 16) |
               t3.id()) {}

    /** 
     * @return constexpr Tile The first tile in the meld.
     */
    constexpr Tile first() const noexcept
    { return Tile(id_ >> 48); }
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
    { return id_ != -1; }
    
    constexpr bool operator==(const Meld &rhs) const
    { return id_ == rhs.id_; }
    constexpr bool operator!=(const Meld &rhs) const
    { return id_ != rhs.id_; }

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

private:
    U64 id_;

    constexpr static U64 mask7 = 127ul << tilelayout::k_NumPos;
    constexpr static U64 Mask7 = mask7 | mask7 << 16 | mask7 << 32 | mask7 << 48;
};

using Melds = s_Vector<Meld, k_MaxNumMeld>;

struct Win
{
    Melds melds;
    Meld pair;
    U64 flags;
    Win() noexcept : flags() {}
    Win(const Melds &melds, const Meld &pair, U64 flags=0)
        : melds(melds), pair(pair), flags(flags) {}
    
    constexpr U64 check(U64 mask, Fast8 offset=0) const noexcept
    { return (flags & mask) >> offset; }
    constexpr void set(U64 mask) noexcept
    { flags |= mask; }
    constexpr void clear(U64 mask) noexcept
    { flags &= ~mask; }
};

using WaitingTiles = s_Vector<Tile, 13>;
using Wins = s_Vector<Win, 8>;

constexpr Fast8 fp_Riichi = 62;
constexpr U64 f_SingleRiichi = 1ull << fp_Riichi;
constexpr U64 f_DoubleRiichi = 2ull << fp_Riichi;
constexpr U64 f_Riichi = 3ull << fp_Riichi;

constexpr Fast8 fp_Ippatsu = 61;
constexpr U64 f_Ippatsu = 1ull << fp_Ippatsu;

class Hand
{
public:
    Hand() = default;
    Hand(const char *);

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

    CONSTEXPR12 Fast8 size() const noexcept
    { return tiles_.size(); }

    CONSTEXPR12 Tile &operator[](Fast8 idx) noexcept
    { sorted_ = false; return tiles_[idx]; }
    CONSTEXPR12 const Tile &operator[](Fast8 idx) const noexcept
    { return tiles_[idx]; }
 
    constexpr void mark_sorted() const noexcept { sorted_ = true; }
    CONSTEXPR12 void push_back(const Tile &t)
    { tiles_.push_back(t); sorted_ = false; }
    CONSTEXPR12 void pop_back()
    { return tiles_.pop_back(); }

    CONSTEXPR12 Fast8 melds() const noexcept
    { return melds_.size(); }
    CONSTEXPR12 const Meld &meld(Fast8 idx) const noexcept
    { return melds_[idx]; }
    CONSTEXPR12 Meld &meld(Fast8 idx) noexcept
    { return melds_[idx]; }

private:
    U64 flags_{};

    mutable s_Vector<Tile, k_MaxHandSize> tiles_{};
    mutable bool sorted_{};

    Melds melds_;
};

} // namespace mahjong
