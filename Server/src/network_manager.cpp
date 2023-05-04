#include "network_manager.h"
#include "logger.h"

namespace PMG {
    NetworkManager::NetworkManager() {
    }

    bool NetworkManager::Initialize() {
        SteamAPI_Init();
        SteamNetworkingUtils()->InitRelayNetworkAccess();

        return true;
    }

    bool NetworkManager::CreateListenSocket() {
        SteamNetworkingIPAddr ipAddr{};
        ipAddr.Clear();
        ipAddr.m_port = 23119;
        listen_socket_ = SteamNetworkingSockets()->CreateListenSocketIP(ipAddr, 0, 0);
        Logger::Msg("Listen socket created");
        return listen_socket_ != 0;
    }

    void NetworkManager::OnConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* payload) {
        if (payload->m_info.m_hListenSocket
            && payload->m_eOldState == k_ESteamNetworkingConnectionState_None
            && payload->m_info.m_eState == k_ESteamNetworkingConnectionState_Connecting) {
            Logger::Msg("Connection status changed, accepting connection...");
            // New client is trying to connect, we have to accept fast
            AcceptConnection(payload->m_hConn);
        }
    }

    bool NetworkManager::AcceptConnection(HSteamNetConnection connection) {
        EResult result = SteamNetworkingSockets()->AcceptConnection(connection);

        if (result != 0) {
            Logger::Err("Failed to accept connection");
            return false;
        }

        net_client_t client{};
        client.isConnected = true;
        client.socket = connection;
        clients_.push_back(client);

        on_clientConnected(client.socket);

        Logger::Msg("Connection accepted");
        return true;
    }

    bool NetworkManager::Close() {
        Logger::Msg("Closing listen socket");
        return SteamNetworkingSockets()->CloseListenSocket(listen_socket_);
    }

    void NetworkManager::Update() {
        for (auto it = clients_.begin(); it != clients_.end(); ++it) {
            net_client_t client = *it;
            packet_t packet = {};

            SteamNetworkingMessage_t** msgs;
            int msgsReceived = SteamNetworkingSockets()->ReceiveMessagesOnConnection(client.socket, msgs, 10);

            for (int msgIndex = 0; msgIndex < msgsReceived; msgIndex++) {
                SteamNetworkingMessage_t* msg = msgs[msgIndex];
                packet << msg->GetData();
                on_clientMessageReceived(client.socket, &packet);
                msg->Release();
            }
        }
    }

    void NetworkManager::SendToAllClients(packet_t* packet) {
        for (auto it = clients_.begin(); it != clients_.end(); ++it) {
            if(it != clients_.end() && it->isConnected) {
                if (!SteamNetworkingSockets()->SendMessageToConnection(it->socket, &packet, sizeof(packet), 0, 0)) {
                    Logger::Err("Failed to send message");
                }
            }
        }
    }

    void NetworkManager::SendToClient(unsigned long id, packet_t* packet) {
        for (auto it = clients_.begin(); it != clients_.end(); ++it) {
            net_client_t client = *it;

            if (client.socket == id) {
                if(client.isConnected) {
                    if (!SteamNetworkingSockets()->SendMessageToConnection(it->socket, &packet, sizeof(packet), 0, 0)) {
                        Logger::Err("Failed to send message");
                    }
                }
            }
        }
    }
}