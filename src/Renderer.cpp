#include "Renderer.hpp"
#include "Math.hpp"

namespace KM {
    Renderer::Renderer() : m_textRenderer(&texturedShader), m_tessellator(64 * 100)
    {
    }

    Renderer::~Renderer()
    {
    }

    void Renderer::Init()
    {
        std::cout << "Initializing Shaders\n";
        this->defaultShader.Init("default");
        this->texturedShader.Init("textured");
        this->terrainShader.Init("terrain");
    
        std::cout << "Loading Textures\n";
        this->terrainTexture.LoadFromFile("terrain.png");
        this->m_guiTexture.LoadFromFile("gui.png");
        this->m_bgTexture.LoadFromFile("bg.png");

        std::cout << "Initializing TextRenderer\n";
        this->m_textRenderer.Init();
        
        std::cout << "GUI\n";

        glGenVertexArrays(1, &this->m_guiVao);
        glBindVertexArray(this->m_guiVao);
        glGenBuffers(1, &this->m_guiVbo);
        glBindBuffer(GL_ARRAY_BUFFER, this->m_guiVbo);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (GLvoid *)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (GLvoid *)((3 + 4) * sizeof(float)));

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void Renderer::Close()
    {
        glDeleteVertexArrays(1, &this->m_guiVao);
        glDeleteBuffers(1, &this->m_guiVbo);
    }

    void Renderer::DrawText(const std::string &text, int x, int y, glm::vec4 color)
    {
        this->m_textRenderer.DrawText(text, x, y, color);
    }

    void Renderer::DrawTextWithShadow(const std::string &text, int x, int y, glm::vec4 color)
    {
        this->m_textRenderer.DrawText(text, x, y, color, true);
    }

    void Renderer::DrawAlignedText(const std::string &text, int x, int y, glm::vec4 color, float align)
    {
        this->m_textRenderer.DrawAlignedText(text, x, y, color, align);
    }

    void Renderer::DrawAlignedTextWithShadow(const std::string &text, int x, int y, glm::vec4 color, float align)
    {
        this->m_textRenderer.DrawAlignedText(text, x, y, color, align, true);
    }

    void Renderer::DrawTexture(Texture &texture, int x, int y, int width, int height, int u, int v, int us, int vs, int tw, int th)
    {
        texture.Bind(0);
        texturedShader.SetUniformInt("uSampler", 0);

        glBindVertexArray(this->m_guiVao);
        glBindBuffer(GL_ARRAY_BUFFER, this->m_guiVbo);

        float u0 = (float) u / (float) tw;
        float v0 = (float) v / (float) th;
        float u1 = (float) (u + us) / (float) tw;
        float v1 = (float) (v + vs) / (float) th;

        this->m_tessellator.begin();
        this->m_tessellator.vertex(x + 0.0f, y + 0.0f, 0.0f)->color(1.0f)->texture(u0, v0);
        this->m_tessellator.vertex(x + 0.0f, y + height + 0.0f, 0.0f)->color(1.0f)->texture(u0, v1);
        this->m_tessellator.vertex(x + width + 0.0f, y + height + 0.0f, 0.0f)->color(1.0f)->texture(u1, v1);
        this->m_tessellator.vertex(x + width + 0.0f, y + height + 0.0f, 0.0f)->color(1.0f)->texture(u1, v1);
        this->m_tessellator.vertex(x + width + 0.0f, y + 0.0f, 0.0f)->color(1.0f)->texture(u1, v0);
        this->m_tessellator.vertex(x + 0.0f, y + 0.0f, 0.0f)->color(1.0f)->texture(u0, v0);
        
        BuiltBufferInfo info = this->m_tessellator.end();
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex3FColor4FTex2F) * info.vertexCount, info.buffer, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, info.vertexCount);

        glBindVertexArray(0);
    }
    
    void Renderer::DefaultBlendFunc()
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}