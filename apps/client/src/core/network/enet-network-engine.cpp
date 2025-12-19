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

    std::vector<uint8_t> buf(22);
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
            Logger::FormatMsg("Found server %s at %s", pkt.szName, res.szIp);
            m_mapFound.emplace(from.sin_addr.S_un.S_addr, res);
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
    return {};
}