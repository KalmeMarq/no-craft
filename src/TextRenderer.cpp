#include <glad/glad.h>
#include "stb_image.h"
#include "TextRenderer.hpp"

static int charWidth[256] = { 8 };
constexpr float SHADOW_FACTOR = 0.3f;

TextRenderer::TextRenderer(Shader *shader)
{
    m_shader = shader;
}

void TextRenderer::Init()
{
    glCreateTextures(GL_TEXTURE_2D, 1, &m_fontTexture);

    glTextureParameteri(m_fontTexture, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_fontTexture, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTextureParameteri(m_fontTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(m_fontTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    std::cout << "Loading Default Font\n";
    int width, height, nrChannels;
    unsigned char *data = stbi_load("res/textures/default.png", &width, &height, &nrChannels, 4);

    int var9;
    int var10;
    int var11;
    int var12;
    int var13;
    int var15;
    int var16;
    for(var9 = 0; var9 < 256; ++var9) {
        var10 = var9 % 16;
        var11 = var9 / 16;

        for(var12 = 7; var12 >= 0; --var12) {
            var13 = var10 * 8 + var12;
            bool var14 = true;

            for(var15 = 0; var15 < 8 && var14; ++var15) {
                var16 = (var11 * 8 + var15) * width;
                int var17 = data[(var13 + var16) * 4] & 255;
                if(var17 > 0) {
                    var14 = false;
                }
            }

            if(!var14) {
                break;
            }
        }

        if(var9 == 32) {
            var12 = 2;
        }

        charWidth[var9] = var12 + 2;
    }

    glTextureStorage2D(m_fontTexture, 1, GL_RGBA8, width, height);
    glTextureSubImage2D(m_fontTexture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (3 + 4 + 2) * sizeof(float), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, (3 + 4 + 2) * sizeof(float), (GLvoid *)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, (3 + 4 + 2) * sizeof(float), (GLvoid *)((3 + 4) * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

TextRenderer::~TextRenderer()
{
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
    glDeleteTextures(1, &m_fontTexture);
}

void TextRenderer::DrawAlignedText(const std::string &text, int x, int y, glm::vec4 color, float align, bool shadow)
{
    DrawText(text, x - (int) ((float) this->GetWidth(text) * align), y, color, shadow);
}

void TextRenderer::DrawText(const std::string &text, int x, int y, glm::vec4 color, bool shadow)
{
    glBindTextureUnit(0, m_fontTexture);
    m_shader->SetUniformInt("uSampler", 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    std::string::const_iterator c;

    glm::vec4 bkcColor = color;

    for (c = text.begin(); c != text.end(); c++) 
    {
        if (*c >= 256) {
            continue;
        }

        float chrWidth = charWidth[*c];
        int u = (*c % 16) * 8;
        int v = (*c / 16) * 8;

        float u0 = u / 128.0f;
        float v0 = v / 128.0f;
        float u1 = (u + chrWidth) / 128.0f;
        float v1 = (v + 8.0f) / 128.0f;

        if (shadow) {
            const float verts[] =
            {
                x + 1 + 0.0f, y + 1 + 0.0f, 0.0f,    color[0] * SHADOW_FACTOR, color[1] * SHADOW_FACTOR, color[2] * SHADOW_FACTOR, color[3],      u0, v0,
                x + 1 + 0.0f, y + 1 + 8.0f, 0.0f,    color[0] * SHADOW_FACTOR, color[1] * SHADOW_FACTOR, color[2] * SHADOW_FACTOR, color[3],      u0, v1,
                x + 1 + chrWidth, y + 1 + 8.0f, 0.0f,    color[0] * SHADOW_FACTOR, color[1] * SHADOW_FACTOR, color[2] * SHADOW_FACTOR, color[3],      u1, v1,
                x + 1 + chrWidth, y + 1 + 8.0f, 0.0f,    color[0] * SHADOW_FACTOR, color[1] * SHADOW_FACTOR, color[2] * SHADOW_FACTOR, color[3],      u1, v1,
                x + 1 + chrWidth, y + 1 + 0.0f, 0.0f,    color[0] * SHADOW_FACTOR, color[1] * SHADOW_FACTOR, color[2] * SHADOW_FACTOR, color[3],      u1, v0,
                x + 1 + 0.0f, y + 1 + 0.0f, 0.0f,    color[0] * SHADOW_FACTOR, color[1] * SHADOW_FACTOR, color[2] * SHADOW_FACTOR, color[3],      u0, v0
            };
            glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        const float verts[] =
        {
            x + 0.0f, y + 0.0f, 0.0f,    color[0], color[1], color[2], color[3],      u0, v0,
            x + 0.0f, y + 8.0f, 0.0f,    color[0], color[1], color[2], color[3],      u0, v1,
            x + chrWidth, y + 8.0f, 0.0f,    color[0], color[1], color[2], color[3],      u1, v1,
            x + chrWidth, y + 8.0f, 0.0f,    color[0], color[1], color[2], color[3],      u1, v1,
            x + chrWidth, y + 0.0f, 0.0f,    color[0], color[1], color[2], color[3],      u1, v0,
            x + 0.0f, y + 0.0f, 0.0f,    color[0], color[1], color[2], color[3],      u0, v0
        };
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += chrWidth;
    }

    glDisable(GL_BLEND);

    glBindVertexArray(0);
}

int TextRenderer::GetWidth(const std::string &text)
{
    int width = 0;
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) 
    {
        if (*c >= 256) {
            continue;
        }

        if (*c == 32) {
            width += 4;
        } else {
            width += charWidth[*c];
        }
    }
    return width;
}
