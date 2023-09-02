// Main entry point for Windows systems

#include <stdio.h>
#include "client.h"
#include "logger.h"
#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
#endif
#include <string>
#include "util.h"

std::string GetDir() {
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	return std::string(buffer).substr(0, pos);
}

int CALLBACK wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow) {
    // Attempt to read command line arguments
    int argc;
    wchar_t** argv = CommandLineToArgvW(pCmdLine, &argc);
    bool showConsole = false;

    std::string ip_address = "127.0.0.1";
    std::string port = std::to_string(DEFAULT_PORT);

    for (int i = 0; i < argc; i++) {
        if (lstrcmpW(argv[i], L"-console") == 0) {
            showConsole = true;
        }
        
        if (lstrcmpW(argv[i], L"-host") == 0) {
            if (i + 1 < argc) {
                i += 1;
                ip_address = PMG::Util::wstring_to_string(std::wstring(argv[i]));
            }
            continue;
        }

        if (lstrcmpW(argv[i], L"-port") == 0) {
            if (i + 1 < argc) {
                i += 1;
                port = PMG::Util::wstring_to_string(std::wstring(argv[i]));
            }
            continue;
        }
    }

    if(showConsole) {
        AllocConsole();
        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
        freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
    }

    //SetCurrentDirectoryA(GetDir().c_str());
    
    PMG::Logger::Msg("Starting Ploinky's MOBA Game...");

    PMG::Client* client = new PMG::Client(ip_address, port);
    client->Run();

    PMG::Logger::Msg("Stopping Ploinky's MOBA Game.");

    delete client;
    client = 0;

    return 0;
}