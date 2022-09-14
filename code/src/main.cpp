// Main entry point for Windows systems

#include <stdio.h>
#include "client.hpp"
#include "logger.hpp"
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
    AllocConsole();
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);

    // Attempt to read command line arguments
    int argc;
    wchar_t** argv = CommandLineToArgvW(pCmdLine, &argc);
    std::wstring ipw = L"";

    printf("--- ARGS:\r\n");
    for(int i = 0; i < argc; i++) {
        printf("%d: %ls\r\n", i, argv[i]);
        if (lstrcmpW(argv[i], L"-connect") == 0) {
            i++;
            printf("\t%ls\r\n", argv[i]);
            ipw = std::wstring(argv[i]);
        }
    }

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::string ip = converter.to_bytes(ipw);
    SetCurrentDirectoryA(GetDir().c_str());
    
    P3D::Logger::Msg("Starting Ploinky's MOBA Game...");

    P3D::Client* client = new P3D::Client(ip);
    client->Run();

    P3D::Logger::Msg("Stopping Ploinky's MOBA Game.");

    delete client;
    client = 0;

    return 0;
}