#pragma once

#include <string>
#include <iostream>
#include "stb_image.h"

namespace KM
{
    enum class TextureFilter {
        NEAREST,
        LINEAR
    };

    enum class TextureWrapping {
        CLAMP_TO_EDGE,
        REPEAT
    };

    class Texture
    {
    public:
        ~Texture();

        void LoadFromFile(const char* name);
        void LoadFromStbData(stbi_uc* data, int width, int height);

        int GetWidth();
        int GetHeight();

        void SetFilter(TextureFilter filter);
        void SetWrap(TextureWrapping wrap);

        void Bind(int unit);
    private:
        GLuint m_id { 4294967295 };
        int m_width;
        int m_height;
    };
}