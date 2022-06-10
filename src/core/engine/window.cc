
#include "window.hpp"
#include "core/utils/logging.hpp"

namespace mj {
namespace draw {
Window::Window(const char* title, int width, int height, bool resizable)
{
    if (!windows)
    {
        MJ_ALWAYS_THROW(glfwInit() != GLFW_TRUE, std::runtime_error, "Failed to initialize GLFW");
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, k_OpenglMajorVersion);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, k_OpenglMinorVersion);
        glfwWindowHint(GLFW_OPENGL_PROFILE, k_OpenglProfile);
        glfwSwapInterval(1);
    }

    glfwWindowHint(GLFW_RESIZABLE, resizable);

    GLFWmonitor* monitor = nullptr;
    if (width && height)
    {
        width_ = width;
        height_ = height;
    }
    else
    {
        monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        width_ = mode->width;
        height_ = mode->height;
    }

    window_ = glfwCreateWindow(width_, height_, title, monitor, nullptr);
    if (!window_)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(window_);

    MJ_ALWAYS_THROW(!windows && glewInit() != GLEW_OK, std::runtime_error, "Failed to initialize GLEW");
    windows++;
}

Window::~Window()
{
    glfwDestroyWindow(window_);
    if (--windows == 0)
        glfwTerminate();
}

void Window::on_update(float ms)
{
    glfwSwapBuffers(window_);
    glfwPollEvents();
    frame_time_ = 1000.0 * (glfwGetTime() - last_update_);
    last_update_ = glfwGetTime();
}

int Window::windows = 0;

} // namespace draw
} // namespace mj