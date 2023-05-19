#include "gui.h"
#include "scene.h"
#include "renderer.h"
#include "client.h"
#include "gui.h"
#include "settings.h"

namespace PMG {
    MainMenuScene::MainMenuScene(ClientStateHandler* stateHandler) : Scene(stateHandler) {
        VBox* root = new VBox();
        root->m_size = { 1024, 768 };
        root->m_pos = { 0, 0 };

        m_textfield = new GuiTextfield();
        m_textfield->m_pos = { 0, 0 };
        m_textfield->m_prefSize = { 400, 100 };

        GuiButton* btnConnect = new GuiButton();
        btnConnect->m_color[0] = 0.2f;
        btnConnect->m_color[1] = 0.2f;
        btnConnect->m_color[2] = 0.2f;
        btnConnect->m_pos = { 0, 0 };
        btnConnect->m_prefSize = { 400, 100 };
        btnConnect->m_text = L"Connect";
        btnConnect->e_onButtonPressed = [this]() {
            m_stateHandler->PushState(ClientState::CONNECT);
        };

        GuiButton* btnSettings = new GuiButton();
        btnSettings->m_color[0] = 0.2f;
        btnSettings->m_color[1] = 0.2f;
        btnSettings->m_color[2] = 0.2f;
        btnSettings->m_pos = { 0, 0 };
        btnSettings->m_prefSize = { 400, 100 };
        btnSettings->m_text = L"Settings";
        btnSettings->e_onButtonPressed = [this]() {
            m_stateHandler->PushState(ClientState::SETTINGS);
        };

        GuiButton* btnQuit = new GuiButton();
        btnQuit->m_color[0] = 0.2f;
        btnQuit->m_color[1] = 0.2f;
        btnQuit->m_color[2] = 0.2f;
        btnQuit->m_pos = { 0, 0 };
        btnQuit->m_prefSize = { 400, 100 };
        btnQuit->m_text = L"Quit";
        btnQuit->e_onButtonPressed = [this]() {
            m_stateHandler->PushState(ClientState::SHUTDOWN);
        };

        root->m_children.push_back(m_textfield);
        root->m_children.push_back(btnConnect);
        root->m_children.push_back(btnSettings);
        root->m_children.push_back(btnQuit);

        rootGuiElement = root;
    }

    void MainMenuScene::Update(float dt) {
        // Update ui? This seems like a bad idea...
        rootGuiElement->LayoutChildren();
    };

    void MainMenuScene::Render(Renderer* renderer) {
        rootGuiElement->Render(renderer);
    };

    void MainMenuScene::KeyReleased(uint32_t key) {

    }

    void MainMenuScene::MouseMoved(int screenX, int screenY) {
        m_mousePos[0] = screenX;
        m_mousePos[1] = screenY;
    }

    void MainMenuScene::MouseButtonPressed(int button) {
        if (button == 0) {
            rootGuiElement->MousePressed(m_mousePos[0], m_mousePos[1]);
        }
    }

    void MainMenuScene::MouseButtonReleased(int button) {

    }

    void MainMenuScene::CharTyped(uint32_t ch) {
        if (ch == '\r' || ch == '\n') {
            m_stateHandler->PushState(ClientState::CONNECT);
            return;
        }

        rootGuiElement->CharTyped(ch);
    }

    void MainMenuScene::KeyPressed(uint32_t key) {
    };

    std::wstring MainMenuScene::GetIp() {
        if(m_textfield->m_text.size() > 0) {
            return m_textfield->m_text;
        } else {
            return L"127.0.0.1";
        }
    };
}