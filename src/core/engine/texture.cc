
#include "texture.hpp"
#include "core/utils/logging.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "extern/stb_image.h"

namespace mj {
namespace draw {
Texture::Texture(const char *path)
{
    // stbi_set_flip_vertically_on_load(1);
    unsigned char *data = stbi_load(path, &width_, &height_, &nrComponents_, 4);
    MJ_ASSERT_CRIT(data, "Failed to load texture");

    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_2D, id_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if (nrComponents_ == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    else if (nrComponents_ == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
}

Texture::~Texture()
{
    glDeleteTextures(1, &id_);
}

void Texture::bind(unsigned int slot) noexcept
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, id_);
}

void Texture::unbind() noexcept
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace draw
} // namespace mj
