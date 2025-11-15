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
    m_navMesh = new NavMesh();
    m_navMesh->LoadFromData(handler->GetAssetManager()->LoadPlainFile("data/Maps/map1/map1.nvm"));

    m_navGrid = new NavigationCellGrid(m_navMesh);

    net_manager_ = server;

    packet_manager = NetworkHandlerManager<PacketType, std::function<void(std::vector<uint8_t>)>>();
    // Register network packets, the fuck...
    packet_manager.RegisterHandler(PacketType::PCK_ATTACK_FINISHED, [this](std::vector<uint8_t> data) {
        CAttackFinishedPacket pck{};
        pck.Read(&data);

        GameObject* go = GetGameObject(pck.content.unit);
        go->bIsAttacking = false;
    });
    packet_manager.RegisterHandler(PacketType::PCK_ATTACK_START, [this](std::vector<uint8_t> data) {
        AttackStartPacket pck{};
        pck.Read(&data);

        GameObject* go = GetGameObject(pck.content.unit);
        go->bIsAttacking = true;
        
        if(MovementComponent_t* pMoveComp = m_gameState.GetComponent<MovementComponent_t>(go->unit_id)) {
            pMoveComp->vec3Target = Vector3::ZERO;
            pMoveComp->bIsMoving = false;
        }

        GameObject* pTarget = GetGameObject(pck.content.target);
        if(pTarget != nullptr) {
            go->rotation.y = CalculateAngle({go->position.x, go->position.z}, {pTarget->position.x, pTarget->position.z});
        }

        CAttackStartEvent* pAttackEvent = new CAttackStartEvent();
        pAttackEvent->idUnit = go->unit_id;
        pAttackEvent->hSound = m_hStormcallerAttack;
        m_gameState.EmitEvent(pAttackEvent);
    });
    packet_manager.RegisterHandler(PacketType::PCK_CLIENT_UNIT_ID, [this](std::vector<uint8_t> data) { HandleUnitIdPacket(data); });
    packet_manager.RegisterHandler(PacketType::GAME_TICK, [this](std::vector<uint8_t> data) { HandleGameTickPacket(data); });
    packet_manager.RegisterHandler(PacketType::UNITSPAWN, [this](std::vector<uint8_t> data) {
        SpawnPacket spawn{};
        spawn.Read(&data);
        SpawnUnit(spawn.unit, spawn.unit_type, spawn.team, Vector3{ spawn.x, spawn.y, spawn.z });
    });
    packet_manager.RegisterHandler(PacketType::UNITMOVE, [this](std::vector<uint8_t> data) { 
        UnitMovePacket move{};
        move.Read(&data);

        GameObject* go = GetGameObject(move.unit);

        if (go == nullptr) {
            Logger::Err("Received move command for object that does not exist!");
            return;
        }

        go->bIsCasting = false;
        go->bIsAttacking = false;

        Vector3 vec3Move = { move.x, move.y, move.z };
        if((go->position - vec3Move).Length() > 100) {
            Logger::FormatMsg("snap: %f", (go->position - vec3Move).Length());
            go->position = vec3Move;
        } else if ((go->position - vec3Move).Length() > 5) {
            Logger::FormatMsg("diff: %f", (go->position - vec3Move).Length());

            Vector3 vec3Catchup = (vec3Move - go->position).ScaleToLength(((vec3Move - go->position).Length() - 5) / 10);
            go->position = go->position + vec3Catchup;
//            go->position.x = vec3Move.x;
//            go->position.y = vec3Move.y;
//            go->position.z = vec3Move.z;
        }

        go->rotation.y = move.r; // this actually looks less fucked for now :O
    });
    packet_manager.RegisterHandler(PacketType::UNITIDLE, [this](std::vector<uint8_t> data) {
        UnitIdlePacket idle{};
        idle.Read(&data);

        GameObject* go = GetGameObject(idle.unit);

        if (go == nullptr) {
            Logger::Err("Received idle command for object that does not exist!");
            return;
        }

        if(MovementComponent_t* pMoveComp = m_gameState.GetComponent<MovementComponent_t>(go->unit_id)) {
            pMoveComp->vec3Target = Vector3::ZERO;
            pMoveComp->bIsMoving = false;
        }

        go->bIsAttacking = false;
        go->bIsCasting = false;
        go->position.x = idle.x;
        go->position.y = idle.y;
        go->position.z = idle.z;
        // go->rotation.y = go->rotation.y + (idle.r - go->rotation.y) * diff;
        go->rotation.y = idle.r; // this actually looks less fucked for now :O
    });
    packet_manager.RegisterHandler(PacketType::PCK_PLAY_PARTICLE, [this](std::vector<uint8_t> data) {
        PlayParticlePacket part{};
        part.Read(&data);

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
    });
    packet_manager.RegisterHandler(PacketType::UNITDESPAWN, [this](std::vector<uint8_t> data) {
        Logger::Msg("despawn!!");
        AddPacketToCurrentTick(data);
    });
    packet_manager.RegisterHandler(PacketType::PCK_STATS, [this](std::vector<uint8_t> data) {
        UnitStatsPacket stats{};
        stats.Read(&data);

        GameObject* go = GetGameObject(stats.unit);

        if (go == nullptr) {
            Logger::Msg("WARNING: received stats message for unknown object");
            return;
        }
	
	HealthComponent_t* pHealthComp = m_gameState.GetComponent<HealthComponent_t>(go->unit_id);
	
	if(pHealthComp) {
		pHealthComp->nHealth = stats.health;
		pHealthComp->nMaxHealth = stats.max_health;
	}
    });
    packet_manager.RegisterHandler(PacketType::PCK_SPELL_COOLDOWN, [this](std::vector<uint8_t> data) {
        CooldownPacket cd{};
        cd.Read(&data);

        if (cd.unit != my_unit_id_) {
            return;
        }

        cooldowns[cd.spell_slot] = cd.cooldown;
        total_cooldowns[cd.spell_slot] = cd.total_cooldown;
    });
    packet_manager.RegisterHandler(PacketType::SCORE_UPDATE_PACKET, [this](std::vector<uint8_t> data) {
        ScoreUpdatePacket pck{};
        pck.Read(&data);

        m_iTeam1Score = pck.usTeam1Score;
        m_iTeam2Score = pck.usTeam2Score;
    });
    packet_manager.RegisterHandler(PacketType::GAME_END_PACKET, [this](std::vector<uint8_t> data) { m_bGameHasEnded = true; });
    packet_manager.RegisterHandler(PacketType::UNITMOVE_INTENTION, [this](std::vector<uint8_t> data) {
        UnitMoveIntentionPacket move{};
        move.Read(&data);

        GameObject* go = GetGameObject(move.unit);

        if (go == nullptr) {
            Logger::Err("Received move intention for object that does not exist!");
            return;
        }

        MovementComponent_t* pMovementComponent = m_gameState.GetComponent<MovementComponent_t>(go->unit_id);

        if(pMovementComponent == nullptr) {
            Logger::FormatErr("Received unit move intention for unit %u which has no move component", go->unit_id);
            return;
        }
        Vector3 vec3NewTarget = { move.x, 0, move.z };
        if((go->position - vec3NewTarget).Length() < 1) {
            pMovementComponent->vec3Target = Vector3::ZERO;
            pMovementComponent->bIsMoving = false;
            return;
        }

        if(pMovementComponent->vec3Target == vec3NewTarget) {
            return;
        }

        pMovementComponent->vec3Target = vec3NewTarget;
        pMovementComponent->bIsMoving = true;
    });
    packet_manager.RegisterHandler(PacketType::PCK_SPELL_CAST_START, [this](std::vector<uint8_t> data) {
        SpellCastStartPacket pck;
        pck.Read(&data);

        GameObject* go = GetGameObject(pck.unit);

        if (go == nullptr) {
            // TODO
            Logger::Err("Received spell cast start for unknown unit");
            return;
        }

        GameObject* pTarget = GetGameObject(pck.idTarget);
        if(pTarget != nullptr) {
            go->rotation.y = CalculateAngle({go->position.x, go->position.z}, {pTarget->position.x, pTarget->position.z});
        }

        go->bIsCasting = true;

        if(MovementComponent_t* pMoveComp = m_gameState.GetComponent<MovementComponent_t>(go->unit_id)) {
            pMoveComp->vec3Target = Vector3::ZERO;
            pMoveComp->bIsMoving = false;
        }
    });
    packet_manager.RegisterHandler(PacketType::PCK_SPELL_HIT, [this](std::vector<uint8_t> data) {
        SpellHitPacket pck;
        pck.Read(&data);

        GameObject* go = GetGameObject(pck.unit);

        if (go == nullptr) {
            // TODO
            Logger::Err("Received spell cast start for unknown unit");
            return;
        }

        // TODO react to event instead of this?
        ParticleEffect* particle_system = ParticleEffect::Load("data/characters/stormcaller/abilities/" + pck.spell + ".pts", assetManager_);
        particle_system->Attach(go);

        if(!m_gameState.GetComponent<ParticleComponent_t>(go->unit_id)) {
            m_gameState.AddComponent<ParticleComponent_t>(go->unit_id);
            particle_system->Attach(go);
        }

        ParticleComponent_t* pParticleComp = m_gameState.GetComponent<ParticleComponent_t>(go->unit_id);
        pParticleComp->vecEffects.push_back(particle_system);

        CSpellHitEvent* pHitEvent = new CSpellHitEvent();
        pHitEvent->idUnit = go->unit_id;
        pHitEvent->hSound = m_hThunderstrikeSound;
        m_gameState.EmitEvent(pHitEvent);
    });
    packet_manager.RegisterHandler(PacketType::PCK_UNIT_DEATH, [this](std::vector<uint8_t> data) {
        CUnitDeathPacket pck;
        pck.Read(&data);

        GameObject* go = GetGameObject(pck.idUnit);

        if (go == nullptr) {
            // TODO
            Logger::Err("Received death for unknown unit");
            return;
        }
        
        go->bIsCasting = false;
        go->dead = true;
        m_pAudioSystem->PlaySoundOnUnit(m_hStormcallerDeath, go->unit_id);
    });
    packet_manager.RegisterHandler(PacketType::PCK_UNIT_RESPAWN, [this](std::vector<uint8_t> data) {
        CUnitRespawnPacket pck;
        pck.Read(&data);

        GameObject* go = GetGameObject(pck.idUnit);

        if (go == nullptr) {
            // TODO
            Logger::Err("Received death for unknown unit");
            return;
        }
        
        go->bIsCasting = false;
        go->bIsAttacking = false;
        go->dead = false;
    });

    net_manager_->Initialize(&packet_manager);

    this->assetManager_ = handler->GetAssetManager();
    this->renderer = handler->GetRenderer();

    m_iTeam1Score = 0;
    m_iTeam2Score = 0;
    m_bGameHasEnded = false;

    // TODO
    m_pAudioSystem = new AudioSystem(handler->GetAudioEngine(), handler->GetAssetManager());
    m_hGenericIcon = handler->GetAssetManager()->GetBitmapImage("data/persons/shared/ability-icon.bmp");
    m_hThunderstrikeSound = handler->GetAssetManager()->LoadSound("data/characters/stormcaller/abilities/thunderstrike.wav");
    m_hStormcallerDeath = handler->GetAssetManager()->LoadSound("data/characters/stormcaller/death.wav");
    m_hStormcallerAttack = handler->GetAssetManager()->LoadSound("data/characters/stormcaller/attack.wav");

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
	Ray ray = ScreenToRay({ static_cast<float>(screenX), static_cast<float>(screenY) },
		{ renderer->m_camera.position.x, renderer->m_camera.position.y, renderer->m_camera.position.z },
		{ renderer->m_camera.rotation.x, renderer->m_camera.rotation.y, renderer->m_camera.rotation.z },
		(float)windowWidth_ / (float)windowHeight_,
		renderer->m_camera.fov * hp,
		renderer->m_camera.nearClip,
		renderer->m_camera.farClip,
		windowWidth_,
		windowHeight_);

    for(auto go_it : game_objects_) {
        GameObject* go = go_it.second;
        Capsule_t capsule = Capsule_t {
            .vec3Start = Vector3(go->position.x, 0, go->position.z),
            .vec3End = Vector3(go->position.x, 200, go->position.z),
            .fRadius = 50,
        };

		if (TestCollision(ray, capsule)) {
			// TODO does this work for multiple objects right behind each other?
			pObjectUnderCursor = go;
			break;
		}
    }
    
    if(pObjectUnderCursor && m_gameState.GetComponent<TargetableComponent_t>(pObjectUnderCursor->unit_id)) {
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

        if (go->dead) {
            desiredAnim = "death";
            bLoop = false;
        } else if (m_gameState.GetComponent<MovementComponent_t>(go->unit_id) && m_gameState.GetComponent<MovementComponent_t>(go->unit_id)->bIsMoving) {
            desiredAnim = "run";
            bLoop = true;
        } else if(go->bIsCasting) {
            desiredAnim = "attack1";
            bLoop = false;
        } else if(go->bIsAttacking) {
            Logger::FormatMsg("PLAY ATTACK!");
            desiredAnim = "attack1";
            bLoop = false;
        } else {
            desiredAnim = "idle";
            bLoop = true;
        }

        AnimationComponent_t* pAnimComp = m_gameState.GetComponent<AnimationComponent_t>(go->unit_id);
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
            GameObject* my_unit = GetGameObject(my_unit_id_);
    
            if (my_unit != nullptr) {
                m_camDir[0] = 0;
                m_camDir[1] = 0;
                m_camPos[0] = my_unit->position.x;
                m_camPos[1] = 1500.0f;
                m_camPos[2] = my_unit->position.z + 800.0f;
            }
        }
        else {
            m_camPos[0] = 0;
            m_camPos[2] = -1000;
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
    for(IGameSystem* pSystem : m_gameState.m_vecGameSystems) {
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
        TransformComponent_t* pTransform = m_gameState.GetComponent<TransformComponent_t>(unit);

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

        /*
        Vector3 point_above = Vector3{ 0, 450, 0 };
        
        float hp = static_cast<float>(M_PI / 180.0);
        mat_t persp = PMathMatPerspectiveRH(renderer->m_camera.fov, (float)windowWidth_ / (float)windowHeight_, renderer->m_camera.nearClip, renderer->m_camera.farClip);
        
        mat_t view = PMathMatRotation(renderer->m_camera.rotation.z, renderer->m_camera.rotation.y, renderer->m_camera.rotation.x) * PMathMatTranslation(renderer->m_camera.position.x, renderer->m_camera.position.y, renderer->m_camera.position.z);
        
        mat_t transMat = PMathMatTranslation(0, 0, 0);
        
        Vector2 screen_point_above = WorldToScreen(point_above, transMat, persp, PMathMatTranspose(PMathMatInverse(view)));
        
        double x = (1.0f + screen_point_above.x) * 0.5 * windowWidth_;
        double y = (1.0f - screen_point_above.y) * 0.5 * windowHeight_;
        */
       double health_bar_height = 5;
       
        mat_t persp = PMathMatPerspectiveRH(59, 1024.0f / 768.0f, 0.1f, 10000.0f);

        mat_t view = PMathMatRotation(0, 0, -60) * PMathMatTranslation(0, 1500, 800);

        mat_t transMat = PMathMatTranslation(0, 0, 0);

        Vector4 point = Vector4(0, 0, 0, 1);
        point = transMat * point;
        point = PMathMatTranspose(PMathMatInverse(view)) * point; // now in camera space
        point = persp * point;
        point = Vector4(point.x / point.w, point.y / point.w, point.z / point.w, 0);
        double x = (1.0f + point.x) * 0.5 * windowWidth_;
        double y = (1.0f + point.y) * 0.5 * windowHeight_;

        /*
        Vector3 point_above = Vector3{ 0, -450, 0 };

        mat_t persp = PMathMatPerspectiveRH(ToRadians(renderer->m_camera.fov), (float)windowWidth_ / (float)windowHeight_, renderer->m_camera.nearClip, renderer->m_camera.farClip);

        mat_t view = PMathMatRollPitchYaw(ToRadians(-renderer->m_camera.rotation.z), ToRadians(-renderer->m_camera.rotation.x), ToRadians(-renderer->m_camera.rotation.y)) * PMathMatTranslation(-renderer->m_camera.position.x, -renderer->m_camera.position.y, -renderer->m_camera.position.z) ;
        view = view.inverse();
        mat_t transMat = PMathMatTranslation(go->position.x, go->position.y, go->position.z).Transpose();

        Vector2 screen_point_above = WorldToScreen(point_above, transMat, persp, view);
        
        Vector4 worldPos = {renderer->m_camera.position.x, renderer->m_camera.position.y, renderer->m_camera.position.z, 1.0f};
        Vector4 eyePos = view * worldPos;

        printf("Camera-space pos: x=%f y=%f z=%f w=%f\n", eyePos.x, eyePos.y, eyePos.z, eyePos.w);
        double x = (1.0f + screen_point_above.x) * 0.5 * windowWidth_;
        double y = (1.0f - screen_point_above.y) * 0.5 * windowHeight_;
        double health_bar_height = 5;
        */

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

        renderer->FillRect(x - 50, y - 10, ((float)m_gameState.GetComponent<HealthComponent_t>(go->unit_id)->nHealth / (float)m_gameState.GetComponent<HealthComponent_t>(go->unit_id)->nMaxHealth) * 100.0f, health_bar_height, color);
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

    if(HealthComponent_t* pHealthComp = m_gameState.GetComponent<HealthComponent_t>(my_go->unit_id)) {
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
        renderer->DrawImage(x + 1, y + 1, 48, 48, m_hGenericIcon);
        if (cooldowns[1] != -1) {
            float cd_remaining = (float)cooldowns[1] / (float)total_cooldowns[1];
            renderer->RenderPartialCover(x + 1, y + 1, 48, 48, cd_remaining);
        }
        renderer->RenderText(x, y, 50, 50, "W");

        x = windowWidth_ / 2 + 5;
        renderer->DrawRect(x, y, 50, 50, black);
        renderer->DrawImage(x + 1, y + 1, 48, 48, m_hGenericIcon);
        if (cooldowns[2] != -1) {
            float cd_remaining = (float)cooldowns[2] / (float)total_cooldowns[2];
            renderer->RenderPartialCover(x + 1, y + 1, 48, 48, cd_remaining);
        }
        renderer->RenderText(x, y, 50, 50, "E");

        x = windowWidth_ / 2 + 65;
        renderer->DrawRect(x, y, 50, 50, black);
        renderer->DrawImage(x + 1, y + 1, 48, 48, m_hGenericIcon);
        if (cooldowns[3] != -1) {
            float cd_remaining = (float)cooldowns[3] / (float)total_cooldowns[3];
            renderer->RenderPartialCover(x + 1, y + 1, 48, 48, cd_remaining);
        }
        renderer->RenderText(x, y, 50, 50, "R");
    }

    float hp = static_cast<float>(M_PI / 180.0);
    Ray ray = ScreenToRay({ static_cast<float>(m_mousePos[0]), static_cast<float>(m_mousePos[1]) },
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
        Sphere sphere(Vector3(go->position.x, 25, go->position.z), 50);
        if (TestCollision(ray, sphere) && go->has_healthbar && go->team != my_go->team) {
            handler_->RequestCursor(CursorId::ATTACK_MOVE);
            break;
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
    mat_t persp = PMathMatPerspectiveRH(renderer->m_camera.fov * hp, (float)windowWidth_ / (float)windowHeight_, renderer->m_camera.nearClip, renderer->m_camera.farClip);
    mat_t view = PMathMatRotation(renderer->m_camera.rotation.z, renderer->m_camera.rotation.y, renderer->m_camera.rotation.x) *
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
    SpawnUnit(unitId, 0, Team::TEAM_1, Vector3(0, 0, 0));
}

void Game::SpawnUnit(uint64_t unitId, uint64_t unit_type, Team team, Vector3 pos) {
    if(unitId == my_unit_id_) {

        switch(unit_type) {
            case UnitPrefab::FOOTBALL_PERSON:
            case UnitPrefab::STORMCALLER:
                m_vecAbilities.resize(4);
                m_vecAbilities[0] = {
                    .strName = "Thunderstrike",
                    .hIcon = assetManager_->GetBitmapImage("data/characters/stormcaller/abilities/thunderstrike_icon.png"),
                    .eTargetType = EAbilityTargetType::UNIT,
                };
                break;
                break;
            default:
                break;
        }
    }

    if (game_objects_.find(unitId) != game_objects_.end()) {
        // already spawned!
        return;
    }

    m_gameState.vecUnits.push_back(unitId);
    // Hacky missile hack
    if (unit_type == UnitPrefab::THROW_FOOTBALL) {
        GameObject* go = new GameObject();
        go->unit_id = unitId;
        m_gameState.AddComponent<HealthComponent_t>(unitId);
        m_gameState.GetComponent<HealthComponent_t>(unitId)->nHealth = 50;
        m_gameState.GetComponent<HealthComponent_t>(unitId)->nMaxHealth = 100;
        m_gameState.AddComponent<RenderableComponent_t>(unitId);
        m_gameState.GetComponent<RenderableComponent_t>(unitId)->strRenderable = "missile";
        go->position = pos;
        go->rotation = { 0, 0, 0 };
        go->has_healthbar = false;
        go->team = team;
        go->uPrefab = unit_type;
        game_objects_.emplace(unitId, go);
        return;
    }

    if (unit_type == UnitPrefab::FOOTBALL_PERSON) {
        GameObject* go = new GameObject();
        go->unit_id = unitId;
        m_gameState.AddComponent<HealthComponent_t>(unitId);
        m_gameState.GetComponent<HealthComponent_t>(unitId)->nHealth = 50;
        m_gameState.GetComponent<HealthComponent_t>(unitId)->nMaxHealth = 100;
        go->position = pos;
        go->rotation = { 0, 0, 0 };
        go->team = team;
        go->uPrefab = unit_type;
        m_gameState.AddComponent<TargetableComponent_t>(unitId);
        m_gameState.AddComponent<TransformComponent_t>(unitId);
        m_gameState.AddComponent<MovementComponent_t>(unitId);
        m_gameState.AddComponent<AudioEmitterComponent_t>(unitId);
        m_gameState.GetComponent<AudioEmitterComponent_t>(unitId)->hEmitter = handler_->GetAudioEngine()->CreateEmitter(pos);
        m_gameState.AddComponent<AnimationComponent_t>(unitId);
        m_gameState.GetComponent<AnimationComponent_t>(unitId)->m_strAnimationName = "idle";
        m_gameState.GetComponent<AnimationComponent_t>(unitId)->m_bLoop = true;
        m_gameState.GetComponent<AnimationComponent_t>(unitId)->m_fAnimationTime = 0.0f;
        m_gameState.AddComponent<RenderableComponent_t>(unitId);
        m_gameState.GetComponent<RenderableComponent_t>(unitId)->strRenderable = "football_person";
        game_objects_.emplace(unitId, go);
        return;
    }

    if (unit_type == UnitPrefab::STORMCALLER) {
        GameObject* go = new GameObject();
        go->unit_id = unitId;
        m_gameState.AddComponent<HealthComponent_t>(unitId);
        m_gameState.GetComponent<HealthComponent_t>(unitId)->nHealth = 50;
        m_gameState.GetComponent<HealthComponent_t>(unitId)->nMaxHealth = 100;
        go->position = pos;
        go->position = pos;
        go->rotation = { 0, 0, 0 };
        go->team = team;
        go->uPrefab = unit_type;
        m_gameState.AddComponent<TargetableComponent_t>(unitId);
        m_gameState.AddComponent<TransformComponent_t>(unitId);
        m_gameState.AddComponent<MovementComponent_t>(unitId);
        m_gameState.AddComponent<AudioEmitterComponent_t>(unitId);
        m_gameState.GetComponent<AudioEmitterComponent_t>(unitId)->hEmitter = handler_->GetAudioEngine()->CreateEmitter(pos);
        m_gameState.AddComponent<AnimationComponent_t>(unitId);
        m_gameState.GetComponent<AnimationComponent_t>(unitId)->m_strAnimationName = "idle";
        m_gameState.GetComponent<AnimationComponent_t>(unitId)->m_bLoop = true;
        m_gameState.GetComponent<AnimationComponent_t>(unitId)->m_fAnimationTime = 0.0f;
        m_gameState.AddComponent<RenderableComponent_t>(unitId);
        m_gameState.GetComponent<RenderableComponent_t>(unitId)->strRenderable = "stormcaller";
        game_objects_.emplace(unitId, go);
        return;
    }

    if (unit_type == UnitPrefab::TOWER) {
        GameObject* go = new GameObject();
        go->unit_id = unitId;
        go->position = pos;
        m_gameState.AddComponent<HealthComponent_t>(unitId);
        m_gameState.GetComponent<HealthComponent_t>(unitId)->nHealth = 50;
        m_gameState.GetComponent<HealthComponent_t>(unitId)->nMaxHealth = 100;
        m_gameState.AddComponent<RenderableComponent_t>(unitId);
        m_gameState.GetComponent<RenderableComponent_t>(unitId)->strRenderable = "tower";
        go->position = pos;
        go->rotation = { 0, 0, 0 };
        go->has_healthbar = true;
        go->has_title = false;
        go->team = team;
        go->uPrefab = unit_type;
        game_objects_.emplace(unitId, go);
        return;
    }

    if (unit_type == UnitPrefab::GENERIC_EMPTY) {
        GameObject* go = new GameObject();
        go->unit_id = unitId;
        m_gameState.AddComponent<HealthComponent_t>(unitId);
        m_gameState.GetComponent<HealthComponent_t>(unitId)->nHealth = 50;
        m_gameState.GetComponent<HealthComponent_t>(unitId)->nMaxHealth = 100;
        go->position = pos;
        go->position = pos;
        go->rotation = { 0, 0, 0 };
        go->has_healthbar = false;
        go->has_title = false;
        go->team = team;
        go->uPrefab = unit_type;
        game_objects_.emplace(unitId, go);
        return;
    }

    if (unit_type == UnitPrefab::MINION) {
        GameObject* go = new GameObject();
        go->unit_id = unitId;
        m_gameState.AddComponent<HealthComponent_t>(unitId);
        m_gameState.GetComponent<HealthComponent_t>(unitId)->nHealth = 50;
        m_gameState.GetComponent<HealthComponent_t>(unitId)->nMaxHealth = 100;
        m_gameState.AddComponent<RenderableComponent_t>(unitId);
        m_gameState.GetComponent<RenderableComponent_t>(unitId)->strRenderable = "minion";
        m_gameState.AddComponent<AnimationComponent_t>(unitId);
        m_gameState.GetComponent<AnimationComponent_t>(unitId)->m_strAnimationName = "idle";
        m_gameState.AddComponent<TransformComponent_t>(unitId);
        m_gameState.AddComponent<MovementComponent_t>(unitId);
        go->position = pos;
        go->rotation = { 0, 0, 0 };
        go->has_healthbar = true;
        go->has_title = false;
        go->team = team;
        go->uPrefab = unit_type;
        m_gameState.AddComponent<TargetableComponent_t>(unitId);
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
    for (auto go_it : game_objects_) {
        GameObject* pGameObject = go_it.second;

        TransformComponent_t* pTransformComp = m_gameState.GetComponent<TransformComponent_t>(pGameObject->unit_id);
        if(pTransformComp) {
            pTransformComp->vec3Position = pGameObject->position;
            pTransformComp->vec3Rotation = pGameObject->rotation;
        }

        MovementComponent_t* pMovementComponent = m_gameState.GetComponent<MovementComponent_t>(pGameObject->unit_id);
        if (!pMovementComponent || !pMovementComponent->bIsMoving) {
            continue;
        }

        Vector3 vec3Move = { pMovementComponent->vec3Target.x - pGameObject->position.x, 0, pMovementComponent->vec3Target.z - pGameObject->position.z};

        if(vec3Move.Length() < 1) {
            // this must mean we arrived
            pMovementComponent->vec3Target = Vector3::ZERO;
            pMovementComponent->bIsMoving = false;
            continue;
        }

        if (vec3Move.Length() > 600 * (7 / 1000.0f)) {
            vec3Move = vec3Move.Normalize().ScaleToLength(600 * (7 / 1000.0f));
        }
        
        pGameObject->position = pGameObject->position + vec3Move;
        pGameObject->rotation.y = CalculateAngle({ pGameObject->position.x, pGameObject->position.z }, { pMovementComponent->vec3Target.x, pMovementComponent->vec3Target.z });

    }
    if (ticks.size() <= 2) {
        // we do not yet have 2 ticks to simulate
        m_fCurrentFrameDelta = 0;
        current_tick_ = 0;
        Logger::Msg("Waiting for enough frames to start simulating!");

        return;
    }

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

    // we're here! we have something to do!

    // let's add the new delta, see what happens
    m_fCurrentFrameDelta += dt / (1000.0 / 60.0);

    if (m_fCurrentFrameDelta > 1.0) {
        // whoops we blew into a new frame
        // let's sim that
        game_tick_t tick = ticks[current_tick_ + 1];
        SimulateTick(tick, 1);
        current_tick_++;
        m_fCurrentFrameDelta -= 1.0;
    }

    if (m_fCurrentFrameDelta > 1.0) {
        Logger::Msg("This should _NEVER_ happen");
    }

    // all right, we're simming no more than 2 frames in the past.
    // now let's make sure we actually have something to sim to...
    if ((current_tick_ + 1) >= ticks.size()) {
        // we do, so simulate completed ticks till we're there
        // Logger::Msg("Simulation is caught up, but has nothing to simulate to!");
        return;
    }

    game_tick_t targetTick = ticks[current_tick_ + 1];
    SimulateTick(targetTick, m_fCurrentFrameDelta);

    /*
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
        Logger::Msg("no ticks yet?");
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
    */

}

void Game::SimulateTick(game_tick_t& tick, double diff) {
    // ignore tick packet header pls
    int offset = sizeof(packet_header_t);
    packet_header_t header{};

    for(std::vector<uint8_t> vecData : tick.Data) {
        std::memcpy(&header, vecData.data(), sizeof(header));
        switch (header.type) {
        case PacketType::UNITSPAWN:
        case PacketType::UNITIDLE:
        case PacketType::UNITMOVE:
        case PacketType::UNITDESPAWN:
        case PacketType::PCK_STATS:
        case PacketType::PCK_SPELL_COOLDOWN:
        case PacketType::PCK_PLAY_PARTICLE:
        case PacketType::PCK_ATTACK_START:
        case PacketType::SCORE_UPDATE_PACKET: {
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

            ParticleEffect* particle_system = ParticleEffect::Load("data/UI/move-to/move_to.pts", assetManager_);
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