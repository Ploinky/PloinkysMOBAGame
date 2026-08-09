#include <common/PMG_Common.h>
#include "NetworkManager.h"
#include "LobbyPlayer.h"

ServerNetworkManager::ServerNetworkManager() {
    // start out with invalid socket
    listenSocket_ = k_HSteamListenSocket_Invalid;
}

ServerNetworkManager::~ServerNetworkManager() {
    Close();
}

bool ServerNetworkManager::Initialize() {
    SteamGameServerNetworkingSockets()->InitAuthentication();
    return true;
}

bool ServerNetworkManager::CreateListenSocket(int nPort, const char* pszName) {
    if(listenSocket_) {
        StopListenSocket();
    }

    SteamNetworkingIPAddr localAddress{};
    localAddress.Clear();
    localAddress.m_port = 23119;

    m_nPort = nPort;
    m_pszName = pszName;

    listenSocket_ = SteamGameServerNetworkingSockets()->CreateListenSocketIP(localAddress, 0, nullptr);

    if(listenSocket_ == k_HSteamListenSocket_Invalid ) {
        Logger::Err("An error occurred while trying to create a Steam server host.");
        return false;
    }

    return true;
}

void ServerNetworkManager::StopListenSocket() {
    if(listenSocket_ == k_HSteamListenSocket_Invalid) {
        return;
    }

    SteamNetworkingSockets()->CloseListenSocket(listenSocket_);
    listenSocket_ = k_HSteamListenSocket_Invalid;
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
    HSteamNetConnection hConn = GetConnectionForPlayer(playerId);

	std::vector<SteamNetworkingMessage_t*> messages;
	messages.resize(1);

	if (SteamGameServerNetworkingSockets()->ReceiveMessagesOnConnection(hConn, messages.data(), 1) > 0) {
		for (SteamNetworkingMessage_t* message : messages) {
			if (message == nullptr) {
				return false;
			}

			packet->resize(message->GetSize());
			std::memcpy(packet->data(), message->GetData(), message->GetSize());

			// release when done!
			message->Release();
		}
	}
	else {
		return false;
	}

	return true;
}

void ServerNetworkManager::Update() {
    // TODO only in lan mode
    for (NetworkPeer* peer : clients_) {
        std::vector<uint8_t> packet = {};

        while (ReceivePacket(peer->idPlayer, &packet)) {
            on_clientMessageReceived(peer->idPlayer, &packet);
        }
    }
    
    /*

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
    */

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
    HSteamNetConnection hConn = GetConnectionForPlayer(playerId);
    EResult eSendResult = SteamGameServerNetworkingSockets()->SendMessageToConnection(hConn, data->data(), data->size(), k_nSteamNetworkingSend_Unreliable, nullptr);

    if (eSendResult != EResult::k_EResultOK) {
        Logger::FormatErr("Failed to send message to player %ul", playerId);
    }
}


HSteamNetConnection ServerNetworkManager::GetConnectionForPlayer(PlayerID playerId) {
    for (NetworkPeer* peer : clients_) {
        if (peer->idPlayer == playerId) {
            return peer->hConnection;
        }
    }

    return k_HSteamNetConnection_Invalid;
}

void ServerNetworkManager::ConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pConnectionStatusChanged) {
    if (pConnectionStatusChanged->m_info.m_eState == ESteamNetworkingConnectionState::k_ESteamNetworkingConnectionState_Connecting) {
        Logger::FormatMsg("Client is trying to connect");
        EResult acceptResult = SteamGameServerNetworkingSockets()->AcceptConnection(pConnectionStatusChanged->m_hConn);

        if (acceptResult != k_EResultOK) {
            Logger::Err("Failed to accept new connection");
            return;
        }

        NetworkPeer* peer = new NetworkPeer();
        peer->hConnection = pConnectionStatusChanged->m_hConn;

        SteamNetConnectionInfo_t info{};
        if (!SteamGameServerNetworkingSockets()->GetConnectionInfo(pConnectionStatusChanged->m_hConn, &info)) {
            Logger::Err("Failed to get connection info");
            // TODO codes
            SteamGameServerNetworkingSockets()->CloseConnection(peer->hConnection, 1, "Failed to get connection info", false);
            delete peer;
            return;
        }

        Logger::Msg(std::string("New client connected: ").append(std::to_string(info.m_identityRemote.GetSteamID64())));
        peer->idPlayer = info.m_identityRemote.GetSteamID().ConvertToUint64();

        // TODO include steam identity with client information, send to other clients...
        if (on_clientConnected) {
            clients_.push_back(peer);
            on_clientConnected(peer->idPlayer);
        }
    } else {
        Logger::FormatMsg("Unknown connection status change");
    }
}