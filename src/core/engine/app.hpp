
#pragma once
#include "layerstack.hpp"
#include "gui.hpp"
#include "window.hpp"
#include <memory>

namespace mj {
namespace draw {
class App
{
public:
    App();
    virtual ~App() {}
    App(const App&) = delete;
    App &operator=(const App&) = delete;

    void push_layer(Layer *layer);
    void push_overlay(Layer *layer);

    void run();

    inline static App &get() { return *instance; }
    inline Window &primary_window() { return *primary_window_; }

protected:
    static App *instance;

    LayerStack layers_;
    std::unique_ptr<Gui> gui_;
    std::unique_ptr<Window> primary_window_;
};

} // namespace draw
} // namespace mj
