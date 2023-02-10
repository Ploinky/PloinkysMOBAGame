#include "scene.h"
#include "client.h"
#include "renderer.h"

namespace PMG {
    ConnectScene::ConnectScene(ClientStateHandler* stateHandler, std::string ip) : Scene(stateHandler) {
        m_text = L"Connecting...";
        m_netConnection = {};
        if(Net_Init()) {
            Net_ConnectToServer(ip, std::to_string(DEFAULT_PORT), &m_netConnection);
        }
    }

    net_client_t ConnectScene::GetConnection() {
      return m_netConnection;
    }

    void ConnectScene::Update(float dt) {
        if(!m_netConnection.isConnected) {
            if(!Net_IsConnected(&m_netConnection)) {
                m_text = L"Connection failed!";
            }
        }
        else {
            m_stateHandler->PushState(ClientState::NETWORKED_GAME);
        }
    };

    void ConnectScene::Render(Renderer* renderer) {
        // Render some sort of ui?
        renderer->RenderText(0, 0, 1000, 1000, m_text);
    };

    void ConnectScene::CharTyped(uint16_t ch) {

    }

    void ConnectScene::KeyPressed(uint16_t key) {
        if (key == VK_ESCAPE) {
            m_stateHandler->PushState(ClientState::MAIN_MENU);
        }
    }

    void ConnectScene::KeyReleased(uint16_t key) {

    }

    void ConnectScene::MouseMoved(int screenX, int screenY) {

    }

    void ConnectScene::MouseButtonPressed(int button) {

    }

    void ConnectScene::MouseButtonReleased(int button) {

    }
}