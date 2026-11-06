#pragma once

#include <map>

#include <steam/steam_api.h>

#include "network-engine.h"
#include "common/pmg_types.h"
#include "common/util/frame-timer.h"

#define BROWSE_TIME 10000

typedef EngineHandle HServerRequest;

typedef struct Connection_s {
    HSteamNetConnection hSteamConn;
} Connection_t;

class CSteamNetworkEngine : public INetworkEngine {
public:
    CSteamNetworkEngine();

    virtual void Update(float fDt) override;

    virtual HConnection ConnectToServer(const char* szAddress, int port) override;
    virtual void SendMessageToConnection(const HConnection hConnection, const BasePacket* pPacket) const override;
    virtual bool PollConnection(const HConnection hConnection, CNetworkEvent* pEvt) const override;
    virtual int GetPingToConnection(const HConnection hConnection) const override;

    virtual bool CSteamNetworkEngine::Disconnect(const HConnection hConnection) override;

private:
    std::map<HConnection, Connection_t> m_mapConnections;

    STEAM_CALLBACK(CSteamNetworkEngine, ConnectionStatusChangedCallback, SteamNetConnectionStatusChangedCallback_t);
};