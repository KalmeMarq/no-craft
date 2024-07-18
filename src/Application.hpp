#pragma once

#include <vector>
#include "Math.hpp"
#include "World.hpp"
#include "Player.hpp"
#include "GameWindow.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Menu.hpp"
#include "TextRenderer.hpp"

namespace KM {
    class Application : public WindowEventHandler {
    public:
        Application();
        ~Application();

        void Run();

        void OnResize() override;
        void OnKey(int key, int scancode, int action, int mods) override;
        void OnMouseButton(int button, int action, int mods) override;
        void OnCursorPos(double x, double y) override;
        void OnScroll(double x, double y) override;

        static void RenderTile(std::vector<KM::Vertex> &vertices, KM::World *world, BlockPos blockPos, int tile, int layer);
        void DrawTexture(int x, int y, int w, int h, int u, int v, int us, int vs, int tw, int th);
        
        void SetMenu(Menu *menu);
        void Shutdown();

        void StartWorld();
        void QuitWorld();
    public:
        Texture m_guiTexture;
        Texture m_bgTexture;
        Shader m_texturedShader;

        static Application *GetInstance();
        static Application *appInstance;
        TextRenderer m_textRenderer;
        GameWindow m_window;
    private:
        void Render();
        void Tick();

        void RenderWorld();
        void RenderGui();
        
        void RenderSelectionBox(std::vector<KM::Vertex3FColor4F> &vertices, KM::HitResult &hitResult);
        int CalculateGuiScale(int width, int height);
    private:
        bool m_running { true };
        bool m_showDebugInfo { false };
        Player* m_player { nullptr };
        int m_selectedItem { 0 };
        bool m_mouseGrabbed { false };
        double m_mousePos[2] = {0};
        double m_mouseDelta[2] = {0};
        std::optional<HitResult> m_hitResult = std::nullopt;
        World* m_world { nullptr };
        Shader m_defaultShader;
        Shader m_terrainShader;
        Texture m_terrainTexture;
        std::vector<Chunk*> m_chunks;
        std::string m_fpsString = "";

        GLuint m_guiVao;
        GLuint m_guiVbo;
        GLuint m_selectionBoxVao;
        int m_selectionVertexCount { 0 };

        Menu *m_menu;
    };
}