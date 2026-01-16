#include <chrono>
#include <Server.h>
#include "NetworkManager.h"
#include <Game.h>
#include <common/PMG_Common.h>
#include <common/util/frame-timer.h>

#include "LobbyState.h"

#include <data/server-data-loader.h>
#include "common/PloinkysJSONLibrary.h"

void Server::Start() {
#ifndef _DEBUG
    assetManager_. LoadPakFile("mnf.pak");
    assetManager_. LoadPakFile("characters/stormcaller.pak");
#endif

    {
        m_gameData = assetManager_.LoadManifest();
    }

    int nPort = DEFAULT_PORT;
    char pszName[1024];
    strcpy_s(pszName, strnlen("Default Server Name", 1024) + 1, "Default Server Name");
    {
        std::list<std::string> listContent = assetManager_.LoadPlainFile("./cfg/settings.cfg");
        for(std::string str : listContent ) {
            if(str.find("=") == std::string::npos) {
                continue;
            }

            std::string key = str.substr(0, str.find("="));
            std::string value = str.substr(str.find("=") + 1);

            if(key == "port") {
                nPort = std::stoi(value);
            }
            if(key == "name") {
                strcpy_s(pszName, strnlen(value.c_str(), 1024) + 1, value.c_str());
            }
        }
    }

#ifndef DEBUG
    // assetManager_. LoadPakFile("Maps/Map1.pak");
#endif

    currentState_ = new LobbyState(this, nPort, pszName);

    Logger::Msg("Server started");

    bool isRunning = true;

    CFrameTimer frameTimer(60);
    while (isRunning) {
        if(!frameTimer.Frame()) {
            continue;
        }

        currentState_->Update(frameTimer.GetFrameDeltaTime());
    }

    // TODO handle player connecting and disconnecting during game
    // m_networkManager->on_clientConnected = std::bind(&Server::OnClientConnected, this, std::placeholders::_1);
    // m_networkManager->on_clientDisconnected = std::bind(&Server::OnClientDisconnected, this, std::placeholders::_1);
}

long long Server::GetSystemTime() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void Server::StartGame(ServerNetworkManager* manager, LobbyPlayer* players[10]) {
    Client* client = new Client(this, manager, &assetManager_, players);
    client->Start();

    if (currentState_ != nullptr) {
        delete currentState_;
    }

    currentState_ = client;
}

void Server::StartLobby(ServerNetworkManager* manager) {
    LobbyState* lobby = new LobbyState(this, DEFAULT_PORT, (char*) "Default Server Name", manager);

    if (currentState_ != nullptr) {
        delete currentState_;
    }

    currentState_ = lobby;
}

const CGameData& Server::GetGameData() {
    return m_gameData;
}