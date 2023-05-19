#include "scene.h"
#include "client.h"
#include "renderer.h"

namespace PMG {
    ConnectScene::ConnectScene(ClientStateHandler* stateHandler, std::string ip) : Scene(stateHandler) {
        m_text = L"Connecting...";
        network_manager_ = NetworkManager();
        if(network_manager_.Initialize()) {
            network_manager_.ConnectToServer(ip);
        }
    }

    NetworkManager ConnectScene::GetConnection() {
      return network_manager_;
    }

    void ConnectScene::Update(float dt) {
        if(!network_manager_.IsConnected()) {
            // TODO: Check if connection has failed?
        }
        else {
            m_stateHandler->PushState(ClientState::NETWORKED_GAME);
        }
    };

    void ConnectScene::Render(Renderer* renderer) {
        // Render some sort of ui?
        renderer->RenderText(0, 0, 1000, 1000, m_text);
    };

    void ConnectScene::CharTyped(uint32_t ch) {

    }

    void ConnectScene::KeyPressed(uint32_t key) {
        if (key == VK_ESCAPE) {
            m_stateHandler->PushState(ClientState::MAIN_MENU);
        }
    }

    void ConnectScene::KeyReleased(uint32_t key) {

    }

    void ConnectScene::MouseMoved(int screenX, int screenY) {

    }

    void ConnectScene::MouseButtonPressed(int button) {

    }

    void ConnectScene::MouseButtonReleased(int button) {

    }
}