#include "Game.h"
#include <Common/PMG_Common.h>
#include "Renderer.h"
#include "Camera.h"
#include "ParticleSystem.h"
#include "MainMenu.h"
#include "../Resources/resource.h"

namespace PMG {
    Game::Game(ServerNetworkManager* server, IClientStateHandler* handler, int width, int height) : IClientState(handler, width, height) {
        m_navMesh = new NavMesh();
        m_navMesh->LoadFromData(handler->GetAssetManager()->LoadPlainFile("Maps/map1\\map1.nvm"));

        m_navGrid = new NavigationCellGrid(m_navMesh);

        net_manager_ = server;

        packet_manager = Networking::NetworkHandlerManager<Networking::PacketType, std::function<void(std::vector<uint8_t>)>>();
        // Register network packets, the fuck...
        packet_manager.RegisterHandler(Networking::PacketType::PCK_CLIENT_UNIT_ID, [this](std::vector<uint8_t> data) { HandleUnitIdPacket(data); });
        packet_manager.RegisterHandler(Networking::PacketType::GAME_TICK, [this](std::vector<uint8_t> data) { HandleGameTickPacket(data); });
        packet_manager.RegisterHandler(Networking::PacketType::UNITSPAWN, [this](std::vector<uint8_t> data) { HandleUnitSpawnPacket(data); });
        packet_manager.RegisterHandler(Networking::PacketType::UNITMOVE, [this](std::vector<uint8_t> data) { HandleUnitMovePacket(data); });
        packet_manager.RegisterHandler(Networking::PacketType::UNITIDLE, [this](std::vector<uint8_t> data) { HandleUnitIdlePacket(data); });
        net_manager_->Initialize(&packet_manager);

        this->direct3D = &handler->GetRenderer()->m_d3d;
        this->assetManager_ = handler->GetAssetManager();
        this->renderer = handler->GetRenderer();

        SteamFriends()->SetRichPresence("steam_display", "#Status_OnCommunityServer");
    }

    Game::~Game() {
        net_manager_->Close();

        for (auto go : game_objects_) {
            delete go.second;
        }

        if (net_manager_->IsConnected()) {
            net_manager_->Close();
        }

        game_objects_.clear();
    }

    void Game::CharTyped(uint32_t ch) {

    }

    void Game::KeyPressed(uint32_t key) {
        m_keys[key] = true;
    }

    void Game::KeyReleased(uint32_t key) {
        m_keys[key] = false;
    }

    void Game::MouseButtonPressed(int button) {
        m_mouseButtons[button] = true;
    }

    void Game::MouseButtonReleased(int button) {
        m_mouseButtons[button] = false;
    }

    void Game::MouseMoved(int screenX, int screenY) {
        m_mousePos[0] = screenX;
        m_mousePos[1] = screenY;
    }

    void Game::Update(float dt) {
        if (!net_manager_->IsConnected()) {
            net_manager_->CheckConnected();
            return;
        }

        int keysX = m_keys[VK_RIGHT] - m_keys[VK_LEFT];
        int mouseX = (m_mousePos[0] >= (short)windowWidth_ - 1) - (m_mousePos[0] == 0);
        int keysZ = m_keys[VK_DOWN] - m_keys[VK_UP];
        int mouseZ = (m_mousePos[1] >= (short)windowHeight_ - 1) - (m_mousePos[1] == 0);

        m_camDir[0] = keysX + mouseX;
        m_camDir[1] = keysZ + mouseZ;

        if (m_mouseButtons[2]) {
            m_mouseClicked[0] = m_mousePos[0];
            m_mouseClicked[1] = m_mousePos[1];
            m_mouseClicked[2] = 1;
        }

        if (m_keys['s']) {
            Networking::StopCommandPacket stop = Networking::StopCommandPacket();

            net_manager_->SendPacket(&stop);
        }

        if (m_keys['q']) {
            float x, y;
            TestIntersect(renderer, m_mousePos[0], m_mousePos[1], &x, &y);

            Networking::CastCommandPacket cast = Networking::CastCommandPacket();
            cast.spell_slot = 0;
            cast.x = x;
            cast.y = 0;
            cast.z = y;
            net_manager_->SendPacket(&cast);
        }

        if (m_keys['w']) {
            float hp = static_cast<float>(M_PI / 180.0);
            Physics::Ray ray = Physics::ScreenToRay({ static_cast<float>(m_mousePos[0]), static_cast<float>(m_mousePos[1]) },
                { renderer->m_camera.position.x, renderer->m_camera.position.y, renderer->m_camera.position.z },
                { renderer->m_camera.rotation.x, renderer->m_camera.rotation.y, renderer->m_camera.rotation.z },
                (float)windowWidth_ / (float)windowHeight_,
                renderer->m_camera.fov * hp,
                renderer->m_camera.nearClip,
                renderer->m_camera.farClip,
                windowWidth_,
                windowHeight_);

            for (auto& go_it : game_objects_) {
                GameObject* go = go_it.second;
                Physics::Sphere sphere(Physics::Vector3(go->position.x, 0, go->position.z), 0.5);
                if (Physics::TestCollision(ray, sphere)) {
                    Networking::CastTargetCommandPacket cmd = Networking::CastTargetCommandPacket();
                    cmd.spell_slot = 1;
                    cmd.target = go->unit_id;
                    net_manager_->SendPacket(&cmd);
                }
            }
        }

        if (m_keys['e']) {
            float x, y;
            TestIntersect(renderer, m_mousePos[0], m_mousePos[1], &x, &y);

            Networking::CastCommandPacket cast = Networking::CastCommandPacket();
            cast.spell_slot = 2;
            cast.x = x;
            cast.y = 0;
            cast.z = y;
            net_manager_->SendPacket(&cast);
        }

        if (m_keys['r']) {
            float hp = static_cast<float>(M_PI / 180.0);
            Physics::Ray ray = Physics::ScreenToRay({ static_cast<float>(m_mousePos[0]), static_cast<float>(m_mousePos[1]) },
                { renderer->m_camera.position.x, renderer->m_camera.position.y, renderer->m_camera.position.z },
                { renderer->m_camera.rotation.x, renderer->m_camera.rotation.y, renderer->m_camera.rotation.z },
                (float)windowWidth_ / (float)windowHeight_,
                renderer->m_camera.fov * hp,
                renderer->m_camera.nearClip,
                renderer->m_camera.farClip,
                windowWidth_,
                windowHeight_);

            for (auto& go_it : game_objects_) {
                GameObject* go = go_it.second;
                Physics::Sphere sphere(Physics::Vector3(go->position.x, 0, go->position.z), 0.5);
                if (Physics::TestCollision(ray, sphere)) {
                    Networking::CastTargetCommandPacket cmd = Networking::CastTargetCommandPacket();
                    cmd.spell_slot = 3;
                    cmd.target = go->unit_id;
                    net_manager_->SendPacket(&cmd);
                }
            }
        }

        last_move = max(0, last_move - dt);

        if (!m_mouseButtons[2]) {
            last_move = 0;
        }

        if (last_move == 0 && m_mouseButtons[2] && m_mouseClicked[2] == 1) {
            float hp = static_cast<float>(M_PI / 180.0);
            Physics::Ray ray = Physics::ScreenToRay({ static_cast<float>(m_mousePos[0]), static_cast<float>(m_mousePos[1]) },
                { renderer->m_camera.position.x, renderer->m_camera.position.y, renderer->m_camera.position.z },
                { renderer->m_camera.rotation.x, renderer->m_camera.rotation.y, renderer->m_camera.rotation.z },
                (float)windowWidth_ / (float)windowHeight_,
                renderer->m_camera.fov * hp,
                renderer->m_camera.nearClip,
                renderer->m_camera.farClip,
                windowWidth_,
                windowHeight_);

            bool pointing_at_unit = FALSE;
            for (auto& go_it : game_objects_) {
                GameObject* go = go_it.second;
                Physics::Sphere sphere(Physics::Vector3(go->position.x, 0, go->position.z), 0.5);
                if (Physics::TestCollision(ray, sphere) && go->has_healthbar) {
                    SetCursor(LoadCursor(NULL, IDC_HAND));
                    pointing_at_unit = true;
                    last_move = 150;

                    if (m_mouseButtons[2]) {
                        Networking::AttackCommandPacket atk_pk = Networking::AttackCommandPacket();
                        atk_pk.target_unit = go->unit_id;

                        net_manager_->SendPacket(&atk_pk);
                    }
                }
            }

            if (!pointing_at_unit) {
                float x, y;
                TestIntersect(renderer, m_mouseClicked[0], m_mouseClicked[1], &x, &y);

                Networking::MoveCommandPacket mv = Networking::MoveCommandPacket();
                mv.x = x;
                mv.y = y;
                net_manager_->SendPacket(&mv);

                ParticleSystem* particle_system = ParticleSystem::Load("UI/MoveTo\\move_to.pts", assetManager_);
                particle_system->Initialize(direct3D);
                particle_system->position = { x, 0, y };

                game_objects_.emplace(Util::GetSystemTime(), particle_system);

                last_move = 150;
            }
        }

        if (m_keys[VK_ESCAPE]) {
            handler_->OpenMainMenu();
            return;
        }

        // Network handling
        while (net_manager_->ReceivePacket()) {
        }

        HandleTicks(dt);

        for (auto go_it : game_objects_) {
            go_it.second->Update(dt);
        }

        std::erase_if(game_objects_, [](auto kv) {
            if (kv.second->destroy) {
                delete kv.second;
                return true;
            }

            return false;
            });

        if (m_keys[' ']) {
            if (unit_id_received_) {
                // Snap to player
                GameObject* my_unit = GetGameObject(my_unit_id_);

                if (my_unit != nullptr) {
                    m_camDir[0] = 0;
                    m_camDir[1] = 0;
                    m_camPos[0] = my_unit->position.x;
                    m_camPos[1] = 1500;
                    m_camPos[2] = my_unit->position.z + 800;
                }
            }
            else {
                m_camPos[0] = 0;
                m_camPos[2] = -1000;
            }

        }
        else {
            m_camPos[0] += m_camDir[0] * dt / 0.2;
            m_camPos[2] += m_camDir[1] * dt / 0.2;
        }
    }

    void Game::Render(CRenderer* renderer) {
        // ===== Loading Screen =====
        if (!net_manager_->IsConnected()) {
            renderer->RenderText(0, 0, 100, 100, "Connecting");
            return;
        }

        // Game Screen
        renderer->m_camera.position.x = m_camPos[0];
        renderer->m_camera.position.y = m_camPos[1];
        renderer->m_camera.position.z = m_camPos[2];

		renderer->DrawMap();

        for (auto go_it : game_objects_) {
            GameObject* go = go_it.second;

			renderer->Draw(go);
        }




        // ==========================================================================
#ifdef _DEBUG123
        
        m_navGrid->Reset();

        for (int i = 0; i < m_navGrid->CellCountX * m_navGrid->CellCountY; i++) {
            NavigationCell* cell = m_navGrid->Cells[i];
            cell->IsOpen = true;
        }

        for (auto go_it : game_objects_) {
            if (go_it.second->has_healthbar) {

                NavigationCell* cell = m_navGrid->GetCellAt(go_it.second->position.x * 100, go_it.second->position.z * 100);
                if (cell) {
                    cell->IsOpen = false;
                }
            }
        }

        GameObject* gameObject = new GameObject();
        
        Renderable* renderable = new Renderable();

        VertexBuffer* vertexBuffer = new VertexBuffer();
        vertexBuffer->stride = sizeof(color_shader_vertex_t);
        vertexBuffer->offset = 0;
        color_shader_vertex_t* vertices = new color_shader_vertex_t[4]
        {
            {{0.5, 0.1, 0.5}, {1, 0, 0, 1}},
            {{0.5, 0.1, 0}, {1, 0, 0, 1}},
            {{0, 0.1, 0}, {1, 0, 0, 1}},
            {{0, 0.1, 0.5}, {1, 0, 0, 1}},
        };

        vertexBuffer->buffer = direct3D->CreateVertexBuffer(vertices, 4, sizeof(texture_shader_vertex_t) * 4);
        VertexBuffer** vertexBuffers = new VertexBuffer * [1];
        vertexBuffers[0] = vertexBuffer;

        delete[] vertices;

        unsigned int* indices = new unsigned int[6] {0, 1, 2, 2, 3, 0};

        IndexBuffer* indexBuffer = new IndexBuffer();
        indexBuffer->indexCount = 6;

        
        indexBuffer->buffer = direct3D->CreateIndexBuffer(indices, 6);
        delete[] indices;
        renderable->SetColoredMeshData(vertexBuffers, indexBuffer);
        renderable->shaderType = ShaderType::COLOR;
        gameObject->renderable = renderable;

        for (int gridX = 0; gridX < m_navGrid->CellCountX; gridX++) {
            for (int gridZ = 0; gridZ < m_navGrid->CellCountY; gridZ++) {
                if (m_navGrid->GetCellAt(gridX * 50, gridZ * 50) && (!m_navGrid->GetCellAt(gridX * 50, gridZ * 50)->IsOpen)) { //(!m_navGrid->GetCellAt(gridX * 50, gridZ * 50)->IsWalkable || !m_navGrid->GetCellAt(gridX * 50, gridZ * 50)->IsOpen)) {
                    NavigationCell* cell = m_navGrid->GetCellAt(gridX * 50, gridZ * 50);
                    gameObject->position.x = cell->X / 100;
                    gameObject->position.z = cell->Y / 100;
                    gameObject->Render(renderer);
                }
            }
        }

        delete renderable;
        delete gameObject;

#endif
        // ==========================================================================


        RenderGameUI(renderer);
    }

    void Game::RenderGameUI(CRenderer* renderer) {
        for (auto go_it : game_objects_) {
            GameObject* go = go_it.second;

            if (!go->has_healthbar) {
                continue;
            }

            Physics::Vector3 point_above = Physics::Vector3{ 0, -450, 0 };

            float hp = static_cast<float>(M_PI / 180.0);
            Physics::mat_t persp = Physics::mat_t::Perspective((float)windowWidth_ / (float)windowHeight_, renderer->m_camera.fov * hp, renderer->m_camera.nearClip, renderer->m_camera.farClip);

            Physics::mat_t view = Physics::mat_t::Rotation(-renderer->m_camera.rotation.z, -renderer->m_camera.rotation.y, -renderer->m_camera.rotation.x) * Physics::mat_t::Translation(renderer->m_camera.position.x, renderer->m_camera.position.y, renderer->m_camera.position.z);

            Physics::mat_t transMat = Physics::mat_t::Translation(go->position.x, go->position.y, go->position.z);

            Physics::Vector2 screen_point_above = Physics::WorldToScreen(point_above, transMat.inverse(), persp, view);

            double x = (1.0f + screen_point_above.x) * 0.5 * windowWidth_;
            double y = (1.0f - screen_point_above.y) * 0.5 * windowHeight_;
            double health_bar_height = 5;

            if (go->has_title) {
                renderer->RenderText(x - 50, y - 50, 100, 50, "Ploinky");
                renderer->FillRect(x - 71, y - 12, 20, 20, new float[3] { 0.0f, 0.0f, 0.0f });
                renderer->RenderText(x - 71, y - 12, 20, 20, "1");
                renderer->FillRect(x - 50, y - 10, 100, 15, new float[3] { 0.0f, 0, 0 });
                health_bar_height = 15;
            }

            float color[3]{ 0, 1, 0 };

            if (go->team == Team::TEAM_2) {
                color[0] = 1;
                color[1] = 0;
                color[2] = 0;
            }


            /*
            int fracs = go->health / 10;
            int last = go->health % 10;

            for (int i = 0; i < fracs; i++) {
                renderer->FillRect(x - 50.0 + (go->health / fracs) * i, y - 10, go->health / fracs - 1, health_bar_height, color);
            }

            if (last > 0) {
                renderer->FillRect(x - 50.0 + go->health - last, y - 10, last, health_bar_height, color);
            }
            */

            renderer->FillRect(x - 50, y - 10, ((float)go->health / (float)go->max_health) * 100.0f, health_bar_height, color);
            renderer->DrawRect(x - 51, y - 11, 102, health_bar_height + 2, new float[3] { 0.0f, 0.0f, 0 });
        }

#ifdef _DEBUG
        std::string go_text("game_objects: ");
        go_text.append(std::to_string(game_objects_.size()));
        renderer->RenderText(0, 20, 150, 20, go_text);
#endif

        int done_ticks = current_tick_;
        int done_seconds = current_tick_ / 60.0;
        int done_minutes = done_seconds / 60;
        done_seconds = done_seconds % 60;

        std::string time = "";

        if (done_minutes < 10) {
            time.append("0");
        }

        time.append(std::to_string(done_minutes)).append(":");

        if (done_seconds < 10) {
            time.append("0");
        }

        time.append(std::to_string(done_seconds));

        float black[3]{ 0, 0, 0 };
        renderer->FillRect(windowWidth_ / 2 - 50, 10, 100, 20, black);
        renderer->RenderText(windowWidth_ / 2 - 50, 10, 100, 20, time);

        if (!unit_id_received_) {
            return;
        }

        auto test = game_objects_.find(my_unit_id_);
        if (test == game_objects_.end()) {
            return;
        }
        GameObject* my_go = test->second;

        int y = windowHeight_ - 50;
        int x = windowWidth_ / 2 - 200;

        int percentage_health = (float)my_go->health / (float)my_go->max_health * 400.0f;

        renderer->FillRect(x - 1, y - 1, 402, 27, black);

        float green[3]{ 0, 0.5, 0 };
        renderer->FillRect(x, y, percentage_health, 25, green);

        renderer->RenderText(x, y, 400, 25, std::to_string(my_go->health).append("/").append(std::to_string(my_go->max_health)).c_str());

        float gray[3]{ 0.5, 0.5, 0.5 };
        // Ability icons ?!
        y = windowHeight_ - 110;
        x = windowWidth_ / 2 - 115;

        renderer->DrawRect(x, y, 50, 50, black);
        renderer->FillRect(x + 1, y + 1, 48, 48, gray);
        if (cooldowns[0] != -1) {
            float cd_remaining = (float)cooldowns[0] / (float)total_cooldowns[0];
			renderer->RenderPartialCover(x + 1, y + 1, 48, 48, cd_remaining);
        }
        renderer->RenderText(x, y, 50, 50, "Q");

        x = windowWidth_ / 2 - 55;
        renderer->DrawRect(x, y, 50, 50, black);
        renderer->FillRect(x + 1, y + 1, 48, 48, gray);
        if (cooldowns[1] != -1) {
            float cd_remaining = (float)cooldowns[1] / (float)total_cooldowns[1];
			renderer->RenderPartialCover(x + 1, y + 1, 48, 48, cd_remaining);
        }
        renderer->RenderText(x, y, 50, 50, "W");

        x = windowWidth_ / 2 + 5;
        renderer->DrawRect(x, y, 50, 50, black);
        renderer->FillRect(x + 1, y + 1, 48, 48, gray);
        if (cooldowns[2] != -1) {
            float cd_remaining = (float)cooldowns[2] / (float)total_cooldowns[2];
			renderer->RenderPartialCover(x + 1, y + 1, 48, 48, cd_remaining);
        }
        renderer->RenderText(x, y, 50, 50, "E");

        x = windowWidth_ / 2 + 65;
        renderer->DrawRect(x, y, 50, 50, black);
        renderer->FillRect(x + 1, y + 1, 48, 48, gray);
        if (cooldowns[3] != -1) {
            float cd_remaining = (float)cooldowns[3] / (float)total_cooldowns[3];
			renderer->RenderPartialCover(x + 1, y + 1, 48, 48, cd_remaining);
        }
        renderer->RenderText(x, y, 50, 50, "R");

        float hp = static_cast<float>(M_PI / 180.0);
        Physics::Ray ray = Physics::ScreenToRay({ static_cast<float>(m_mousePos[0]), static_cast<float>(m_mousePos[1]) },
            { renderer->m_camera.position.x, renderer->m_camera.position.y, renderer->m_camera.position.z },
            { renderer->m_camera.rotation.x, renderer->m_camera.rotation.y, renderer->m_camera.rotation.z },
            (float)windowWidth_ / (float)windowHeight_,
            renderer->m_camera.fov * hp,
            renderer->m_camera.nearClip,
            renderer->m_camera.farClip,
            windowWidth_,
            windowHeight_);


        for (auto& go_it : game_objects_) {
            GameObject* go = go_it.second;
            Physics::Sphere sphere(Physics::Vector3(go->position.x, 0, go->position.z), 50);
            if (Physics::TestCollision(ray, sphere) && go->has_healthbar && go->team != my_go->team) {
                handler_->RequestCursor(CursorId::ATTACK_MOVE);
                break;
            }
        }
    }

    void Game::TestIntersect(CRenderer* renderer, int mx, int my, float* x, float* y) {
        float hp = static_cast<float>(M_PI / 180.0);

        Physics::Vector2 screenCoord = { static_cast<float>(mx), static_cast<float>(my) };
        Physics::Vector3 rayOrigin = renderer->m_camera.position;
        Physics::mat_t persp = Physics::mat_t::Perspective((float)windowWidth_ / (float)windowHeight_, renderer->m_camera.fov * hp, renderer->m_camera.nearClip, renderer->m_camera.farClip);
        Physics::mat_t view = Physics::mat_t::Rotation(renderer->m_camera.rotation.z, renderer->m_camera.rotation.y, renderer->m_camera.rotation.x) *
            Physics::mat_t::Translation(rayOrigin.x, rayOrigin.y, rayOrigin.z);


        Physics::Vector3 relScreen = {
            screenCoord.x * 2.0f / (float)windowWidth_ - 1.0f,
            1.0f - (screenCoord.y * 2.0f) / (float)windowHeight_,
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

        rayEye = { rayEye.x, rayEye.y, -1.0, 0.0 };

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

    void Game::SpawnUnit(uint64_t unitId) {
        SpawnUnit(unitId, 0, Team::TEAM_1, Physics::Vector3(0, 0, 0));
    }

    void Game::SpawnUnit(uint64_t unitId, uint64_t unit_type, Team team, Physics::Vector3 pos) {
        if (game_objects_.find(unitId) != game_objects_.end()) {
            // already spawned!
            return;
        }
        // Hacky missile hack
        if (unit_type == UnitPrefab::THROW_FOOTBALL) {
            GameObject* go = new GameObject();
            go->renderable = "missile";
            go->unit_id = unitId;
            go->health = 50;
            go->max_health = 100;
            go->position = pos;
            go->rotation = { 0, 0, 0 };
            go->has_healthbar = false;
            go->team = team;
            game_objects_.emplace(unitId, go);
            return;
        }

        if (unit_type == UnitPrefab::FOOTBALL_PERSON) {
            GameObject* go = new GameObject();
            go->renderable = "football_person";
            go->unit_id = unitId;
            go->health = 50;
            go->max_health = 100;
            go->position = pos;
            go->rotation = { 0, 0, 0 };
            go->team = team;
            game_objects_.emplace(unitId, go);
            return;
        }

        if (unit_type == UnitPrefab::TOWER) {
            GameObject* go = new GameObject();
            go->renderable = "tower";
            go->unit_id = unitId;
            go->health = 50;
            go->max_health = 100;
            go->position = pos;
            go->rotation = { 0, 0, 0 };
            go->has_healthbar = true;
            go->has_title = false;
            go->team = team;
            game_objects_.emplace(unitId, go);
            return;
        }

        if (unit_type == UnitPrefab::GENERIC_EMPTY) {
            GameObject* go = new GameObject();
            go->unit_id = unitId;
            go->health = 50;
            go->max_health = 100;
            go->position = pos;
            go->rotation = { 0, 0, 0 };
            go->has_healthbar = false;
            go->has_title = false;
            go->team = team;
            game_objects_.emplace(unitId, go);
            return;
        }

        if (unit_type == UnitPrefab::MINION) {
            GameObject* go = new GameObject();
            go->renderable = "minion";
            go->unit_id = unitId;
            go->health = 10;
            go->max_health = 10;
            go->position = pos;
            go->rotation = { 0, 0, 0 };
            go->has_healthbar = true;
            go->has_title = false;
            go->team = team;
            game_objects_.emplace(unitId, go);
            return;
        }

        Logger::Err("Received spawn message for prefab that does not exist");
    }

    void Game::DespawnUnit(uint64_t unitId) {
        GameObject* go = GetGameObject(unitId);

        if (go == nullptr) {
            return;
        }

        go->destroy = true;
    }

    void Game::HandleTicks(float dt) {
        long long frameTime = Util::GetSystemTime();

        unsigned long long sim_time = frameTime - (1000.0 / 60.0);

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

        float frame_dt = (1000.0 / 60.0) + 20 - (frameTime - current_tick.received);

        while (frame_dt > (1000.0 / 60.0)) {
            frame_dt -= (1000.0 / 60.0);
            current_tick_index--;
        }

        float remaining = (1000.0 / 60.0) - ((1000.0 / 60.0) - frame_dt);
        float diff = dt / (double)remaining;
        if (diff > 1) {
            // this we can do better?!
            diff = 1;
        }
        current_tick = ticks[current_tick_index];

        game_tick_t to_tick = current_tick;

        SimulateTick(to_tick, diff);
    }

    void Game::SimulateTick(game_tick_t& tick, double diff) {
        std::vector<uint8_t> new_data;

        // ignore tick packet header pls
        int offset = sizeof(Networking::packet_header_t);

        while (offset < tick.data.size() - sizeof(current_tick_)) {
            Networking::packet_header_t header{};
            std::memcpy(&header, tick.data.data() + offset, sizeof(header));

            new_data.resize(header.size);
            std::memcpy(new_data.data(), tick.data.data() + offset, header.size);
            offset += header.size;

            switch (header.type) {
            case Networking::PacketType::UNITSPAWN: {
                Networking::SpawnPacket spawn = Networking::SpawnPacket();
                spawn.Read(&new_data);

                SpawnUnit(spawn.unit, spawn.unit_type, spawn.team, Physics::Vector3{ spawn.x, spawn.y, spawn.z });
                break;
            }
            case Networking::PacketType::UNITIDLE: {
                Networking::UnitIdlePacket idle = Networking::UnitIdlePacket();
                idle.Read(&new_data);

                GameObject* go = GetGameObject(idle.unit);

                if (go == nullptr) {
                    Logger::Err("Received idle command for object that does not exist!");
                    continue;
                }

				if(go->GetCurrentAnimation().GetAnimationName().compare("idle") != 0) {
					go->PlayAnimation("idle", true);
				}
                go->position.x = go->position.x + (idle.x - go->position.x) * diff;
                go->position.y = go->position.y + (idle.y - go->position.y) * diff;
                go->position.z = go->position.z + (idle.z - go->position.z) * diff;
                // go->rotation.y = go->rotation.y + (idle.r - go->rotation.y) * diff;
                go->rotation.y = idle.r; // this actually looks less fucked for now :O
				break;
			}
            case Networking::PacketType::UNITMOVE: {
                Networking::UnitMovePacket move = Networking::UnitMovePacket();
                move.Read(&new_data);

                GameObject* go = GetGameObject(move.unit);

                if (go == nullptr) {
                    Logger::Err("Received move command for object that does not exist!");
                    continue;
                }

                go->position.x = go->position.x + (move.x - go->position.x) * diff;
                go->position.y = go->position.y + (move.y - go->position.y) * diff;
                go->position.z = go->position.z + (move.z - go->position.z) * diff;
                // go->rotation.y = go->rotation.y + (move.r - go->rotation.y) * diff;
                go->rotation.y = move.r; // this actually looks less fucked for now :O

				if(go->GetCurrentAnimation().GetAnimationName().compare("run") != 0) {
					go->PlayAnimation("run", true);
				}
                break;
            }
            case Networking::PacketType::UNITDESPAWN: {
                if (diff < 1) {
                    // not interpolated
                    continue;
                }
                Networking::DespawnPacket despawn = Networking::DespawnPacket();
                despawn.Read(&new_data);
                DespawnUnit(despawn.unit);
                break;
            }
            case Networking::PacketType::PCK_STATS: {
                if (diff < 1) {
                    // not interpolated
                    continue;
                }
                Networking::UnitStatsPacket stats = Networking::UnitStatsPacket();
                stats.Read(&new_data);

                GameObject* go = GetGameObject(stats.unit);

                if (go == nullptr) {
                    Logger::Msg("WARNING: received stats message for unknown object");
                    break;
                }

                go->health = stats.health;
                go->max_health = stats.max_health;
                break;
            }
            case Networking::PacketType::PCK_SPELL_COOLDOWN: {
                if (diff < 1) {
                    // not interpolated
                    continue;
                }
                Networking::CooldownPacket cd = Networking::CooldownPacket();
                cd.Read(&new_data);

                if (cd.unit != my_unit_id_) {
                    break;
                }

                cooldowns[cd.spell_slot] = cd.cooldown;
                total_cooldowns[cd.spell_slot] = cd.total_cooldown;
                break;
            }
            case Networking::PacketType::PCK_PLAY_PARTICLE: {
                if (diff < 1) {
                    // not interpolated
                    continue;
                }
                Networking::PlayParticlePacket part = Networking::PlayParticlePacket();
                part.Read(&new_data);

                GameObject* go = GetGameObject(part.unit);

                if (game_objects_.find(current_tick_) == game_objects_.end()) {
                    ParticleSystem* particle_system = ParticleSystem::Load(part.particle, assetManager_);
                    particle_system->Initialize(direct3D);
                    particle_system->Attach(go);

                    game_objects_.emplace(current_tick_, particle_system);
                }

                break;
            }
            case Networking::PacketType::PCK_ATTACK_START: {
                if (diff < 1) {
                    continue;
                }

                Networking::AttackStartPacket pck = Networking::AttackStartPacket();
                pck.Read(&new_data);

                GameObject* go = GetGameObject(pck.content.unit);

                if (go->GetCurrentAnimation().GetAnimationName() != "attack1") {
                    go->PlayAnimation("attack1", true);
                }
                break;
            }
            default:
                Logger::Err("Received unknown packet type");
                break;
            }
        }
    }

    GameObject* Game::GetGameObject(UnitId unit_id) {
        auto it = game_objects_.find(unit_id);

        if (it == game_objects_.end()) {
            return nullptr;
        }

        return it->second;
    }

    void Game::HandleUnitIdPacket(std::vector<uint8_t> data) {
        Networking::UnitIdPacket pck = Networking::UnitIdPacket();
        pck.Read(&data);

        my_unit_id_ = pck.unit_id;
        unit_id_received_ = true;
    }

    void Game::HandleGameTickPacket(std::vector<uint8_t> data) {
        game_tick_t new_tick{};
        new_tick.received = Util::GetSystemTime();
        Networking::packet_header_t header{};

        std::memcpy(&header, data.data(), sizeof(header));
        new_tick.data.resize(header.size);

        std::memcpy(new_tick.data.data(), data.data(), header.size);

        ticks.push_back(new_tick);
    }

    void Game::HandleUnitMovePacket(std::vector<uint8_t> data) {
        uint64_t tick = 0;
        uint64_t unitId = 0;
        Physics::Vector3 pos;
        float rotation;

        std::memcpy(&tick, data.data() + 16, 8);
        std::memcpy(&unitId, data.data() + 24, 8);
        std::memcpy(&pos.x, data.data() + 32, sizeof(float));
        std::memcpy(&pos.y, data.data() + 32 + sizeof(float), sizeof(float));
        std::memcpy(&pos.z, data.data() + 32 + sizeof(float) + sizeof(float), sizeof(float));
        std::memcpy(&rotation, data.data() + 32 + sizeof(float) + sizeof(float) + sizeof(float), sizeof(float));

        GameObject* object = GetGameObject(unitId);
        object->position = pos;
        object->rotation.y = rotation;

        if (object->GetCurrentAnimation().GetAnimationName().compare("run")) {
            object->PlayAnimation("run", true);
        }
    }

    void Game::HandleUnitIdlePacket(std::vector<uint8_t> data) {
        uint64_t unitId = 0;
        Physics::Vector3 pos;
        float rotation;

        std::memcpy(&unitId, data.data() + 16, 8);
        std::memcpy(&pos.x, data.data() + 24, sizeof(float));
        std::memcpy(&pos.y, data.data() + 24 + sizeof(float), sizeof(float));
        std::memcpy(&pos.z, data.data() + 24 + sizeof(float) + sizeof(float), sizeof(float));
        std::memcpy(&rotation, data.data() + 24 + sizeof(float) + sizeof(float) + sizeof(float), sizeof(float));

        GameObject* object = GetGameObject(unitId);

        if (object != nullptr && object->GetCurrentAnimation().GetAnimationName().compare("idle")) {
            object->PlayAnimation("idle", true);
        }
    }

    void Game::HandleUnitSpawnPacket(std::vector<uint8_t> data) {
        uint64_t tick = 0;
        uint64_t unitId = 0;
        uint64_t unitType = 0;
        uint64_t team = 0;
        Physics::Vector3 pos;

        std::memcpy(&tick, data.data() + 16, 8);
        std::memcpy(&unitId, data.data() + 24, 8);
        std::memcpy(&unitType, data.data() + 32, 8);
        std::memcpy(&team, data.data() + 40, 8);
        std::memcpy(&pos.x, data.data() + 48, sizeof(float));
        std::memcpy(&pos.y, data.data() + 48 + sizeof(float), sizeof(float));
        std::memcpy(&pos.z, data.data() + 48 + sizeof(float) + sizeof(float), sizeof(float));
        SpawnUnit(unitId, unitType, (Team)team, pos);
    }
}