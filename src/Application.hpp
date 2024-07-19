#pragma once

#include <vector>
#include "Renderer.hpp"
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

        static void RenderTile(std::vector<KM::Vertex> &vertices, KM::World* world, BlockPos blockPos, int tile, int layer);
        void RenderTileGui(std::vector<KM::Vertex> &vertices, int tile);
        
        void SetMenu(Menu* menu);
        void Shutdown();

        void StartWorld();
        void QuitWorld();
    public:
        static Application* GetInstance();
        static Application* appInstance;
        GameWindow window;
        Renderer renderer;
        bool m_mouseGrabbed { false };
        int m_hotbar[9] { 0 };
        int m_selectedItem { 0 };
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
        double m_mousePos[2] = {0};
        double m_mouseDelta[2] = {0};
        std::optional<HitResult> m_hitResult = std::nullopt;
        World* m_world { nullptr };
        std::vector<Chunk*> m_chunks;
        std::string m_fpsString = "";

        GLuint m_selectionBoxVao;
        int m_selectionVertexCount { 0 };

        Menu* m_menu;
    };
}