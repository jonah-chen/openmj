
#pragma once
#include "core/mahjong/mahjong.hpp"
#include "core/engine/gui.hpp"
#include <array>

namespace mj {
namespace draw {
class TileSelector : public Layer
{
public:
    TileSelector(intptr_t tex_id, Hand &hand);

    void on_gui_render() override;
    std::array<float, k_UniqueTiles> prob() const;
    std::array<float, k_UniqueTiles> cum() const;

private:
    std::array<float, k_UniqueTiles> weights_;
    std::array<Fast8, k_UniqueTiles> remain_;
    intptr_t tex_id_;
    Hand &hand_;

private:
    constexpr static float k_TileAspect = 1.4f;
    constexpr static float k_TileXGap = 0.2f;
    constexpr static ImVec4 k_BackgroundColor = {0.7f, 0.2f, 0.2, 0.3f};
};

} // namespace draw
} // namespace mj
