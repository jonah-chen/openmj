
#include "hand.hpp"
#include "core/utils/logging.hpp"
#include <algorithm>

namespace mj {
namespace draw {
/**
 * Default values
 */
Config Config::d_GameConfig()
{
    Config cfg;
    cfg.tile_width = 0.05f;
    cfg.tile_aspect = 1.4f;
    cfg.x_gap = 0.1f;
    cfg.hand_base = {{{0.1f,0.9f},{0.9f,0.9f},{0.9f,0.1f},{0.1f,0.1f}}};
    cfg.discard_base = {};
    cfg.dx = {{{1,0}, {0,-1}, {-1,0}, {0,1}}};
    cfg.tsumogiri_tint = {0.8f, 0.8f, 0.8f, 0.8f};
    return cfg;
}
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

HandElem::HandElem(const Config &cfg, const HandDense &hand, Dir relative_pos)
    : cfg_(cfg), hand_(hand)
{
    std::size_t i_relative_pos = static_cast<std::size_t>(relative_pos);
    
    move_dir_ = cfg.dx[i_relative_pos];
    rot_ = cfg.rot[i_relative_pos];
    base_ = cfg.hand_base[i_relative_pos];
}

void HandElem::on_gui_render()
{
    MJ_ASSERT_CRIT(cfg_.tex_id, "HandElem::on_gui_render: tex_id is 0");

    ImGui::SetCursorPos(ImVec2(0,0));
    ImVec2 size = ImGui::GetContentRegionAvail();
    // calculate tile size
    float tile_width_abs = size.x * cfg_.tile_width;
    float tile_height_abs = tile_width_abs * cfg_.tile_aspect;
    ImVec2 tile_size(tile_width_abs, tile_height_abs);
    ImVec2 base_abs(base_.x * size.x, base_.y * size.y);

    // calculate tile gap
    float tile_spacing = tile_width_abs * (1 + cfg_.x_gap);
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
            if (ImGui::ImageButtonRotated((void*)(intptr_t)cfg_.tex_id, 
                    tile_size, rot_, ImVec2(left/34., 0), ImVec2((left+1)/34., 0.5)))
                callback_(hand_[i]);
            ImGui::PopID();
        }
        else
        {
            ImGui::ImageRotated((void*)(intptr_t)cfg_.tex_id, tile_size, rot_, 
            ImVec2(left/34., 0), ImVec2((left+1)/34., 0.5));
        }
    }
}

DiscardElem::DiscardElem(const Config &cfg, const Discards &discards, Dir relative_pos)
    : cfg_(cfg), discards_(discards)
{
    std::size_t i_relative_pos = static_cast<std::size_t>(relative_pos);
    base_ = cfg.discard_base[i_relative_pos];
    dx_ = cfg.dx[i_relative_pos];
    dy_ = cfg.dy[i_relative_pos];
    rot_ = cfg.rot[i_relative_pos];
    throw std::runtime_error("DiscardElem::DiscardElem: not fully implemented");
}


void DiscardElem::on_gui_render()
{
    MJ_ASSERT_CRIT(cfg_.tex_id, "DiscardElem::on_gui_render: tex_id is 0");

    ImGui::SetCursorPos(ImVec2(0,0));
    ImVec2 size = ImGui::GetContentRegionAvail();
    // calculate tile size
    float tile_width_abs = size.x * cfg_.tile_width;
    float tile_height_abs = tile_width_abs * cfg_.tile_aspect;
    ImVec2 tile_size(tile_width_abs, tile_height_abs);
    ImVec2 base_abs(base_.x * size.x, base_.y * size.y);

    float tile_width_gap = tile_width_abs * (1 + cfg_.x_gap);
    float tile_height_gap = tile_height_abs * (1 + cfg_.x_gap);

    for (unsigned int i = 0; i < discards_.size(); ++i)
    {
        int dy = std::min(i/6, 2u);
        int dx = i - (dy*6);

        ImVec2 pos = base_abs + (tile_width_gap * dx * dx_) + (tile_height_gap * dy * dy_);
        int left = discards_[i].id34();
        ImVec4 tint = discards_[i] & Tile::f_Tsumogiri ? cfg_.tsumogiri_tint : ImVec4(1,1,1,1);
        ImGui::SetCursorPos(pos);
        if (callback_)
        {
            // we need to make an id. we can use 8 bytes
            char *pos_ptr = (char *)&pos;
            ImGui::PushID(pos_ptr, pos_ptr+8);
            if (ImGui::ImageButtonRotated((void*)(intptr_t)cfg_.tex_id, 
                    tile_size, rot_, ImVec2(left/34., 0), ImVec2((left+1)/34., 0.5), 
                    -1, ImVec4(0,0,0,0), tint))
                callback_(discards_[i]);
            ImGui::PopID();
        }
        else
        {
            ImGui::ImageRotated((void*)(intptr_t)cfg_.tex_id, tile_size, rot_,
            ImVec2(left/34., 0), ImVec2((left+1)/34., 0.5), tint);
        }
    }
}

} // namespace draw
} // namespace mj
