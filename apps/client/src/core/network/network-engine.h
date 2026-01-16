#pragma once

#include <vector>
#include "common/pmg_types.h"
#include "common/packets/BasePacket.h"

typedef EngineHandle HServerRequest;
typedef EngineHandle HConnection;

typedef struct RequestResult_s {
    const char* szIp;
    int nPort;
    const char* szName;
} RequestResult_t;

enum class ENetworkEventType {
    CONNECTED,
    PACKET_RECEIVED,
    DISCONNECTED,
    DISCONNECTED_TIMEOUT,
};

class CNetworkEvent {
public:
    ENetworkEventType type;
    std::vector<uint8_t> data;
};


class IRequestObserver {
public:
	virtual void ServerFound(RequestResult_t result) = 0;
};
class INetworkEngine {
public:
    static INetworkEngine* Create();

    virtual void Update(float fDt) = 0;

    virtual HServerRequest RequestServers() = 0;
    virtual void StopServerRequest(HServerRequest hRequest) = 0;
    virtual std::vector<RequestResult_t> GetRequestResults(HServerRequest hRequest) = 0;

    virtual void SendMessageToConnection(const HConnection hConnection, const BasePacket* pPacket) const = 0;
    virtual bool PollConnection(const HConnection hConnection, CNetworkEvent* pEvt) const = 0;
    virtual HConnection ConnectToServer(const char* szAddress, int port) = 0;

    void RegisterRequestObserver(IRequestObserver* pObserver) {
        for(const auto& pExistingObserver : m_vecObservers) {
            if(pExistingObserver == pObserver) {
                return;
            }
        }

        m_vecObservers.push_back(pObserver);
    }

protected:
	std::vector<IRequestObserver*> m_vecObservers;
};