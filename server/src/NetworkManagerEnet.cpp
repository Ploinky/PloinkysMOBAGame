#include "NetworkManager.h"
#include <Common/PMG_Common.h>
#include <LobbyPlayer.h>

ServerNetworkManager::ServerNetworkManager() {
    // start out with invalid socket
    m_pServerSocket = nullptr;
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

bool ServerNetworkManager::CreateListenSocket(std::string port) {
    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = DEFAULT_PORT;
    
    m_pServerSocket = enet_host_create(&address, 10, 2, 0, 0);

    if (m_pServerSocket == nullptr) {
        Logger::FormatErr("Failed to create listen socket");
        // this must mean there was an error, surely?
        return false;
    }

    return true;
}

void ServerNetworkManager::StopListenSocket() {
    // TODO does this need to be done gracefully?
    if(m_pServerSocket == nullptr) {
        Logger::Err("Trying to stop invalid listen socket");
        return;
    }

    enet_host_destroy(m_pServerSocket);
    m_pServerSocket = nullptr;
}

bool ServerNetworkManager::Close() {
    // TODO what is actually needed here? checks?
    for (NetworkPeer* peer : clients_) {
        delete peer;
    }

    clients_.clear();

    enet_deinitialize();
    return true;
}

bool ServerNetworkManager::ReceivePacket(PlayerID playerId, std::vector<uint8_t>* packet) {
    return false;
}

void ServerNetworkManager::Update() {
    ENetEvent event;
    NetworkPeer* pPeer = nullptr;
    uint64_t* newId = nullptr;
    while (m_pServerSocket != nullptr && enet_host_service(m_pServerSocket, &event, 0) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                Logger::FormatMsg("A new client connected from %x:%u.\n", event.peer->address.host, event.peer->address.port);

                /* Store any relevant client information here. */
                newId = new uint64_t(m_uCurrId++);
                event.peer->data = newId;

                pPeer = new NetworkPeer();
                pPeer->idPlayer = *newId;
                pPeer->pConnection = event.peer;
                clients_.push_back(pPeer);
                on_clientConnected(pPeer->idPlayer);
                break;
            case ENET_EVENT_TYPE_RECEIVE:
                for(NetworkPeer* pPeer : clients_) {
                    PlayerID receivedFrom = (*(uint64_t*)event.peer->data);
                    if(pPeer->idPlayer == receivedFrom) {
                        
                        std::vector<uint8_t> dat;
                        dat.resize(event.packet->dataLength);
                        std::memcpy(dat.data(), event.packet->data, dat.size());
                        on_clientMessageReceived(pPeer->idPlayer, &dat);
                    }
                }

                /* Clean up the packet now that we're done using it. */
                enet_packet_destroy (event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                printf ("%s disconnected.\n", event.peer -> data);
        
                for(NetworkPeer* pPeer : clients_) {
                    PlayerID receivedFrom = (*(uint64_t*)event.peer->data);
                    if(pPeer->idPlayer == receivedFrom) {
                        on_clientDisconnected(pPeer->idPlayer);
                        break; // breaks out of for loop, not switch/case
                    }
                }
                /* Reset the peer's client information. */
                event.peer -> data = NULL;
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
    std::vector<uint8_t> buf;
    packet->Write(&buf);

    SendToClient(playerId, &buf);
}

void ServerNetworkManager::SendToClient(PlayerID playerId, std::vector<uint8_t>* data) {
    ENetPacket* pPacket = enet_packet_create (data->data(), data->size(), ENET_PACKET_FLAG_RELIABLE);
 
    ENetPeer* peer = GetConnectionForPlayer(playerId);
    enet_peer_send (peer, 0, pPacket);
    enet_host_flush(m_pServerSocket);
}


ENetPeer* ServerNetworkManager::GetConnectionForPlayer(PlayerID playerId) {
    for (NetworkPeer* peer : clients_) {
        if (peer->idPlayer == playerId) {
            return peer->pConnection;
        }
    }

    return nullptr;
}