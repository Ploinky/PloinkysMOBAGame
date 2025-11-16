#include <NetworkManager.h>
#include <common/PMG_Common.h>
#include <LobbyPlayer.h>

ServerNetworkManager::ServerNetworkManager() {
    // start out with invalid socket
    listenSocket_ = k_HSteamListenSocket_Invalid;
}

ServerNetworkManager::~ServerNetworkManager() {
    Close();
}

bool ServerNetworkManager::Initialize() {
    // TODO do we need to do anything here? like check for errors?
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

void ServerNetworkManager::StopListenSocket() {
    // TODO does this need to be done gracefully?
    if (!SteamGameServerNetworkingSockets()->CloseListenSocket(listenSocket_)) {
        Logger::Err("Failed to close listen socket");
        return;
    }

    listenSocket_ = k_HSteamListenSocket_Invalid;
}

void ServerNetworkManager::OnConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* callback) {
    if (callback->m_info.m_hListenSocket && callback->m_eOldState == k_ESteamNetworkingConnectionState_None
        && callback->m_info.m_eState == k_ESteamNetworkingConnectionState_Connecting) {
        // new client is connecting
        NetworkPeer* peer = new NetworkPeer();
        peer->pConnection = callback->m_hConn;

        EResult acceptResult = SteamGameServerNetworkingSockets()->AcceptConnection(callback->m_hConn);

        if (acceptResult != k_EResultOK) {
            Logger::Err("Failed to accept new connection");
            delete peer;
            return;
        }
        
        SteamNetConnectionInfo_t info{};
        if (!SteamGameServerNetworkingSockets()->GetConnectionInfo(callback->m_hConn, &info)) {
            Logger::Err("Failed to get connection info");
            delete peer;
            // TODO disconnect from client maybe?
            return;
        }

        Logger::Msg(std::string("New client connected: ").append(std::to_string(info.m_identityRemote.GetSteamID64())));
        peer->idPlayer = info.m_identityRemote.GetSteamID();

        // TODO include steam identity with client information, send to other clients...
        if (on_clientConnected) {
            clients_.push_back(peer);
            on_clientConnected(peer->idPlayer);
        }
    }
    else if ((callback->m_eOldState == k_ESteamNetworkingConnectionState_Connecting || callback->m_eOldState == k_ESteamNetworkingConnectionState_Connected)
        && (callback->m_info.m_eState == k_ESteamNetworkingConnectionState_ClosedByPeer || callback->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally)) {
        Logger::Msg("Client disconnected");

        for (NetworkPeer* peer : clients_) {
            if (peer->pConnection == callback->m_hConn) {
                on_clientDisconnected(peer->idPlayer);
                clients_.remove(peer);
                delete peer;
                return;
            }
        }
        // TODO no return in loop means disconnect failed?
        Logger::Err("Failed to disconnect client");
    }
    else {
        Logger::Err("Unknown connection status change received");
    }
}

bool ServerNetworkManager::Close() {
    // TODO what is actually needed here? checks?
    for (NetworkPeer* peer : clients_) {
        SteamNetworkingSockets()->CloseConnection(peer->pConnection, 0, nullptr, false);
        delete peer;
    }

    clients_.clear();

    SteamGameServerNetworkingSockets()->CloseListenSocket(listenSocket_);

    return true;
}

bool ServerNetworkManager::ReceivePacket(PlayerID playerId, std::vector<uint8_t>* packet) {
    // TODO only receive 1 message every time?
    std::vector<SteamNetworkingMessage_t*> messages;
    messages.resize(1);

    if (SteamGameServerNetworkingSockets()->ReceiveMessagesOnConnection(GetConnectionForPlayer(playerId), messages.data(), 1)) {
        for (SteamNetworkingMessage_t* message : messages) {
            if (message == nullptr) {
                Logger::Err("Received NULL message");
                continue;
            }

            packet->resize(message->GetSize());
            memcpy(packet->data(), message->GetData(), message->GetSize());

            // release when done!
            message->Release();

            return true;
        }
    }

    return false;
}

void ServerNetworkManager::Update() {
    for (NetworkPeer* peer : clients_) {
        std::vector<uint8_t> packet = {};

        while (ReceivePacket(peer->idPlayer, &packet)) {
            on_clientMessageReceived(peer->idPlayer, &packet);
        }
    }
}

void ServerNetworkManager::SendToAllClients(BasePacket& packet) {
    std::vector<uint8_t> data;
    packet.Write(&data);

    for (NetworkPeer* peer : clients_) {
        SendToClient(peer->idPlayer, &data);
    }

}

void ServerNetworkManager::SendToAllClients(std::vector<uint8_t>* data) {
    for (NetworkPeer* peer : clients_) {
        SendToClient(peer->idPlayer, data);
    }
}

void ServerNetworkManager::SendToAllClients(BasePacket* packet) {
    for (NetworkPeer* peer : clients_) {
        SendToClient(peer->idPlayer, packet);
    }
}

void ServerNetworkManager::SendToClient(PlayerID playerId, BasePacket* packet) {
    std::vector<uint8_t>* buf = new std::vector<uint8_t>();
    packet->Write(buf);

    EResult result = SteamGameServerNetworkingSockets()->SendMessageToConnection(GetConnectionForPlayer(playerId), buf->data(), buf->size(), 0, nullptr);

    if (result != k_EResultOK) {
        Logger::Err("Failed to send message to client");
    }
}

void ServerNetworkManager::SendToClient(PlayerID playerId, std::vector<uint8_t>* data) {
    EResult result = SteamGameServerNetworkingSockets()->SendMessageToConnection(GetConnectionForPlayer(playerId), data->data(), data->size(), 0, nullptr);

    if (result != k_EResultOK) {
        Logger::Err("Failed to send message to client");
    }
}


HSteamNetConnection ServerNetworkManager::GetConnectionForPlayer(PlayerID playerId) {
    for (NetworkPeer* peer : clients_) {
        if (peer->idPlayer == playerId) {
            return peer->pConnection;
        }
    }

    return k_HSteamNetConnection_Invalid;
}