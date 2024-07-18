#pragma once

#include <string>
#include <iostream>

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

    void LoadFromFile(const char *name);

    int GetWidth();
    int GetHeight();

    void SetFilter(TextureFilter filter);
    void SetWrap(TextureWrapping wrap);

    void Bind(int unit);
private:
    unsigned int m_id;
    int m_width;
    int m_height;
};
