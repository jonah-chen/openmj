
#pragma once
#ifndef GLEW_STATIC
#define GLEW_STATIC
#include <GL/glew.h>
#endif

namespace mj {
namespace draw {
class Texture
{
public:
    Texture(const char *path);
    ~Texture();

    void bind(unsigned int slot=0) noexcept;
    void unbind() noexcept;
    
    constexpr GLuint id() const noexcept { return id_; }
    constexpr int width() const noexcept { return width_; }
    constexpr int height() const noexcept { return height_; }
    constexpr int nrComponents() const noexcept { return nrComponents_; }

private:
    GLuint id_;
    int width_, height_, nrComponents_;

};

} // namespace draw
} // namespace mj
