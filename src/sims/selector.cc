
#include "selector.hpp"
#include <string>
#include <algorithm>
#include <numeric>

namespace mj {
namespace draw {

void TileSelector::on_gui_render()
{
    ImGui::Begin("Tile Selector");
    ImGui::SetCursorPos(ImVec2(0,0));
    ImVec2 size = ImGui::GetContentRegionAvail();
    float width_abs = size.x / 34.f;
    float height_abs = width_abs * k_TileAspect;
    ImVec2 tile_size(width_abs, height_abs);

    for (Fast8 i = 0; i < k_UniqueTiles; ++i)
    {
        ImVec2 pos = i < 18 ? ImVec2(i * width_abs * (1.f + k_TileXGap), 0) :
            ImVec2((i - 18) * width_abs * (1.f + k_TileXGap), size.y / 2);

        ImGui::SetCursorPos(pos);

        if (ImGui::ImageButton(
            (void*)(&tex_id_),
            tile_size,
            ImVec2(i/34.f, 0.0f),
            ImVec2((i+1)/34.f, 0.5f)
        )){
            remain_[i]--;
        }
        std::string prefix = "###Tile Selector " + std::to_string(i);
        // log slider but hide label
        ImGui::SliderFloat(prefix.c_str(), &weights_[i], 1e-3f, 1e3f, "%.1f", ImGuiSliderFlags_Logarithmic);
    }

    ImGui::End();
}

std::array<float, k_UniqueTiles> TileSelector::prob() const
{
    std::array<float, k_UniqueTiles> ret;
    // sum
    float sum = std::inner_product(weights_.begin(), weights_.end(), remain_.begin(), 0.f);
    for (Fast8 i = 0; i < k_UniqueTiles; ++i)
        ret[i] = weights_[i] * remain_[i] / sum;
    return ret;
}

std::array<float, k_UniqueTiles> TileSelector::cum() const
{
    std::array<float, k_UniqueTiles> ret;
    auto probability = prob();
    std::partial_sum(probability.begin(), probability.end(), ret.begin());
    return ret;
}

} // namespace draw
} // namespace mj
