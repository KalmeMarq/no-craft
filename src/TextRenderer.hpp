#pragma once

#include "Shader.hpp"
#include "Texture.hpp"
#include <glm/glm.hpp>

class TextRenderer
{
public:
    TextRenderer(Shader* shader);
    ~TextRenderer();

    void Init();
    
    void DrawAlignedText(const std::string &text, int x, int y, glm::vec4 color, float align, bool shadow = true);
    void DrawText(const std::string &text, int x, int y, glm::vec4 color, bool shadow = true);
    int GetWidth(const std::string &text);
private:
    Shader* m_shader; 
    KM::Texture m_fontTexture;
    unsigned int m_vao;
    unsigned int m_vbo;
};