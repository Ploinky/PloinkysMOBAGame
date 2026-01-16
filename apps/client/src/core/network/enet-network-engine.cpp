#include "core/network/enet-network-engine.h"

#include "common/logger.h"
#include "common/NewPackets.h"

INetworkEngine* INetworkEngine::Create() {
    if(enet_initialize()) {
        Logger::FormatErr("Failed to initialize enet");
        return nullptr;
    }

    return new CEnetNetworkEngine();
}


CEnetNetworkEngine::CEnetNetworkEngine() {
}


void CEnetNetworkEngine::Update(float fDt) {
    if(m_hCurrentRequest == INVALID_HANDLE) {
        return;
    }

    ServerInfoPacket pkt;
    sockaddr_in from;
    socklen_t fromLen = sizeof(from);

    std::vector<uint8_t> buf(24);
    int n = recvfrom(m_request.sock, (char*)buf.data(), buf.size(), 0,
            (sockaddr*)&from, &fromLen);

    if(n == SOCKET_ERROR) {
        int e = WSAGetLastError();
        if(e != WSAEWOULDBLOCK) {
           Logger::FormatMsg("Error %d", e);
        }
    }
    else if (n > 0) {
        pkt.Read(&buf);

        if(m_mapFound.find(from.sin_addr.S_un.S_addr) == m_mapFound.end()) {
            RequestResult_t res;
            res.szIp = inet_ntoa(from.sin_addr);
            res.nPort = pkt.usPort;
            Logger::FormatMsg("Found server %s at %s:%d", pkt.szName, res.szIp, pkt.usPort);
            m_mapFound.emplace(from.sin_addr.S_un.S_addr, res);
            
            for(const auto& obs : m_vecObservers) {
                obs->ServerFound(res);
            }
        }
    }
}

HServerRequest CEnetNetworkEngine::RequestServers() {
    m_request.fBrowseTimeMS = 0.0f;

    m_request.sock = socket(AF_INET, SOCK_DGRAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(23119);
    addr.sin_addr.s_addr = INADDR_ANY;
            int reuse = 1;
    setsockopt(m_request.sock, SOL_SOCKET, SO_REUSEADDR,
           (char*)&reuse, sizeof(reuse));
    bind(m_request.sock, (sockaddr*)&addr, sizeof(addr));
    
    u_long nonBlocking = 1;
    ioctlsocket(m_request.sock, FIONBIO, &nonBlocking);
    m_hCurrentRequest = 0;
    return m_hCurrentRequest;
}

void CEnetNetworkEngine::StopServerRequest(HServerRequest hRequest) {
    
}

std::vector<RequestResult_t> CEnetNetworkEngine::GetRequestResults(HServerRequest hRequest) {
    std::vector<RequestResult_t> values;

    // Transform map values into a vector
    std::transform(m_mapFound.begin(), m_mapFound.end(), std::back_inserter(values),
    [](const std::pair<unsigned long, RequestResult_t>& pair) { return pair.second; });

    return values;
}

void CEnetNetworkEngine::SendMessageToConnection(const HConnection hConnection, const BasePacket* pPacket) const {
    if(hConnection == INVALID_HANDLE) {
        Logger::Err("Attempted to send packet to invalid connection");
        return;
    }

    Connection_t connection = m_mapConnections.at(hConnection);
    
	std::vector<uint8_t> buf = std::vector<uint8_t>();
	pPacket->Write(&buf);

    ENetPacket* pEnetPacket = enet_packet_create (buf.data(), buf.size(), ENET_PACKET_FLAG_RELIABLE);
 
    enet_peer_send (connection.pPeer, 0, pEnetPacket);
    enet_host_flush(connection.pHost);
}

bool CEnetNetworkEngine::PollConnection(const HConnection hConnection, CNetworkEvent* pEvt) const {
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
}

HConnection CEnetNetworkEngine::ConnectToServer(const char* szAddress, int port) {
	ENetHost* pHost = enet_host_create (NULL, 1, 2, 0, 0);
	if (pHost == NULL) {
		Logger::Err("An error occurred while trying to create an ENet client host.");
		return INVALID_HANDLE;
	}
	
	ENetAddress address = {0};
	ENetEvent event;
	ENetPeer *peer;
	
	enet_address_set_host(&address, szAddress);
	address.port = port;
	
	ENetPeer* pPeer = enet_host_connect(pHost, &address, 2, 0);
	
	if (pPeer == NULL) {
        enet_host_destroy(pHost);
		Logger::Err("Failed to connect to server: No available peers for initiating an ENet connection.");
		return INVALID_HANDLE;
	}

    Connection_t conn;
    conn.pHost = pHost;
    conn.pPeer = pPeer;

    HConnection hConn = m_mapConnections.size();
    m_mapConnections.emplace(hConn, conn);
    return hConn;
};