#include "client.hpp"
#include <iostream>
#include "network_connection.hpp"
#include "window.hpp"
#include "mesh.hpp"
#include "vertex.hpp"
#include "Shared/code/logger.hpp"
#include "direct3d.hpp"
#include "renderer.hpp"
#include <directxmath.h>
#include "camera.hpp"
#include <chrono>
#include "map.hpp"
#include <sstream>
#include "Shared/code/util.hpp"
#include "Shared/code/navigation.hpp"

namespace PMG {
    Client::Client(std::string ip) {
        if (ip.empty()) {
            this->ip = "127.0.0.1";
        } else {
            this->ip = ip;
        }

        m_navMesh = new NavMesh();
        m_navMesh->LoadFromFile("map1");
    }

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
    }

    void Client::Run() {
        Logger::Msg("Starting Ploinky's MOBA Game client...");

        network = new NetworkConnection();
        network->Connect(ip);

        Map* map = new Map();
        map->Load("map1");

        // Create and show window
        window = new Window();
        window->Show();
        
        // ----- Initialize Direct3D -----
        // Must be done after window is showing, otherwise swap chain creation fails
        direct3D = new Direct3D();

        // Immediately close window if initialization of Direct3D fails
        if (!direct3D->Initialize(window->GetWindowHandle())) {
            Logger::Err("Direct3D initialization failed, PMG will quit.");
            window->SetShouldClose();
        }

        window->windowResizedHandler = [this]() {
            direct3D->SetWindowDimensions(window->width, window->height);
            renderer->SetAspectRatio((float) window->width / (float) window->height);
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

            HandleTicks(thisFrame);

            // Event handling
            window->HandleEvents();

            // Game logic
            HandlePlayerInput(nullptr, dt);

            // Render scene
            BeginRender();

            std::list<Mesh*> mapMeshes = map->GetMeshes();
            std::vector<Mesh*> mapMeshVector(mapMeshes.begin(), mapMeshes.end());
            renderer->RenderMeshes(mapMeshVector);
            renderer->RenderMeshes(models);

            std::wstring fpsText(L"FPS: ");
            fpsText.append(std::to_wstring((int) (1000.0f / (dt * 1000.0f))));

            direct3D->RenderText(0, 0, fpsText);

            FinishRender();
        }

        Logger::Msg("Game loop has been stopped.");
    }

    void Client::HandlePlayerInput(Mesh* model, float dt) {
        int keysX = window->IsKeyDown('D') - window->IsKeyDown('A');
        int mouseX = (window->GetMouseX() == (short) window->width - 1) - (window->GetMouseX() == 0);
        int keysZ = window->IsKeyDown('W') - window->IsKeyDown('S');
        int mouseZ = (window->GetMouseY() == 0) - (window->GetMouseY() == (short) window->height - 1);
        
        DirectX::XMFLOAT3 move = DirectX::XMFLOAT3(
            keysX + mouseX,
            0,
            keysZ + mouseZ);
        
        renderer->camera->position.x += move.x * 10 * dt;
        renderer->camera->position.y += move.y * 10 * dt;
        renderer->camera->position.z += move.z * 10 * dt;

        if(window->IsKeyDown(VK_ESCAPE)) {
            isRunning = false;
        }

        if(window->IsKeyDown(VK_SPACE)) {
            if (!models.empty()) {
                // Snap to player
                renderer->camera->position.x = models.front()->position.x;
                renderer->camera->position.z = models.front()->position.z - 10;
            }
            else {
                renderer->camera->position.x = 0;
                renderer->camera->position.z = -10;
            }

        }

        if (window->IsButtonDown(2)) {
            float x, y;
            renderer->TestIntersect(window->GetMouseX(), window->GetMouseY(), &x, &y);
            std::cout << "PRESSED! " << x << "-" << y << std::endl;
            std::string msg = std::string()
                .append("MoveCommand|")
                .append(std::to_string(x))
                .append(";")
                .append(std::to_string(y));
            network->WriteMessage(msg);
        }
    }
    
    void Client::BeginRender() {
        direct3D->ClearScreen();
        renderer->UpdateCameraMatrix();
    }

    void Client::FinishRender() {
        direct3D->Present();
    }

    long long Client::GetSystemTime() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    void Client::HandleNetworkMessage(std::string msg) {
        std::list<std::string> tokens = Util::SplitString(msg, std::string("|"));

        OutputDebugString(msg.c_str());
        OutputDebugString("\r\n");

        if(tokens.front() == "Tick") {
            tokens.pop_front();

            std::string tickIndex = tokens.front();
            tokens.pop_front();

            game_tick_t newTick = { 0 };
            newTick.index = stoi(tickIndex);

            while(!tokens.empty()) {
                std::string tokenData = tokens.front();
                tokens.pop_front();

                std::list<std::string> subTokens = Util::SplitString(tokenData, std::string(";"));
                if(subTokens.front() == "UnitSpawn") {
                    subTokens.pop_front();
                    
                    unit_t unit = { 0 };

                    unsigned long id = std::stoi(subTokens.front());
                    subTokens.pop_front();

                    float x = std::stof(subTokens.front());
                    subTokens.pop_front();

                    float y = std::stof(subTokens.front());
                    subTokens.pop_front();

                    SpawnUnit(id);

                    unit.unitId = id;
                    unit.x = x;
                    unit.y = y;

                    newTick.units.push_back(unit);
                } else if (subTokens.front() == "UnitMove" || subTokens.front() == "UnitIdle") {
                    subTokens.pop_front();
                    
                    unit_t unit = { 0 };

                    unsigned long id = std::stoi(subTokens.front());
                    subTokens.pop_front();

                    float x = std::stof(subTokens.front());
                    subTokens.pop_front();

                    float y = std::stof(subTokens.front());
                    subTokens.pop_front();

                    unit.unitId = id;
                    unit.x = x;
                    unit.y = y;

                    newTick.units.push_back(unit);
                } else if (subTokens.front() == "UnitDespawn") {
                    subTokens.pop_front();

                    unsigned long id = std::stoi(subTokens.front());
                    subTokens.pop_front();

                    DespawnUnit(id);
                }
            }
            newTick.received = GetSystemTime();
            ticks.push_back(newTick);
        } else if (tokens.front() == "SpawnUnits") {
            tokens.pop_front();

            while (!tokens.empty()) {
                std::string tokenData = tokens.front();
                tokens.pop_front();

                std::list<std::string> subTokens = Util::SplitString(tokenData, std::string(";"));

                unit_t unit = { 0 };

                unsigned long id = std::stoi(subTokens.front());
                subTokens.pop_front();

                float x = std::stof(subTokens.front());
                subTokens.pop_front();

                float y = std::stof(subTokens.front());
                subTokens.pop_front();

                SpawnUnit(id);

                unit.unitId = id;
                unit.x = x;
                unit.y = y;
            }
        }
    }

    void Client::HandleTicks(long long frameTime) {
        if(ticks.size() <= 3) {
            // We need at least 2 frames for interpolation
            return;
        }

        game_tick_t lastTick = *std::prev(std::prev(ticks.end()));
        game_tick_t nextLastTick = *std::prev(std::prev(std::prev(ticks.end())));

        float time = (frameTime - lastTick.received) / 1000000.0f / 1000.0f;
        float diff = time / (16.66f / 1000.0f);

        for(auto unit = units.begin(); unit != units.end(); unit++) {
            float lastX = unit->x;
            float lastY = unit->y;
            float nextLastX = unit->x;
            float nextLastY = unit->y;

            for(auto lastUnit = lastTick.units.begin(); lastUnit != lastTick.units.end(); ++lastUnit) {
                if(lastUnit->unitId == unit->unitId) {
                    lastX = lastUnit->x;
                    lastY = lastUnit->y;
                }
            }

            for(auto nextLastUnit = nextLastTick.units.begin(); nextLastUnit != nextLastTick.units.end(); ++nextLastUnit) {
                if(nextLastUnit->unitId == unit->unitId) {
                    nextLastX = nextLastUnit->x;
                    nextLastY = nextLastUnit->y;
                }
            }

            Mesh* model = GetModelForUnit(unit->unitId);

            if(model == nullptr) {
                printf("No model for unit %ld\r\n", unit->unitId);
                continue;
            }

            // Interpolate from second to last to last tick
            model->position.x = nextLastX + (lastX - nextLastX) * diff;
            model->position.z = nextLastY + (lastY - nextLastY) * diff;
        }

    }

    void Client::SpawnUnit(unsigned long unitId) {
        unit_t unit = { 0 };
        unit.unitId = unitId;
        units.push_back(unit);

        Mesh* model = new Mesh();
        Vertex* vert = new Vertex[4]{
            Vertex{{-0.5f, 1, -0.5f}, {1.0f, 0, 0, 1}},
            Vertex{{0.5f, 1, 0.5f}, {0, 0, 1.0f, 1}},
            Vertex{{0.5f, 1, -0.5f}, {0, 1.0f, 0, 1}},
            Vertex{{-0.5f, 1, 0.5f}, {0, 0, 1.0f, 1}},
        };
        unsigned int* indices = new unsigned int[6]{0, 1, 2, 1, 0, 3};
        model->vertices = vert;
        model->vertexCount = 4;
        model->indices = indices;
        model->indexCount = 6;
        model->unit = unitId;
        models.push_back(model);
        printf("Spawned model for unit %ld\r\n", unitId);
    }

    Mesh* Client::GetModelForUnit(unsigned long unitId) {
        for(auto model = models.begin(); model != models.end(); ++model) {
            if((*model)->unit == unitId) {
                return *model;
            }
        }

        return nullptr;
    }

    void Client::DespawnUnit(unsigned long unitId) {
        for(auto unit = units.begin(); unit != units.end(); ++unit) {
            if(unit->unitId == unitId) {
                unit = units.erase(unit);

                if (unit == units.end()) {
                    break;
                }
            }
        }

        for(auto model = models.begin(); model != models.end(); ++model) {
            Mesh* mdl = *model;
            if(mdl->unit == unitId) {
                models.erase(model);
                break;
            }
        }

        printf("Despawned model for unit %ld\r\n", unitId);
    }
}