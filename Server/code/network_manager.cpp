#include "network_manager.hpp"
#include "network_server.hpp"
#include "network_client.hpp"

namespace PMG {
    NetworkManager::NetworkManager() {
        m_isHosting = false;
    }

    void NetworkManager::Host() {
        m_networkServer = new NetworkServer();
        m_networkServer->Start();

        if (m_networkServer->IsOpen()) {
            m_isHosting = true;
        }
    }

    void NetworkManager::Close() {
        m_isHosting = false;

        m_networkServer->Close();
        delete m_networkServer;
        m_networkServer = NULL;
    }

    void NetworkManager::Update() {
        // Currently allowing new connections at any time
        if (m_networkServer != NULL && m_networkServer->IsOpen()) {
            NetworkClient* client = m_networkServer->CheckForConnection();
            if (client != NULL) {
                clients.push_back(client);
                on_clientConnected(client->id);
            }
        }

        for (auto it = clients.begin(); it != clients.end(); ++it) {
            NetworkClient* client = *it;
            client->ReceiveNetworkMessages();

            while (client->HasNetworkMessage()) {
                std::string msg = client->GetNetworkMessage();
                on_clientMessageReceived(client->id, msg);
            }
        }

        for (auto it = clients.begin(); it != clients.end(); ++it) {
            NetworkClient* client = *it;

            if (client != NULL && !client->IsConnected()) {
                it = clients.erase(it);
                on_clientDisconnected(client->id);
                if (it == clients.end()) {
                    break;
                }
                delete client;
            }
        }
    }

    void NetworkManager::SendToAllClients(std::string msg) {
        for (auto it = clients.begin(); it != clients.end(); ++it) {
            NetworkClient* client = *it;

            if (client != NULL && client->IsConnected()) {
                printf("Sending to %d: %s\r\n", client->id, msg.c_str());
                client->SendNetworkMessage(msg);
            }
        }
    }

    void NetworkManager::SendToClient(unsigned long id, std::string msg) {
        for (auto it = clients.begin(); it != clients.end(); ++it) {
            NetworkClient* client = *it;

            if (client->id == id) {
                client->SendNetworkMessage(msg);
            }
        }
    }
}