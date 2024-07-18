#pragma once

#include <GLFW/glfw3.h>
#include <functional>
#include <memory>

namespace KM {
    class WindowEventHandler
    {
    public:
        virtual void OnResize() {}
        virtual void OnKey(int key, int scancode, int action, int mods) {}
        virtual void OnMouseButton(int button, int action, int mods) {}
        virtual void OnScroll(double x, double y) {}
        virtual void OnCursorPos(double x, double y) {}
    };
    
    class GameWindow
    {
    public:
        ~GameWindow();

        bool Init(int width, int height, const char *title, WindowEventHandler* eventHandler);

        bool ShouldClose();
        void Update();

        GLFWwindow *GetHandle() const;
        int GetWidth() const;
        int GetHeight() const;
    
        void SetVsync(bool flag);
    private:
        GLFWwindow *m_handle = nullptr;
        int m_width;
        int m_height;

        WindowEventHandler* m_eventHandler = nullptr;

        static void framebufferSizeCallback(GLFWwindow* win, int x, int y);
        static void keyCallback(GLFWwindow* win, int key, int scancode, int action, int mods);
        static void mouseButtonCallback(GLFWwindow* win, int button, int action, int mods);
        static void scrollCallback(GLFWwindow* win, double x, double y);
        static void cursorPosCallback(GLFWwindow* win, double x, double y);
    };
}