#include "client.h"
#include <iostream>
#include "window.h"
#include "mesh.h"
#include "vertex.h"
#include "logger.h"
#include "direct3d.h"
#include "renderer.h"
#include <directxmath.h>
#include "camera.h"
#include "map.h"
#include <sstream>
#include "util.h"
#include "navigation.h"
#include <locale>
#include <codecvt>
#include "settings.h"
#include "audio_system.h"
#include "pmg_physics.h"

namespace PMG {
    Client::Client(std::string ip_address, std::string port) {
        isRunning = false;
        lastFrame = 0;

        m_navMesh = new NavMesh();
        m_navMesh->LoadFromFile("map1");


        m_map = new Map();
        m_map->Load("map1");

        fps = 0;

        net_manager_ = new ClientNetworkManager();
        net_manager_->Initialize();

        // TODO: this does not actually work, you know?
        Logger::Msg(std::string("Connecting to server at <").append(ip_address).append(":").append(port).append(">"));

        net_manager_->ConnectToServer(ip_address, port);
    }

    Client::~Client() {
        for (Mesh* m : models) {
            delete m;
        }

        if (net_manager_->IsConnected()) {
            net_manager_->Close();
        }

        if (m_map) {
            delete m_map;
        }

        models.clear();

        if (renderer != nullptr) {
            delete renderer;
            renderer = 0;
        }

        if (direct3D != nullptr) {
            delete direct3D;
            direct3D = 0;
        }

        if (window != nullptr) {
            delete window;
            window = 0;
        }
    }

    void Client::Run() {
        Logger::Msg("Starting Ploinky's MOBA Game client...");

        std::vector<PMGSystem*> systems;

        Logger::Msg("Loading settings...");

        settings_.LoadDefaults();
        settings_.LoadFromFile("./settings.cfg");
        settings_.OnSettingChanged = [this](std::string setting) {
            HandleSettingChanged(setting);
        };

        // Create and show window
        window = new Window(
            settings_.GetInt(PMGSettings::RESOLUTION_X),
            settings_.GetInt(PMGSettings::RESOLUTION_Y),
            (WindowMode) settings_.GetInt(PMGSettings::WINDOW_MODE)
        );
        window->Show();

        // ----- Initialize Direct3D -----
        // Must be done after window is showing, otherwise swap chain creation fails
        direct3D = new Direct3D();

        // Immediately close window if initialization of Direct3D fails
        if (!direct3D->Initialize(window->GetWindowHandle(), settings_.GetInt(PMGSettings::WINDOW_MODE) == (int)WindowMode::FULLSCREEN)) {
            Logger::Err("Direct3D initialization failed, PMG will quit.");
            window->SetShouldClose();
        }

        window->windowResizedHandler = [this]() {
            if (window->ShouldClose()) {
                return;
            }

            direct3D->SetWindowDimensions(window->width, window->height);
            renderer->SetDimensions(window->width, window->height);
        };

        window->e_charTyped = [this](WORD ch) { this->CharTyped(ch); };
        window->e_keyPressed = [this](WORD key) { this->KeyPressed(key); };
        window->e_keyReleased= [this](WORD key) { this->KeyReleased(key); };
        window->e_mouseButtonPressed = [this](int button) { this->MouseButtonPressed(button); };
        window->e_mouseButtonReleased = [this](int button) { this->MouseButtonReleased(button); };
        window->e_mouseMoved = [this](int x, int y) { this->MouseMoved(x, y); };

        renderer = new Renderer();
        renderer->Initialize(direct3D, window->width, window->height);

        if (!audio_system_.Initialize()) {
            Logger::Msg("Failed to initialize audio system");
            return;
        }

        std::wstring strFileName = TEXT("audio.wav");
        AudioComponent comp;
        comp.fileName = strFileName;
        comp.isPlaying = false;

        systems.push_back(&audio_system_);

        Logger::Msg("Starting main game loop");

        // Main game loop
        // Keep running while both the client wants to keep runnning and the window has not been closed
        isRunning = true;
        lastFrame = Util::GetSystemTime();
        while(isRunning && !window->ShouldClose()) {
            auto thisFrame = Util::GetSystemTime();
            float dt = static_cast<float>(thisFrame - lastFrame);
            lastFrame = thisFrame;

            m_sceneWidth = window->width;
            m_sceneHeight = window->height;

            // Event handling
            window->HandleEvents();

            Update(dt);

            // Render scene
            BeginRender();
            // Render 3D world
            Render(renderer);
            // Render 2D graphics
            // Render UI
            // Render Menu/Chat/...
            FinishRender();

            for (PMGSystem* system : systems) {
                system->Update();
            }
        }

        // Game has endeded, close window if it isn't already closing
        if (!window->ShouldClose()) {
            window->SetShouldClose();
        }

        // Save settings to settings file
        settings_.SaveToFile("./settings.cfg");

        Logger::Msg("Game loop has been stopped.");
    }
    
    void Client::BeginRender() {
        direct3D->ClearScreen();
        renderer->UpdateCameraMatrix();
    }

    void Client::FinishRender() {
        direct3D->Present();
    }

    void Client::HandleSettingChanged(std::string setting) {
        if (setting == PMGSettings::MASTER_VOLUME) {
            audio_system_.SetMasterVolume(settings_.GetDouble(PMGSettings::MASTER_VOLUME));
        }
        else if (setting == PMGSettings::WINDOW_MODE) {
            window->SetWindowMode(static_cast<WindowMode>(settings_.GetInt(PMGSettings::WINDOW_MODE)), settings_.GetInt(PMGSettings::RESOLUTION_X), settings_.GetInt(PMGSettings::RESOLUTION_Y));
            direct3D->SetFullScreen(settings_.GetInt(PMGSettings::WINDOW_MODE) == static_cast<int>(WindowMode::FULLSCREEN));
            direct3D->SetWindowDimensions(window->width, window->height);
        }
    }

    void Client::CharTyped(uint32_t ch) {

    }

    void Client::KeyPressed(uint32_t key) {
        m_keys[key] = true;
    }

    void Client::KeyReleased(uint32_t key) {
        m_keys[key] = false;
    }

    void Client::MouseButtonPressed(int button) {
        m_mouseButtons[button] = true;
    }

    void Client::MouseButtonReleased(int button) {
        m_mouseButtons[button] = false;
    }

    void Client::MouseMoved(int screenX, int screenY) {
        m_mousePos[0] = screenX;
        m_mousePos[1] = screenY;
    }

    void Client::Update(float dt) {
        // if (!network->g_isConnected) {
        //     network->CheckConnected();
        //     return;
        // }

        int keysX = m_keys['D'] - m_keys['A'];
        int mouseX = (m_mousePos[0] >= (short)m_sceneWidth - 1) - (m_mousePos[0] == 0);
        int keysZ = m_keys['W'] - m_keys['S'];
        int mouseZ = (m_mousePos[1] == 0) - (m_mousePos[1] >= (short)m_sceneHeight - 1);

        m_camDir[0] = keysX + mouseX;
        m_camDir[1] = keysZ + mouseZ;

        if (m_keys[' ']) {
            if (!models.empty()) {
                // Snap to player
                m_camDir[0] = 0;
                m_camDir[1] = 0;
                m_camPos[0] = models.front()->position.x;
                m_camPos[2] = models.front()->position.z - 10;
            }
            else {
                m_camPos[0] = 0;
                m_camPos[2] = -10;
            }

        }

        if (m_keys[VK_ESCAPE]) {
            isRunning = false;
        }

        if (m_mouseButtons[2]) {
            m_mouseClicked[0] = m_mousePos[0];
            m_mouseClicked[1] = m_mousePos[1];
            m_mouseClicked[2] = 1;
        }

        // Network handling
        packet_t packet = {};
        while (net_manager_->ReceivePacket(&packet)) {
            HandleNetworkMessage(&packet);
        }

        HandleTicks();

        m_camPos[0] += m_camDir[0] * dt / 20;
        m_camPos[1] = 20;
        m_camPos[2] += m_camDir[1] * dt / 20;

        fps = (int)(1000.0f / dt);
    }
    
    void Client::Render(Renderer* renderer) {
        renderer->camera->position.x = m_camPos[0];
        renderer->camera->position.y = m_camPos[1];
        renderer->camera->position.z = m_camPos[2];

        if (m_mouseClicked[2] == 1) {
            renderer->FillRect(m_mouseClicked[0] - 1, m_mouseClicked[1] - 1, 3, 3, new float[3] {1.0f, 1.0f, 0.0f});

            m_mouseClicked[2] = 0;

            float x, y;
            TestIntersect(renderer, m_mouseClicked[0], m_mouseClicked[1], &x, &y);


            packet_t packet = {};
            packet.header.type = PacketType::UNITMOVE;
            packet << cmd_move_t{ x, y };

            net_manager_->SendPacket(&packet);
        }


        std::list<Mesh*> mapMeshes = m_map->GetMeshes();
        std::vector<Mesh*> mapMeshVector(mapMeshes.begin(), mapMeshes.end());
        renderer->RenderMeshes(mapMeshVector);
        renderer->RenderMeshes(models);

        std::wstring fpsText(L"FPS: ");
        fpsText.append(std::to_wstring(fps));
        renderer->RenderText(0, 0, 100, 50, fpsText);
    }

    void Client::TestIntersect(Renderer* renderer, int mx, int my, float* x, float* y) {
        float hp = static_cast<float>(M_PI / 180.0);

        vec2_t screenCoord = { static_cast<float>(mx), static_cast<float>(my) };
        vec3_t rayOrigin = renderer->camera->position;
        mat_t persp = mat_t::Perspective((float)m_sceneWidth / (float)m_sceneHeight, renderer->camera->fov * hp, renderer->camera->nearClip, renderer->camera->farClip);
        mat_t view = mat_t::Rotation(renderer->camera->rotation.z, renderer->camera->rotation.y, renderer->camera->rotation.x) *
            mat_t::Translation(rayOrigin.x, rayOrigin.y, rayOrigin.z);
        view = view.inverse().Transpose();


        vec3_t relScreen = {
            screenCoord.x * 2.0f / (float)m_sceneWidth - 1.0f,
            1.0f - (screenCoord.y * 2.0f) / (float)m_sceneHeight,
            1.0f
        };

        vec4_t rayClip = {
            relScreen.x,
            relScreen.y,
            1.0f,
            1.0f
        };

        mat_t perspInverse = persp.inverse();

        vec4_t rayEye = perspInverse * rayClip;

        rayEye = { rayEye.x, rayEye.y, 1.0, 0.0 };

        vec4_t rw4 = (view * rayEye);
        vec3_t rayWorld = { rw4.x, rw4.y, rw4.z };

        rayWorld = rayWorld.normalize();

        for (unit_t unit : units) {

            float xd = rayWorld.x;
            float yd = rayWorld.y;
            float zd = rayWorld.z;
            float xo = rayOrigin.x;
            float yo = rayOrigin.y;
            float zo = rayOrigin.z;
            float a = unit.pos.x;
            float b = unit.pos.y;
            float c = 0;
            float r = 1;
            float A = (pow(xd, 2) + pow(yd, 2) + pow(zd, 2));
            float B = (2 * (xd * (xo - a) + yd * (yo - b) + zd * (zo - c)));
            float C = (pow((xo - a), 2) + pow((yo - b), 2) + pow((zo - c), 2) - pow(r, 2));

            float t = (-B - sqrt(pow(B, 2) - 4.0f * A * C)) / 2.0f * A;

            Physics::Sphere sphere(Physics::Vector3(unit.pos.x, 0, unit.pos.y), 0.5);
            Physics::Ray ray(Physics::Vector3(rayOrigin.x, rayOrigin.y, rayOrigin.z), Physics::Vector3(rayWorld.x, rayWorld.y, rayWorld.z));
            if (Physics::TestCollision(ray, sphere)) {
                MessageBox(NULL, L"YO", L"HIT", MB_ICONINFORMATION);
                return;
            }
        }

        vec3_t planeNormal = { 0.0, 1.0, 0.0 };
        vec3_t planeOrigin = { 0.0, 0.0, 0.0 };

        float denom = planeNormal * rayWorld;

        if (fabs(denom) > 0.0001f) {
            float t = -(planeNormal * rayOrigin) / (planeNormal * rayWorld);
            *x = rayOrigin.x + rayWorld.x * t;
            *y = rayOrigin.z + rayWorld.z * t;
        }
    }

    Mesh* Client::GetModelForUnit(unsigned long unitId) {
        for (auto model = models.begin(); model != models.end(); ++model) {
            if ((*model)->unit == unitId) {
                return *model;
            }
        }

        return nullptr;
    }


    void Client::SpawnUnit(unsigned long unitId) {
        unit_t unit = { 0 };
        unit.unitId = unitId;
        units.push_back(unit);

        Mesh* model = new Mesh();
        Vertex* vert = new Vertex[4]{
            Vertex{{-0.5f, 0.1f, -0.5f}, {1.0f, 0, 0, 1}},
            Vertex{{0.5f, 0.1f, 0.5f}, {0, 0, 1.0f, 1}},
            Vertex{{0.5f, 0.1f, -0.5f}, {0, 1.0f, 0, 1}},
            Vertex{{-0.5f, 0.1f, 0.5f}, {0, 0, 1.0f, 1}},
        };
        unsigned int* indices = new unsigned int[6] {0, 1, 2, 1, 0, 3};
        model->vertices = vert;
        model->vertexCount = 4;
        model->indices = indices;
        model->indexCount = 6;
        model->unit = unitId;
        models.push_back(model);
    }

    void Client::DespawnUnit(unsigned long unitId) {
        for (auto unit = units.begin(); unit != units.end(); ++unit) {
            if (unit->unitId == unitId) {
                unit = units.erase(unit);

                if (unit == units.end()) {
                    break;
                }
            }
        }

        for (auto model = models.begin(); model != models.end(); ++model) {
            Mesh* mdl = *model;
            if (mdl->unit == unitId) {
                models.erase(model);
                break;
            }
        }
    }

    void Client::HandleTicks() {
        long long frameTime = Util::GetSystemTime();

        if (ticks.size() <= 3) {
            // We need at least 2 frames for interpolation
            return;
        }

        // local client time of first received packet
        long long startTime = ticks.front().received;

        // total time elapsed since first packet
        long long totalTime = frameTime - ticks.front().received;

        // current frame that we are rendering, including fraction
        double currentFrame = totalTime / (1000.0 / 30.0);

        // little hacky, this integer cutoff?
        int startFrame = static_cast<int>(currentFrame);
        int endFrame = static_cast<int>(startFrame) + 1;

        if (endFrame >= ticks.size()) {
            return;
        }

        game_tick_t lastTick = ticks[endFrame];
        game_tick_t nextLastTick = ticks[startFrame];

        float diff = static_cast<float>(currentFrame) - ((int)currentFrame);

        for (auto unit = units.begin(); unit != units.end(); unit++) {
            float lastX = unit->pos.x;
            float lastY = unit->pos.y;
            float lastR = unit->rot;
            float nextLastX = unit->pos.x;
            float nextLastY = unit->pos.y;
            float nextLastR = unit->rot;

            for (auto lastUnit = lastTick.units.begin(); lastUnit != lastTick.units.end(); ++lastUnit) {
                if (lastUnit->unitId == unit->unitId) {
                    lastX = lastUnit->pos.x;
                    lastY = lastUnit->pos.y;
                    lastR = lastUnit->rot;
                }
            }

            for (auto nextLastUnit = nextLastTick.units.begin(); nextLastUnit != nextLastTick.units.end(); ++nextLastUnit) {
                if (nextLastUnit->unitId == unit->unitId) {
                    nextLastX = nextLastUnit->pos.x;
                    nextLastY = nextLastUnit->pos.y;
                    nextLastR = nextLastUnit->rot;
                }
            }

            Mesh* model = GetModelForUnit(unit->unitId);

            if (model == nullptr) {
                printf("No model for unit %ld\r\n", unit->unitId);
                continue;
            }

            // Interpolate from second to last to last tick
            model->position.x = nextLastX + (lastX - nextLastX) * diff;
            model->position.z = nextLastY + (lastY - nextLastY) * diff;
            model->rotation.y = nextLastR + (lastR - nextLastR) * diff;

            unit->pos.x = lastX;
            unit->pos.y = lastY;
            unit->rot = lastR;
        }
    }

    void Client::HandleNetworkMessage(packet_t* packet) {
        if (packet->header.type == PacketType::GAME_TICK) {
            game_tick_t newTick{};

            *packet >> newTick.index;
            newTick.index = static_cast<unsigned long>(ticks.size());

            while (packet->data.size() > 0) {
                packet_t tickData{};
                *packet >> tickData;

                switch (tickData.header.type) {
                case PacketType::UNITSPAWN: {
                    pck_unit_spawn_t pck{};
                    tickData >> pck;

                    unit_t unit = { vec2_t{ pck.x, pck.y }, 0, pck.unit };
                    SpawnUnit(unit.unitId);

                    newTick.units.push_back(unit);
                    break;
                }
                case PacketType::UNITMOVE:
                case PacketType::UNITIDLE: {
                    pck_unit_move_t move{};
                    tickData >> move;
                    unit_t unit = { {move.x, move.y}, move.r, move.unit };
                    newTick.units.push_back(unit);

                    break;
                }
                case PacketType::UNITDESPAWN: {
                    unsigned long id;
                    tickData >> id;
                    DespawnUnit(id);
                    break;
                }
                }
            }
            newTick.received = Util::GetSystemTime();
            ticks.push_back(newTick);
        }
        else if (packet->header.type == PacketType::UNITSPAWN) {
            Logger::Msg("UNITSPAWN");
            unit_t unit{};
            unit.unitId = 0;
            // *packet >> unit.pos >> unit.rot >> unit.unitId;
            SpawnUnit(unit.unitId);
        }
    }
}