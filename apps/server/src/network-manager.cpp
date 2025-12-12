#include <NetworkManager.h>
#include <common/PMG_Common.h>
#include <LobbyPlayer.h>

#define ENET_IMPLEMENTATION
#include "enet/enet.h"

ServerNetworkManager::ServerNetworkManager() {
    // start out with invalid socket
    listenSocket_ = nullptr;
}

ServerNetworkManager::~ServerNetworkManager() {
    Close();
}

bool ServerNetworkManager::Initialize() {    // TODO do we need to do anything here? like check for errors? 
    if(enet_initialize()) {
        Logger::FormatErr("Failed to initialize enet!");
        return false;
    }

    return true;
}

bool ServerNetworkManager::CreateListenSocket(std::string port) {
    if(listenSocket_) {
        StopListenSocket();
    }

    ENetAddress address = {0};
    address.host = ENET_HOST_ANY;
    address.port = 23119;

    listenSocket_ = enet_host_create(&address, 10, 2, 0, 0);
    if(listenSocket_ == nullptr) {
        Logger::Err("An error occurred while trying to create an ENet server host.");
        return false;
    }

    return true;
}

void ServerNetworkManager::StopListenSocket() {
    if(listenSocket_ == nullptr) {
        return;
    }

    enet_host_destroy(listenSocket_);
    listenSocket_ = nullptr;
}
/*
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
*/

bool ServerNetworkManager::Close() {
    StopListenSocket();

    // TODO what is actually needed here? checks?
    for (NetworkPeer* peer : clients_) {
        // TODO close connection
        delete peer;
    }

    clients_.clear();

    // TODO close socket

    return true;
}

bool ServerNetworkManager::ReceivePacket(PlayerID playerId, std::vector<uint8_t>* packet) {
    // TODO only receive 1 message every time?
    return false;
}

void ServerNetworkManager::Update() {
    for (NetworkPeer* peer : clients_) {
        std::vector<uint8_t> packet = {};

        while (ReceivePacket(peer->idPlayer, &packet)) {
            on_clientMessageReceived(peer->idPlayer, &packet);
        }
    }
    

        ENetEvent event;
        NetworkPeer* pPeer = nullptr;
        PlayerID idPlayer;
        std::vector<uint8_t> data;

        /* Wait up to 1000 milliseconds for an event. (WARNING: blocking) */
        while (enet_host_service(listenSocket_, &event, 0) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    printf("A new client connected from %x:%u.\n",  event.peer->address.host, event.peer->address.port);
                    /* Store any relevant client information here. */
                    pPeer = new NetworkPeer();
                    pPeer->idPlayer = clients_.size();
                    pPeer->pConnection = event.peer;
                    clients_.push_back(pPeer);
                    event.peer->data = (void*)&pPeer->idPlayer;
                    on_clientConnected(pPeer->idPlayer);
                    break;

                case ENET_EVENT_TYPE_RECEIVE:
                    printf("A packet of length %lu containing %s was received from %s on channel %u.\n",
                            event.packet->dataLength,
                            event.packet->data,
                            event.peer->data,
                            event.channelID);
                    data.resize(event.packet->dataLength);
                    std::memcpy(data.data(), event.packet->data, data.size());
                    idPlayer = *(PlayerID*)(event.peer->data);
                    on_clientMessageReceived(idPlayer, &data);
                    /* Clean up the packet now that we're done using it. */
                    enet_packet_destroy (event.packet);
                    break;

                case ENET_EVENT_TYPE_DISCONNECT:
                    printf("%s disconnected.\n", event.peer->data);
                    /* Reset the peer's client information. */
                    idPlayer = *(PlayerID*)(event.peer->data);
                    on_clientDisconnected(idPlayer);
                    event.peer->data = NULL;
                    break;

                case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
                    printf("%s disconnected due to timeout.\n", event.peer->data);
                    /* Reset the peer's client information. */
                    event.peer->data = NULL;
                    break;

                case ENET_EVENT_TYPE_NONE:
                    break;
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
    std::vector<uint8_t> buf = std::vector<uint8_t>();
    packet->Write(&buf);
    SendToClient(playerId, &buf);
}

void ServerNetworkManager::SendToClient(PlayerID playerId, std::vector<uint8_t>* data) {
    // TODO send
    ENetPacket* pPacket = enet_packet_create (data->data(), data->size(), ENET_PACKET_FLAG_RELIABLE);
 
    ENetPeer* pPeer = GetConnectionForPlayer(playerId);
    enet_peer_send (pPeer, 0, pPacket);
    enet_host_flush(listenSocket_);
}


ENetPeer* ServerNetworkManager::GetConnectionForPlayer(PlayerID playerId) {
    for (NetworkPeer* peer : clients_) {
        if (peer->idPlayer == playerId) {
            return peer->pConnection;
        }
    }

    return nullptr;
}