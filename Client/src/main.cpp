// Main entry point for Windows systems

#include <stdio.h>
#include "client.h"
#include "logger.h"
#include <Windows.h>
#include <string>
#include <dxgidebug.h>
#include <locale>
#include <codecvt>

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
    std::wstring ip;

    for (int i = 0; i < argc; i++) {
        if (lstrcmpW(argv[i], L"-console") == 0) {
            showConsole = true;
        }
        if (lstrcmpW(argv[i], L"-connect") == 0) {
            ip = std::wstring(argv[++i]);
        }
    }

    if(showConsole) {
        AllocConsole();
        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
        freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
    }

    //SetCurrentDirectoryA(GetDir().c_str());
    
    PMG::Logger::Msg("Starting Ploinky's MOBA Game...");

    PMG::Client* client = new PMG::Client(ip);
    client->Run();

    PMG::Logger::Msg("Stopping Ploinky's MOBA Game.");

    delete client;
    client = 0;

    return 0;
}