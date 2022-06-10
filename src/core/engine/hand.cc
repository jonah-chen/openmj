
#include "hand.hpp"
#include "core/utils/logging.hpp"
#include <algorithm>

namespace mj {
namespace draw {
namespace {
inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) 
{
    return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
}
inline ImVec2 operator*(const ImVec2& lhs, float rhs) 
{
    return ImVec2(lhs.x * rhs, lhs.y * rhs);
}
inline ImVec2 operator*(float lhs, const ImVec2& rhs) 
{
    return ImVec2(lhs * rhs.x, lhs * rhs.y);
}
} // anon namespace

float tileproperty::val::tile_width = 0.05f;
float tileproperty::val::tile_aspect = 1.4f;
float tileproperty::val::x_gap = 0.1f;
int tileproperty::val::tex_id = 0;

HandElem::HandElem(const HandDense &hand, Dir relative_pos)
    : hand_(hand)
{
    switch (relative_pos) 
    {
    case Dir::East:
        move_dir_ = ImVec2(1, 0);
        base_ = ImVec2(0.1f, 0.9f);
        btn_fn_ = ImGui::ImageButton;
        img_fn_ = ImGui::Image;
        break;
    case Dir::West:
        move_dir_ = ImVec2(-1, 0);
        base_ = ImVec2(0.9f, 0.1f);
        btn_fn_ = ImGui::ImageButton180;
        img_fn_ = ImGui::Image180;
        break;
    case Dir::North:
        move_dir_ = ImVec2(0, 1);
        base_ = ImVec2(0.1f, 0.1f);
        btn_fn_ = ImGui::ImageButtonCW;
        img_fn_ = ImGui::ImageCW;
        break;
    case Dir::South:
        move_dir_ = ImVec2(0, -1);
        base_ = ImVec2(0.9f, 0.9f);
        btn_fn_ = ImGui::ImageButtonCCW;
        img_fn_ = ImGui::ImageCCW;
        break;
    }
}

void HandElem::on_gui_render()
{
    using namespace tileproperty;

    MJ_ASSERT_CRIT(val::tex_id, "HandElem::on_gui_render: tex_id is 0");

    ImGui::SetCursorPos(ImVec2(0,0));
    ImVec2 size = ImGui::GetContentRegionAvail();
    // calculate tile size
    float tile_width_abs = size.x * val::tile_width;
    float tile_height_abs = tile_width_abs * val::tile_aspect;
    ImVec2 tile_size(tile_width_abs, tile_height_abs);
    ImVec2 base_abs(base_.x * size.x, base_.y * size.y);

    // calculate tile gap
    float tile_spacing = tile_width_abs * (1 + val::x_gap);
    for (std::size_t i = 0; i < hand_.size(); ++i)
    {
        ImVec2 pos = base_abs + (tile_spacing * i) * move_dir_;
        int left = hand_[i].id34();
        ImGui::SetCursorPos(pos);
        if (callback_)
        {
            // we need to make an id. we can use 8 bytes
            char *pos_ptr = (char *)&pos;
            ImGui::PushID(pos_ptr, pos_ptr+8);
            if (btn_fn_((void*)(intptr_t)val::tex_id, 
                    tile_size, ImVec2(left/34., 0), ImVec2((left+1)/34., 0.5), 
                    -1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1)))
                callback_(hand_[i]);
            ImGui::PopID();
        }
        else
        {
            img_fn_((void*)(intptr_t)val::tex_id, tile_size, ImVec2(left/34., 0), ImVec2((left+1)/34., 0.5), ImVec4(1,1,1,1), ImVec4(0,0,0,0));
        }
    }
}


void DiscardElem::on_gui_render()
{
    using namespace tileproperty;

    MJ_ASSERT_CRIT(val::tex_id, "DiscardElem::on_gui_render: tex_id is 0");

    ImGui::SetCursorPos(ImVec2(0,0));
    ImVec2 size = ImGui::GetContentRegionAvail();
    // calculate tile size
    float tile_width_abs = size.x * val::tile_width;
    float tile_height_abs = tile_width_abs * val::tile_aspect;
    ImVec2 tile_size(tile_width_abs, tile_height_abs);
    ImVec2 base_abs(base_.x * size.x, base_.y * size.y);

    float tile_width_gap = tile_width_abs * (1 + val::x_gap);
    float tile_height_gap = tile_height_abs * (1 + val::x_gap);

    for (unsigned int i = 0; i < discards_.size(); ++i)
    {
        int dy = std::min(i/6, 2u);
        int dx = i - (dy*6);

        ImVec2 pos = base_abs + (tile_width_gap * dx * dx_) + (tile_height_gap * dy * dy_);
        int left = discards_[i].id34();
        ImGui::SetCursorPos(pos);
        if (callback_)
        {
            // we need to make an id. we can use 8 bytes
            char *pos_ptr = (char *)&pos;
            ImGui::PushID(pos_ptr, pos_ptr+8);
            if (btn_fn_((void*)(intptr_t)val::tex_id, 
                    tile_size, ImVec2(left/34., 0), ImVec2((left+1)/34., 0.5), 
                    -1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1)))
                callback_(discards_[i]);
            ImGui::PopID();
        }
        else
        {
            img_fn_((void*)(intptr_t)val::tex_id, tile_size, 
            ImVec2(left/34., 0), ImVec2((left+1)/34., 0.5), ImVec4(1,1,1,1), ImVec4(0,0,0,0));
        }
    }
}

} // namespace draw
} // namespace mj
