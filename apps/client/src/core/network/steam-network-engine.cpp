#include "steam-network-engine.h"

#include "common/logger.h"
#include "common/NewPackets.h"

INetworkEngine* INetworkEngine::Create() {
    return new CSteamNetworkEngine();
}


CSteamNetworkEngine::CSteamNetworkEngine() {
}


void CSteamNetworkEngine::Update(float fDt) {
}

void CSteamNetworkEngine::SendMessageToConnection(const HConnection hConnection, const BasePacket* pPacket) const {
    if(hConnection == INVALID_HANDLE) {
        Logger::Err("Attempted to send packet to invalid connection");
        return;
    }

    Connection_t connection = m_mapConnections.at(hConnection);
    
	std::vector<uint8_t> buf = std::vector<uint8_t>();
	pPacket->Write(&buf);

	SteamNetworkingSockets()->SendMessageToConnection(connection.hSteamConn, buf.data(), buf.size(), 0, nullptr);
}

bool CSteamNetworkEngine::PollConnection(const HConnection hConnection, CNetworkEvent* pEvt) const {
    if(hConnection == INVALID_HANDLE) {
        Logger::Err("Attempt to poll invalid connection");
        return false;
    }

    Connection_t connection = m_mapConnections.at(hConnection);
	std::vector<uint8_t> data;

	std::vector<SteamNetworkingMessage_t*> messages;
	messages.resize(1);

	if (SteamNetworkingSockets()->ReceiveMessagesOnConnection(connection.hSteamConn, messages.data(), 1) > 0) {
		for (SteamNetworkingMessage_t* message : messages) {

			if (message == nullptr) {
				return false;
			}

			data.resize(message->GetSize());
			std::memcpy(data.data(), message->GetData(), message->GetSize());

			// release when done!
			message->Release();
		}
	}
	else {
		return false;
	}

    pEvt->type = ENetworkEventType::PACKET_RECEIVED;
    pEvt->data = data;

	return true;
    /*
    if(hConnection == INVALID_HANDLE) {
        Logger::Err("Attempt to poll invalid connection");
        return false;
    }

    Connection_t connection = m_mapConnections.at(hConnection);

    ENetEvent event;
	std::vector<uint8_t> data;

    if (enet_host_service(connection.pHost, &event, 0) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                char pszHostName[40];
                enet_address_get_host_ip_new(&event.peer->address, pszHostName, 40);
                Logger::FormatMsg("Connected to server at %s:%u.\n", pszHostName, event.peer->address.port);
                pEvt->type = ENetworkEventType::CONNECTED;
				return true;
            case ENET_EVENT_TYPE_RECEIVE:
                pEvt->type = ENetworkEventType::PACKET_RECEIVED;
				pEvt->data.resize(event.packet->dataLength);
				std::memcpy(pEvt->data.data(), event.packet->data, pEvt->data.size());
                enet_packet_destroy (event.packet);
                return true;
            case ENET_EVENT_TYPE_DISCONNECT:
                printf ("%s disconnected.\n", event.peer -> data);
                pEvt->type = ENetworkEventType::DISCONNECTED;
                return true;
            case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
                printf("%s disconnected due to timeout.\n", event.peer->data);
                pEvt->type = ENetworkEventType::DISCONNECTED_TIMEOUT;
                return true;
        }
        
    }
	return false;
    */

}

void CSteamNetworkEngine::ConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* pStatusChanged) {
    Logger::FormatMsg("Connection status changed!");

    switch(pStatusChanged->m_info.m_eState) {
        case ESteamNetworkingConnectionState::k_ESteamNetworkingConnectionState_Connecting:
            Logger::FormatMsg("Connecting...");
            break;
        case ESteamNetworkingConnectionState::k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
            Logger::FormatMsg("Problem detected locally: %d", pStatusChanged->m_info.m_eEndReason);
            break;
        case ESteamNetworkingConnectionState::k_ESteamNetworkingConnectionState_ClosedByPeer:
            Logger::FormatMsg("Connection closed by peer: %d", pStatusChanged->m_info.m_eEndReason);
            break;
        case ESteamNetworkingConnectionState::k_ESteamNetworkingConnectionState_Connected:
            Logger::FormatMsg("Connected!");
            bIsConnected = true;
            break;
        default:
            Logger::FormatMsg("Unknown connection status change");
            break;
    }
}

HConnection CSteamNetworkEngine::ConnectToServer(const char* szAddress, int port) {
    SteamNetworkingIPAddr address{};
    address.ParseString(szAddress);

    HSteamNetConnection hSteamConn =  SteamNetworkingSockets()->ConnectByIPAddress(address, 0, nullptr);

    if (hSteamConn == k_HSteamNetConnection_Invalid) {
        return INVALID_HANDLE;
    }

    Connection_t conn;
    conn.hSteamConn = hSteamConn;
    
    HConnection hConn = m_mapConnections.size();
    m_mapConnections.emplace(hConn, conn);
    return hConn;
};