
#include "core/engine/app.hpp"
#include "core/engine/texture.hpp"

using namespace mj::draw;

class ImGuiSample : public Layer
{
    void on_gui_render() override
    {
        static Texture tex("../assets/textures/score.png");
        ImGui::ShowDemoWindow();
        ImGui::Begin("Viewport");
        // draw texture 100 pixels down from top
        auto amin = ImGui::GetCursorPos();
        ImGui::Text("%f, %f", amin.x, amin.y);
        ImGui::SetCursorPos(ImVec2(100,100));
        ImGui::Image((void*)(intptr_t)tex.id(), ImVec2(200, 200));
        ImGui::SetCursorPos(ImVec2(300, 300));
        // draw same image rotated 90 degrees
        ImGui::Image90((void*)(intptr_t)tex.id(), ImVec2(200, 200));
        ImGui::End();
    }
};

int main(int, char**)
{
    App app;
    app.push_layer(new ImGuiDockspace);
    app.push_layer(new ImGuiSample);
    app.run();
}
