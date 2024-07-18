#include "stb_image.h"
#include <glad/glad.h>
#include "Texture.hpp"

void Texture::LoadFromFile(const char *name)
{
    glCreateTextures(GL_TEXTURE_2D, 1, &m_id);

    this->SetWrap(TextureWrapping::REPEAT);
    this->SetFilter(TextureFilter::NEAREST);

    int nrChannels;
    unsigned char *data = stbi_load((std::string(RESOURCES_PATH "textures/") + name).c_str(), &m_width, &m_height, &nrChannels, 4); 

    glTextureStorage2D(m_id, 1, GL_RGBA8, m_width, m_height);
    glTextureSubImage2D(m_id, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_id);
}

int Texture::GetWidth()
{
    return m_width;
}

int Texture::GetHeight()
{
    return m_height;
}

void Texture::SetFilter(TextureFilter filter)
{
    int mode = filter == TextureFilter::NEAREST ? GL_NEAREST : GL_LINEAR;
    glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, mode);
    glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, mode);
}

void Texture::SetWrap(TextureWrapping wrap)
{
    int mode = wrap == TextureWrapping::CLAMP_TO_EDGE ? GL_CLAMP_TO_EDGE : GL_REPEAT;
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, mode);
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, mode);
}

void Texture::Bind(int unit)
{
    glBindTextureUnit(unit, m_id);
}
