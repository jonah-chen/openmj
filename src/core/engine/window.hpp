
#pragma once

#ifndef GLEW_STATIC
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include "gui.hpp"
#include "layerstack.hpp"
#include <memory>

namespace mj {
namespace draw {

class Window
{
public:
    constexpr static int k_OpenglMajorVersion = 4;
    constexpr static int k_OpenglMinorVersion = 5;
    constexpr static int k_OpenglProfile = GLFW_OPENGL_CORE_PROFILE;

public:
    Window(const char *title, int width = 0, int height = 0,
           bool resizable = false);
    ~Window();
    inline GLFWwindow *window() { return window_; }
    constexpr int width() const { return width_; }
    constexpr int height() const { return height_; }
    constexpr bool resizable() const { return resizable_; }
    constexpr bool fullscreen() const { return fullscreen_; }

    void on_update(float ms);
    constexpr double frame_time() const { return frame_time_; }

private:
    GLFWwindow *window_;
    int width_;
    int height_;
    bool fullscreen_;
    bool resizable_;

    double last_update_;
    double frame_time_;

    static int windows;
};

} // namespace draw
} // namespace mj