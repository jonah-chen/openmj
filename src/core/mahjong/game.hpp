
#pragma once
#include "mahjong.hpp"

namespace mj {
class TileSrc {
public:
    virtual ~TileSrc() = default;

    /**
     * @return Tile draw the next tile for the player at direction dir.
     */
    virtual Tile operator()(Dir dir) = 0;

    /**
     * @return Tile draw a tile without changing the number of live tiles remaining.
     */
    virtual Tile dora() = 0;
};




} // namespace mj
