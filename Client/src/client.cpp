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
#include "settings.h"
#include "audio_system.h"
#include "red_box.h"

namespace PMG {
    Client::Client(std::string ip_address, std::string port) {
        isRunning = false;
        lastFrame = 0;

        m_navMesh = new NavMesh();
        m_navMesh->LoadFromFile("map1");


        m_map = new Map();
        m_map->Load("map1");

        fps = 0;

        net_manager_ = ClientNetworkManager();
        net_manager_.Initialize();

        // TODO: this does not actually work, you know?
        Logger::Msg(std::string("Connecting to server at <").append(ip_address).append(":").append(port).append(">"));

        net_manager_.ConnectToServer(ip_address, port);
    }

    Client::~Client() {
        for (auto go : game_objects_) {
            delete go.second;
        }

        if (net_manager_.IsConnected()) {
            net_manager_.Close();
        }

        if (m_map) {
            delete m_map;
        }

        game_objects_.clear();

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
            Render();
            RenderGameUI();
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
        if (!net_manager_.IsConnected()) {
            net_manager_.CheckConnected();
            return;
        }

        int keysX = m_keys['D'] - m_keys['A'];
        int mouseX = (m_mousePos[0] >= (short)m_sceneWidth - 1) - (m_mousePos[0] == 0);
        int keysZ = m_keys['W'] - m_keys['S'];
        int mouseZ = (m_mousePos[1] == 0) - (m_mousePos[1] >= (short)m_sceneHeight - 1);

        m_camDir[0] = keysX + mouseX;
        m_camDir[1] = keysZ + mouseZ;

        if (m_keys[' ']) {
            if (unit_id_received_) {
                // Snap to player
                Mesh* my_unit = GetModelForUnit(my_unit_id_);

                if (my_unit != nullptr) {
                    m_camDir[0] = 0;
                    m_camDir[1] = 0;
                    m_camPos[0] = my_unit->position.x;
                    m_camPos[1] = 20;
                    m_camPos[2] = my_unit->position.z - 10;
                }
            }
            else {
                m_camPos[0] = 0;
                m_camPos[2] = -10;
            }

        }
        else {
            m_camPos[0] += m_camDir[0] * dt / 20;
            m_camPos[2] += m_camDir[1] * dt / 20;
        }

        if (m_keys['s']) {
            packet_t packet = {};
            packet.header.type = PacketType::CMD_STOP;

            net_manager_.SendPacket(&packet);
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
        while (net_manager_.ReceivePacket(&packet)) {
            HandleNetworkMessage(&packet);
        }

        HandleTicks(dt);

        fps = (int)(1000.0f / dt);
    }
    
    void Client::Render() {
        // ===== Loading Screen =====
        if (!net_manager_.IsConnected()) {
            renderer->RenderText(0, 0, 100, 100, L"Connecting");
            return;
        }

        // Game Screen
        renderer->camera->position.x = m_camPos[0];
        renderer->camera->position.y = m_camPos[1];
        renderer->camera->position.z = m_camPos[2];

        float hp = static_cast<float>(M_PI / 180.0);
        Physics::Ray ray = Physics::ScreenToRay({ static_cast<float>(m_mousePos[0]), static_cast<float>(m_mousePos[1]) },
            { renderer->camera->position.x, renderer->camera->position.y, renderer->camera->position.z },
            { renderer->camera->rotation.x, renderer->camera->rotation.y, renderer->camera->rotation.z },
            (float)m_sceneWidth / (float)m_sceneHeight,
            renderer->camera->fov* hp,
            renderer->camera->nearClip,
            renderer->camera->farClip,
            m_sceneWidth,
            m_sceneHeight);
        
        bool pointing_at_unit = FALSE;
        for (auto &go_it : game_objects_) {
            GameObject* go = go_it.second;
            Physics::Sphere sphere(Physics::Vector3(go->position.x, 0, go->position.z), 0.5);
            if (Physics::TestCollision(ray, sphere)) {
                SetCursor(LoadCursor(NULL, IDC_HAND));
                pointing_at_unit = true;

                if (m_mouseButtons[2]) {
                    packet_t packet = {};
                    packet.header.type = PacketType::CMD_ATTACK;
                    packet << cmd_attack_t{ go->unit_id };

                    net_manager_.SendPacket(&packet);
                }
            }
        }

        if (!pointing_at_unit && m_mouseButtons[2] && m_mouseClicked[2] == 1) {
            renderer->FillRect(m_mouseClicked[0] - 1, m_mouseClicked[1] - 1, 3, 3, new float[3] {1.0f, 1.0f, 0.0f});

            m_mouseClicked[2] = 0;

            float x, y;
            TestIntersect(renderer, m_mouseClicked[0], m_mouseClicked[1], &x, &y);


            packet_t packet = {};
            packet.header.type = PacketType::UNITMOVE;
            packet << cmd_move_t{ x, y };

            net_manager_.SendPacket(&packet);
        }

        std::list<Mesh*> mapMeshes = m_map->GetMeshes();
        std::vector<Mesh*> mapMeshVector(mapMeshes.begin(), mapMeshes.end());
        renderer->RenderMeshes(mapMeshVector);

        for (auto go : game_objects_) {
            renderer->RenderMeshes({ go.second->mesh });
        }
    }

    void Client::RenderGameUI() {
        for (auto go_it : game_objects_) {
            GameObject* go = go_it.second;

            if (!go->has_healthbar) {
                continue;
            }

            Mesh* my_model = go->mesh;

            Physics::Vector3 point_above = Physics::Vector3{ 0, -4.5, 0 };

            float hp = static_cast<float>(M_PI / 180.0);
            Physics::mat_t persp = Physics::mat_t::Perspective((float)m_sceneWidth / (float)m_sceneHeight, renderer->camera->fov * hp, renderer->camera->nearClip, renderer->camera->farClip);

            Physics::mat_t view = Physics::mat_t::Rotation(-renderer->camera->rotation.z, -renderer->camera->rotation.y, -renderer->camera->rotation.x) * Physics::mat_t::Translation(renderer->camera->position.x, renderer->camera->position.y, renderer->camera->position.z);

            Physics::mat_t transMat = Physics::mat_t::Translation(my_model->position.x, my_model->position.y, my_model->position.z);

            Physics::Vector2 screen_point_above = Physics::WorldToScreen(point_above, transMat.inverse(), persp, view);

            double x = (1.0f + screen_point_above.x) * 0.5 * m_sceneWidth;
            double y = (1.0f - screen_point_above.y) * 0.5 * m_sceneHeight;
            renderer->RenderText(x - 50, y - 50, 100, 50, L"Ploinky");
            renderer->FillRect(x - 71, y - 12, 20, 20, new float[3] { 0.0f, 0.0f, 0.0f });
            renderer->RenderText(x - 71, y - 12, 20, 20, L"1");
            renderer->FillRect(x - 50, y - 10, 100, 15, new float[3] { 0.0f, 0, 0 });
            renderer->FillRect(x - 50, y - 10, go->health, 15, new float[3] { 0.0f, 1.0f, 0 });
            renderer->DrawRect(x - 51, y - 11, 102, 17, new float[3] { 0.0f, 0.0f, 0 });
        }

        std::wstring fpsText(L"FPS: ");
        fpsText.append(std::to_wstring(fps));
        renderer->RenderText(0, 0, 100, 50, fpsText);

        int done_ticks = current_tick_;
        int done_seconds = current_tick_ / 30.0;
        int done_minutes = done_seconds / 60;
        done_seconds = done_seconds % 60;

        std::wstring time = L"";

        if (done_minutes < 10) {
            time.append(L"0");
        }

        time.append(std::to_wstring(done_minutes)).append(L":");

        if (done_seconds < 10) {
            time.append(L"0");
        }

        time.append(std::to_wstring(done_seconds));

        float black[3]{ 0, 0, 0 };
        renderer->FillRect(m_sceneWidth / 2 - 50, 10, 100, 20, black);
        renderer->RenderText(m_sceneWidth / 2 - 50, 10, 100, 20, time);

        if (!unit_id_received_) {
            return;
        }

        auto test = game_objects_.find(my_unit_id_);
        if (test == game_objects_.end()) {
            return;
        }
        GameObject* my_go = test->second;

        int y = m_sceneHeight - 50;
        int x = m_sceneWidth / 2 - 200;

        int percentage_health = (float) my_go->health / (float) my_go->max_health * 400.0f;

        renderer->FillRect(x - 1, y - 1, 402, 27, black);

        float green[3]{ 0, 0.5, 0 };
        renderer->FillRect(x, y, percentage_health, 25, green);

        renderer->RenderText(x, y, 400, 25, std::to_wstring(my_go->health).append(L"/").append(std::to_wstring(my_go->max_health)).c_str());
    }

    void Client::TestIntersect(Renderer* renderer, int mx, int my, float* x, float* y) {
        float hp = static_cast<float>(M_PI / 180.0);

        Physics::Vector2 screenCoord = { static_cast<float>(mx), static_cast<float>(my) };
        Physics::Vector3 rayOrigin = renderer->camera->position;
        Physics::mat_t persp = Physics::mat_t::Perspective((float)m_sceneWidth / (float)m_sceneHeight, renderer->camera->fov * hp, renderer->camera->nearClip, renderer->camera->farClip);
        Physics::mat_t view = Physics::mat_t::Rotation(renderer->camera->rotation.z, renderer->camera->rotation.y, renderer->camera->rotation.x) *
            Physics::mat_t::Translation(rayOrigin.x, rayOrigin.y, rayOrigin.z);


        Physics::Vector3 relScreen = {
            screenCoord.x * 2.0f / (float)m_sceneWidth - 1.0f,
            1.0f - (screenCoord.y * 2.0f) / (float)m_sceneHeight,
            1.0f
        };

        Physics::Vector4 rayClip = {
            relScreen.x,
            relScreen.y,
            1.0f,
            1.0f
        };

        Physics::mat_t perspInverse = persp.inverse();

        Physics::Vector4 rayEye = perspInverse * rayClip;

        rayEye = { rayEye.x, rayEye.y, 1.0, 0.0 };

        Physics::Vector4 rw4 = (view * rayEye);
        Physics::Vector3 rayWorld = { rw4.x, rw4.y, rw4.z };

        rayWorld = rayWorld.Normalize();

        Physics::Vector3 planeNormal = { 0.0, 1.0, 0.0 };
        Physics::Vector3 planeOrigin = { 0.0, 0.0, 0.0 };

        float denom = planeNormal * rayWorld;

        if (fabs(denom) > 0.0001f) {
            float t = -(planeNormal * rayOrigin) / (planeNormal * rayWorld);
            *x = rayOrigin.x + rayWorld.x * t;
            *y = rayOrigin.z + rayWorld.z * t;
        }
    }

    Mesh* Client::GetModelForUnit(unsigned long unitId) {
        auto test = game_objects_.find(unitId);
        if (test == game_objects_.end()) {
            return nullptr;
        }
        GameObject* go = test->second;
        return go->mesh;
    }

    void Client::SpawnUnit(unsigned long unitId) {
        SpawnUnit(unitId, 0, Team::TEAM_1, Physics::Vector2( 0, 0 ));
    }

    void Client::SpawnUnit(unsigned long unitId, unsigned long unit_type, Team team, Physics::Vector2 pos) {
        // Hacky missile hack
        if (unit_type == 1) {
            Mesh* model = new Mesh();
            color_shader_vertex_t* vert = new color_shader_vertex_t[8]{
                color_shader_vertex_t{{-0.1f, 1.0f, -0.1f}, {0, 0, 1.0f, 1}},
                color_shader_vertex_t{{0.1f, 1.0f, 0.1f}, {0, 0, 1.0f, 1}},
                color_shader_vertex_t{{0.1f, 1.0f, -0.1f}, {0, 0, 1.0f, 1}},
                color_shader_vertex_t{{-0.1f, 1.0f, 0.1f}, {0, 0, 1.0f, 1}},
            };

            unsigned int* indices = new unsigned int[6] {0, 1, 2, 1, 0, 3};
            model->vertices = vert;
            model->vertexCount = 8;
            model->indices = indices;
            model->indexCount = 6;
            model->unit = unitId;
            model->position = { pos.x, 0, pos.y };

            GameObject* go = new GameObject();
            go->unit_id = unitId;
            go->health = 50;
            go->max_health = 100;
            go->mesh = model;
            go->position = { pos.x, 0, pos.y };
            go->rotation = { 0, 0, 0 };
            go->has_healthbar = false;
            go->team = team;
            game_objects_.emplace(unitId, go);
        }

        if (team == Team::TEAM_1) {
            GameObject* red_box = new RedBox();
            red_box->unit_id = unitId;
            red_box->position = { pos.x, 0, pos.y };
            red_box->mesh->position = { pos.x, 0, pos.y };
            red_box->team;
            game_objects_.emplace(unitId, red_box);
            return;
        }

        Mesh* model = new Mesh();
        color_shader_vertex_t* vert = new color_shader_vertex_t[8]{
            color_shader_vertex_t{{-0.5f, 2.0f, -0.5f}, {1.0f, 0, 0, 1}},
            color_shader_vertex_t{{0.5f, 2.0f, 0.5f}, {0, 0, 1.0f, 1}},
            color_shader_vertex_t{{0.5f, 2.0f, -0.5f}, {0, 1.0f, 0, 1}},
            color_shader_vertex_t{{-0.5f, 2.0f, 0.5f}, {0, 0, 1.0f, 1}},
            color_shader_vertex_t{{-0.5f, 0.2f, -0.5f}, {0, 0, 0.0f, 1}},
            color_shader_vertex_t{{0.5f, 0.2f, 0.5f}, {0, 0, 0.0f, 1}},
            color_shader_vertex_t{{0.5f, 0.2f, -0.5f}, {0, 0, 0.0f, 1}},
            color_shader_vertex_t{{-0.5f, 0.2f, 0.5f}, {0, 0, 0.0f, 1}},
        };
        unsigned int* indices = new unsigned int[12] {0, 1, 2, 1, 0, 3, 4, 5, 6, 5, 4, 7};
        model->vertices = vert;
        model->vertexCount = 8;
        model->indices = indices;
        model->indexCount = 12;
        model->unit = unitId;
        model->position = { pos.x, 0, pos.y };

        GameObject* go = new GameObject();
        go->unit_id = unitId;
        go->health = 50;
        go->max_health = 100;
        go->mesh = model;
        go->position = { pos.x, 0, pos.y };
        go->rotation = { 0, 0, 0 };
        go->team = team;
        game_objects_.emplace(unitId, go);
    }

    void Client::DespawnUnit(unsigned long unitId) {
        auto test = game_objects_.find(unitId);
        if (test == game_objects_.end()) {
            return;
        }
        GameObject* go = test->second;
        game_objects_.erase(unitId);
        delete go;
    }

    void Client::HandleTicks(float dt) {
        long long frameTime = Util::GetSystemTime();
        
        unsigned long long sim_time = frameTime - 100;

        int start_tick = ticks.size() - 1;
        for (; start_tick >= 0; start_tick--) {
            if (ticks[start_tick].received <= sim_time) {
                start_tick += 1;
                break;
            }
        }

        if (start_tick < 0 || start_tick > ticks.size()) {
            // mhm...
            return;
        }

        while (start_tick >= current_tick_ + 2) {
            // uhm? we're missing some ticks?
            game_tick_t tick = ticks[current_tick_];

            SimulateTick(tick, 1);

            current_tick_++;
        }

        if (start_tick >= ticks.size()) {
            start_tick -= 1;
        }

        int current_tick_index = ticks.size() - 1;
        game_tick_t current_tick = ticks[current_tick_index];

        double frame_dt = 100.0 - (frameTime - current_tick.received);
        
        while (frame_dt > (1000.0 / 30.0)) {
            frame_dt -= (1000.0 / 30.0);
            current_tick_index--;
        }

        double remaining = (1000.0 / 30.0)  - ((1000.0 / 30.0) - frame_dt);
        double diff = dt / (double)remaining;
        if (diff > 1) {
            // this we can do better?!
            diff = 1;
        }
        current_tick = ticks[current_tick_index];

        game_tick_t to_tick = current_tick;

        SimulateTick(to_tick, diff);
    }

    void Client::SimulateTick(game_tick_t& tick, double diff) {
        packet_t packet_copy = tick.packet;
        packet_t* packet = &packet_copy;

        unsigned long tick_index;
        *packet >> tick_index;
        current_tick_ = tick_index;

        while (packet->data.size() > 0) {
            packet_t tickData{};
            *packet >> tickData;

            switch (tickData.header.type) {
            case PacketType::UNITSPAWN: {
                pck_unit_spawn_t spawn{};
                tickData >> spawn;

                SpawnUnit(spawn.unit, spawn.unit_type, spawn.team, Physics::Vector2{ spawn.x, spawn.y });
                break;
            }
            case PacketType::UNITMOVE:
            case PacketType::UNITIDLE: {
                pck_unit_move_t move{};
                tickData >> move;

                GameObject* go = GetGameObject(move.unit);
                go->position.x = move.x;
                go->position.z = move.y;
                go->rotation.y = move.r;

                Mesh* model = GetModelForUnit(go->unit_id);

                if (model == nullptr) {
                    printf("No model for unit %ld\r\n", go->unit_id);
                    continue;
                }

                model->position.x = model->position.x + (go->position.x - model->position.x) * diff;
                model->position.z = model->position.z + (go->position.z - model->position.z) * diff;
                model->rotation.y = model->rotation.y + (go->rotation.y - model->rotation.y) * diff;

                break;
            }
            case PacketType::UNITDESPAWN: {
                UnitId id;
                tickData >> id;
                DespawnUnit(id);
                break;
            }
            case PacketType::PCK_STATS: {
                pck_unit_stats_t stats{};
                tickData >> stats;

                GameObject* go = GetGameObject(stats.unit);

                if (go == nullptr) {
                    Logger::Msg("WARNING: received stats message for unknown object");
                    break;
                }

                go->health = stats.health;
                go->max_health = stats.max_health;
                break;
            }
            }
        }
    }

    void Client::HandleNetworkMessage(packet_t* packet) {
        if (packet->header.type == PacketType::GAME_TICK) {
            game_tick_t newTick{};
            newTick.packet = *packet;
            newTick.received = Util::GetSystemTime();
            ticks.push_back(newTick);
        }
        else if (packet->header.type == PacketType::UNITSPAWN) {
            Logger::Msg("UNITSPAWN");

            pck_unit_spawn_t spawn{};
            *packet >> spawn;

            SpawnUnit(spawn.unit, spawn.unit_type, spawn.team, Physics::Vector2{ spawn.x, spawn.y });
        }
        else if (packet->header.type == PacketType::UNITDESPAWN) {
            Logger::Msg("UNITDESPAWN");
            pck_unit_despawn_t pck{};
            *packet >> pck;
            DespawnUnit(pck.unit);
        }
        else if (packet->header.type == PacketType::PCK_CLIENT_UNIT_ID) {
            Logger::Msg("CLIENT_UNIT_ID");

            pck_client_unit_id_t unit_id{};
            *packet >> unit_id;

            my_unit_id_ = unit_id.unit;
            unit_id_received_ = TRUE;
        } else if (packet->header.type == PacketType::UNITMOVE || packet->header.type == PacketType::UNITIDLE) {
            pck_unit_move_t move{};
            *packet >> move;

            GameObject* go = GetGameObject(move.unit);

            if (go == nullptr) {
                Logger::Msg("WARNING: received move/idle message for unknown object");
                return;
            }
            go->position.x = move.x;
            go->position.z = move.y;
            go->rotation.y = move.r;
            go->position_received = Util::GetSystemTime();;
        }
        else if (packet->header.type == PacketType::PCK_STATS) {
            pck_unit_stats_t stats{};
            *packet >> stats;

            GameObject* go = GetGameObject(stats.unit);

            if (go == nullptr) {
                Logger::Msg("WARNING: received stats message for unknown object");
                return;
            }
            
            go->health = stats.health;
            go->max_health = stats.max_health;
        }
    }

    GameObject* Client::GetGameObject(UnitId unit_id) {
        auto it = game_objects_.find(unit_id);

        if (it == game_objects_.end()) {
            return nullptr;
        }

        return it->second;
    }
}