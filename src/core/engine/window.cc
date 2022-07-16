
#include "window.hpp"
#include "core/utils/logging.hpp"

namespace mj {
namespace draw {
#if MJ_LOGGING > 0
namespace {
struct GLException : public std::exception
{
    constexpr const char *what() const noexcept override
    {
        return "GLException";
    }
};
void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id,
                            GLenum severity, GLsizei length,
                            const GLchar *message, const void *userParam)
{
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        MJ_CRIT("HIGH SEVERITY (%d:%d): %s", type, id, message);
        throw GLException();
    case GL_DEBUG_SEVERITY_MEDIUM:
        MJ_WARN("Medium Severity (%d:%d): %s", type, id, message);
        throw GLException();
    case GL_DEBUG_SEVERITY_LOW:
        MJ_INFO("Low Severity (%d:%d): %s", type, id, message);
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        MJ_INFO("GL Notification (%d:%d): %s", type, id, message);
        break;
    }
}
} // namespace
#endif

Window::Window(const char *title, int width, int height, bool resizable)
{
    if (!windows)
    {
        MJ_ALWAYS_THROW(glfwInit() != GLFW_TRUE, std::runtime_error,
                        "Failed to initialize GLFW");
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, k_OpenglMajorVersion);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, k_OpenglMinorVersion);
        glfwWindowHint(GLFW_OPENGL_PROFILE, k_OpenglProfile);
        glfwSwapInterval(1);
    }

    glfwWindowHint(GLFW_RESIZABLE, resizable);

    GLFWmonitor *monitor = nullptr;
    if (width && height)
    {
        width_ = width;
        height_ = height;
    }
    else
    {
        monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
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

    MJ_ALWAYS_THROW(!windows && glewInit() != GLEW_OK, std::runtime_error,
                    "Failed to initialize GLEW");
    windows++;

#if MJ_LOGGING > 0
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(debugCallback, nullptr);
#endif

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
