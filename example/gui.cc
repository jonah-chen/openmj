
#include "core/engine/app.hpp"
#include "core/engine/texture.hpp"
#include "core/engine/hand.hpp"
#include "core/mahjong/mahjong.hpp"
#include "core/engine/layerstack.hpp"
#include "sims/selector.hpp"
#include <iostream>

int main(int, char**)
{
    mj::draw::App app;
    mj::Hand h1("s1112345678999");
    mj::draw::Texture tex("../assets/textures/tiles.png");
    mj::draw::Config cfg = mj::draw::Config::d_GameConfig();
    cfg.tex_id = tex.id();
    app.push_layer(new mj::draw::ImGuiDockspace);
    auto *vp = new mj::draw::Viewport("Viewport");
    app.push_layer(vp);
    auto *mh = new mj::draw::HandElem(cfg, h1.tiles(), mj::k_East);
    vp->push_element(mh);
    mh->set_callback([](mj::Tile t) { std::cout << (int)t.id34() << std::endl; });
    app.push_layer(new mj::draw::TileSelector(cfg.tex_id, h1));
    app.run();
}
