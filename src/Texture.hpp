#pragma once

#include <string>
#include <iostream>

class Texture
{
public:
    ~Texture();

    void LoadFromFile(const char *name);

    int GetWidth();
    int GetHeight();

    void Bind(int unit);
private:
    unsigned int m_id;
    int m_width;
    int m_height;
};
