#include "gamer-service.h"

#include "steam-gamer-service.h"

std::unique_ptr<IGamerService> IGamerService::Create() {
    return std::make_unique<CSteamGamerService>();
}
