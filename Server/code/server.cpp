#include <chrono>
#include "server.hpp"
#include "network_manager.hpp"
#include "main.hpp"
#include "game.hpp"
#include "util.hpp"
#include "Shared/code/logger.hpp"

namespace PMG {
    void Server::Start() {
        m_networkManager = new NetworkManager();
        m_networkManager->on_clientConnected = std::bind(&Server::OnClientConnected, this, std::placeholders::_1);
        m_networkManager->on_clientDisconnected = std::bind(&Server::OnClientDisconnected, this, std::placeholders::_1);
        m_networkManager->on_clientMessageReceived = std::bind(&Server::OnMessageReceived, this, std::placeholders::_1, std::placeholders::_2);
        m_networkManager->Host();

        m_game = new Game();
        m_game->on_newGameTick = std::bind(&Server::BroadcastMessage, this, std::placeholders::_1);
        m_game->on_sendToClient = std::bind(&Server::SendMessageToClient, this, std::placeholders::_1, std::placeholders::_2);

        long long lastFrame = GetSystemTime();

        bool isRunning = true;
        while(isRunning) {
            auto thisFrame = GetSystemTime();
            float dt = (thisFrame - lastFrame) / 1000000.0f / 1000.0f;
            lastFrame = thisFrame;

            m_networkManager->Update();

            m_game->Update(dt);
        }

        m_networkManager->Close();
        delete m_networkManager;
        m_networkManager = NULL;
    }

    long long Server::GetSystemTime() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    void Server::OnClientConnected(unsigned long id) {
        printf("New client connected, id: %ld\r\n", id);
        m_game->AddPlayerForNetworkId(id);
    }

    void Server::OnClientDisconnected(unsigned long id) {
        printf("Client %ld disconnected\r\n", id);
        m_game->RemovePlayerForNetworkId(id);
    }

    void Server::OnMessageReceived(unsigned long clientId, std::string msg) {
        std::string txt = "Reveived message";
        txt.append(msg);
        Logger::Msg(txt);

        std::list<std::string> tokens = Util::SplitString(msg, std::string("|"));

        if(tokens.front() == "MoveCommand") {
            tokens.pop_front();
            while(!tokens.empty()) {
                std::string moveCmd = tokens.front();
                tokens.pop_front();
    
                std::list<std::string> mcTokens = Util::SplitString(moveCmd, std::string(";"));
                float nx = std::stof(mcTokens.front());
                mcTokens.pop_front();

                float ny = std::stof(mcTokens.front());
                mcTokens.pop_front();
            
                m_game->PlayerMoveCommand(clientId, nx, ny);
            }
        }
    }

    void Server::BroadcastMessage(std::string msg) {
        m_networkManager->SendToAllClients(msg);
    }

    void Server::SendMessageToClient(unsigned long clientId, std::string msg) {
        m_networkManager->SendToClient(clientId, msg);
    }
}