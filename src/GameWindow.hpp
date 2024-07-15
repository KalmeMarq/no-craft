#pragma once

#include <GLFW/glfw3.h>
#include <functional>

namespace KM {
    class GameWindow
    {
    public:
        ~GameWindow();

        bool Init(int width, int height, const char *title);

        bool ShouldClose();
        void Update();

        GLFWwindow *GetHandle() const;
        int GetWidth() const;
        int GetHeight() const;
    
        void SetVsync(bool flag);

        void AddResizeHandler(std::function<void(void)> handler);
        void AddKeyboardHandler(std::function<void(int, int, int, int)> handler);
        void AddMouseButtonHandler(std::function<void(int, int, int)> handler);
        void AddScrollHandler(std::function<void(double, double)> handler);
        void AddCursorPosHandler(std::function<void(double, double)> handler);
    private:
        GLFWwindow *m_handle = nullptr;
        int m_width;
        int m_height;

        std::vector<std::function<void(void)>> resizeHandlers;
        std::vector<std::function<void(int, int, int, int)>> keyHandlers;
        std::vector<std::function<void(int, int, int)>> mouseButtonHandlers;
        std::vector<std::function<void(double, double)>> scrollHandlers;
        std::vector<std::function<void(double, double)>> cursorPosHandlers;

        static void framebufferSizeCallback(GLFWwindow* win, int x, int y);
        static void keyCallback(GLFWwindow* win, int key, int scancode, int action, int mods);
        static void mouseButtonCallback(GLFWwindow* win, int button, int action, int mods);
        static void scrollCallback(GLFWwindow* win, double x, double y);
        static void cursorPosCallback(GLFWwindow* win, double x, double y);
    };
}