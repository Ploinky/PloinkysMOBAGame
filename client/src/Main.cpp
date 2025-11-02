#include <stdio.h>
#include <Client.h>
#include <common/PMG_Common.h>
// #ifndef WIN32_LEAN_AND_MEAN
//     #define WIN32_LEAN_AND_MEAN
//     #include <Windows.h>
// #endif
#include <string>
#include <locale>
#include <codecvt>
#include <steam/steam_api.h>

std::string FromWStringToString(std::wstring stringToConvert) {
    //setup converter
    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;

    //use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
    return converter.to_bytes(stringToConvert);
}


std::string GetDir() {
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("//");
	return std::string(buffer).substr(0, pos);
}

#include "Window.h"
#include "Renderer.h"
#include "GameObject.h"
#include "Camera.h"
#include "Particle.h"
#include "ParticleEffect.h"

// Main entry point into the application
int CALLBACK wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow) {
#ifndef _DEBUG
    if (SteamAPI_RestartAppIfNecessary(1756910)) {
        MessageBoxA(nullptr, "Fatal Error - Steam must be running to play this game (SteamAPI_Init() failed).\n", "Error", MB_ICONERROR);
        return 1;
    }
#endif

     if (!SteamAPI_Init())
    {
        MessageBoxA(nullptr, "Fatal Error - Steam must be running to play this game (SteamAPI_Init() failed).\n", "Error", MB_ICONERROR);
        return 1;
    }

    // Initialize COM?
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        MessageBoxA(nullptr, "Fatal Error - failed to initialize COM\n", "Error", MB_ICONERROR);
        return 1;
    }

    // This is done as early as possible because it may take a while...
    SteamNetworkingSockets()->InitAuthentication();

    // Attempt to read command line arguments
    int argc;
    wchar_t** argv = CommandLineToArgvW(pCmdLine, &argc);
    bool showConsole = false;
    std::string connectString;

    for (int i = 0; i < argc; i++) {
        if (lstrcmpW(argv[i], L"-console") == 0) {
            showConsole = true;
        }
        else if (lstrcmpW(argv[i], L"-connect") == 0) {
            if (argc < i + 1) {
                continue;
            }

            connectString.append(FromWStringToString(std::wstring(argv[i + 1])).c_str());
        }
    }

    if(showConsole) {
        AllocConsole();
        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
        freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
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
        MessageBox(NULL, msg.c_str(), "Fatal Error", MB_ICONERROR);
    }
#endif

    SteamAPI_Shutdown();
    Logger::Msg("Stopping Ploinky's MOBA Game.");

    delete client;
    client = 0;

    return 0;
}