#include <stdio.h>
#include <Client.h>
#include <Common/PMG_Common.h>
#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
#endif
#include <string>
#include <locale>
#include <codecvt>

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
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	return std::string(buffer).substr(0, pos);
}

#include "Window.h"
#include "Renderer.h"
#include "GameObject.h"
#include "Camera.h"
#include "Particle.h"
#include "ParticleSystem.h"

#include "steam/steam_api.h"

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

    PMG::Logger::Msg("Starting Ploinky's MOBA Game...");

    PMG::Client* client = new PMG::Client();
    client->Run(connectString);

    PMG::Logger::Msg("Stopping Ploinky's MOBA Game.");

    delete client;
    client = 0;

    SteamAPI_Shutdown();

	return 0;
    /*
    */
    
    /*

    AllocConsole();
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);

    PMG::Window window = PMG::Window(
        1024, 768, PMG::WindowMode::WINDOWED
    );
    window.Show();


    PMG::Direct3D d3d = PMG::Direct3D();
    if (!d3d.Initialize(window.GetWindowHandle(), false)) {
        MessageBoxA(NULL, "fml", "fml", MB_ICONERROR);
    }

    PMG::AssetManager assetManager = PMG::AssetManager();

    PMG::Renderer renderer = PMG::Renderer();
    renderer.Initialize(&d3d, &assetManager, window.width_, window.height_);
    renderer.camera->position = { 0, 0, -5 };
    renderer.camera->rotation = { 0, 0, 0 };
    PMG::Renderer* p_renderer = &renderer;
    
    PMG::ParticleSystem particle_system = PMG::ParticleSystem();
    particle_system.Initialize(&d3d);

    PMG::GameObject go = PMG::GameObject();
    PMG::Renderable mesh = PMG::Renderable();
    // mesh.LoadTexturedMesh(assetManager.LoadFile("models\\cube_minion.p3d"), "models/cube_minion.p3d", &d3d);
    mesh.LoadTexturedMesh(assetManager.LoadFile("Models\\minion.p3d"), "models/minion", &d3d);
    go.PlayAnimation("idle", true);
    go.renderable = &mesh;

    window.e_charTyped = [](WORD ch) {};
    window.e_keyPressed = [&window, p_renderer](WORD key) {
        if (key == 'w') {
            p_renderer->camera->position.z += 1;
            return;
        }
        if (key == 's') {
            p_renderer->camera->position.z -= 1;
            return;
        }
        if (key == 'd') {
            p_renderer->camera->position.x += 1;
            return;
        }
        if (key == 'a') {
            p_renderer->camera->position.x -= 1;
            return;
        }
        if (key == 'e') {
            p_renderer->camera->rotation.y += 5;
            return;
        }
        if (key == 'q') {
            p_renderer->camera->rotation.y -= 5;
            return;
        }
        if (key == 'r') {
            p_renderer->camera->position.y += 1;
            return;
        }
        if (key == 'f') {
            p_renderer->camera->position.y -= 1;
            return;
        }
        window.SetShouldClose();
    };
    window.e_keyReleased = [](WORD key) {};
    int c = 0;
    int* p_c = &c;
    window.e_mouseButtonPressed = [p_renderer, p_c](int button) {};
    window.e_mouseButtonReleased = [](int button) {};
    window.e_mouseMoved = [](int x, int y) {};

    while (!window.ShouldClose()) {
        window.HandleEvents();

        d3d.ClearScreen();
        renderer.UpdateCameraMatrix();

        particle_system.Update(0.1);

        particle_system.Render(&renderer);
        renderer.Draw(&go);
        d3d.Present();
    }
    return 0;
    /*
    */

}