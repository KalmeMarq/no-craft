#pragma once

#include <string>
#include <vector>

namespace KM {
    struct Button
    {
        int id;
        int x;
        int y;
        int width;
        int height;
        std::string message;

        Button(int id, int x, int y, int width, int height, std::string message);
    };
    
    class Menu
    {
    public:
        void Init(int width, int height);
        virtual void InitGui();
        virtual void ActionPerformed(Button &button);
        virtual void Resize(int width, int height);
        virtual void Render(int mouseX, int mouseY);
        void MouseClicked(int mouseX, int mouseY, int button);
    protected:
        int width;
        int height;
        std::vector<Button> m_buttons;
    };

    class TitleMenu : public Menu
    {
    public:
        void InitGui() override;
        void Resize(int width, int height) override;
        void ActionPerformed(Button &button) override;
        void Render(int mouseX, int mouseY) override;
    };

    class GameMenu : public Menu
    {
    public:
        void InitGui() override;
        void Resize(int width, int height) override;
        void ActionPerformed(Button &button) override;
        void Render(int mouseX, int mouseY) override;
    };

    class InventoryMenu : public Menu
    {
    public:
        void InitGui() override;
        void Resize(int width, int height) override;
        void ActionPerformed(Button &button) override;
        void Render(int mouseX, int mouseY) override;
    };
}