#include <stdio.h>
#include <Client.h>
#include <common/PMG_Common.h>
#include <string>
#include <locale>
#include <codecvt>
#include <steam/steam_api.h>
#include <core/window/Window.h>
#include "Renderer.h"
#include "GameObject.h"
#include "Camera.h"
#include "Particle.h"
#include "ParticleEffect.h"
#include <core/platform/platform.h>

#include <DirectXMath.h>

// Main entry point into the application
int main(int argc, char* argv[]) {
#ifndef _DEBUG
    if (SteamAPI_RestartAppIfNecessary(1756910)) {
        CPlatform::Alert("Error", "Fatal Error - Steam must be running to play this game (SteamAPI_Init() failed).\n", EAlertType::ERR);
        return 1;
    }
#endif

    if (!SteamAPI_Init()) {
        CPlatform::Alert("Error", "Fatal Error - Steam must be running to play this game (SteamAPI_Init() failed).\n", EAlertType::ERR);
        return 1;
    }

    if (!CPlatform::Initialize()) {
        CPlatform::Alert("Error", "Fatal Error - failed to initialize platform\n", EAlertType::ERR);
        return 1;
    }

    // This is done as early as possible because it may take a while...
    SteamNetworkingSockets()->InitAuthentication();

    // Attempt to read command line arguments
    bool showConsole = false;
    std::string connectString;

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-console") == 0) {
            showConsole = true;
        }
        else if (strcmp(argv[i], "-connect") == 0) {
            if (argc < i + 1) {
                continue;
            }

            connectString.append(argv[i + 1]);
        }
    }

    if(showConsole) {
        CPlatform::ShowConsole();
    }

    Logger::Msg("Starting Ploinky's MOBA Game...");

    Client* client = nullptr;
    #ifdef _DEBUG
        client = new Client();
        client->Run(connectString);
#else
    try {
        client = new Client();
        client->Run(connectString);
    }
    catch (std::exception  e) {
        std::string msg = "An error occured and the application is quitting:\r\n";
        msg.append(e.what());
        CPlatform::Alert("Fatal Error", msg.c_str(), EAlertType::ERR);
    }
#endif

    SteamAPI_Shutdown();
    Logger::Msg("Stopping Ploinky's MOBA Game.");

    delete client;
    client = 0;

    return 0;
}