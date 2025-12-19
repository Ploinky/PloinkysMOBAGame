#pragma once

#include <map>
#include <enet/enet.h>

#include "network-engine.h"
#include "common/pmg_types.h"
#include "common/util/frame-timer.h"

#define BROWSE_TIME 10000

typedef EngineHandle HServerRequest;

typedef struct ServerRequest_s {
    float fBrowseTimeMS = 0.0f;
    SOCKET sock;
} ServerRequest_t;

class CEnetNetworkEngine : public INetworkEngine {
public:
    CEnetNetworkEngine();

    virtual void Update(float fDt) override;

    virtual HServerRequest RequestServers() override;
    virtual void StopServerRequest(HServerRequest hRequest) override;
    virtual std::vector<RequestResult_t> GetRequestResults(HServerRequest hRequest) override;

private:
    HServerRequest m_hCurrentRequest = INVALID_HANDLE;
    ServerRequest_t m_request;
    std::map<unsigned long, RequestResult_t> m_mapFound;
};