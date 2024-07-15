#include "Application.hpp"
#include "Menu.hpp"
#include <iostream>

namespace KM {
    void Menu::Init(int width, int height)
    {;
        this->width = width;
        this->height = height;
        this->InitGui();
    }

    void Menu::InitGui()
    {
    }

    void Menu::ActionPerformed(Button &button)
    {
    }

    void Menu::Resize(int width, int height)
    {
        this->width = width;
        this->height = height;
    }

    void Menu::Render(int mouseX, int mouseY)
    {
        Application *app = Application::GetInstance();

        app->m_guiTexture.Bind(0);
        app->m_texturedShader.SetUniformInt("uSampler", 0);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        app->DrawTexture(0, 0, this->width, this->height, 182, 0, 4, 4, 256, 256);
        glDisable(GL_BLEND);

        for (auto &button : this->m_buttons) {
            bool hovered = mouseX > button.x && mouseX <= button.x + button.width && mouseY > button.y && mouseY <= button.y + button.height;

            app->m_guiTexture.Bind(0);
            app->m_texturedShader.SetUniformInt("uSampler", 0);

            app->DrawTexture(button.x, button.y, button.width, button.height, 0, hovered ? 86 : 66, 200, 20, 256, 256);
            app->m_textRenderer.DrawAlignedText(button.message, button.x + button.width / 2, button.y + 6, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 0.5f);
        }
    }

    void Menu::MouseClicked(int mouseX, int mouseY, int button)
    {
        if (button != 0) return;

        for (auto &button : this->m_buttons) {
            bool hovered = mouseX > button.x && mouseX <= button.x + button.width && mouseY > button.y && mouseY <= button.y + button.height;
            std::cout << mouseX << "," << mouseY << ";" << button.id << "," << button.x << "," << button.y << "," << button.width << "," << button.height << "\n";
            if (hovered) {
                ActionPerformed(button);
            }
        }
    }
    
    Button::Button(int id, int x, int y, int width, int height, std::string message)
    {
        this->id = id;
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
        this->message = message;
    }

    void GameMenu::InitGui() {
        Button continueBtn(0, this->width / 2 - 100, this->height / 4 + 45, 200, 20, "Return to Game");
        this->m_buttons.push_back(continueBtn);
        Button quitBtn(1, this->width / 2 - 100, this->height / 4 + 45 + 24, 200, 20, "Quit Game");
        this->m_buttons.push_back(quitBtn);
    }

    void GameMenu::Resize(int width, int height)
    {
        Menu::Resize(width, height);

        this->m_buttons[0].x = this->width / 2 - 100;
        this->m_buttons[0].y = this->height / 4 + 45;

        this->m_buttons[1].x = this->width / 2 - 100;
        this->m_buttons[1].y = this->height / 4 + 45 + 24;
    }

    void GameMenu::ActionPerformed(Button &button)
    {
        if (button.id == 0)
        {
            Application::GetInstance()->SetMenu(nullptr);
        }

        if (button.id == 1)
        {
            Application::GetInstance()->Shutdown();
        }
    }
}