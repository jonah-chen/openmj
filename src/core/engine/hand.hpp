
#pragma once
#include "core/mahjong/mahjong.hpp"
#include "gui.hpp"
#include "layerstack.hpp"
#include <functional>

namespace mj {
namespace draw {
struct Config
{
    using callback_fn = std::function<void(Tile)>;
    template <typename T> using p_cfg = std::array<T, k_NumPlayers>;

    float tile_width;
    float tile_aspect;
    float x_gap;
    int tex_id{};
    p_cfg<ImVec2> hand_base;
    p_cfg<ImVec2> discard_base;
    p_cfg<ImVec2> dx{{{1, 0}, {0, -1}, {-1, 0}, {0, 1}}};
    p_cfg<ImVec2> dy{{{0, 1}, {1, 0}, {0, -1}, {-1, 0}}};
    p_cfg<int> rot{0, 90, 180, 270};
    ImVec4 tsumogiri_tint{1, 1, 1, 1};

    static Config d_GameConfig();
};

class HandElem : public ViewportElement
{
public:
    using callback_fn = Config::callback_fn;

public:
    HandElem(const Config &cfg, const HandDense &hand,
             ImVec2 base = ImVec2(0, 0), ImVec2 move_dir = ImVec2(1, 0),
             int rot = 0, callback_fn callback = nullptr)
        : cfg_(cfg), hand_(hand), base_(base), move_dir_(move_dir), rot_(rot),
          callback_(callback)
    {
    }
    HandElem(const Config &cfg, const HandDense &hand, Dir relative_pos);
    virtual ~HandElem() {}
    void on_gui_render() override;

    void set_callback(callback_fn fn) { callback_ = fn; }

private:
    Config cfg_;
    const HandDense &hand_;
    ImVec2 base_;
    ImVec2 move_dir_;
    int rot_;
    callback_fn callback_{nullptr};
};

class DiscardElem : public ViewportElement
{
public:
    using callback_fn = Config::callback_fn;

public:
    DiscardElem(const Config &cfg, const Discards &discards,
                ImVec2 base = ImVec2(0, 0), ImVec2 dx = ImVec2(1, 0),
                ImVec2 dy = ImVec2(0, 1), int rot = 0,
                callback_fn callback = nullptr)
        : cfg_(cfg), discards_(discards), base_(base), dx_(dx), dy_(dy),
          rot_(rot), callback_(callback)
    {
    }
    DiscardElem(const Config &cfg, const Discards &discards, Dir relative_pos);
    void on_gui_render() override;

    void set_callback(callback_fn fn) { callback_ = fn; }

private:
    Config cfg_;
    const Discards &discards_;
    ImVec2 base_;
    ImVec2 dx_;
    ImVec2 dy_;
    int rot_;
    callback_fn callback_{nullptr};
};

} // namespace draw
} // namespace mj
