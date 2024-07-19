#include "Application.hpp"
#include "Menu.hpp"
#include <iostream>
#include "Block.hpp"

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
        Application* app = Application::GetInstance();

        glEnable(GL_BLEND);
        app->renderer.DefaultBlendFunc();
        app->renderer.DrawTexture(app->renderer.m_guiTexture, 0, 0, this->width, this->height, 182, 0, 4, 4, 256, 256);
        glDisable(GL_BLEND);

        for (auto &button : this->m_buttons) {
            bool hovered = mouseX > button.x && mouseX <= button.x + button.width && mouseY > button.y && mouseY <= button.y + button.height;

            app->renderer.DrawTexture(app->renderer.m_guiTexture, button.x, button.y, button.width, button.height, 0, hovered ? 86 : 66, 200, 20, 256, 256);
            app->renderer.DrawAlignedText(button.message, button.x + button.width / 2, button.y + 6, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 0.5f);
        }
    }

    bool Menu::MouseClicked(int mouseX, int mouseY, int button)
    {
        if (button != 0) return false;

        for (auto &button : this->m_buttons) {
            bool hovered = mouseX > button.x && mouseX <= button.x + button.width && mouseY > button.y && mouseY <= button.y + button.height;
            if (hovered)
            {
                ActionPerformed(button);
                return true;
            }
        }

        return false;
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
        Button quitBtn(1, this->width / 2 - 100, this->height / 4 + 45 + 24, 200, 20, "Quit to Title Menu");
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
            Application::GetInstance()->QuitWorld();
        }
    }

    void GameMenu::Render(int mouseX, int mouseY)
    {
        Menu::Render(mouseX, mouseY);
        Application* app = Application::GetInstance();
        app->renderer.m_textRenderer.DrawAlignedText("Game Menu", this->width / 2, 30, glm::vec4(1.0f), 0.5f);
    }

    void InventoryMenu::InitGui()
    {
    }
    
    void InventoryMenu::Resize(int width, int height)
    {
        Menu::Resize(width, height);
    }
    
    void InventoryMenu::ActionPerformed(Button &button)
    {
    }
    
    void InventoryMenu::Render(int mouseX, int mouseY)
    {
        Menu::Render(mouseX, mouseY);

        Application* app = Application::GetInstance();
        glEnable(GL_BLEND);
        app->renderer.DefaultBlendFunc();
        app->renderer.DrawTexture(app->renderer.m_guiTexture, this->width / 2 - (20 * 5), this->height / 2 - 20 * 2.5f, 20 * 10, 20 * 5, 182, 0, 4, 4, 256, 256);
        glDisable(GL_BLEND);

        this->hoveredSlot = -1;
        for (int i = 0; i < BLOCK_COUNT - 1; ++i) {
            int c = i % 9;
            int r = i / 9;
            {
                glEnable(GL_BLEND);
                app->renderer.DefaultBlendFunc();

                int slotX = this->width / 2 - 91 + 1 + c * 20 - 1 + 2;
                int slotY = this->height / 2 - 21 + r * 20 + 1;

                if (mouseX > slotX && mouseX < slotX + 18 && mouseY > slotY && mouseY < slotY + 18) {
                    app->renderer.DrawTexture(app->renderer.m_guiTexture, slotX, slotY, 18, 18, 186, 0, 4, 4, 256, 256);
                    this->hoveredSlot = i;
                }
                glDisable(GL_BLEND);

                glEnable(GL_DEPTH_TEST);
                glm::mat4 mv = glm::mat4(1.0);
                mv = glm::translate(mv, glm::vec3(0, 0, -2000));

                mv = glm::translate(mv, glm::vec3(this->width / 2 - 91 + 1 + c * 20 - 1 + 3 - 9, this->height / 2 - 22 + 2 + 8.5f + r * 20, 10));
                mv = glm::scale(mv, glm::vec3(10, 10, 10));
                mv = glm::translate(mv, glm::vec3(1.0f, 0.5f, 1.0f));
                mv = glm::scale(mv, glm::vec3(1.0f, 1.0f, -1.0f));
                mv = glm::rotate(mv, glm::radians(210.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                mv = glm::rotate(mv, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                app->renderer.texturedShader.SetUniformMat4("uModelView", mv);

                app->renderer.terrainTexture.Bind(0);
                app->renderer.terrainShader.SetUniformInt("uSampler", 0);
                std::vector<Vertex> verts;
                glEnable(GL_BLEND);
                app->renderer.DefaultBlendFunc();
                glBindVertexArray(app->renderer.m_guiVao);
                glBindBuffer(GL_ARRAY_BUFFER, app->renderer.m_guiVbo);
                app->RenderTileGui(verts, i + 1);
                glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * verts.size(), &verts[0], GL_DYNAMIC_DRAW);
                glDrawArrays(GL_TRIANGLES, 0, verts.size());
                glDisable(GL_BLEND);
                glBindVertexArray(0);
                glDisable(GL_DEPTH_TEST);

                mv = glm::mat4(1.0);
                mv = glm::translate(mv, glm::vec3(0, 0, -2000));
                app->renderer.texturedShader.SetUniformMat4("uModelView", mv);
            }
        }

        if (this->hoveredSlot != -1)
        {
            app->renderer.DrawAlignedText(Blocks[this->hoveredSlot + 1].name, this->width / 2, this->height / 5, glm::vec4(1.0), 0.5f);
        }
    }

    bool InventoryMenu::MouseClicked(int mouseX, int mouseY, int button)
    {
        Application* app = Application::GetInstance();

        glfwSetCursorPos(app->window.GetHandle(), app->window.GetWidth() / 2, app->window.GetHeight() / 2);
        glfwSetInputMode(app->window.GetHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        app->m_mouseGrabbed = true;

        if (this->hoveredSlot != -1)
        {
            app->m_hotbar[app->m_selectedItem] = this->hoveredSlot + 1;
        }

        Application::GetInstance()->SetMenu(nullptr);
        return true;

    }

    void TitleMenu::InitGui()
    {
        Button continueBtn(0, this->width / 2 - 100, this->height / 4 + 45, 200, 20, "Play");
        this->m_buttons.push_back(continueBtn);
        Button quitBtn(1, this->width / 2 - 100, this->height / 4 + 45 + 24, 200, 20, "Quit Game");
        this->m_buttons.push_back(quitBtn);
    }
    
    void TitleMenu::Resize(int width, int height)
    {
        Menu::Resize(width, height);

        this->m_buttons[0].x = this->width / 2 - 100;
        this->m_buttons[0].y = this->height / 4 + 45;

        this->m_buttons[1].x = this->width / 2 - 100;
        this->m_buttons[1].y = this->height / 4 + 45 + 24;
    }
    
    void TitleMenu::ActionPerformed(Button &button)
    {
        if (button.id == 0)
        {
            Application::GetInstance()->StartWorld();
        }

        if (button.id == 1)
        {
            Application::GetInstance()->Shutdown();
        }
    }
    
    void TitleMenu::Render(int mouseX, int mouseY)
    {
        Application* app = Application::GetInstance();

        app->renderer.DrawTexture(app->renderer.m_bgTexture, 0, 0, this->width, this->height, 0, 0, this->width, this->height, 32, 32);
        Menu::Render(mouseX, mouseY);
        app->renderer.DrawAlignedText("NoCraft 0.0.1", this->width / 2, 30, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), 0.5f);
    }
}