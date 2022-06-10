
#pragma once
#include "core/mahjong/mahjong.hpp"
#include "layerstack.hpp"
#include "gui.hpp"
#include <functional>

namespace mj {
namespace draw {
namespace tileproperty {
using button_fn = std::function<bool(ImTextureID, const ImVec2&, const ImVec2&, const ImVec2&, int, const ImVec4&, const ImVec4&)>;
using image_fn = std::function<void(ImTextureID, const ImVec2&, const ImVec2&, const ImVec2&, const ImVec4&, const ImVec4&)>;
using callback_fn = std::function<void(Tile)>;
struct val
{
    static float tile_width;
    static float tile_aspect;
    static float x_gap;
    static int tex_id;
};

constexpr void set_tex_id(unsigned int id) { val::tex_id = id; }
constexpr void set_tile_width(float w) { val::tile_width = w; }
constexpr void set_tile_aspect(float a) { val::tile_aspect = a; }
constexpr void set_x_gap(float g) { val::x_gap = g; }

} // namespace tileproperty

class HandElem : public ViewportElement
{
    using button_fn = tileproperty::button_fn;
    using image_fn = tileproperty::image_fn;
    using callback_fn = tileproperty::callback_fn;
public:
    HandElem(const HandDense &hand, ImVec2 base=ImVec2(0,0), ImVec2 move_dir=ImVec2(1,0), button_fn btn_fn=ImGui::ImageButton, image_fn img_fn=ImGui::Image, callback_fn callback=nullptr)
        : hand_(hand), base_(base), move_dir_(move_dir), btn_fn_(btn_fn), img_fn_(img_fn), callback_(callback) {}
    HandElem(const HandDense &hand, Dir relative_pos);
    virtual ~HandElem() {}
    void on_gui_render() override;

    void set_callback(callback_fn fn) { callback_ = fn; }
private:
    const HandDense &hand_;
    ImVec2 base_;
    ImVec2 move_dir_;
    button_fn btn_fn_;
    image_fn img_fn_;
    callback_fn callback_ { nullptr };
};

class DiscardElem : public ViewportElement
{
public:
    using button_fn = tileproperty::button_fn;
    using image_fn = tileproperty::image_fn;
    using callback_fn = tileproperty::callback_fn;
public:
    DiscardElem(const Discards &discards, ImVec2 base=ImVec2(0,0), ImVec2 dx=ImVec2(1,0), ImVec2 dy=ImVec2(0,1), button_fn btn_fn=ImGui::ImageButton, image_fn img_fn=ImGui::Image, callback_fn callback=nullptr) 
    :discards_(discards), base_(base), dx_(dx), dy_(dy), btn_fn_(btn_fn), img_fn_(img_fn), callback_(callback) {}
    DiscardElem(const Discards &discards, Dir relative_pos);
    void on_gui_render() override;
    
    void set_callback(callback_fn fn) { callback_ = fn; }

private:
    const Discards &discards_;
    ImVec2 base_;
    ImVec2 dx_;
    ImVec2 dy_;
    button_fn btn_fn_;
    image_fn img_fn_;
    callback_fn callback_ { nullptr };
};

} // namespace draw
} // namespace mj
