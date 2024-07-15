#include <glad/glad.h>
#include "GameWindow.hpp"
#include <iostream>

namespace KM {
    GameWindow::~GameWindow()
    {
        glfwTerminate();
    }

    bool GameWindow::Init(int width, int height, const char *title)
    {
        glfwSetErrorCallback([](int, const char* desc) { std::cerr << desc << "\n"; std::exit(EXIT_FAILURE); });
    
        if (!glfwInit()) {
            return false;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    
        GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);

        if (window == NULL) {
            return false;
        }
        
        glfwMakeContextCurrent(window);
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

        glfwSetWindowUserPointer(window, this);

        glfwGetFramebufferSize(window, &this->m_width, &this->m_height);

        glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
        glfwSetKeyCallback(window, keyCallback);
        glfwSetMouseButtonCallback(window, mouseButtonCallback);
        glfwSetCursorPosCallback(window, cursorPosCallback);
        glfwSetScrollCallback(window, scrollCallback);

        glfwShowWindow(window);

        this->m_handle = window;
        return true;
    }

    bool GameWindow::ShouldClose()
    {
        return glfwWindowShouldClose(this->m_handle);
    }

    void GameWindow::Update()
    {
        glfwSwapBuffers(this->m_handle);
        glfwPollEvents();
    }

    GLFWwindow *GameWindow::GetHandle() const
    {
        return this->m_handle;
    }

    int GameWindow::GetWidth() const
    {
        return this->m_width;
    }
    
    int GameWindow::GetHeight() const
    {
        return this->m_height;
    }

    void GameWindow::SetVsync(bool flag)
    {
        glfwSwapInterval(flag);
    }

    void GameWindow::AddResizeHandler(std::function<void(void)> handler)
    {
        this->resizeHandlers.push_back(handler);
    }

    void GameWindow::AddKeyboardHandler(std::function<void(int, int, int, int)> handler)
    {
        this->keyHandlers.push_back(handler);
    }

    void GameWindow::AddMouseButtonHandler(std::function<void(int, int, int)> handler)
    {
        this->mouseButtonHandlers.push_back(handler);
    }

    void GameWindow::AddScrollHandler(std::function<void(double, double)> handler)
    {
        this->scrollHandlers.push_back(handler);
    }

    void GameWindow::AddCursorPosHandler(std::function<void(double, double)> handler)
    {
        this->cursorPosHandlers.push_back(handler);
    }

    void GameWindow::framebufferSizeCallback(GLFWwindow *win, int x, int y)
    {
        auto window = static_cast<GameWindow*>(glfwGetWindowUserPointer(win));
        window->m_width = x;
        window->m_height = y;

        for (auto const &handler : window->resizeHandlers) {
            handler();
        }
    }

    void GameWindow::keyCallback(GLFWwindow* win, int key, int scancode, int action, int mods) {
        auto window = static_cast<GameWindow*>(glfwGetWindowUserPointer(win));

        for (auto const &handler : window->keyHandlers) {
            handler(key, scancode, action, mods);
        }
    }

    void GameWindow::mouseButtonCallback(GLFWwindow* win, int button, int action, int mods) {
        auto window = static_cast<GameWindow*>(glfwGetWindowUserPointer(win));

        for (auto const &handler : window->mouseButtonHandlers) {
            handler(button, action, mods);
        }
    }

    void GameWindow::scrollCallback(GLFWwindow* win, double x, double y) {
        auto window = static_cast<GameWindow*>(glfwGetWindowUserPointer(win));

        for (auto const &handler : window->scrollHandlers) {
            handler(x, y);
        }
    }

    void GameWindow::cursorPosCallback(GLFWwindow* win, double x, double y) {
        auto window = static_cast<GameWindow*>(glfwGetWindowUserPointer(win));

        for (auto const &handler : window->cursorPosHandlers) {
            handler(x, y);
        }
    }
}