#include "NetworkManager.h"
#include "logger.h"

namespace PMG {
    ServerNetworkManager::ServerNetworkManager() {
        // start out with invalid socket
        listenSocket_ = k_HSteamListenSocket_Invalid;
    }

    ServerNetworkManager::~ServerNetworkManager() {
        Close();
    }

    bool ServerNetworkManager::Initialize() {
        // do we need to do anything here?
        SteamGameServerNetworkingSockets()->InitAuthentication();
        return true;
    }

    bool ServerNetworkManager::CreateListenSocket(std::string port) {
        SteamNetworkingIPAddr addr{};
        addr.Clear();
        addr.m_port = 23119;

        listenSocket_ = SteamGameServerNetworkingSockets()->CreateListenSocketIP(addr, 0, 0);

        if (listenSocket_ == k_HSteamListenSocket_Invalid) {
            // this must mean there was an error, surely?
            return false;
        }

        return true;
    }

    void ServerNetworkManager::OnConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* callback) {
        if (callback->m_info.m_hListenSocket && callback->m_eOldState == k_ESteamNetworkingConnectionState_None
            && callback->m_info.m_eState == k_ESteamNetworkingConnectionState_Connecting) {
            // new client is connecting
            EResult acceptResult = SteamGameServerNetworkingSockets()->AcceptConnection(callback->m_hConn);
            if (acceptResult != k_EResultOK) {
                throw new std::exception("Failed to accept new connection");
            }

            clients_.push_back(callback->m_hConn);
            
            SteamNetConnectionInfo_t info{};
            if (!SteamGameServerNetworkingSockets()->GetConnectionInfo(callback->m_hConn, &info)) {
                Logger::Err("Failed to get connection info");
                // TODO disconnect from client maybe?
                return;
            }

            Logger::Msg(std::string("New client connected: ").append(std::to_string(info.m_identityRemote.GetSteamID64())));

            // TODO include steam identity with client information, send to other clients...
            if (on_clientConnected) {
                on_clientConnected(callback->m_hConn);
            }
        }
        else if ((callback->m_eOldState == k_ESteamNetworkingConnectionState_Connecting || callback->m_eOldState == k_ESteamNetworkingConnectionState_Connected)
            && (callback->m_info.m_eState == k_ESteamNetworkingConnectionState_ClosedByPeer || callback->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally)) {
            Logger::Msg("Client disconnected");

            clients_.remove(callback->m_hConn);
            
            if (on_clientDisconnected) {
                on_clientDisconnected(callback->m_hConn);
            }
        }
        else {
            throw new std::exception("Unknown connection status change received");
        }
    }

    bool ServerNetworkManager::Close() {
        // TODO
        for (HSteamNetConnection conn : clients_) {
            SteamNetworkingSockets()->CloseConnection(conn, 0, nullptr, false);
        }
        SteamGameServerNetworkingSockets()->CloseListenSocket(listenSocket_);
        return true;
    }

    bool ServerNetworkManager::ReceivePacket(HSteamNetConnection conn, std::vector<uint8_t>* packet) {
        // TODO only receive 1 message every time?
        std::vector<SteamNetworkingMessage_t*> messages;
        messages.resize(1);

        if (SteamGameServerNetworkingSockets()->ReceiveMessagesOnConnection(conn, messages.data(), 1)) {
            for (SteamNetworkingMessage_t* message : messages) {

                packet->resize(message->GetSize());
                std::memcpy(packet->data(), message->GetData(), message->GetSize());

                // release when done!
                message->Release();

                return true;
            }
        }

        return false;
    }

    void ServerNetworkManager::Update() {
        for (HSteamNetConnection conn : clients_) {
            std::vector<uint8_t> packet = {};

            while (ReceivePacket(conn, &packet)) {
                on_clientMessageReceived(conn, &packet);
            }
        }
    }

    void ServerNetworkManager::SendToAllClients(std::vector<uint8_t>* data) {
        for (HSteamNetConnection conn : clients_) {
            SendToClient(conn, data);
        }
    }

    void ServerNetworkManager::SendToClient(HSteamNetConnection conn, Networking::BasePacket* packet) {
        std::vector<uint8_t>* buf = new std::vector<uint8_t>();
        packet->Write(buf);

        EResult result = SteamGameServerNetworkingSockets()->SendMessageToConnection(conn, buf->data(), buf->size(), 0, nullptr);

        if (result != k_EResultOK) {
            Logger::Err("Failed to send message to client");
        }
    }

    void ServerNetworkManager::SendToClient(HSteamNetConnection conn, std::vector<uint8_t>* data) {
        EResult result = SteamGameServerNetworkingSockets()->SendMessageToConnection(conn, data->data(), data->size(), 0, nullptr);

        if (result != k_EResultOK) {
            Logger::Err("Failed to send message to client");
        }
    }
}