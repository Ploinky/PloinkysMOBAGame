#include "scene.h"
#include "map.h"
#include "renderer.h"
#include "window.h"
#include "camera.h"
#include "client.h"
#include "mesh.h"
#include "vertex.h"
#include "util.h"
#include "logger.h"

namespace PMG {
    NetworkedGame::NetworkedGame(ClientStateHandler* stateHandler, net_client_t connection) : Scene(stateHandler) {
        m_navMesh = new NavMesh();
        m_navMesh->LoadFromFile("map1");

        this->m_netConnection = connection;

        m_map = new Map();
        m_map->Load("map1");

        fps = 0;
    }

    NetworkedGame::~NetworkedGame() {
        for (Mesh* m : models) {
            delete m;
        }

        if (m_netConnection.isConnected) {
          Net_CloseConnection(&m_netConnection);
        }

        if (m_map) {
            delete m_map;
        }

        models.clear();
    }

    void NetworkedGame::Update(float dt) {
        // if (!network->g_isConnected) {
        //     network->CheckConnected();
        //     return;
        // }

        int keysX = m_keys['D'] - m_keys['A'];
        int mouseX = (m_mousePos[0] >= (short)m_sceneWidth - 17) - (m_mousePos[0] == 0);
        int keysZ = m_keys['W'] - m_keys['S'];
        int mouseZ = (m_mousePos[1] == 0) - (m_mousePos[1] >= (short)m_sceneHeight - 40);

        m_camDir[0] = keysX + mouseX;
        m_camDir[1] = keysZ + mouseZ;

        if (m_keys[VK_ESCAPE]) {
            m_stateHandler->PopState();
        }

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

        if (m_mouseButtons[2]) {
            m_mouseClicked[0] = m_mousePos[0];
            m_mouseClicked[1] = m_mousePos[1];
            m_mouseClicked[2] = 1;
        }

        // Network handling
        
        packet_t packet{};
        while(Net_ReceivePacket(&m_netConnection, &packet)) {
          HandleNetworkMessage(&packet);

          packet = {};
        }

        HandleTicks(dt);

        m_camPos[0] += m_camDir[0] * dt / 20;
        m_camPos[1] = 20;
        m_camPos[2] += m_camDir[1] * dt / 20;

        fps = (int)(1000.0f / dt);
    }

    void NetworkedGame::TestIntersect(Renderer* renderer, int mx, int my, float* x, float* y) {
        float hp = M_PI / 180.0f;
        
        vec2_t screenCoord = { mx, my };
        vec3_t rayOrigin = renderer->camera->position;
        mat_t persp = mat_t::Perspective((float) m_sceneWidth / (float) m_sceneHeight, renderer->camera->fov * hp, renderer->camera->nearClip, renderer->camera->farClip);
        mat_t view = mat_t::Rotation(renderer->camera->rotation.z, renderer->camera->rotation.y, renderer->camera->rotation.x) *
          mat_t::Translation(rayOrigin.x, rayOrigin.y, rayOrigin.z);
        view = view.inverse().Transpose();


        vec3_t relScreen = {
            screenCoord.x * 2.0f / (float) m_sceneWidth - 1.0f,
            1.0f - (screenCoord.y * 2.0f) / (float) m_sceneHeight,
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

        rayEye = {rayEye.x, rayEye.y, 1.0, 0.0};

        vec4_t rw4 = (view * rayEye);
        vec3_t rayWorld = {rw4.x, rw4.y, rw4.z};

        rayWorld = rayWorld.normalize();

        vec3_t planeNormal = {0.0, 1.0, 0.0};
        vec3_t planeOrigin = {0.0, 0.0, 0.0};


        float denom = planeNormal * rayWorld;

        if (fabs(denom) > 0.0001f) {
            float t = -(planeNormal * rayOrigin) / (planeNormal * rayWorld);
            *x = rayOrigin.x + rayWorld.x * t;
            *y = rayOrigin.z + rayWorld.z * t;
        }
    }

    void NetworkedGame::Render(Renderer* renderer) {
        renderer->camera->position.x = m_camPos[0];
        renderer->camera->position.y = m_camPos[1];
        renderer->camera->position.z = m_camPos[2];

        if (m_mouseClicked[2] == 1) {
          renderer->FillRect(m_mouseClicked[0] - 1, m_mouseClicked[1] - 1, 3, 3, new float[3]{1.0f, 1.0f, 0.0f});

          m_mouseClicked[2] = 0;

          float x, y;
          TestIntersect(renderer, m_mouseClicked[0], m_mouseClicked[1], &x, &y);

            
            packet_t packet = {};
            packet.header.type = PacketType::UNITMOVE;
            packet << cmd_move_t{ x, y };

          Net_SendPacket(&packet, &m_netConnection);
        }


        std::list<Mesh*> mapMeshes = m_map->GetMeshes();
        std::vector<Mesh*> mapMeshVector(mapMeshes.begin(), mapMeshes.end());
        renderer->RenderMeshes(mapMeshVector);
        renderer->RenderMeshes(models);

        std::wstring fpsText(L"FPS: ");
        fpsText.append(std::to_wstring(fps));
        renderer->RenderText(0, 0, 100, 50, fpsText);
    }

    void NetworkedGame::CharTyped(uint16_t ch) {

    }

    void NetworkedGame::KeyReleased(uint16_t key) {
        m_keys[key] = false;
    }

    void NetworkedGame::KeyPressed(uint16_t key) {
        m_keys[key] = true;
    }

    void NetworkedGame::MouseMoved(int screenX, int screenY) {
        m_mousePos[0] = screenX;
        m_mousePos[1] = screenY;
    }

    void NetworkedGame::MouseButtonPressed(int button) {
        m_mouseButtons[button] = true;
    }

    void NetworkedGame::MouseButtonReleased(int button) {
        m_mouseButtons[button] = false;
    }

    void NetworkedGame::SpawnUnit(unsigned long unitId) {
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

    Mesh* NetworkedGame::GetModelForUnit(unsigned long unitId) {
        for (auto model = models.begin(); model != models.end(); ++model) {
            if ((*model)->unit == unitId) {
                return *model;
            }
        }

        return nullptr;
    }

    void NetworkedGame::DespawnUnit(unsigned long unitId) {
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

    void NetworkedGame::HandleTicks(long long frameTime) {
        frameTime = Util::GetSystemTime();

        if (ticks.size() <= 3) {
            // We need at least 2 frames for interpolation
            return;
        }

        // local client time of first received packet
        float startTime = ticks.front().received;

        // total time elapsed since first packet
        float totalTime = frameTime - ticks.front().received;

        // current frame that we are rendering, including fraction
        float currentFrame = totalTime / (1000.0f / 30.0f);

        // little hacky, this integer cutoff?
        int startFrame = currentFrame - 2; 
        int endFrame = startFrame + 1;

        if (endFrame >= ticks.size()) {
            return;
        }

        game_tick_t lastTick = ticks[endFrame];
        game_tick_t nextLastTick = ticks[startFrame];

        float diff = currentFrame - ((int) currentFrame);

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
        }
    }

    void NetworkedGame::HandleNetworkMessage(packet_t* packet) {
        if (packet->header.type == PacketType::GAME_TICK) {
            game_tick_t newTick{};

            *packet >> newTick.index;
            newTick.index = ticks.size();

            while(packet->data.size() > 0) {
                packet_t tickData{};
                *packet >> tickData;

                switch(tickData.header.type) {
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
            unit_t unit{};
            *packet >> unit.pos >> unit.unitId;
            SpawnUnit(unit.unitId);
        }
    }
}