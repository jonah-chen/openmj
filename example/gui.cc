
#include "core/engine/app.hpp"
#include "core/engine/texture.hpp"

using namespace mj::draw;

class ImGuiSample : public Layer
{
    void on_gui_render() override
    {
        static Texture tex("../assets/textures/tiles.png");
        ImGui::ShowDemoWindow();
        ImGui::Begin("Viewport");
        // get the viewport size
        ImVec2 size = ImGui::GetContentRegionAvail();
        // draw texture 100 pixels down from top
        auto amin = ImGui::GetCursorPos();
        ImGui::Text("%f/%f, %f/%f", amin.x, size.x, amin.y, size.y);
        for (int i = 0; i < 9; ++i)
        {
            ImGui::SetCursorPos(ImVec2(300+80*i,900));
            ImGui::ImageButton((void*)(intptr_t)tex.id(), ImVec2(75, 110), ImVec2(i/34., 0), ImVec2((i+1)/34., 0.5));
        }
        for (int i = 0; i < 9; ++i)
        {
            ImGui::SetCursorPos(ImVec2(1200,800-80*i));
            ImGui::ImageButtonCCW((void*)(intptr_t)tex.id(), ImVec2(75, 110), ImVec2((i+12)/34., 0), ImVec2((i+13)/34., 0.5));
        }
        for (int i = 0; i < 9; ++i)
        {
            ImGui::SetCursorPos(ImVec2(1020-80*i,150));
            ImGui::ImageButton180((void*)(intptr_t)tex.id(), ImVec2(75, 110), ImVec2((i+20)/34., 0), ImVec2((i+21)/34., 0.5));
        }
        for (int i = 0; i < 7; ++i)
        {
            ImGui::SetCursorPos(ImVec2(100,280+80*i));
            ImGui::ImageButtonCW((void*)(intptr_t)tex.id(), ImVec2(75, 110), ImVec2((i+27)/34., 0), ImVec2((i+28)/34., 0.5));
        }
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
