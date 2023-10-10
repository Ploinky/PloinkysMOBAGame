#pragma once

#include "steam/steam_api.h"

namespace PMG {
    class IClientState;
    class IClientStateHandler {
    public:
        virtual void NewState(IClientState* newState) = 0;

        virtual void JoinGame(servernetadr_t addr) = 0;
        virtual void JoinLobby(servernetadr_t addr) = 0;

        virtual void OpenSettingsMenu() = 0;
        virtual void OpenMainMenu() = 0;
        virtual void OpenServerBrowser() = 0;
    };
}