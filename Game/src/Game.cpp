#include "Game.h"
#include "navigation.h"
#include "Map.h"
#include "Renderer.h"
#include "Camera.h"
#include "logger.h"
#include "Mesh.h"
#include "AssetManager.h"
#include "ParticleSystem.h"
#include "util.h"
#include "MainMenu.h"

namespace PMG {
    Game::Game(std::string server, IClientStateHandler* handler, int width, int height, Renderer* renderer, AssetManager* assetManager) : IClientState(handler, width, height) {
        m_navMesh = new NavMesh();
        m_navMesh->LoadFromFile("map1");

        m_map = new Map();
        m_map->Load("map1");
        net_manager_ = ServerNetworkManager();

        packet_manager = Networking::NetworkHandlerManager<Networking::PacketType>();
        // Register network packets, the fuck...
        packet_manager.RegisterHandler(Networking::PacketType::PCK_CLIENT_UNIT_ID, [this](std::vector<uint8_t> data) { HandleUnitIdPacket(data); });
        packet_manager.RegisterHandler(Networking::PacketType::GAME_TICK, [this](std::vector<uint8_t> data) { HandleGameTickPacket(data); });
        net_manager_.Initialize(&packet_manager);

        net_manager_.ConnectToServer(server);

        m_map->Initialize(renderer->direct3D);

        this->direct3D = renderer->direct3D;
        this->assetManager_ = assetManager;
        this->renderer = renderer;
    }

    Game::~Game() {
        net_manager_.Close();

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
        if (!net_manager_.IsConnected()) {
            net_manager_.CheckConnected();
            return;
        }

        int keysX = m_keys['D'] - m_keys['A'];
        int mouseX = (m_mousePos[0] >= (short)windowWidth_ - 1) - (m_mousePos[0] == 0);
        int keysZ = m_keys['W'] - m_keys['S'];
        int mouseZ = (m_mousePos[1] == 0) - (m_mousePos[1] >= (short)windowHeight_ - 1);

        m_camDir[0] = keysX + mouseX;
        m_camDir[1] = keysZ + mouseZ;

        if (m_mouseButtons[2]) {
            m_mouseClicked[0] = m_mousePos[0];
            m_mouseClicked[1] = m_mousePos[1];
            m_mouseClicked[2] = 1;
        }

        if (m_keys['s']) {
            Networking::StopCommandPacket stop = Networking::StopCommandPacket();

            net_manager_.SendPacket(&stop);
        }

        if (m_keys['q']) {
            float x, y;
            TestIntersect(renderer, m_mousePos[0], m_mousePos[1], &x, &y);

            Networking::CastCommandPacket cast = Networking::CastCommandPacket();
            cast.spell_slot = 0;
            cast.x = x;
            cast.y = 0;
            cast.z = y;
            net_manager_.SendPacket(&cast);
        }

        if (m_keys['c']) {
            if (unit_id_received_) {
                // GetGameObject(my_unit_id_)->renderable->PlayAnimation("run");
            }
            m_keys['c'] = false;
        }

        if (m_keys['w']) {
            float hp = static_cast<float>(M_PI / 180.0);
            Physics::Ray ray = Physics::ScreenToRay({ static_cast<float>(m_mousePos[0]), static_cast<float>(m_mousePos[1]) },
                { renderer->camera->position.x, renderer->camera->position.y, renderer->camera->position.z },
                { renderer->camera->rotation.x, renderer->camera->rotation.y, renderer->camera->rotation.z },
                (float)windowWidth_ / (float)windowHeight_,
                renderer->camera->fov * hp,
                renderer->camera->nearClip,
                renderer->camera->farClip,
                windowWidth_,
                windowHeight_);

            for (auto& go_it : game_objects_) {
                GameObject* go = go_it.second;
                Physics::Sphere sphere(Physics::Vector3(go->position.x, 0, go->position.z), 0.5);
                if (Physics::TestCollision(ray, sphere)) {
                    Networking::CastTargetCommandPacket cmd = Networking::CastTargetCommandPacket();
                    cmd.spell_slot = 1;
                    cmd.target = go->unit_id;
                    net_manager_.SendPacket(&cmd);
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
            net_manager_.SendPacket(&cast);
        }

        if (m_keys['r']) {
            float hp = static_cast<float>(M_PI / 180.0);
            Physics::Ray ray = Physics::ScreenToRay({ static_cast<float>(m_mousePos[0]), static_cast<float>(m_mousePos[1]) },
                { renderer->camera->position.x, renderer->camera->position.y, renderer->camera->position.z },
                { renderer->camera->rotation.x, renderer->camera->rotation.y, renderer->camera->rotation.z },
                (float)windowWidth_ / (float)windowHeight_,
                renderer->camera->fov * hp,
                renderer->camera->nearClip,
                renderer->camera->farClip,
                windowWidth_,
                windowHeight_);

            for (auto& go_it : game_objects_) {
                GameObject* go = go_it.second;
                Physics::Sphere sphere(Physics::Vector3(go->position.x, 0, go->position.z), 0.5);
                if (Physics::TestCollision(ray, sphere)) {
                    Networking::CastTargetCommandPacket cmd = Networking::CastTargetCommandPacket();
                    cmd.spell_slot = 3;
                    cmd.target = go->unit_id;
                    net_manager_.SendPacket(&cmd);
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
                { renderer->camera->position.x, renderer->camera->position.y, renderer->camera->position.z },
                { renderer->camera->rotation.x, renderer->camera->rotation.y, renderer->camera->rotation.z },
                (float)windowWidth_ / (float)windowHeight_,
                renderer->camera->fov * hp,
                renderer->camera->nearClip,
                renderer->camera->farClip,
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

                        net_manager_.SendPacket(&atk_pk);
                    }
                }
            }

            if (!pointing_at_unit) {
                float x, y;
                TestIntersect(renderer, m_mouseClicked[0], m_mouseClicked[1], &x, &y);

                Networking::MoveCommandPacket mv = Networking::MoveCommandPacket();
                mv.x = x;
                mv.y = y;
                net_manager_.SendPacket(&mv);

                ParticleSystem* particle_system = ParticleSystem::Load("models/move_to.pts");
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
        while (net_manager_.ReceivePacket()) {
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
                    m_camPos[1] = 15;
                    m_camPos[2] = my_unit->position.z - 8;
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
    }

    void Game::Render(Renderer* renderer) {
        // ===== Loading Screen =====
        if (!net_manager_.IsConnected()) {
            renderer->RenderText(0, 0, 100, 100, "Connecting");
            return;
        }

        // Game Screen
        renderer->camera->position.x = m_camPos[0];
        renderer->camera->position.y = m_camPos[1];
        renderer->camera->position.z = m_camPos[2];

        std::list<Mesh*> mapMeshes = m_map->GetMeshes();
        std::vector<Mesh*> mapMeshVector(mapMeshes.begin(), mapMeshes.end());
        for (auto mesh : mapMeshVector) {
            mesh->Render(renderer);
        }

        for (auto go_it : game_objects_) {
            GameObject* go = go_it.second;

            go->Render(renderer);
        }

        RenderGameUI(renderer);
    }

    void Game::RenderGameUI(Renderer* renderer) {
        for (auto go_it : game_objects_) {
            GameObject* go = go_it.second;

            if (!go->has_healthbar) {
                continue;
            }

            Physics::Vector3 point_above = Physics::Vector3{ 0, -4.5, 0 };

            float hp = static_cast<float>(M_PI / 180.0);
            Physics::mat_t persp = Physics::mat_t::Perspective((float)windowWidth_ / (float)windowHeight_, renderer->camera->fov * hp, renderer->camera->nearClip, renderer->camera->farClip);

            Physics::mat_t view = Physics::mat_t::Rotation(-renderer->camera->rotation.z, -renderer->camera->rotation.y, -renderer->camera->rotation.x) * Physics::mat_t::Translation(renderer->camera->position.x, renderer->camera->position.y, renderer->camera->position.z);

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

            if (cd_remaining > 0.875) {
                float dx = 0.125 - (cd_remaining - 0.875);
                float fx = 24 * (dx / 0.125);

                Physics::Vector2 points[7]{
                    { x + 25.0f, y + 25.0f },
                    { x + 25.0f + fx, y + 1.0f },
                    { x + 49.0f, y + 1.0f },
                    { x + 49.0f, y + 49.0f },
                    { x + 1.0f, y + 49.0f },
                    { x + 1.0f, y + 1.0f },
                    { x + 25.0f, y + 1.0f },
                };
                renderer->FillShape(points, 7, black);
            }
            else if (cd_remaining > 0.625) {
                float dx = 0.25 - (cd_remaining - 0.625);
                float fx = 49 * (dx / 0.25);

                Physics::Vector2 points[6]{
                    { x + 25.0f, y + 25.0f },
                    { x + 49.0f, y + 1.0f + fx },
                    { x + 49.0f, y + 49.0f },
                    { x + 1.0f, y + 49.0f },
                    { x + 1.0f, y + 1.0f },
                    { x + 25.0f, y + 1.0f },
                };
                renderer->FillShape(points, 6, black);
            }
            else if (cd_remaining > 0.375f) {
                float dx = 0.25 - (cd_remaining - 0.375);
                float fx = 49 * (dx / 0.25);

                Physics::Vector2 points[5]{
                    { x + 25.0f, y + 25.0f },
                    { x + 49.0f - fx, y + 49.0f },
                    { x + 1.0f, y + 49.0f },
                    { x + 1.0f, y + 1.0f },
                    { x + 25.0f, y + 1.0f },
                };
                renderer->FillShape(points, 5, black);
            }
            else if (cd_remaining > 0.125f) {
                float dx = 0.25f - (cd_remaining - 0.125f);
                float fx = 49 * (dx / 0.25f);

                Physics::Vector2 points[4]{
                    { x + 25.0f, y + 25.0f },
                    { x + 1.0f, y + 49.0f - fx },
                    { x + 1.0f, y + 1.0f },
                    { x + 25.0f, y + 1.0f },
                };
                renderer->FillShape(points, 4, black);
            }
            else if (cd_remaining > 0) {
                float dx = 0.125f - cd_remaining;
                float fx = 24.0f * (dx / 0.125f);

                Physics::Vector2 points[3]{
                    { x + 25.0f, y + 25.0f },
                    { x + 1.0f + fx, y + 1.0f },
                    { x + 25.0f, y + 1.0f },
                };
                renderer->FillShape(points, 3, black);
            }
        }
        renderer->RenderText(x, y, 50, 50, "Q");

        x = windowWidth_ / 2 - 55;
        renderer->DrawRect(x, y, 50, 50, black);
        renderer->FillRect(x + 1, y + 1, 48, 48, gray);
        if (cooldowns[1] != -1) {
            float cd_remaining = (float)cooldowns[1] / (float)total_cooldowns[1];

            if (cd_remaining > 0.875) {
                float dx = 0.125 - (cd_remaining - 0.875);
                float fx = 24 * (dx / 0.125);

                Physics::Vector2 points[7]{
                    { x + 25.0f, y + 25.0f },
                    { x + 25.0f + fx, y + 1.0f },
                    { x + 49.0f, y + 1.0f },
                    { x + 49.0f, y + 49.0f },
                    { x + 1.0f, y + 49.0f },
                    { x + 1.0f, y + 1.0f },
                    { x + 25.0f, y + 1.0f },
                };
                renderer->FillShape(points, 7, black);
            }
            else if (cd_remaining > 0.625f) {
                float dx = 0.25f - (cd_remaining - 0.625f);
                float fx = 49 * (dx / 0.25f);

                Physics::Vector2 points[6]{
                    { x + 25.0f, y + 25.0f },
                    { x + 49.0f, y + 1.0f + fx },
                    { x + 49.0f, y + 49.0f },
                    { x + 1.0f, y + 49.0f },
                    { x + 1.0f, y + 1.0f },
                    { x + 25.0f, y + 1.0f },
                };
                renderer->FillShape(points, 6, black);
            }
            else if (cd_remaining > 0.375f) {
                float dx = 0.25f - (cd_remaining - 0.375f);
                float fx = 49 * (dx / 0.25f);

                Physics::Vector2 points[5]{
                    { x + 25.0f, y + 25.0f },
                    { x + 49.0f - fx, y + 49.0f },
                    { x + 1.0f, y + 49.0f },
                    { x + 1.0f, y + 1.0f },
                    { x + 25.0f, y + 1.0f },
                };
                renderer->FillShape(points, 5, black);
            }
            else if (cd_remaining > 0.125) {
                float dx = 0.25 - (cd_remaining - 0.125);
                float fx = 49 * (dx / 0.25);

                Physics::Vector2 points[4]{
                    { x + 25.0f, y + 25.0f },
                    { x + 1.0f, y + 49.0f - fx },
                    { x + 1.0f, y + 1.0f },
                    { x + 25.0f, y + 1.0f },
                };
                renderer->FillShape(points, 4, black);
            }
            else if (cd_remaining > 0) {
                float dx = 0.125f - cd_remaining;
                float fx = 24.0f * (dx / 0.125f);

                Physics::Vector2 points[3]{
                    { x + 25.0f, y + 25.0f },
                    { x + 1.0f + fx, y + 1.0f },
                    { x + 25.0f, y + 1.0f },
                };
                renderer->FillShape(points, 3, black);
            }
        }
        renderer->RenderText(x, y, 50, 50, "W");

        x = windowWidth_ / 2 + 5;
        renderer->DrawRect(x, y, 50, 50, black);
        renderer->FillRect(x + 1, y + 1, 48, 48, gray);
        if (cooldowns[2] != -1) {
            float cd_remaining = (float)cooldowns[2] / (float)total_cooldowns[2];

            if (cd_remaining > 0.875) {
                float dx = 0.125 - (cd_remaining - 0.875);
                float fx = 24 * (dx / 0.125);

                Physics::Vector2 points[7]{
                    { x + 25.0f, y + 25.0f },
                    { x + 25.0f + fx, y + 1.0f },
                    { x + 49.0f, y + 1.0f },
                    { x + 49.0f, y + 49.0f },
                    { x + 1.0f, y + 49.0f },
                    { x + 1.0f, y + 1.0f },
                    { x + 25.0f, y + 1.0f },
                };
                renderer->FillShape(points, 7, black);
            }
            else if (cd_remaining > 0.625) {
                float dx = 0.25 - (cd_remaining - 0.625);
                float fx = 49 * (dx / 0.25);

                Physics::Vector2 points[6]{
                    { x + 25.0f, y + 25.0f },
                    { x + 49.0f, y + 1.0f + fx },
                    { x + 49.0f, y + 49.0f },
                    { x + 1.0f, y + 49.0f },
                    { x + 1.0f, y + 1.0f },
                    { x + 25.0f, y + 1.0f },
                };
                renderer->FillShape(points, 6, black);
            }
            else if (cd_remaining > 0.375) {
                float dx = 0.25 - (cd_remaining - 0.375);
                float fx = 49 * (dx / 0.25);

                Physics::Vector2 points[5]{
                    { x + 25.0f, y + 25.0f },
                    { x + 49.0f - fx, y + 49.0f },
                    { x + 1.0f, y + 49.0f },
                    { x + 1.0f, y + 1.0f },
                    { x + 25.0f, y + 1.0f },
                };
                renderer->FillShape(points, 5, black);
            }
            else if (cd_remaining > 0.125) {
                float dx = 0.25 - (cd_remaining - 0.125);
                float fx = 49 * (dx / 0.25);

                Physics::Vector2 points[4]{
                    { x + 25.0f, y + 25.0f },
                    { x + 1.0f, y + 49.0f - fx },
                    { x + 1.0f, y + 1.0f },
                    { x + 25.0f, y + 1.0f },
                };
                renderer->FillShape(points, 4, black);
            }
            else if (cd_remaining > 0) {
                float dx = 0.125 - cd_remaining;
                float fx = 24.0 * (dx / 0.125);

                Physics::Vector2 points[3]{
                    { x + 25.0f, y + 25.0f },
                    { x + 1.0f + fx, y + 1.0f },
                    { x + 25.0f, y + 1.0f },
                };
                renderer->FillShape(points, 3, black);
            }
        }
        renderer->RenderText(x, y, 50, 50, "E");

        x = windowWidth_ / 2 + 65;
        renderer->DrawRect(x, y, 50, 50, black);
        renderer->FillRect(x + 1, y + 1, 48, 48, gray);
        if (cooldowns[3] != -1) {
            float cd_remaining = (float)cooldowns[3] / (float)total_cooldowns[3];

            if (cd_remaining > 0.875) {
                float dx = 0.125 - (cd_remaining - 0.875);
                float fx = 24 * (dx / 0.125);

                Physics::Vector2 points[7]{
                    { x + 25.0f, y + 25.0f },
                    { x + 25.0f + fx, y + 1.0f },
                    { x + 49.0f, y + 1.0f },
                    { x + 49.0f, y + 49.0f },
                    { x + 1.0f, y + 49.0f },
                    { x + 1.0f, y + 1.0f },
                    { x + 25.0f, y + 1.0f },
                };
                renderer->FillShape(points, 7, black);
            }
            else if (cd_remaining > 0.625) {
                float dx = 0.25 - (cd_remaining - 0.625);
                float fx = 49 * (dx / 0.25);

                Physics::Vector2 points[6]{
                    { x + 25.0f, y + 25.0f },
                    { x + 49.0f, y + 1.0f + fx },
                    { x + 49.0f, y + 49.0f },
                    { x + 1.0f, y + 49.0f },
                    { x + 1.0f, y + 1.0f },
                    { x + 25.0f, y + 1.0f },
                };
                renderer->FillShape(points, 6, black);
            }
            else if (cd_remaining > 0.375) {
                float dx = 0.25 - (cd_remaining - 0.375);
                float fx = 49 * (dx / 0.25);

                Physics::Vector2 points[5]{
                    { x + 25.0f, y + 25.0f },
                    { x + 49.0f - fx, y + 49.0f },
                    { x + 1.0f, y + 49.0f },
                    { x + 1.0f, y + 1.0f },
                    { x + 25.0f, y + 1.0f },
                };
                renderer->FillShape(points, 5, black);
            }
            else if (cd_remaining > 0.125) {
                float dx = 0.25 - (cd_remaining - 0.125);
                float fx = 49 * (dx / 0.25);

                Physics::Vector2 points[4]{
                    { x + 25.0f, y + 25.0f },
                    { x + 1.0f, y + 49.0f - fx },
                    { x + 1.0f, y + 1.0f },
                    { x + 25.0f, y + 1.0f },
                };
                renderer->FillShape(points, 4, black);
            }
            else if (cd_remaining > 0) {
                float dx = 0.125 - cd_remaining;
                float fx = 24.0 * (dx / 0.125);

                Physics::Vector2 points[3]{
                    { x + 25.0f, y + 25.0f },
                    { x + 1.0f + fx, y + 1.0f },
                    { x + 25.0f, y + 1.0f },
                };
                renderer->FillShape(points, 3, black);
            }
        }
        renderer->RenderText(x, y, 50, 50, "R");
    }

    void Game::TestIntersect(Renderer* renderer, int mx, int my, float* x, float* y) {
        float hp = static_cast<float>(M_PI / 180.0);

        Physics::Vector2 screenCoord = { static_cast<float>(mx), static_cast<float>(my) };
        Physics::Vector3 rayOrigin = renderer->camera->position;
        Physics::mat_t persp = Physics::mat_t::Perspective((float)windowWidth_ / (float)windowHeight_, renderer->camera->fov * hp, renderer->camera->nearClip, renderer->camera->farClip);
        Physics::mat_t view = Physics::mat_t::Rotation(renderer->camera->rotation.z, renderer->camera->rotation.y, renderer->camera->rotation.x) *
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

    void Game::SpawnUnit(unsigned long unitId) {
        SpawnUnit(unitId, 0, Team::TEAM_1, Physics::Vector3(0, 0, 0));
    }

    void Game::SpawnUnit(unsigned long unitId, unsigned long unit_type, Team team, Physics::Vector3 pos) {
        if (game_objects_.find(unitId) != game_objects_.end()) {
            // already spawned!
            return;
        }
        // Hacky missile hack
        if (unit_type == UnitPrefab::THROW_FOOTBALL) {
            GameObject* go = new GameObject();
            go->unit_id = unitId;
            go->health = 50;
            go->max_health = 100;
            go->renderable = TextureMesh::Load(assetManager_->LoadFile("models\\missile.p3d"), "models/missile", direct3D);
            go->position = pos;
            go->rotation = { 0, 0, 0 };
            go->has_healthbar = false;
            go->team = team;
            game_objects_.emplace(unitId, go);

            go->renderable->position = go->position;
            go->renderable->rotation = go->rotation;
            return;
        }

        if (unit_type == UnitPrefab::FOOTBALL_PERSON) {
            GameObject* go = new GameObject();
            go->unit_id = unitId;
            go->health = 50;
            go->max_health = 100;
            go->renderable = SkinnedTexturedMesh::Load(assetManager_->LoadFile("models\\chess_person.p3d"), "models\\chess_person", direct3D);
            go->position = pos;
            go->rotation = { 0, 0, 0 };
            go->team = team;
            game_objects_.emplace(unitId, go);
            return;
        }

        if (unit_type == UnitPrefab::TOWER) {
            GameObject* go = new GameObject();
            go->unit_id = unitId;
            go->health = 50;
            go->max_health = 100;
            go->renderable = TextureMesh::Load(assetManager_->LoadFile("models\\tower.p3d"), "models/tower", direct3D);
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
            go->unit_id = unitId;
            go->health = 10;
            go->max_health = 10;
            go->position = pos;
            go->rotation = { 0, 0, 0 };
            go->has_healthbar = true;
            go->has_title = false;
            go->team = team;
            go->renderable = TextureMesh::Load(assetManager_->LoadFile("models\\cube_minion.p3d"), "models/cube_minion", direct3D);
            go->renderable->position = go->position;
            game_objects_.emplace(unitId, go);
            return;
        }

        Logger::Err("Received spawn message for prefab that does not exist");
    }

    void Game::DespawnUnit(unsigned long unitId) {
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
            case Networking::PacketType::UNITMOVE:
            case Networking::PacketType::UNITIDLE: {
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
            case Networking::PacketType::PCK_START_ANIMATION: {
                if (diff < 1) {
                    // not interpolated
                    continue;
                }
                Networking::AnimationPacket anim = Networking::AnimationPacket();
                anim.Read(&new_data);

                GameObject* gp = GetGameObject(anim.unit_id);

                if (gp != nullptr) {
                    GetGameObject(anim.unit_id)->renderable->PlayAnimation(anim.animation_name, anim.loop);
                }
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
                    ParticleSystem* particle_system = ParticleSystem::Load(part.particle);
                    particle_system->Initialize(direct3D);
                    particle_system->Attach(go);

                    game_objects_.emplace(current_tick_, particle_system);
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
}