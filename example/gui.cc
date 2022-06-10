
#include "core/engine/app.hpp"
#include "core/engine/texture.hpp"
#include "core/engine/hand.hpp"
#include "core/mahjong/mahjong.hpp"
#include "core/engine/layerstack.hpp"
#include <iostream>

int main(int, char**)
{
    mj::draw::App app;
    mj::Hand h1("s1112345678999");
    mj::Hand h2("m1112345678999");
    mj::Hand h3("p1112345678999");
    mj::Hand h4("w111222333444d1");
    mj::Discards d1;
    for (const mj::Tile &t : h1)
    {
        d1.push_back(t);
    }
    mj::draw::Texture tex("../assets/textures/tiles.png");
    mj::draw::tileproperty::set_tex_id(tex.id());
    app.push_layer(new mj::draw::ImGuiDockspace);
    auto *vp = new mj::draw::Viewport("Viewport");
    auto *mh = new mj::draw::HandElem(h1.tiles(), mj::Dir::East);
    vp->push_element(mh);
    auto *md = new mj::draw::DiscardElem(d1);
    vp->push_element(new mj::draw::HandElem(h2.tiles(), mj::Dir::South));
    vp->push_element(new mj::draw::HandElem(h3.tiles(), mj::Dir::West));
    vp->push_element(new mj::draw::HandElem(h4.tiles(), mj::Dir::North));
    vp->push_element(md);
    md->set_callback([](mj::Tile t) {
        std::cout << "Discard: " << (int)t.id34() << std::endl;
    });
    mh->set_callback([](mj::Tile t) { std::cout << (int)t.id34() << std::endl; });
    app.push_layer(vp);
    app.run();
}
