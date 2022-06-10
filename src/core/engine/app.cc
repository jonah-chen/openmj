
#include "app.hpp"
#include "core/utils/logging.hpp"

namespace mj {
namespace draw {
App *App::instance = nullptr;

App::App()
{
    MJ_THROW(instance, std::runtime_error, "App already exists");

    instance = this;
    primary_window_ = std::make_unique<Window>("Default App", 1920, 1080);
    gui_ = std::make_unique<Gui>("default_app.ini");
    gui_->on_attach();
}

void App::push_overlay(Layer *layer)
{
    layers_.push_overlay(layer);
    layer->on_attach();
}

void App::push_layer(Layer *layer)
{
    layers_.push_layer(layer);
    layer->on_attach();
}

void App::run()
{
    while (!glfwWindowShouldClose(primary_window_->window()))
    {
        double frame_time = primary_window_->frame_time();
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        for (auto *layer : layers_)
            layer->on_update(frame_time);
        
        gui_->begin();
        for (auto *layer : layers_)
            layer->on_gui_render();
        gui_->end();

        primary_window_->on_update(frame_time);
    }
}

} // namespace draw
} // namespace mj
