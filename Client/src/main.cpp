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

#include "window.h"
#include "direct3d.h"
#include "renderer.h"
#include "game_object.h"
#include "camera.h"
#include "mesh.h"
#include "particle.h"
#include "particle_system.h"

int CALLBACK wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow) {
    /*

    */
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

    PMG::Client* client = new PMG::Client();
    client->Run(ip_address, port);

    PMG::Logger::Msg("Stopping Ploinky's MOBA Game.");

    delete client;
    client = 0;

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

    PMG::Renderer renderer = PMG::Renderer();
    renderer.Initialize(&d3d, window.width, window.height);
    renderer.camera->position = { 0, 0, -5 };
    renderer.camera->rotation = { 0, 0, 0 };
    PMG::Renderer* p_renderer = &renderer;
    
    PMG::ParticleSystem particle_system = PMG::ParticleSystem();
    particle_system.Initialize(&d3d);

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
        d3d.Present();
    }
    return 0;

    */
}