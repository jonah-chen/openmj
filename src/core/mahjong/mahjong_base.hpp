
#pragma once
#include "constants.hpp"
#include "core/utils/logging.hpp"
#include <algorithm>
#include <array>


namespace mj {
/**
 * Convert computer-readable tile number to human-readable tile number.
 * 
 * @param c_num computer-readable tile number
 * @return constexpr U8f human-readable tile number
 * @warning Does not check for invalid tile number
 */
constexpr U8f h_num(U8f c_num) { return c_num + 1; }

/**
 * Convert human-readable tile number to computer-readable tile number.
 * 
 * @param h_num human-readable tile number 
 * @return constexpr U8f computer-readable tile number
 * @warning Does not check for invalid tile number
 */
constexpr U8f c_num(U8f h_num) { return h_num - 1; }

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

enum Dir : U8f
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
    constexpr Tile(Suit suit, U8f num, U8f player=k_East, U16 flags=0)
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

    /** 
     * @return constexpr U8f the tile's number (0-8 for non-honors)
     */
    constexpr U8f num() const noexcept
    { return (id_ >> tilelayout::k_NumPos) & 15; }
    /** 
     * @return constexpr U8f the tile's human-readable number (1-9 for 
     * non-honors)
     */
    constexpr U8f num1() const noexcept
    { return num() + 1; }

    constexpr U8f player() const noexcept
    { return (id_ >> tilelayout::k_PlayerPos) & 3; }

    constexpr operator bool() const noexcept
    { return id_ != f_All16; }
    constexpr U16 id() const noexcept
    { return id_; }

    /**
     * @return constexpr U8f The tile content ignoring the flags. 
     */
    constexpr U8f id7() const noexcept
    { return (id_ >> tilelayout::k_NumPos) & 127; }

    /** 
     * @return constexpr U8f The tile in the order 0 to 33.
     */
    constexpr U8f id34() const noexcept
    { return k_Offsets[static_cast<U8f>(suit())] + num(); }

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
    CONSTEXPR12 std::string to_string() const
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

    constexpr void add_fourth(const Tile &t) MJ_EXCEPT_WARN
    {
        MJ_ASSERT(!fourth(), "add_fourth() called on meld with fourth tile already");
        id_ |= t.id();
    }

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
    CONSTEXPR12 std::string to_string() const
    {
        return first().to_string() + second().to_string() +
            third().to_string() + fourth().to_string();
    }
};

} // namespace mahjong