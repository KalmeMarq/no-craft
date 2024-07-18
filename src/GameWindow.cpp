#include <glad/glad.h>
#include "GameWindow.hpp"
#include <iostream>

namespace KM {
    GameWindow::~GameWindow()
    {
        if (this->m_handle != nullptr) 
            glfwTerminate();
    }

    bool GameWindow::Init(int width, int height, const char *title, WindowEventHandler* eventHandler)
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
            glfwTerminate();
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

        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        if (monitor != NULL) {
            const GLFWvidmode* vidMode = glfwGetVideoMode(monitor);
            if (vidMode != NULL) {
                glfwSetWindowPos(window, (vidMode->width - width) / 2, (vidMode->height - height) / 2);
            }
        }

        glfwShowWindow(window);

        this->m_handle = window;
        this->m_eventHandler = eventHandler;
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

    void GameWindow::framebufferSizeCallback(GLFWwindow *win, int x, int y)
    {
        auto window = static_cast<GameWindow*>(glfwGetWindowUserPointer(win));
        window->m_width = x;
        window->m_height = y;
        if (window->m_eventHandler != nullptr)
            window->m_eventHandler->OnResize();
    }

    void GameWindow::keyCallback(GLFWwindow* win, int key, int scancode, int action, int mods) {
        auto window = static_cast<GameWindow*>(glfwGetWindowUserPointer(win));

        if (window->m_eventHandler != nullptr)
            window->m_eventHandler->OnKey(key, scancode, action, mods);
    }

    void GameWindow::mouseButtonCallback(GLFWwindow* win, int button, int action, int mods) {
        auto window = static_cast<GameWindow*>(glfwGetWindowUserPointer(win));

         if (window->m_eventHandler != nullptr)
            window->m_eventHandler->OnMouseButton(button, action, mods);
    }

    void GameWindow::scrollCallback(GLFWwindow* win, double x, double y) {
        auto window = static_cast<GameWindow*>(glfwGetWindowUserPointer(win));

        if (window->m_eventHandler != nullptr)
            window->m_eventHandler->OnScroll(x, y);
    }

    void GameWindow::cursorPosCallback(GLFWwindow* win, double x, double y) {
        auto window = static_cast<GameWindow*>(glfwGetWindowUserPointer(win));

         if (window->m_eventHandler != nullptr)
            window->m_eventHandler->OnCursorPos(x, y);
    }
}