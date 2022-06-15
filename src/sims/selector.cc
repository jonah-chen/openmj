
#include "selector.hpp"
#include <string>
#include <algorithm>
#include <numeric>

namespace mj {
namespace draw {

TileSelector::TileSelector(intptr_t tex_id, Hand &hand)
    : tex_id_(tex_id), hand_(hand)
{
    std::fill(weights_.begin(), weights_.end(), 1.0f);
    std::fill(remain_.begin(), remain_.end(), 4);
    for (auto &t : hand.tiles())
        remain_[t.id34()]--;
}

void TileSelector::on_gui_render()
{
    ImGui::Begin("Tile Selector");
    ImGui::SetCursorPos(ImVec2(0,0));
    ImVec2 size = ImGui::GetContentRegionAvail();
    float width_abs = size.x / 18.f / (1.f + k_TileXGap);
    float height_abs = width_abs * k_TileAspect;
    ImVec2 tile_size(width_abs, height_abs);
    float info_height = height_abs * 0.5f;
    ImVec2 child_size(width_abs, height_abs + info_height);

    for (Fast8 i = 0; i < k_UniqueTiles; ++i)
    {
        ImVec2 pos = i < 18 ? ImVec2(i * width_abs * (1.f + k_TileXGap), size.y * 0.2f) :
            ImVec2((i - 18) * width_abs * (1.f + k_TileXGap), size.y * 0.6f);
        ImGui::SetCursorPos(pos);
        char *pos_ptr = reinterpret_cast<char*>(&pos);
        std::string s_prefix = "###Tile Selector " + std::to_string(i);
        std::string c_prefix = "###Selector Child " + std::to_string(i);
        ImGui::BeginChild(c_prefix.c_str(), child_size);
        ImGui::PushID(pos_ptr, pos_ptr+8);
        if (ImGui::ImageButton(
            (void*)tex_id_,
            tile_size,
            ImVec2(i/34.f, 0.0f),
            ImVec2((i+1)/34.f, 0.5f),
            -1,
            k_BackgroundColor,
            ImVec4(1,1,1, (1.0f + remain_[i]) / 5)
        )){
            if (remain_[i] > 0)
            {
                remain_[i]--;
                hand_.push_back(convert34(i));
            }
        }
        ImGui::PopID();
        ImGui::Text("%d", remain_[i]);
        ImGui::SameLine();
        // log slider but hide label
        ImGui::DragFloat(s_prefix.c_str(), &weights_[i], 0.5, 1e-3, 1e3, "%.2f", ImGuiSliderFlags_Logarithmic);
        ImGui::EndChild();
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
