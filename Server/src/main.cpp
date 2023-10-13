#include <iostream>
#include "server.h"
#include "logger.h"
#include "steam/steam_gameserver.h"
#include "steam/isteamgameserver.h"

void CaptainHook(int, const char*) {
    PMG::Logger::Msg("HOOKED!");
}

int main(int argc, char** argv) {
    PMG::Logger::Msg("Starting Ploinky's MOBA Game Server...");

    if (!SteamGameServer_Init(0, 23119, 27016, EServerMode::eServerModeAuthenticationAndSecure, "0.0.0.1")) {
        PMG::Logger::Err("Failed to initialize steam gameserver");
        return 1;
    }
    SteamGameServerUtils()->SetWarningMessageHook(&CaptainHook);

    SteamGameServer()->SetServerName("R U SERIOUS");
    SteamGameServer()->SetProduct("1756910");
    SteamGameServer()->SetGameDescription("A description of this game");
    SteamGameServer()->SetModDir("1756910");
    SteamGameServer()->SetRegion("");
    SteamGameServer()->SetAdvertiseServerActive(true);
    SteamGameServer()->SetDedicatedServer(true);

    SteamGameServer()->LogOnAnonymous();

    PMG::Server* server = new PMG::Server();
    server->Start();

    SteamGameServer_Shutdown();

    return 0;
}