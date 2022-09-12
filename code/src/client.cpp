#include "client.hpp"
#include <iostream>
#include "window.hpp"
#include "mesh.hpp"
#include "vertex.hpp"
#include "logger.hpp"
#include "direct3d.hpp"
#include "renderer.hpp"
#include <directxmath.h>
#include "camera.hpp"
#include "keyboard_input.hpp"
#include "mouse_input.hpp"
#include <chrono>
#include "map.hpp"
#include "network_connection.hpp"
#include <sstream>
#include "util.hpp"

namespace P3D {
    Client::~Client() {
        for(Mesh* m : models) {
            delete m;
        }

        models.clear();

        delete renderer;
        renderer = 0;

        delete direct3D;
        direct3D = 0;

        delete window;
        window = 0;

        delete keyboardInput;
        keyboardInput = 0;
    }

    void Client::Run() {
        Logger::Msg("Starting Ploinky's MOBA Game client...");

        network = new NetworkConnection();
        network->Connect();

        Map* map = new Map();
        map->Load("./data/maps/map1/map1.omp");


        Mesh* model = new Mesh();
        Vertex vert[] = {
            Vertex{{-0.5f, 1, -0.5f}, {1.0f, 0, 0, 1}},
            Vertex{{0.5f, 1, 0.5f}, {0, 0, 1.0f, 1}},
            Vertex{{0.5f, 1, -0.5f}, {0, 1.0f, 0, 1}},
            Vertex{{-0.5f, 1, 0.5f}, {0, 0, 1.0f, 1}},
        };
        unsigned int indices[] = {0, 1, 2, 1, 0, 3};
        model->vertices = vert;
        model->vertexCount = 4;
        model->indices = indices;
        model->indexCount = 6;
        models.push_back(model);

        // Create and show window
        window = new Window();
        window->Show();
        
        // ----- Initialize Direct3D -----
        // Must be done after window is showing, otherwise swap chain creation fails
        direct3D = new Direct3D();

        // Immediately close window if initialization of Direct3D fails
        if (!direct3D->Initialize(window->GetWindowHandle())) {
            Logger::Err("Direct3D initialization failed, P3D will quit.");
            window->SetShouldClose();
        }

        keyboardInput = new KeyboardInput();
        m_mouseInput = new MouseInput();

        window->windowResizedHandler = [this]() {
            direct3D->SetWindowDimensions(window->width, window->height);
            renderer->SetAspectRatio((float) window->width / (float) window->height);
        };

        window->keyHandler = [this](long key, bool down) {
            keyboardInput->SetKeyDown((char) key, down);
        };

        window->mouseHandler = [this](short x, short y) {
            m_mouseInput->SetMousePosition(x, y);
        };

        renderer = new Renderer();
        renderer->Initialize(direct3D, window->width, window->height);
        
        Logger::Msg("Starting main game loop");

        // Main game loop
        // Keep running while both the client wants to keep runnning and the window has not been closed
        isRunning = true;
        lastFrame = GetSystemTime();
        while(isRunning && !window->ShouldClose()) {
            auto thisFrame = GetSystemTime();
            float dt = (thisFrame - lastFrame) / 1000000.0f / 1000.0f;
            lastFrame = thisFrame;

            // Network handling
            network->ReceiveMessages();
            
            while(network->HasMessage()) {
                std::string msg = network->NextMessage();

                HandleNetworkMessage(msg);
            }

            HandleTicks();

            // Event handling
            window->HandleEvents();

            // Game logic
            HandlePlayerInput(model, dt);

            // Render scene
            BeginRender();

            Render(map->GetMesh());

            for(Mesh* m : models) {
                Render(m);
            }

            FinishRender();
        }

        Logger::Msg("Game loop has been stopped.");
    }

    void Client::HandlePlayerInput(Mesh* model, float dt) {
        
        int keysX = keyboardInput->IsKeyDown('D') - keyboardInput->IsKeyDown('A');
        int mouseX = (m_mouseInput->GetMouseX() == (short) window->width - 1) - (m_mouseInput->GetMouseX() == 0);
        int keysZ = keyboardInput->IsKeyDown('W') - keyboardInput->IsKeyDown('S');
        int mouseZ = (m_mouseInput->GetMouseY() == 0) - (m_mouseInput->GetMouseY() == (short) window->height - 1);
        
        DirectX::XMFLOAT3 move = DirectX::XMFLOAT3(
            keysX + mouseX,
            0,
            keysZ + mouseZ);
        
        renderer->camera->position.x += move.x * 10 * dt;
        renderer->camera->position.y += move.y * 10 * dt;
        renderer->camera->position.z += move.z * 10 * dt;

        if(keyboardInput->IsKeyDown(VK_ESCAPE)) {
            isRunning = false;
        }

        if(keyboardInput->IsKeyDown(VK_SPACE)) {
            // Snap to player
            renderer->camera->position.x = model->position.x;
            renderer->camera->position.z = model->position.z - 5;
        }

        if(keyboardInput->IsKeyDown('H')) {
            std::string msg = std::string()
                .append("MoveCommand|")
                .append(std::to_string(-3))
                .append(";")
                .append(std::to_string(-3));
            network->WriteMessage(msg);
        }
    }
    
    void Client::BeginRender() {
        direct3D->ClearScreen();
        renderer->UpdateCameraMatrix();
    }

    void Client::Render(Mesh* model) {
        renderer->Render(model);
    }

    void Client::FinishRender() {
        direct3D->Present();
    }

    long long Client::GetSystemTime() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    void Client::HandleNetworkMessage(std::string msg) {
        std::list<std::string> tokens = Util::SplitString(msg, std::string("|"));

        if(tokens.front() == "Tick") {
            tokens.pop_front();
            while(!tokens.empty()) {
                std::string tickIndex = tokens.front();
                tokens.pop_front();

                game_tick_t newTick = { 0 };
                newTick.index = stoi(tickIndex);


                std::string playerData = tokens.front();
                tokens.pop_front();

                std::list<std::string> pdTokens = Util::SplitString(playerData, std::string(";"));
                if(pdTokens.front() == "Player1") {
                    pdTokens.pop_front();

                    float x = std::stof(pdTokens.front());
                    pdTokens.pop_front();

                    float y = std::stof(pdTokens.front());
                    pdTokens.pop_front();

                    newTick.x = x;
                    newTick.y = y;
                }

                newTick.received = GetSystemTime();
                ticks.push_back(newTick);
            }
        }
    }

    void Client::HandleTicks() {
        game_tick_t lastTick = ticks.back();
        game_tick_t nextLastTick = *std::prev(ticks.end());

        // See how much time passed since last tick
        float diff = (GetSystemTime() - lastTick.received) / 1000000.0f / 1000.0f;

        // Interpolate from second to last to last tick
        models.front()->position.x = nextLastTick.x + (lastTick.x - nextLastTick.x) / diff;
        models.front()->position.z = nextLastTick.y + (lastTick.y - nextLastTick.y) / diff;
    }
}