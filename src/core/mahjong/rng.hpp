
#include "game.hpp"
#include "core/adt/vector.hpp"

namespace mj {
namespace random {
using WallContainer = vector<Tile, k_DeckSize>;
using WallContainer34 = vector<int, k_DeckSize>;

constexpr void fill_wall(WallContainer &w) noexcept
{
    for (U8f i = 0; i < k_UniqueTiles; ++i)
        for (U8f j = 0; j < 4; ++j)
            w.push_back(convert34(i));
}

constexpr void fill_wall(WallContainer34 &w) noexcept
{
    for (U8f i = 0; i < k_UniqueTiles; ++i)
        for (U8f j = 0; j < 4; ++j)
            w.push_back(i);
}

template<typename RngType>
class RngDeck : public TileSrc
{
public:
    RngDeck(RngType &rng) : rng_(rng) 
    {
        fill_wall(wall_);
        std::shuffle(wall_.begin(), wall_.end(), rng_);
        live_tiles_ = wall_.size() - k_DeadWallSize;
    }
    virtual Tile operator()(Dir dir) override
    {
        if (live_tiles_ == 0)
            return Tile();
        else live_tiles_--;
    
        auto tile = wall_.back();
        wall_.pop_back();
        tile.set_dir(dir);
        return tile;
    }
    virtual Tile dora() override
    {
        if (live_tiles_ == 0)
            return Tile();
        auto tile = wall_.back();
        wall_.pop_back();
        return tile;
    }
protected:
    RngType &rng_;
    WallContainer wall_;
    U16f live_tiles_;
};

class StdDeck : public RngDeck<std::mt19937_64>
{
public:
    StdDeck() : RngDeck(mt_) {}
private:
    std::mt19937_64 mt_ {std::random_device{}()};
};

template<typename RngType>
class Rng34
{
public:
    Rng34(RngType &rng) : rng_(rng) 
    {
        fill_wall(wall_);
        std::shuffle(wall_.begin(), wall_.end(), rng_);
    }
    int operator()() MJ_EXCEPT_WARN
    {
        MJ_ASSERT(!wall_.empty(), "Rng34: empty wall");
        auto tile = wall_.back();
        wall_.pop_back();
        return tile;
    }
    void draw_n(U8f n, Hand4Hot &h) MJ_EXCEPT_WARN
    {
        MJ_ASSERT(n <= wall_.size(), "Rng34: not enough tiles");
        for (U8f i = 0; i < n; ++i)
            h[(*this)()]++;
    }
    Hand4Hot draw_n(U8f n) MJ_EXCEPT_WARN
    {
        Hand4Hot h {};
        draw_n(n, h);
        return h;
    }
private:
    RngType &rng_;
    WallContainer34 wall_;
};

} // namespace random
} // namespace mj
