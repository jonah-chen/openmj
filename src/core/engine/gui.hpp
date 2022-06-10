
#pragma once

#ifndef GLEW_STATIC
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include "extern/imgui/imgui.h"
#include "extern/imgui/imgui_impl_glfw.h"
#include "extern/imgui/imgui_impl_opengl3.h"

#include "layerstack.hpp"

namespace mj {
namespace draw {

class Gui : public Layer
{
public:
    Gui(const char *ini="imgui.ini") : ini_ { ini } {}

    void on_attach() override;
    void on_detach() override;

    void begin();
    void end();
private:
    const char *ini_;
};

class ImGuiDockspace : public Layer
{
    void on_gui_render() override;
};

} // namespace draw
} // namespace mj
