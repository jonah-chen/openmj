
#include "core/adt/itertools.hpp"
#include "core/adt/vector.hpp"
#include "game.hpp"

namespace mj {
namespace random {
using WallContainer = vector<Tile, k_DeckSize>;
using WallContainer34 = vector<int, k_DeckSize>;

/**
 * Fill a wall with the full deck of tiles
 *
 * @param w The wall to fill
 */
constexpr void fill_wall(WallContainer &w) noexcept
{
    for (U8f i : range(k_UniqueTiles))
        for (U8f j : range(4))
            w.push_back(convert34(i));
}

/**
 * Fill a wall with the full deck of tiles
 *
 * @param w The wall to fill
 */
constexpr void fill_wall(WallContainer34 &w) noexcept
{
    for (U8f i : range(k_UniqueTiles))
        for (U8f j : range(4))
            w.push_back(i);
}

/**
 * Fill a wall with given tiles
 *
 * @param w The wall to fill
 * @param tiles The tiles to fill the wall with
 */
constexpr void fill_wall(WallContainer34 &w, const Hand4Hot &tiles) noexcept
{
    for (const auto &[idx, num] : enumerate(tiles))
        for (U8f i = 0; i < num; ++i)
            w.push_back(idx);
}

/**
 * Tile source that acts like the deck (tiles in the wall) containing 4 of each
 * type of tile in the game. This contains a live wall and a dead wall.
 *
 * @tparam RngType The type of random number generator to use.
 */
template <typename RngType> class RngDeck : public TileSrc
{
public:
    RngDeck(RngType &rng) : rng_(rng)
    {
        fill_wall(wall_);
        std::shuffle(wall_.begin(), wall_.end(), rng_);
        live_tiles_ = wall_.size() - k_DeadWallSize;
        dead_tiles_ = k_DeadWallSize;
    }

    /**
     * Draws the next tile from the live wall for a given player.
     *
     * @param dir the seat of the player
     * @return Tile the next tile to be drawn
     * @return Invalid Tile if the wall is empty
     */
    virtual Tile operator()(Dir dir) override
    {
        if (live_tiles_ == 0)
            return Tile();
        else
            live_tiles_--;

        auto tile = wall_.back();
        wall_.pop_back();
        tile.set_dir(dir);
        return tile;
    }

    /**
     * Draws a dora tile from the dead wall.
     *
     * @return Tile the next dora tile to be drawn
     * @return Invalid Tile if the dead wall is empty
     */
    virtual Tile dora() override
    {
        if (dead_tiles_ == 0)
            return Tile();
        else
            dead_tiles_--;

        auto tile = wall_.back();
        wall_.pop_back();
        return tile;
    }

protected:
    RngType &rng_;
    WallContainer wall_;
    U16f live_tiles_;
    U16f dead_tiles_;
};

/**
 * Standard deck using the mt19937_64 random number generator and a random seed.
 */
class StdDeck : public RngDeck<std::mt19937_64>
{
public:
    StdDeck() : RngDeck(mt_) {}

private:
    std::mt19937_64 mt_{std::random_device{}()};
};

/**
 * For debugging purposes, a deck with set seed (default 34) using the
 * mt19937_64 random number generator.
 */
class DbgDeck : public RngDeck<std::mt19937_64>
{
public:
    DbgDeck(U64 seed = 34ULL) : seed_(seed), RngDeck(mt_) {}

private:
    U64 seed_{34ULL};
    std::mt19937_64 mt_{seed_};
};

/**
 * A tile source for Tile34 (4hot labels rather than tiles) that can act as a
 * deck for simulation purposes.
 *
 * @tparam RngType The type of random number generator to use.
 */
template <typename RngType> class Rng34
{
public:
    Rng34(RngType &rng) : rng_(rng)
    {
        fill_wall(wall_);
        std::shuffle(wall_.begin(), wall_.end(), rng_);
    }

    Rng34(RngType &rng, const Hand4Hot &tiles) : rng_(rng)
    {
        fill_wall(wall_, tiles);
        std::shuffle(wall_.begin(), wall_.end(), rng_);
    }

    /**
     * Draws the next tile from the live wall.
     *
     * @return int the id34 of the next tile to be drawn
     * @throws AssertionError if the wall is empty. This should never happen.
     */
    int operator()() MJ_EXCEPT_WARN
    {
        MJ_ASSERT(!wall_.empty(), "Rng34: empty wall");
        auto tile = wall_.back();
        wall_.pop_back();
        return tile;
    }

    /**
     * Draws multiple tiles from the live wall and places it into the hand.
     *
     * @param n the number of tiles to draw
     * @param h the hand to place the drawn tiles in
     * @throws AssertionError if the wall does not have n tiles.
     */
    void draw_n(U8f n, Hand4Hot &h) MJ_EXCEPT_WARN
    {
        MJ_ASSERT(n <= wall_.size(), "Rng34: not enough tiles");
        for (U8f i = 0; i < n; ++i)
            h[(*this)()]++;
    }

    /**
     * Draws multiple tiles from the live wall and places it into an empty hand.
     *
     * @param n the number of tiles to draw
     * @return Hand4Hot the hand containing the drawn tiles
     */
    Hand4Hot draw_n(U8f n) MJ_EXCEPT_WARN
    {
        Hand4Hot h{};
        draw_n(n, h);
        return h;
    }

private:
    RngType &rng_;
    WallContainer34 wall_;
};

template <typename RngType> class Weighted34
{
public:
    Weighted34(RngType &rng) : rng_(rng) {}
    int operator()(const TileWeights &weights, const Hand4Hot &remain) noexcept
    {
        TileWeights pmf, cmf;
        // sum
        float sum = std::inner_product(weights.begin(), weights.end(),
                                       remain.begin(), 0.f);
        for (U8f i = 0; i < k_UniqueTiles; ++i)
            pmf[i] = weights[i] * remain[i] / sum;
        // cum
        std::partial_sum(pmf.begin(), pmf.end(), cmf.begin());
        return (*this)(cmf);
    }
    int operator()(const TileWeights &cmf) noexcept
    {
        float r = dist_(rng_);
        auto it = std::lower_bound(cmf.begin(), cmf.end(), r);
        return std::distance(cmf.begin(), it);
    }

private:
    RngType &rng_;
    std::uniform_real_distribution<F32> dist_;
};

} // namespace random
} // namespace mj
