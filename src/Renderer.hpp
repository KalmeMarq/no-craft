#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Texture.hpp"
#include "Shader.hpp"
#include "TextRenderer.hpp"

namespace KM {
    struct BuiltBufferInfo
    {
        float* buffer;
        int vertexCount;
    };

    class Tessellator
    {
    public:
        float* m_buffer;
        int cursor { 0 };
        int vertexCount { 0 };

        Tessellator(int capacity)
        {
            this->m_buffer = new float[capacity];
        }
        
        ~Tessellator()
        {
            std::cout << "Cleaning Tessellator\n";
            delete this->m_buffer;
        }

        void begin()
        {
            this->cursor = 0;
            this->vertexCount = 0;
        }

        BuiltBufferInfo end()
        {
            return { this->m_buffer, this->vertexCount };
        }

        Tessellator* vertex(float x, float y, float z)
        {
            this->m_buffer[this->cursor] = x;
            this->m_buffer[this->cursor + 1] = y;
            this->m_buffer[this->cursor + 2] = z;
            this->cursor += 3;
            this->vertexCount++;
            return this;
        }

        Tessellator* normal(float x, float y, float z)
        {
            this->m_buffer[this->cursor] = x;
            this->m_buffer[this->cursor + 1] = y;
            this->m_buffer[this->cursor + 2] = z;
            this->cursor += 3;
            return this;
        }

        Tessellator* texture(float u, float v)
        {
            this->m_buffer[this->cursor] = u;
            this->m_buffer[this->cursor + 1] = v;
            this->cursor += 2;
            return this;
        }

        Tessellator* color(float component)
        {
            return this->color(component, component, component, 1.0f);
        }

        Tessellator* color(float r, float g, float b, float a)
        {
            this->m_buffer[this->cursor] = r;
            this->m_buffer[this->cursor + 1] = g;
            this->m_buffer[this->cursor + 2] = b;
            this->m_buffer[this->cursor + 3] = a;
            this->cursor += 4;
            return this;
        }
    };

    class Renderer
    {
    public:
        Renderer();
        ~Renderer();

        void Init();
        void Close();
        void DrawText(const std::string& text, int x, int y, glm::vec4 color = glm::vec4(1.0f));
        void DrawTextWithShadow(const std::string& text, int x, int y, glm::vec4 color = glm::vec4(1.0f));
        void DrawAlignedText(const std::string& text, int x, int y, glm::vec4 color = glm::vec4(1.0f), float align = 0.0f);
        void DrawAlignedTextWithShadow(const std::string& text, int x, int y, glm::vec4 color = glm::vec4(1.0f), float align = 0.0f);
        void DrawTexture(Texture& texture, int x, int y, int width, int height, int u, int v, int us, int vs, int tw, int th);

        void DefaultBlendFunc();
    public:
        TextRenderer m_textRenderer;
        Texture m_guiTexture;
        Texture m_bgTexture;
        Texture terrainTexture;
        Shader texturedShader;
        Shader defaultShader;
        Shader terrainShader;
        GLuint m_guiVao;
        GLuint m_guiVbo;
    private:
        Tessellator m_tessellator;
    };
}