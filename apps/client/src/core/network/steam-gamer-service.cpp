#include "steam-gamer-service.h"

#define APP_ID 1756910

#include <common/logger.h>

bool CSteamGamerService::Initialize() {
#ifndef _DEBUG
    if (SteamAPI_RestartAppIfNecessary(APP_ID)) {
        return false;
    }
#endif
    if (!SteamAPI_Init()) {
        return false;
    }

    // This is done as early as possible because it may take a while...
    SteamNetworkingSockets()->InitAuthentication();

    return true;
}

void CSteamGamerService::Shutdown() {
    SteamAPI_Shutdown();
}

void CSteamGamerService::Update() {
    SteamAPI_RunCallbacks();
}

std::string CSteamGamerService::GetPlayerName() {
    return std::string(SteamFriends()->GetPersonaName());
}


void CSteamGamerService::RequestLANServers(ServerListCallback callback) {
    Logger::FormatMsg("Requesting server list from steam");
    SteamMatchmakingServers()->RequestLANServerList(APP_ID, this);
    m_callback = callback;
}

void CSteamGamerService::CancelQuery() {

}

void CSteamGamerService::ServerResponded(HServerListRequest hRequest, int iServer) {
    gameserveritem_t* pServer = SteamMatchmakingServers()->GetServerDetails(hRequest, iServer);
    ServerInfo_t info;
    info.name = pServer->GetName();
    info.address = pServer->m_NetAdr.GetConnectionAddressString();
    info.ping = pServer->m_nPing;
    m_callback(info);
}

void CSteamGamerService::ServerFailedToRespond(HServerListRequest hRequest, int iServer) {
    Logger::FormatMsg("Server failed to respond");
}

void CSteamGamerService::RefreshComplete(HServerListRequest hRequest, EMatchMakingServerResponse response) {
    Logger::FormatMsg("Done requesting");
}