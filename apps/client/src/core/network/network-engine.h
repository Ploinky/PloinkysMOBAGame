#pragma once

#include <vector>
#include "common/pmg_types.h"

typedef EngineHandle HServerRequest;

typedef struct RequestResult_s {
    const char* szIp;
} RequestResult_t;

class INetworkEngine {
public:
    static INetworkEngine* Create();

    virtual void Update(float fDt) = 0;

    virtual HServerRequest RequestServers() = 0;
    virtual void StopServerRequest(HServerRequest hRequest) = 0;
    virtual std::vector<RequestResult_t> GetRequestResults(HServerRequest hRequest) = 0;
};