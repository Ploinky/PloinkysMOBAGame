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

bool ServerNetworkManager::Initialize() {
   // TODO do we need to do anything here? like check for errors? 
    if(enet_initialize()) {
        Logger::FormatErr("Failed to initialize enet!");
        return false;
    }

    return true;
}

bool ServerNetworkManager::CreateListenSocket(int nPort, const char* pszName) {
    if(listenSocket_) {
        StopListenSocket();
    }

    m_nPort = nPort;
    m_pszName = pszName;

    ENetAddress address = {0};
    address.host = ENET_HOST_ANY;
    address.port = nPort;

    listenSocket_ = enet_host_create(&address, 10, 2, 0, 0);
    if(listenSocket_ == nullptr) {
        Logger::Err("An error occurred while trying to create an ENet server host.");
        return false;
    }
    
    
    sock = socket(AF_INET, SOCK_DGRAM, 0);

    int broadcastEnable = 1;
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST,
            (char*)&broadcastEnable, sizeof(broadcastEnable));

    return true;
}

void ServerNetworkManager::StopListenSocket() {
    if(listenSocket_ == nullptr) {
        return;
    }

    enet_host_destroy(listenSocket_);
    listenSocket_ = nullptr;
}

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
    // TODO only in lan mode
    if(m_frameTimer.Frame()) {
        ServerInfoPacket pkt{};
        pkt.usPort = m_nPort;
        pkt.szName = (char*) m_pszName;
        pkt.ubNameLen = std::strlen(m_pszName);
        pkt.ubPlayerCount = 0;
        pkt.ubPlayerMaxCount = 10;

        std::vector<uint8_t> data;
        pkt.Write(&data);

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(23119);
        addr.sin_addr.s_addr = INADDR_BROADCAST;

        int sent = sendto(sock, (char*)data.data(), data.size(), 0,
            (sockaddr*)&addr, sizeof(addr));

        if(sent == SOCKET_ERROR) {
            Logger::FormatErr("Failed to send lan discovery packet; %d", WSAGetLastError());
        }
    }

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

        while (enet_host_service(listenSocket_, &event, 0) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    pPeer = new NetworkPeer();
                    pPeer->idPlayer = clients_.size();
                    pPeer->pConnection = event.peer;
                    clients_.push_back(pPeer);
                    event.peer->data = (void*)&pPeer->idPlayer;
                    on_clientConnected(pPeer->idPlayer);
                    break;

                case ENET_EVENT_TYPE_RECEIVE:
                    data.resize(event.packet->dataLength);
                    std::memcpy(data.data(), event.packet->data, data.size());
                    idPlayer = *(PlayerID*)(event.peer->data);
                    on_clientMessageReceived(idPlayer, &data);
                    enet_packet_destroy (event.packet);
                    break;

                case ENET_EVENT_TYPE_DISCONNECT:
                    idPlayer = *(PlayerID*)(event.peer->data);
                    on_clientDisconnected(idPlayer);
                    event.peer->data = NULL;
                    break;

                case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
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