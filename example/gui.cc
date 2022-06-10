
#include "core/engine/app.hpp"

using namespace mj::draw;

class ImGuiSample : public Layer
{
    void on_gui_render() override
    {
        ImGui::ShowDemoWindow();
    }
};

int main(int, char**)
{
    App app;
    app.push_layer(new ImGuiDockspace);
    app.push_layer(new ImGuiSample);
    app.run();
}
