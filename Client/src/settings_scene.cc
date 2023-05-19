#include "settings_scene.h"
#include "gui.h"
#include "client.h"
#include "settings.h"

namespace PMG {
	SettingsScene::SettingsScene(ClientStateHandler* stateHandler) : Scene(stateHandler) {
        VBox* root = new VBox();
        root->m_size = { 1024, 768 };
        root->m_pos = { 0, 0 };

        GuiButton* btnFullscreen = new GuiButton();
        btnFullscreen->m_color[0] = 0.2f;
        btnFullscreen->m_color[1] = 0.2f;
        btnFullscreen->m_color[2] = 0.2f;
        btnFullscreen->m_pos = { 0, 0 };
        btnFullscreen->m_prefSize = { 400, 100 };
        btnFullscreen->m_text = L"FullScreen";
        btnFullscreen->e_onButtonPressed = [this]() {
            Settings::SetInt(PMGSettings::WINDOW_MODE, (int)WindowMode::FULLSCREEN);
        };

        GuiButton* btnBack = new GuiButton();
        btnBack->m_color[0] = 0.2f;
        btnBack->m_color[1] = 0.2f;
        btnBack->m_color[2] = 0.2f;
        btnBack->m_pos = { 0, 0 };
        btnBack->m_prefSize = { 400, 100 };
        btnBack->m_text = L"Back";
        btnBack->e_onButtonPressed = [this]() {
            m_stateHandler->PopState();
        };

        root->m_children.push_back(btnFullscreen);
        root->m_children.push_back(btnBack);

        rootGuiElement = root;
	}
    void SettingsScene::Update(float dt) {
        // Update ui? This seems like a bad idea...
        rootGuiElement->LayoutChildren();
    };

    void SettingsScene::Render(Renderer* renderer) {
        rootGuiElement->Render(renderer);
    };

    void SettingsScene::KeyReleased(uint32_t key) {

    }

    void SettingsScene::MouseMoved(int screenX, int screenY) {
        m_mousePos[0] = screenX;
        m_mousePos[1] = screenY;
    }

    void SettingsScene::MouseButtonPressed(int button) {
        if (button == 0) {
            rootGuiElement->MousePressed(m_mousePos[0], m_mousePos[1]);
        }
    }

    void SettingsScene::MouseButtonReleased(int button) {

    }

    void SettingsScene::CharTyped(uint32_t ch) {
        rootGuiElement->CharTyped(ch);
    }

    void SettingsScene::KeyPressed(uint32_t key) {
    };
}