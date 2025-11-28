#include "Game.h"
#include <common/PMG_Common.h>
#include "Renderer.h"
#include "Camera.h"
#include "ParticleEffect.h"
#include "ParticleEmitter.h"
#include "MainMenu.h"
#include "../Resources/resource.h"
#include "game/components/components.h"
#include "game/systems/animation-system.h"
#include "game/systems/particle-system.h"

Game::Game(ClientNetworkManager* server, IClientStateHandler* handler, int width, int height) : IClientState(handler, width, height) {
    // Initialize navigation mesh for selected map
    // TODO probably need to get this from a loading / connecting state
    m_navMesh = new NavMesh();
    m_navMesh->LoadFromData(handler->GetAssetManager()->LoadPlainFile("data/Maps/map1/map1.nvm"));

    // Convert the navigation mesh to a cell grid we can actually use
    m_navGrid = new NavigationCellGrid(m_navMesh);

    // NetworkManager is persistent from the lobby scene
    net_manager_ = server;

    // Register network packets, the fuck...
    std::function<void(std::vector<uint8_t>)> addToPacket = [this](std::vector<uint8_t> data) {AddPacketToCurrentTick(data);};
    packet_manager = NetworkHandlerManager<PacketType, std::function<void(std::vector<uint8_t>)>>();
    packet_manager.RegisterHandler(PacketType::PCK_ATTACK_FINISHED, addToPacket);
    packet_manager.RegisterHandler(PacketType::PCK_CLIENT_UNIT_ID, [this](std::vector<uint8_t> data) { HandleUnitIdPacket(data); });
    packet_manager.RegisterHandler(PacketType::GAME_TICK, [this](std::vector<uint8_t> data) { HandleGameTickPacket(data); });
    packet_manager.RegisterHandler(PacketType::PCK_ATTACK_START, addToPacket);
    packet_manager.RegisterHandler(PacketType::UNITSPAWN, addToPacket);
    packet_manager.RegisterHandler(PacketType::UNITMOVE, addToPacket);
    packet_manager.RegisterHandler(PacketType::UNITIDLE, addToPacket);
    packet_manager.RegisterHandler(PacketType::PCK_PLAY_PARTICLE, addToPacket);
    packet_manager.RegisterHandler(PacketType::UNITDESPAWN, addToPacket);
    packet_manager.RegisterHandler(PacketType::PCK_STATS, addToPacket);
    packet_manager.RegisterHandler(PacketType::PCK_SPELL_COOLDOWN, addToPacket);
    packet_manager.RegisterHandler(PacketType::SCORE_UPDATE_PACKET, addToPacket);
    packet_manager.RegisterHandler(PacketType::GAME_END_PACKET, addToPacket);
    packet_manager.RegisterHandler(PacketType::UNITMOVE_INTENTION, addToPacket);
    packet_manager.RegisterHandler(PacketType::PCK_SPELL_CAST_START, addToPacket);
    packet_manager.RegisterHandler(PacketType::PCK_SPELL_HIT, addToPacket);
    packet_manager.RegisterHandler(PacketType::PCK_UNIT_DEATH, addToPacket);
    packet_manager.RegisterHandler(PacketType::PCK_UNIT_RESPAWN, addToPacket);

    // Let the network manager know about our new packet handler
    net_manager_->Initialize(&packet_manager);

    this->assetManager_ = handler->GetAssetManager();
    this->renderer = handler->GetRenderer();

    m_iTeam1Score = 0;
    m_iTeam2Score = 0;
    m_bGameHasEnded = false;

    // TODO
    m_pAudioSystem = new AudioSystem(handler->GetAudioEngine(), handler->GetAssetManager());
    m_hGenericIcon = handler->GetAssetManager()->GetBitmapImage("ability-icon", "assets/persons/shared/ability-icon.bmp");
    m_hThunderstrikeSound = handler->GetAssetManager()->LoadSound("assets/characters/stormcaller/abilities/thunderstrike.wav");
    m_hStormcallerDeath = handler->GetAssetManager()->LoadSound("assets/characters/stormcaller/death.wav");
    m_hStormcallerAttack = handler->GetAssetManager()->LoadSound("assets/characters/stormcaller/attack.wav");

    m_gameState.AddSystem(m_pAudioSystem);
    m_gameState.AddSystem(new CAnimationSystem(handler->GetAssetManager()));
    m_gameState.AddSystem(new CParticleSystem(handler->GetAssetManager()));

    m_playerInput = {
        .bKeyScrollLeft = false,
        .bKeyScrollRight = false,
        .bKeyScrollUp = false,
        .bKeyScrollDown = false,
        .bFocusUnit = false
    };
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

    pObjectUnderCursor = nullptr;
	float hp = static_cast<float>(M_PI / 180.0);
	Ray ray = renderer->m_camera.CameraRay({ static_cast<float>(screenX), static_cast<float>(screenY) }, windowWidth_, (float)windowHeight_);

    for(auto go_it : game_objects_) {
        GameObject* go = go_it.second;
        TransformComponent_t* pTransform = m_gameState.GetTransform(go->unit_id);
        Capsule_t capsule = Capsule_t {
            .vec3Start = Vector3(pTransform->vec3Position.x, 0, pTransform->vec3Position.z),
            .vec3End = Vector3(pTransform->vec3Position.x, 200, pTransform->vec3Position.z),
            .fRadius = 50,
        };

		if (TestCollision(ray, capsule)) {
			// TODO does this work for multiple objects right behind each other?
			pObjectUnderCursor = go;
			break;
		}
    }
    
    if(pObjectUnderCursor && m_gameState.GetTargetable(pObjectUnderCursor->unit_id)) {
        handler_->RequestCursor(CursorId::ATTACK_MOVE);
    } else {
        handler_->RequestCursor(CursorId::DEFAULT);
    }

}

void Game::Update(float dt) {
    if (!net_manager_->IsConnected()) {
        net_manager_->CheckConnected();
        return;
    }

    int keysX = m_playerInput.bKeyScrollRight - m_playerInput.bKeyScrollLeft;
    int mouseX = (m_mousePos[0] >= (short)windowWidth_ - 1) - (m_mousePos[0] == 0);
    int keysZ = m_playerInput.bKeyScrollDown - m_playerInput.bKeyScrollUp;
    int mouseZ = (m_mousePos[1] >= (short)windowHeight_ - 1) - (m_mousePos[1] == 0);

    m_camDir[0] = keysX + mouseX;
    m_camDir[1] = keysZ + mouseZ;

    if (m_mouseButtons[2]) {
        m_mouseClicked[0] = m_mousePos[0];
        m_mouseClicked[1] = m_mousePos[1];
        m_mouseClicked[2] = 1;
    }

	for (auto& go_it : game_objects_) {
		GameObject* go = go_it.second;
        std::string desiredAnim;
        bool bLoop;
        const CModelData& modelData = assetManager_->GetGameData().mapModelData.at(m_gameState.GetRenderable(go->unit_id)->strRenderable);
    
        if (go->dead) {
            desiredAnim = "death";
            bLoop = false;
        } else if (m_gameState.GetMovement(go->unit_id) && m_gameState.GetMovement(go->unit_id)->bIsMoving) {
            desiredAnim = "run";
            bLoop = true;
        } else if(go->bIsCasting) {
            desiredAnim = "ability1";
            bLoop = false;
        } else if(go->bIsAttacking) {
            desiredAnim = "attack1";
            bLoop = true;
        } else {
            desiredAnim = "idle";
            bLoop = true;
        }

        if(!modelData.mapAnimations.contains(desiredAnim)) {
            continue;
        }

        CAnimationData animData = modelData.mapAnimations.at(desiredAnim);
        desiredAnim = animData.name;

        AnimationComponent_t* pAnimComp = m_gameState.GetAnimation(go->unit_id);
        if (pAnimComp && pAnimComp->m_strAnimationName != desiredAnim) {
            Logger::FormatMsg("Playing animation %s", desiredAnim.c_str());
            pAnimComp->m_strAnimationName = desiredAnim;
            pAnimComp->m_bLoop = bLoop;
            pAnimComp->m_fAnimationTime = 0.0f;
        } else if (pAnimComp) {
            pAnimComp->m_fAnimationTime += dt;
        }
	}

    if (m_keys['q']) {
        switch(m_vecAbilities[0].eTargetType) {
            case EAbilityTargetType::UNIT:
                if (pObjectUnderCursor) {
                    CastTargetCommandPacket cmd = CastTargetCommandPacket();
                    cmd.spell_slot = 0;
                    cmd.target = pObjectUnderCursor->unit_id;
                    net_manager_->SendPacket(&cmd);
                }
                break;
            default:
                float x, y;
                TestIntersect(renderer, m_mousePos[0], m_mousePos[1], &x, &y);
                
                CastCommandPacket cast = CastCommandPacket();
                cast.spell_slot = 0;
                cast.x = x;
                cast.y = 0;
                cast.z = y;
                net_manager_->SendPacket(&cast);
                break;
        }
    }

    if (m_keys['w']) {
		if (pObjectUnderCursor) {
			CastTargetCommandPacket cmd = CastTargetCommandPacket();
			cmd.spell_slot = 1;
			cmd.target = pObjectUnderCursor->unit_id;
			net_manager_->SendPacket(&cmd);
		}
    }

    if (m_keys['e']) {
        float x, y;
        TestIntersect(renderer, m_mousePos[0], m_mousePos[1], &x, &y);

        CastCommandPacket cast = CastCommandPacket();
        cast.spell_slot = 2;
        cast.x = x;
        cast.y = 0;
        cast.z = y;
        net_manager_->SendPacket(&cast);
    }

    if (m_keys['r']) {
		if (pObjectUnderCursor) {
			CastTargetCommandPacket cmd = CastTargetCommandPacket();
			cmd.spell_slot = 3;
			cmd.target = pObjectUnderCursor->unit_id;
			net_manager_->SendPacket(&cmd);
		}
    }

    for (int i = 0; i < 4; i++) {
        if (cooldowns[i] > 0) {
            cooldowns[i] -= dt;
        }
    }

    last_move = std::max(0.0, last_move - dt);

    if (!m_mouseButtons[2]) {
        last_move = 0;
    }

    if (last_move == 0 && m_mouseButtons[2] && m_mouseClicked[2] == 1) {
		if (pObjectUnderCursor && pObjectUnderCursor->has_healthbar) {
			last_move = 150;

			if (m_mouseButtons[2]) {
				AttackCommandPacket atk_pk = AttackCommandPacket();
				atk_pk.target_unit = pObjectUnderCursor->unit_id;

				net_manager_->SendPacket(&atk_pk);
			}
		} else if (!pObjectUnderCursor) {
            float x, y;
            TestIntersect(renderer, m_mouseClicked[0], m_mouseClicked[1], &x, &y);

            MoveCommandPacket mv = MoveCommandPacket();
            mv.x = x;
            mv.y = y;
            net_manager_->SendPacket(&mv);

            ParticleEffect* particle_system = ParticleEffect::Load("UI/MoveTo/move_to.pts", assetManager_);
            particle_system->position = { x, 0, y };
            m_vecGlobalParticles.push_back(particle_system);

            last_move = 150;
        }
    }

    // Network handling
    while (net_manager_->ReceivePacket()) {
    }

    HandleTicks(dt);

    for (auto go_it : game_objects_) {
        go_it.second->Update(dt);
    }

    for(ParticleEffect* pEffect : m_vecGlobalParticles) {
        pEffect->Update(dt);
    }
    
    if(m_playerInput.bFocusUnit) {
        if (unit_id_received_) {
            // Snap to player
            if (TransformComponent_t* pTransform = m_gameState.GetTransform(my_unit_id_)) {
                m_camPos[0] = pTransform->vec3Position.x;
                m_camPos[1] = 800.0f;
                m_camPos[2] = pTransform->vec3Position.z + 350.0f;
            }
        }
        else {
            m_camPos[0] = 0;
            m_camPos[1] = 800.0f;
            m_camPos[2] = 0;
        }
    }

    std::erase_if(m_vecGlobalParticles, [](auto p) {
        if (p->destroy) {
            delete p;
            return true;
        }

        return false;
    });


    std::erase_if(game_objects_, [](auto kv) {
        if (kv.second->destroy) {
            delete kv.second;
            return true;
        }

        return false;
        });

    if (m_keys[' ']) {
    //    if (unit_id_received_) {
    //        // Snap to player
    //        GameObject* my_unit = GetGameObject(my_unit_id_);
    //        
    //        if (my_unit != nullptr) {
    //            m_camDir[0] = 0;
    //            m_camDir[1] = 0;
    //            m_camPos[0] = my_unit->position.x;
    //            m_camPos[1] = 1500.0f;
    //            m_camPos[2] = my_unit->position.z + 800.0f;
    //        }
    //    }
    //    else {
    //        m_camPos[0] = 0;
    //        m_camPos[2] = -1000;
    //    }

    }
    else {
        m_camPos[0] += m_camDir[0] * dt / 0.2;
        m_camPos[2] += m_camDir[1] * dt / 0.2;
    }

    // clamp camera position to avoid scrolling off map
    m_camPos[0] = std::min(std::max(m_camPos[0], 0.0f), 9000.0f);
    m_camPos[2] = std::max(std::min(m_camPos[2], 1000.0f), -4400.0f);

    m_pAudioSystem->SetListenerPosition({m_camPos[0], m_camPos[1], m_camPos[2]});
    for(ISystem* pSystem : m_gameState.m_vecGameSystems) {
        pSystem->Update(&m_gameState, dt);
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
    renderer->Render(&m_gameState);

    // ===============================================
    // probably should move this somewhere
    for(const ParticleEffect* pParticleEffect : m_vecGlobalParticles) {
        for(ParticleEmitter* pParticleEmitter : pParticleEffect->emitters_) {
            renderer->QueueParticle(pParticleEmitter);
        }
    }

    RenderGameUI(renderer);
}

void Game::RenderGameUI(CRenderer* renderer) {
#ifdef NAV_DEBUG
    for (int c = 0; c < m_navGrid->CellCountX * m_navGrid->CellCountY; c++) {
        m_navGrid->Cells[c]->IsOpen = true;
    }
    m_navGrid->Reset();
    for (UnitId unit : m_gameState.vecUnits) {
        TransformComponent_t* pTransform = m_gameState.GetTransform(unit);

        if (!pTransform) {
            continue;   
        }
        m_navGrid->GetCellAt(pTransform->vec3Position.x - 25, pTransform->vec3Position.z - 25)->IsOpen = false;
        m_navGrid->GetCellAt(pTransform->vec3Position.x - 25, pTransform->vec3Position.z - 25)->UnitId = unit;
        m_navGrid->GetCellAt(pTransform->vec3Position.x + 25, pTransform->vec3Position.z - 25)->IsOpen = false;
        m_navGrid->GetCellAt(pTransform->vec3Position.x + 25, pTransform->vec3Position.z - 25)->UnitId = unit;
        m_navGrid->GetCellAt(pTransform->vec3Position.x + 25, pTransform->vec3Position.z + 25)->IsOpen = false;
        m_navGrid->GetCellAt(pTransform->vec3Position.x + 25, pTransform->vec3Position.z + 25)->UnitId = unit;
        m_navGrid->GetCellAt(pTransform->vec3Position.x - 25, pTransform->vec3Position.z + 25)->IsOpen = false;
        m_navGrid->GetCellAt(pTransform->vec3Position.x - 25, pTransform->vec3Position.z + 25)->UnitId = unit;
    }
    renderer->RenderNavGrid(m_navGrid);
#endif

    for (auto go_it : game_objects_) {
        GameObject* go = go_it.second;

        if (!go->has_healthbar || go->dead) {
            continue;
        }

       double health_bar_height = 5;

       if(TransformComponent_t* pTransform = m_gameState.GetTransform(go->unit_id)) {

        Vector2 vec2Screen = renderer->m_camera.UnprojectWorldPoint({pTransform->vec3Position.x, pTransform->vec3Position.y + 200, pTransform->vec3Position.z}, windowWidth_, windowHeight_);
            double x = vec2Screen.x;
            double y = vec2Screen.y;

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

            renderer->FillRect(x - 50, y - 10, ((float)m_gameState.GetHealth(go->unit_id)->nHealth / (float)m_gameState.GetHealth(go->unit_id)->nMaxHealth) * 100.0f, health_bar_height, color);
            renderer->DrawRect(x - 51, y - 11, 102, health_bar_height + 2, new float[3] { 0.0f, 0.0f, 0 });
       }
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

    if(HealthComponent_t* pHealthComp = m_gameState.GetHealth(my_go->unit_id)) {
        int nPercentageHealth = (float)pHealthComp->nHealth / (float)pHealthComp->nMaxHealth * 400.0f;

        renderer->FillRect(x - 1, y - 1, 402, 27, black);

        float green[3]{ 0, 0.5, 0 };
        renderer->FillRect(x, y, nPercentageHealth, 25, green);

        renderer->RenderText(x, y, 400, 25, std::to_string(pHealthComp->nHealth).append("/").append(std::to_string(pHealthComp->nMaxHealth)).c_str());
    }

    float gray[3]{ 0.5, 0.5, 0.5 };

    if(unit_id_received_ && m_vecAbilities.size() == 4) {
        // Ability icons ?!
        y = windowHeight_ - 110;
        x = windowWidth_ / 2 - 115;

        renderer->DrawRect(x, y, 50, 50, black);
        renderer->DrawImage(x + 1, y + 1, 48, 48, m_vecAbilities.at(0).hIcon);
        if (cooldowns[0] != -1) {
            float cd_remaining = (float)cooldowns[0] / (float)total_cooldowns[0];
            renderer->RenderPartialCover(x + 1, y + 1, 48, 48, cd_remaining);
        }
        renderer->RenderText(x, y, 50, 50, "Q");

        x = windowWidth_ / 2 - 55;
        renderer->DrawRect(x, y, 50, 50, black);
        renderer->DrawImage(x + 1, y + 1, 48, 48, m_vecAbilities.at(1).hIcon);
        if (cooldowns[1] != -1) {
            float cd_remaining = (float)cooldowns[1] / (float)total_cooldowns[1];
            renderer->RenderPartialCover(x + 1, y + 1, 48, 48, cd_remaining);
        }
        renderer->RenderText(x, y, 50, 50, "W");

        x = windowWidth_ / 2 + 5;
        renderer->DrawRect(x, y, 50, 50, black);
        renderer->DrawImage(x + 1, y + 1, 48, 48, m_vecAbilities.at(2).hIcon);
        if (cooldowns[2] != -1) {
            float cd_remaining = (float)cooldowns[2] / (float)total_cooldowns[2];
            renderer->RenderPartialCover(x + 1, y + 1, 48, 48, cd_remaining);
        }
        renderer->RenderText(x, y, 50, 50, "E");

        x = windowWidth_ / 2 + 65;
        renderer->DrawRect(x, y, 50, 50, black);
        renderer->DrawImage(x + 1, y + 1, 48, 48, m_vecAbilities.at(3).hIcon);
        if (cooldowns[3] != -1) {
            float cd_remaining = (float)cooldowns[3] / (float)total_cooldowns[3];
            renderer->RenderPartialCover(x + 1, y + 1, 48, 48, cd_remaining);
        }
        renderer->RenderText(x, y, 50, 50, "R");
    }

    float hp = static_cast<float>(M_PI / 180.0);
    Ray ray = renderer->m_camera.CameraRay({ static_cast<float>(m_mousePos[0]), static_cast<float>(m_mousePos[1]) }, windowWidth_, windowHeight_);

    for (auto& go_it : game_objects_) {
        GameObject* go = go_it.second;
        if(TransformComponent_t* pTransform = m_gameState.GetTransform(go->unit_id)) {
            Sphere sphere(Vector3(pTransform->vec3Position.x, 25, pTransform->vec3Position.z), 50);
            if (TestCollision(ray, sphere) && go->has_healthbar && go->team != my_go->team) {
                handler_->RequestCursor(CursorId::ATTACK_MOVE);
                break;
            }
        }
    }

    renderer->FillRect(windowWidth_ - 100, 0, 100, 20, black);
    renderer->RenderText(windowWidth_ - 100, 0, 100, 20, std::to_string(m_iTeam1Score) + " - " + std::to_string(m_iTeam2Score));

    if (m_bGameHasEnded) {
        renderer->FillRect(windowWidth_ / 2 - 200, windowHeight_ / 2 - 50, 400, 100, black);
        renderer->RenderText(windowWidth_ / 2 - 200, windowHeight_ / 2 - 50, 400, 100, "Game Over!");
    }

    renderer->RenderChat(m_vecChat);
}

void Game::TestIntersect(CRenderer* renderer, int mx, int my, float* x, float* y) {
    float hp = static_cast<float>(M_PI / 180.0);

    Vector2 screenCoord = { static_cast<float>(mx), static_cast<float>(my) };
    Vector3 rayOrigin = renderer->m_camera.position;
    mat_t persp = PMathMatPerspectiveRH(ToRadians(renderer->m_camera.fov), (float)windowWidth_ / (float)windowHeight_, renderer->m_camera.nearClip, renderer->m_camera.farClip);
    mat_t view = PMathMatRotation(renderer->m_camera.rotation.y, renderer->m_camera.rotation.x, renderer->m_camera.rotation.z) *
        PMathMatTranslation(rayOrigin.x, rayOrigin.y, rayOrigin.z);


    Vector3 relScreen = {
        screenCoord.x * 2.0f / (float)windowWidth_ - 1.0f,
        1.0f - (screenCoord.y * 2.0f) / (float)windowHeight_,
        1.0f
    };

    Vector4 rayClip = {
        relScreen.x,
        relScreen.y,
        1.0f,
        1.0f
    };

    mat_t perspInverse = PMathMatInverse(persp);

    Vector4 rayEye = perspInverse * rayClip;

    rayEye = { rayEye.x, rayEye.y, -1.0, 0.0 };

    Vector4 rw4 = (view * rayEye);
    Vector3 rayWorld = { rw4.x, rw4.y, rw4.z };

    rayWorld = rayWorld.Normalize();

    Vector3 planeNormal = { 0.0, 1.0, 0.0 };
    Vector3 planeOrigin = { 0.0, 0.0, 0.0 };

    float denom = planeNormal * rayWorld;

    if (fabs(denom) > 0.0001f) {
        float t = -(planeNormal * rayOrigin) / (planeNormal * rayWorld);
        *x = rayOrigin.x + rayWorld.x * t;
        *y = rayOrigin.z + rayWorld.z * t;
    }
}

void Game::SpawnUnit(uint64_t unitId) {
    SpawnUnit(unitId, "", Team::TEAM_1, Vector3(0, 0, 0));
}

void Game::SpawnUnit(UnitId unitId, std::string entityId, Team team, Vector3 pos) {
    if(unitId == my_unit_id_) {
        const CCharacterData entityData = assetManager_->GetGameData().mapCharacterData.at(entityId);
        for(int i = 0; i < 4; i++) {
            auto abilityMapIt = entityData.mapAbilityIds.find(i);
            if(abilityMapIt == entityData.mapAbilityIds.end()) {
                m_vecAbilities[i] = {
                    .strName = "",
                    .hIcon = assetManager_->GetBitmapImage("empty-ability-slot"),
                    .eTargetType = EAbilityTargetType::UNIT,
                };
                continue;
            }

            std::string abilityId = entityData.mapAbilityIds.at(i);
            const CAbilityData& abilityData = assetManager_->GetGameData().mapAbilityData.at(abilityId);
            m_vecAbilities[i] = {
                .strName = abilityData.strName,
                .hIcon = assetManager_->GetBitmapImage(abilityData.iconId),
                .eTargetType = abilityData.eTargetType,
            };
        }
    }

    if (game_objects_.find(unitId) != game_objects_.end()) {
        // already spawned!
        Logger::FormatMsg("Unit with id %d already spawned", unitId);
        return;
    }

    m_gameState.vecUnits.push_back(unitId);

    auto entMapIt = assetManager_->GetGameData().mapCharacterData.find(entityId);

    if(entMapIt == assetManager_->GetGameData().mapCharacterData.end()) {
        Logger::FormatMsg("Attempt to spawn unknown unit type %s", entityId);
        return;
    }

    const CCharacterData& entData = entMapIt->second;

    GameObject* go = new GameObject();
    go->uPrefab = entData.strId;
    go->team = team;
    go->unit_id = unitId;
    UnitId entId = go->unit_id;
    m_gameState.AddRenderable(entId)->strRenderable = entData.modelId;
    
    if(entData.optHealthData) {
        HealthComponent_t* health = m_gameState.AddHealth(entId);
        health->nMaxHealth = entData.optHealthData.value().nMaxHealth;
        health->nHealth = health->nMaxHealth;
        go->has_healthbar = true;
    }

    if(entData.optTransformData) {
        TransformComponent_t* transform = m_gameState.AddTransform(entId);
        transform->vec3Position = pos;
        transform->vec3Rotation = {0, 0, 0};
    }

    if(entData.optMovementData) {
        MovementComponent_t* movement = m_gameState.AddMovement(entId);
        movement->vec3Target = pos;
    }

    if(entData.optTargetableData) {
        m_gameState.AddTargetable(entId);
    }

    if(entData.optAudioData) {
        AudioEmitterComponent_t* audio = m_gameState.AddAudioEmitter(entId);
        audio->hEmitter = handler_->GetAudioEngine()->CreateEmitter(pos);
    }

    if(!assetManager_->GetGameData().mapModelData.at(entData.modelId).mapAnimations.empty()) {
        AnimationComponent_t* animation = m_gameState.AddAnimation(entId);
        animation->m_strAnimationName = "idle";
        animation->m_bLoop = true;
        animation->m_fAnimationTime = 0.0f;
    }

    // TODO do we need these here
    if(entData.optNavigationData) {}
    if(entData.optNetworkData) {}
    if(entData.optIntentData) {}

    game_objects_.emplace(unitId, go);

    // TODO need to figure out if it's a player controlled unit or no
    // go->has_title = false;

    Logger::FormatMsg("Spawned new unit %d from template %s at <%f, %f, %f>", unitId, entityId.c_str(), pos.x, pos.y, pos.z);
}

void Game::DespawnUnit(uint64_t unitId) {
    GameObject* go = GetGameObject(unitId);

    if (go == nullptr) {
        return;
    }

    go->destroy = true;
}

void Game::HandleTicks(float dt) {
    /*
    for (auto go_it : game_objects_) {
        GameObject* pGameObject = go_it.second;

        TransformComponent_t* pTransformComp = m_gameState.GetTransform(pGameObject->unit_id);
        MovementComponent_t* pMovementComponent = m_gameState.GetMovement(pGameObject->unit_id);
        if (!pMovementComponent || !pMovementComponent->bIsMoving) {
            continue;
        }

        Vector3 vec3Move = { pMovementComponent->vec3Target.x - pTransformComp->vec3Position.x, 0, pMovementComponent->vec3Target.z - pTransformComp->vec3Position.z};

        if(vec3Move.Length() < 1) {
            // this must mean we arrived
            pMovementComponent->vec3Target = Vector3::ZERO;
            pMovementComponent->bIsMoving = false;
            continue;
        }

        if (vec3Move.Length() > 330 * (16.6666f / 1000.0f)) {
            vec3Move = vec3Move.Normalize().ScaleToLength(330.0f * (16.6666f / 1000.0f));
        }
        
        pTransformComp->vec3Position = pTransformComp->vec3Position + vec3Move;
        pTransformComp->vec3Rotation.y = CalculateAngle({ pTransformComp->vec3Position.x, pTransformComp->vec3Position.z }, { pMovementComponent->vec3Target.x, pMovementComponent->vec3Target.z });

    }
    */

    // === Tick simulation logic ===
    // I am trying to do the thing that some valve dev or other talked about in the past - interpolate between world snapshots
    // So I am saving whatever packets I am getting to whatever frame the server says it's working on at the moment.
    // Now I need to interpolate from a while back towards the current packet.
    // I guess the difficulty is never running past our current frame, and not falling behind, and not getting out of order...
    // Right now I want to try to interpolate between 2 frames - one that is 2 frames back and the most recent completed frame.
    // My hope is that I will never need to interpolate to the current frame.
    // But I don't think that's realistic.
    // So here we go now.
    if (ticks.size() <= 2) {
        // we do not yet have 2 ticks to simulate
        m_fCurrentFrameDelta = 0;
        current_tick_ = 0;
        Logger::Msg("Waiting for enough frames to start simulating");
        return;
    }

    //  Below is one of my old crashouts. Maybe I won't need this now.
    /*
    if (current_tick_ == 0) {
        Logger::Msg("Gotta simulate that first tick somehow!");
        game_tick_t tick = ticks[0];
        SimulateTick(tick, 1);
    }

    // now we're simming

    // first lets see if we need to catch up
    // this should probably not happen a lot... might look real bad
    while (current_tick_ < (ticks.size() - 3)) {
        // we do, so simulate completed ticks till we're there
        // Logger::Msg("Simulation is catching up...");
        game_tick_t tick = ticks[current_tick_];
        SimulateTick(tick, 1);
        current_tick_++;
        m_fCurrentFrameDelta = 0;
    }
    // all right, we're simming no more than 2 frames in the past.
    // now let's make sure we actually have something to sim to...
    if ((current_tick_ + 1) >= ticks.size()) {
        // we do, so simulate completed ticks till we're there
        // Logger::Msg("Simulation is caught up, but has nothing to simulate to!");
        return;
    }
    */


    // we're here! we have something to do!

    // let's add the new delta, see what happens
    m_fCurrentFrameDelta += dt / (1000.0 / 60.0);
    
    if (m_fCurrentFrameDelta > 1.0) {
        // whoops we blew into a new frame
        // let's sim that
        if(current_tick_ + 1 == ticks.size()) {
            Logger::Err("ALARM! we are now ahead of the server?!. This is a problem");
            m_fCurrentFrameDelta = 0.0;
            current_tick_ -= 1;
            return;
        }

        game_tick_t tick = ticks[current_tick_ + 1];
        SimulateTick(tick, 1);
        current_tick_++;
        m_fCurrentFrameDelta -= 1.0;
    }

    if (m_fCurrentFrameDelta > 1.0) {
        Logger::FormatErr("We are trying to simulate but we're already past this frame %d. This should _NEVER_ happen!", current_tick_);
    }

    if((current_tick_ + 1) >= ticks.size()) {
        Logger::FormatErr("ALARM! no more frames after %d. This should _NEVER_ happen!", current_tick_);
        return;
    }

    game_tick_t targetTick = ticks[current_tick_ + 1];
    SimulateTick(targetTick, m_fCurrentFrameDelta);
}

void Game::SimulateTick(game_tick_t& tick, double diff) {
    // ignore tick packet header pls
    int offset = sizeof(packet_header_t);
    packet_header_t header{};

    for(std::vector<uint8_t> vecData : tick.Data) {
        std::memcpy(&header, vecData.data(), sizeof(header));
        if(header.type == PacketType::UNITSPAWN){
            SpawnPacket spawn{};
            spawn.Read(&vecData);
            SpawnUnit(spawn.unit, spawn.strEntId, spawn.team, Vector3{ spawn.x, spawn.y, spawn.z });
            continue;
        }
        if(header.type == PacketType::UNITIDLE){
            UnitIdlePacket idle{};
            idle.Read(&vecData);

            GameObject* go = GetGameObject(idle.unit);

            if (go == nullptr) {
                Logger::Err("Received idle command for object that does not exist!");
                continue;
            }

            if(MovementComponent_t* pMoveComp = m_gameState.GetMovement(go->unit_id)) {
                pMoveComp->vec3Target = Vector3::ZERO;
                pMoveComp->bIsMoving = false;
            }

            if(TransformComponent_t* pTransformComp = m_gameState.GetTransform(idle.unit)) {
                pTransformComp->vec3Position.x = idle.x;
                pTransformComp->vec3Position.y = idle.y;
                pTransformComp->vec3Position.z = idle.z;
                // go->rotation.y = go->rotation.y + (idle.r - go->rotation.y) * diff;
                pTransformComp->vec3Rotation.y = idle.r; // this actually looks less fucked for now :O
            }

            go->bIsAttacking = false;
            go->bIsCasting = false;
            continue;
        }
        if(header.type == PacketType::UNITMOVE){
            UnitMovePacket move{};
            move.Read(&vecData);

            GameObject* go = GetGameObject(move.unit);

            if (go == nullptr) {
                Logger::Err("Received move command for object that does not exist!");
                continue;
            }

            go->bIsCasting = false;
            go->bIsAttacking = false;

            TransformComponent_t* transform = m_gameState.GetTransform(move.unit);

            //  We're trying to go here
            Vector3 vec3Dest = { move.x, move.y, move.z };
            // And we're here
            Vector3 vec3Pos = transform->vec3Position;
            // So this is our movement vector now
            Vector3 vec3Move = vec3Dest - vec3Pos;
            
            // To get there by the end of the frame we have
            double dRemaining = 1.0 - diff;

            // If we just run we'll manage
            double dPredictedDistance = 330.0 / (1000.0 / 16.66666) * dRemaining;

            Vector3 vec3ActualMove = vec3Move.ScaleToLength(330.0 / (1000.0 / 16.66666) * dRemaining);
            transform->vec3Position = vec3Dest - vec3ActualMove;

            transform->vec3Rotation.y = move.r; // move.r; // this actually looks less fucked for now :O
            continue;
        }
        if(header.type == PacketType::UNITDESPAWN){
            Logger::Msg("despawn!!");
            AddPacketToCurrentTick(vecData);
            continue;
        }
        if(header.type == PacketType::PCK_STATS){
            UnitStatsPacket stats{};
            stats.Read(&vecData);

            GameObject* go = GetGameObject(stats.unit);

            if (go == nullptr) {
                Logger::Msg("WARNING: received stats message for unknown object");
                continue;
            }
            
            HealthComponent_t* pHealthComp = m_gameState.GetHealth(go->unit_id);
            
            if(pHealthComp) {
                pHealthComp->nHealth = stats.health;
                pHealthComp->nMaxHealth = stats.max_health;
            }
            continue;
        }
        if(header.type == PacketType::PCK_SPELL_COOLDOWN){
            CooldownPacket cd{};
            cd.Read(&vecData);

            if (cd.unit != my_unit_id_) {
                continue;
            }

            cooldowns[cd.spell_slot] = cd.cooldown;
            total_cooldowns[cd.spell_slot] = cd.total_cooldown;
            continue;
        }
        if(header.type == PacketType::PCK_PLAY_PARTICLE){
            PlayParticlePacket part{};
            part.Read(&vecData);

            ParticleEffect* particle_system = ParticleEffect::Load(part.particle, assetManager_);

            GameObject* go = GetGameObject(part.unit);
            if (part.unit != 0 && go != nullptr) {
                particle_system->Attach(go);
            }
            else {
                particle_system->position.x = part.x;
                particle_system->position.y = 0;
                particle_system->position.z = part.y;
            }

            game_objects_.emplace(current_tick_, particle_system);
            continue;
        }
        if(header.type == PacketType::PCK_ATTACK_START){
            AttackStartPacket pck{};
            pck.Read(&vecData);

            GameObject* go = GetGameObject(pck.content.unit);
            go->bIsAttacking = true;
            
            if(MovementComponent_t* pMoveComp = m_gameState.GetMovement(go->unit_id)) {
                pMoveComp->vec3Target = Vector3::ZERO;
                pMoveComp->bIsMoving = false;
            }

            if(TransformComponent_t* transform = m_gameState.GetTransform(pck.content.unit)) {
                if(TransformComponent_t* targetTransform = m_gameState.GetTransform(pck.content.target)) {
                    targetTransform->vec3Rotation.y = CalculateAngle({transform->vec3Position.x, transform->vec3Position.z}, {targetTransform->vec3Position.x, targetTransform->vec3Position.z});
                }
            }
            

            CAttackStartEvent* pAttackEvent = new CAttackStartEvent();
            pAttackEvent->idUnit = go->unit_id;
            pAttackEvent->hSound = m_hStormcallerAttack;
            m_gameState.EmitEvent(pAttackEvent);

            if(AnimationComponent_t* pAnim = m_gameState.GetAnimation(pck.content.unit)) {
                pAnim->m_strAnimationName = "attack1";
                pAnim->m_bLoop = true;
                pAnim->m_fAnimationTime = 0.0f;
            }
            continue;
        }
        if(header.type == PacketType::SCORE_UPDATE_PACKET){
            ScoreUpdatePacket pck{};
            pck.Read(&vecData);

            m_iTeam1Score = pck.usTeam1Score;
            m_iTeam2Score = pck.usTeam2Score;
            continue;
        }

        if(header.type == PacketType::PCK_ATTACK_FINISHED) {
            CAttackFinishedPacket pck{};
            pck.Read(&vecData);

            GameObject* go = GetGameObject(pck.content.unit);
            // go->bIsAttacking = false;
        }

        if(header.type == PacketType::UNITMOVE_INTENTION) {
            UnitMoveIntentionPacket move{};
            move.Read(&vecData);

            GameObject* go = GetGameObject(move.unit);

            if (go == nullptr) {
                Logger::Err("Received move intention for object that does not exist!");
                continue;
            }

            MovementComponent_t* pMovementComponent = m_gameState.GetMovement(go->unit_id);
            TransformComponent_t* pTransform = m_gameState.GetTransform(move.unit);
            if(pMovementComponent == nullptr || pTransform == nullptr) {
                Logger::FormatErr("Received unit move intention for unit %u which has no move component", go->unit_id);
                continue;
            }
            Vector3 vec3NewTarget = { move.x, 0, move.z };
            if((pTransform->vec3Position - vec3NewTarget).Length() < 1) {
                pMovementComponent->vec3Target = Vector3::ZERO;
                pMovementComponent->bIsMoving = false;
                continue;
            }

            if(pMovementComponent->vec3Target == vec3NewTarget) {
                continue;
            }

            pMovementComponent->vec3Target = vec3NewTarget;
            pMovementComponent->bIsMoving = true;
        }

        if(header.type == PacketType::PCK_SPELL_CAST_START) {
            SpellCastStartPacket pck;
            pck.Read(&vecData);

            GameObject* go = GetGameObject(pck.unit);

            if (go == nullptr) {
                // TODO
                Logger::Err("Received spell cast start for unknown unit");
                continue;
            }

            if(TransformComponent_t* pTransform = m_gameState.GetTransform(pck.unit)) {
                if(TransformComponent_t* pTarget = m_gameState.GetTransform(pck.idTarget)) {
                    pTarget->vec3Rotation.y = CalculateAngle({pTransform->vec3Position.x, pTransform->vec3Position.z}, {pTarget->vec3Position.x, pTarget->vec3Position.z});
                }
            }

            go->bIsCasting = true;

            if(MovementComponent_t* pMoveComp = m_gameState.GetMovement(go->unit_id)) {
                pMoveComp->vec3Target = Vector3::ZERO;
                pMoveComp->bIsMoving = false;
            }
        
        }

        if(header.type == PacketType::PCK_SPELL_HIT) {
            SpellHitPacket pck;
            pck.Read(&vecData);

            GameObject* go = GetGameObject(pck.unit);

            if (go == nullptr) {
                // TODO
                Logger::Err("Received spell cast hit for unknown unit");
                continue;
            }

            // TODO react to event instead of this?
            ParticleEffect* particle_system = ParticleEffect::Load("assets/characters/stormcaller/abilities/" + pck.spell + ".pts", assetManager_);
            particle_system->Attach(go);

            if(!m_gameState.GetParticle(go->unit_id)) {
                m_gameState.AddParticle(go->unit_id);
                particle_system->Attach(go);
            }

            ParticleComponent_t* pParticleComp = m_gameState.GetParticle(go->unit_id);
            pParticleComp->vecEffects.push_back(particle_system);

            CSpellHitEvent* pHitEvent = new CSpellHitEvent();
            pHitEvent->idUnit = go->unit_id;
            pHitEvent->hSound = m_hThunderstrikeSound;
            m_gameState.EmitEvent(pHitEvent);
        }

        if(header.type == PacketType::PCK_UNIT_DEATH) {
            CUnitDeathPacket pck;
            pck.Read(&vecData);

            GameObject* go = GetGameObject(pck.idUnit);

            if (go == nullptr) {
                // TODO
                Logger::Err("Received death for unknown unit");
                continue;
            }
            
            go->bIsCasting = false;
            go->dead = true;
            m_pAudioSystem->PlaySoundOnUnit(m_hStormcallerDeath, go->unit_id);
        }

        if(header.type == PacketType::PCK_UNIT_RESPAWN) {
            CUnitRespawnPacket pck;
            pck.Read(&vecData);

            GameObject* go = GetGameObject(pck.idUnit);

            if (go == nullptr) {
                // TODO
                Logger::Err("Received death for unknown unit");
                return;
            }
            
            go->bIsCasting = false;
            go->bIsAttacking = false;
            go->dead = false;
        }

        Logger::Err("Received unknown packet type");
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
    UnitIdPacket pck = UnitIdPacket();
    pck.Read(&data);

    my_unit_id_ = pck.unit_id;
    unit_id_received_ = true;
}

void Game::HandleGameTickPacket(std::vector<uint8_t> data) {
    ticks.push_back(m_receivingTick);

    game_tick_t new_tick{};
    new_tick.received = Util::GetSystemTime();
    m_receivingTick = new_tick;
}

void Game::AddPacketToCurrentTick(std::vector<uint8_t> data) {
    m_receivingTick.Data.push_back(data);
}

void Game::Action(EInputAction eAction) {
    if(eAction == EInputAction::GAME_ESCAPE) {
        handler_->OpenMainMenu();
    }

    if(eAction == EInputAction::GAME_FOCUS_UNIT) {
        m_playerInput.bFocusUnit = true;
    }

    if(eAction == EInputAction::GAME_STOP) {
        StopCommandPacket stop = StopCommandPacket();

        net_manager_->SendPacket(&stop);
    }

    if(eAction == EInputAction::GAME_SECONDARY) {
        if (pObjectUnderCursor && pObjectUnderCursor->has_healthbar) {
			last_move = 150;

            AttackCommandPacket atk_pk = AttackCommandPacket();
            atk_pk.target_unit = pObjectUnderCursor->unit_id;

            net_manager_->SendPacket(&atk_pk);
		} else if (!pObjectUnderCursor) {
            float x, y;
            TestIntersect(renderer, m_mousePos[0], m_mousePos[1], &x, &y);

            MoveCommandPacket mv = MoveCommandPacket();
            mv.x = x;
            mv.y = y;
            net_manager_->SendPacket(&mv);

            ParticleEffect* particle_system = ParticleEffect::Load("assets/UI/move-to/move_to.pts", assetManager_);
            particle_system->position = { x, 0, y };
            m_vecGlobalParticles.push_back(particle_system);

            last_move = 150;
        }
    }

    if(eAction == EInputAction::GAME_CAST_SPELL_1) {
        switch(m_vecAbilities[0].eTargetType) {
            case EAbilityTargetType::UNIT:
                if (pObjectUnderCursor) {
                    CastTargetCommandPacket cmd = CastTargetCommandPacket();
                    cmd.spell_slot = 0;
                    cmd.target = pObjectUnderCursor->unit_id;
                    net_manager_->SendPacket(&cmd);
                }
                break;
            default:
                float x, y;
                TestIntersect(renderer, m_mousePos[0], m_mousePos[1], &x, &y);
                
                CastCommandPacket cast = CastCommandPacket();
                cast.spell_slot = 0;
                cast.x = x;
                cast.y = 0;
                cast.z = y;
                net_manager_->SendPacket(&cast);
                break;
        }
    }
    if(eAction == EInputAction::GAME_CAST_SPELL_2) {
        switch(m_vecAbilities[0].eTargetType) {
            case EAbilityTargetType::UNIT:
                if (pObjectUnderCursor) {
                    CastTargetCommandPacket cmd = CastTargetCommandPacket();
                    cmd.spell_slot = 1;
                    cmd.target = pObjectUnderCursor->unit_id;
                    net_manager_->SendPacket(&cmd);
                }
                break;
            default:
                float x, y;
                TestIntersect(renderer, m_mousePos[0], m_mousePos[1], &x, &y);
                
                CastCommandPacket cast = CastCommandPacket();
                cast.spell_slot = 1;
                cast.x = x;
                cast.y = 0;
                cast.z = y;
                net_manager_->SendPacket(&cast);
                break;
        }
    }
    if(eAction == EInputAction::GAME_CAST_SPELL_3) {
        switch(m_vecAbilities[0].eTargetType) {
            case EAbilityTargetType::UNIT:
                if (pObjectUnderCursor) {
                    CastTargetCommandPacket cmd = CastTargetCommandPacket();
                    cmd.spell_slot = 2;
                    cmd.target = pObjectUnderCursor->unit_id;
                    net_manager_->SendPacket(&cmd);
                }
                break;
            default:
                float x, y;
                TestIntersect(renderer, m_mousePos[0], m_mousePos[1], &x, &y);
                
                CastCommandPacket cast = CastCommandPacket();
                cast.spell_slot = 2;
                cast.x = x;
                cast.y = 0;
                cast.z = y;
                net_manager_->SendPacket(&cast);
                break;
        }
    }
    if(eAction == EInputAction::GAME_CAST_SPELL_4) {
        switch(m_vecAbilities[0].eTargetType) {
            case EAbilityTargetType::UNIT:
                if (pObjectUnderCursor) {
                    CastTargetCommandPacket cmd = CastTargetCommandPacket();
                    cmd.spell_slot = 3;
                    cmd.target = pObjectUnderCursor->unit_id;
                    net_manager_->SendPacket(&cmd);
                }
                break;
            default:
                float x, y;
                TestIntersect(renderer, m_mousePos[0], m_mousePos[1], &x, &y);
                
                CastCommandPacket cast = CastCommandPacket();
                cast.spell_slot = 3;
                cast.x = x;
                cast.y = 0;
                cast.z = y;
                net_manager_->SendPacket(&cast);
                break;
        }
    }
    
    if(eAction == EInputAction::GAME_SCROLL_LEFT) {
        m_playerInput.bKeyScrollLeft = true;
    }
    
    if(eAction == EInputAction::GAME_SCROLL_RIGHT) {
        m_playerInput.bKeyScrollRight = true;
    }
    
    if(eAction == EInputAction::GAME_SCROLL_UP) {
        m_playerInput.bKeyScrollUp = true;
    }
    
    if(eAction == EInputAction::GAME_SCROLL_DOWN) {
        m_playerInput.bKeyScrollDown = true;
    }
}


void Game::ActionReleased(EInputAction eAction) {
    if(eAction == EInputAction::GAME_SCROLL_LEFT) {
        m_playerInput.bKeyScrollLeft = false;
    }
    
    if(eAction == EInputAction::GAME_SCROLL_RIGHT) {
        m_playerInput.bKeyScrollRight = false;
    }
    
    if(eAction == EInputAction::GAME_SCROLL_UP) {
        m_playerInput.bKeyScrollUp = false;
    }
    
    if(eAction == EInputAction::GAME_SCROLL_DOWN) {
        m_playerInput.bKeyScrollDown = false;
    }

    if(eAction == EInputAction::GAME_FOCUS_UNIT) {
        m_playerInput.bFocusUnit = false;
    }
}