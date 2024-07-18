#include "stb_image.h"
#include <glad/glad.h>
#include "Texture.hpp"

namespace KM
{
    void Texture::LoadFromFile(const char* name)
    {
        glCreateTextures(GL_TEXTURE_2D, 1, &this->m_id);

        this->SetWrap(TextureWrapping::REPEAT);
        this->SetFilter(TextureFilter::NEAREST);

        int nrChannels;
        stbi_uc* data = stbi_load((std::string(RESOURCES_PATH "textures/") + name).c_str(), &this->m_width, &this->m_height, &nrChannels, 4); 

        if (data != nullptr)
        {
            glTextureStorage2D(this->m_id, 1, GL_RGBA8, this->m_width, this->m_height);
            glTextureSubImage2D(this->m_id, 0, 0, 0, this->m_width, this->m_height, GL_RGBA, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
    }

    void Texture::LoadFromStbData(stbi_uc *data, int width, int height)
    {
        glCreateTextures(GL_TEXTURE_2D, 1, &this->m_id);

        this->m_width = width;
        this->m_height = height;

        this->SetWrap(TextureWrapping::REPEAT);
        this->SetFilter(TextureFilter::NEAREST);

        if (data != nullptr)
        {
            glTextureStorage2D(this->m_id, 1, GL_RGBA8, width, m_height);
            glTextureSubImage2D(this->m_id, 0, 0, 0, width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
    }

    Texture::~Texture()
    {
        if (this->m_id != 4294967295)
            glDeleteTextures(1, &m_id);
    }

    int Texture::GetWidth()
    {
        return this->m_width;
    }

    int Texture::GetHeight()
    {
        return this->m_height;
    }

    void Texture::SetFilter(TextureFilter filter)
    {
        int mode = filter == TextureFilter::NEAREST ? GL_NEAREST : GL_LINEAR;
        glTextureParameteri(this->m_id, GL_TEXTURE_MIN_FILTER, mode);
        glTextureParameteri(this->m_id, GL_TEXTURE_MAG_FILTER, mode);
    }

    void Texture::SetWrap(TextureWrapping wrap)
    {
        int mode = wrap == TextureWrapping::CLAMP_TO_EDGE ? GL_CLAMP_TO_EDGE : GL_REPEAT;
        glTextureParameteri(this->m_id, GL_TEXTURE_WRAP_S, mode);
        glTextureParameteri(this->m_id, GL_TEXTURE_WRAP_T, mode);
    }

    void Texture::Bind(int unit)
    {
        glBindTextureUnit(unit, this->m_id);
    }
}