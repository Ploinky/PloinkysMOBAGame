// Main entry point for Windows systems

#include <stdio.h>
#include "client.hpp"
#include "logger.hpp"
#include <Windows.h>
#include <string>
#include <dxgidebug.h>

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

    printf("ARGS:\r\n");
    for(int i = 0; i < argc; i++) {
        printf("\t%d: %ls\r\n", i, argv[i]);
    }

    SetCurrentDirectoryA(GetDir().c_str());
    
    // AllocConsole();

    // freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    // freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
    
    P3D::Logger::Msg("Starting Ploinky's MOBA Game...");

    P3D::Client* client = new P3D::Client();
    client->Run();

    P3D::Logger::Msg("Stopping Ploinky's MOBA Game.");

    delete client;
    client = 0;

    return 0;
}