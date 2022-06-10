
#pragma once
#include "core/mahjong/mahjong.hpp"
#include "layerstack.hpp"
#include "gui.hpp"
#include <functional>

namespace mj {
namespace draw {
namespace config {
using callback_fn = std::function<void(Tile)>;
template<typename T>
using p_cfg = std::array<T, k_NumPlayers>;
struct val
{
    static float tile_width;
    static float tile_aspect;
    static float x_gap;
    static int tex_id;
    static p_cfg<ImVec2> hand_base;
    static p_cfg<ImVec2> discard_base;
    static p_cfg<ImVec2> dx;
    static p_cfg<ImVec2> dy;
    static p_cfg<int> rot;
    static ImVec4 tsumogiri_tint;
};

constexpr void set_tex_id(unsigned int id) { val::tex_id = id; }
constexpr void set_tile_width(float w) { val::tile_width = w; }
constexpr void set_tile_aspect(float a) { val::tile_aspect = a; }
constexpr void set_x_gap(float g) { val::x_gap = g; }

} // namespace tileproperty

class HandElem : public ViewportElement
{
public:
    using callback_fn = std::function<void(Tile)>;
public:
    HandElem(const HandDense &hand, ImVec2 base=ImVec2(0,0), ImVec2 move_dir=ImVec2(1,0), int rot=0, callback_fn callback=nullptr)
        : hand_(hand), base_(base), move_dir_(move_dir), rot_(rot), callback_(callback) {}
    HandElem(const HandDense &hand, Dir relative_pos);
    virtual ~HandElem() {}
    void on_gui_render() override;

    void set_callback(callback_fn fn) { callback_ = fn; }
private:
    const HandDense &hand_;
    ImVec2 base_;
    ImVec2 move_dir_;
    int rot_;
    callback_fn callback_ { nullptr };
};

class DiscardElem : public ViewportElement
{
public:
    using callback_fn = std::function<void(Tile)>;
public:
    DiscardElem(const Discards &discards, ImVec2 base=ImVec2(0,0), ImVec2 dx=ImVec2(1,0), ImVec2 dy=ImVec2(0,1), int rot=0, callback_fn callback=nullptr) 
    :discards_(discards), base_(base), dx_(dx), dy_(dy), rot_(rot), callback_(callback) {}
    DiscardElem(const Discards &discards, Dir relative_pos);
    void on_gui_render() override;
    
    void set_callback(callback_fn fn) { callback_ = fn; }

private:
    const Discards &discards_;
    ImVec2 base_;
    ImVec2 dx_;
    ImVec2 dy_;
    int rot_;
    callback_fn callback_ { nullptr };
};

} // namespace draw
} // namespace mj
