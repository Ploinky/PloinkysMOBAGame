#include "test-framework.h"

#include "common/PMG_Common.h"
#include "GameState.h"
#include "GameObject.h"
#include "components/Components.h"
#include "systems/spell-system.h"
#include "systems/damage-system.h"
#include "systems/movement-system.h"
#include "events/spell-cast-start-event.h"
#include "events.h"

TEST_CASE(TestFirebolt) {
    CGameState GameState;

    CGameObject* pPlayer = new CGameObject();
    CNetworkComponent* pNetComponent = new CNetworkComponent();
    pNetComponent->SetSyncMovement(true);
    pPlayer->AddComponent(new CTransformComponent());
    pPlayer->AddComponent(pNetComponent);

    CMovementComponent* pMoveComponent = new CMovementComponent();
    pPlayer->AddComponent(pMoveComponent);
    pPlayer->AddComponent(new CNavigationComponent());

    std::vector<SpellSlot_t> vecSpells;
    /*
    SpellSlot_t spell1;
    spell1.pSpell = new CThrowFootball();
    SpellSlot_t spell2;
    spell2.pSpell = new CHealPerson();
    vecSpells.push_back(spell1);
    vecSpells.push_back(spell2);
    */
    CSpellCastComponent* pSpellCast = new CSpellCastComponent(vecSpells);
    pPlayer->AddComponent(pSpellCast);

    CHealthComponent* health = new CHealthComponent(200);
    pPlayer->AddComponent(health);
    GameState.GameObjects.emplace(pPlayer->GetId(), pPlayer);

    
    CGameObject* pDummy = new CGameObject();
    pDummy->AddComponent(new CTransformComponent());
    pDummy->GetComponent<CTransformComponent>()->SetPosition({2000, 0, -2000});
    pDummy->AddComponent(new CMovementComponent());
    pDummy->GetComponent<CMovementComponent>()->SetTarget({2000, 0, -2000});
    pDummy->AddComponent(new CNetworkComponent());
    pDummy->AddComponent(new CHealthComponent(100));
    GameState.GameObjects.emplace(pDummy->GetId(), pDummy);

    // Add systems
    CSpellSystem spellSystem;
    CDamageSystem damageSystem;

    std::vector<ISystem*> systems = {
        &spellSystem, &damageSystem
    };

    // Push spell cast
    SpellTargetInfo tar;
    tar.target = pDummy->GetId();
    CSpellAttemptCastEvent* pEvt = new CSpellAttemptCastEvent(pPlayer->GetId(), tar, 0);
    GameState.VecEvent.push(pEvt);

    // === Frame 1
    // - cast start event -> cast queued up
    // === Frame 2
    // - start of animation, 00:00 -> 16:66
    // === Frame 3 ... 18
    // - done, on cast -> damage event queued
    // - damage event handled

    for(int i = 0; i < 17; i++) {
        // Game loop simulation
        for (auto* system : systems)
            system->Update(&GameState, 1000.0f / 60.0f); // no cast time for test
    
        while (!GameState.VecEvent.empty()) {
            auto ev = GameState.VecEvent.front();
            
            for (auto* system : systems)
            system->Process(&GameState, ev);
            
            GameState.VecEvent.pop();
            for (auto* system : systems)
                system->Finalize(&GameState);
        }
    }

    REQUIRE(pDummy->GetComponent<CHealthComponent>()->nHealth == 100);

    // Game loop simulation
    for (auto* system : systems)
        system->Update(&GameState, 1000.0f / 60.0f); // no cast time for test

    while (!GameState.VecEvent.empty()) {
        auto ev = GameState.VecEvent.front();
        
        for (auto* system : systems)
        system->Process(&GameState, ev);
        
        GameState.VecEvent.pop();
        for (auto* system : systems)
            system->Finalize(&GameState);
    }

    REQUIRE(pDummy->GetComponent<CHealthComponent>()->nHealth == 90);
}