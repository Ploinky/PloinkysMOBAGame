#include "common/data/game-data.h"
#include "game/server-game.h"
#include "data/server-data-loader.h"

int main(int argc, char* argv[]) {
    CServerDataLoader loader;
    CGameData gameData = loader.LoadManifest();
    NavMesh navMesh = NavMesh();
    navMesh.LoadFromData({
        "v 0.0 0.0 0.0", "v 1000.0 0.0 0.0", "v 2000.0 0.0 0.0",
        "v 0.0 0.0 -1000.0", "v 1000.0 0.0 -1000.0", "v 2000.0 0.0 -1000.0",
        "v 1000.0 0.0 -2000.0", "v 2000.0 0.0 -2000.0",
        "f 0 1 4", "f 0 4 3",
        "f 1 2 5", "f 1 5 4",
        "f 4 5 7", "f 4 7 6"
    });

    {
        CServerGame game;
        game.Start(gameData, &navMesh);
        UnitId idMovingUnit = game.GetState()->SpawnUnit(gameData, "stormcaller", {100, -100});
        UnitId idBlockingUnit = game.GetState()->SpawnUnit(gameData, "stormcaller", {500, -250});
        game.Input(CMoveAttemptEvent(idMovingUnit, {1000, 0, -100}, 0));
        game.Input(CMoveAttemptEvent(idBlockingUnit, {500, 0, -100}, 0));

        for(int i = 0; i < 30; i++) {
            game.Tick();
        }

        if (game.GetState()->CurrentTick != 30) {
            printf("FAILED: expected currentTick = 30, got %lld\n", game.GetState()->CurrentTick);
            return 1;
        }

        Vector3 blockerPos = game.GetState()->GetTransform(idBlockingUnit)->GetPosition();
        if (blockerPos != Vector3(500, 0, -100)) {
            printf("FAILED: blocker did not reach target at tick 30, is at {%f,%f,%f}\n", blockerPos.x, blockerPos.y, blockerPos.z);
            return 1;
        }

        for(int i = 0; i < 40; i++) {
            game.Tick();
        }

        if (game.GetState()->GetNavigation(idMovingUnit)->eStatus != ENavigationStatus::PATHING) {
            printf("FAILED: mover not pathing at tick 70\n");
            return 1;
        }

        game.Tick();

        if (game.GetState()->GetNavigation(idMovingUnit)->eStatus != ENavigationStatus::BLOCKED) {
            printf("FAILED: mover not blocked at tick 71\n");
            return 1;
        }

        for(int i = 0; i < 3; i++) {
            game.Tick();
        }

        if (game.GetState()->GetNavigation(idMovingUnit)->eStatus != ENavigationStatus::PATHING) {
            printf("FAILED: mover not re-pathed at tick 74\n");
            return 1;
        }

        for(int i = 0; i < 116; i++) {
            game.Tick();
        }
        
        Vector3 moverPos = game.GetState()->GetTransform(idMovingUnit)->GetPosition();
        if (moverPos != Vector3(1000, 0, -100)) {
            printf("FAILED: mover has not arrived by tick %lld, is at {%f,%f,%f}\n", game.GetState()->CurrentTick, moverPos.x, moverPos.y, moverPos.z);
            return 1;
        }
    }

    {
        CServerGameState gameState;
        std::vector<UnitId> found = gameState.GetUnitsInAra({500, -500}, 50);
        if (!found.empty()) {
            printf("FAILED: Initially no units should be found\n");
            return 1;
        }

        gameState.SpawnUnit(gameData, "dummy", {0, -500});
        gameState.SpawnUnit(gameData, "dummy", {1000, -500});
        gameState.SpawnUnit(gameData, "dummy", {500, 0});
        gameState.SpawnUnit(gameData, "dummy", {500, -1000});
        found = gameState.GetUnitsInAra({500, -500}, 50);
        if (!found.empty()) {
            printf("FAILED: Expected 0 units, got %lld\n", found.size());
            return 1;
        }

        gameState.SpawnUnit(gameData, "dummy", {549, -500});
        gameState.SpawnUnit(gameData, "dummy", {451, -500});
        gameState.SpawnUnit(gameData, "dummy", {500, -549});
        gameState.SpawnUnit(gameData, "dummy", {500, -451});
        found = gameState.GetUnitsInAra({500, -500}, 50);
        if (found.size() != 4) {
            printf("FAILED: Expected 4 units, got %lld\n", found.size());
            return 1;
        }
    }

    {
        CServerGame game;
        game.Start(gameData, &navMesh);
        UnitId idAttackingMinion = game.GetState()->SpawnUnit(gameData, "minion", {100, -100});
        BasicAttackComponent_t* pAttack = game.GetState()->AddBasicAttack(idAttackingMinion);
        pAttack->fRange = 200;
        pAttack->fAttackPoint = 500;
        pAttack->fAttackTime = 1000;
        game.GetState()->AddTeam(idAttackingMinion)->eTeam = Team::TEAM_1;
        UnitId idFurtherAwayMinionAddedFirst = game.GetState()->SpawnUnit(gameData, "minion", {400, -100});
        game.GetState()->AddTeam(idFurtherAwayMinionAddedFirst)->eTeam = Team::TEAM_2;
        UnitId idAttackedMinion = game.GetState()->SpawnUnit(gameData, "minion", {101, -100});
        game.GetState()->AddTeam(idAttackedMinion)->eTeam = Team::TEAM_2;
        game.Tick();

        std::optional<ActiveAttack_t> atk = game.GetState()->GetBasicAttack(idAttackingMinion)->optCurrentAttack;
        if (!atk.has_value()) {
            printf("FAILED: expected attack on target %lld, but is not attacking\n", idAttackedMinion);
            return 1;
        }
        if (atk.value().idTarget != idAttackedMinion) {
            printf("FAILED: expected attack on target %lld, but attack is targeting %lld\n", idAttackedMinion, atk.value().idTarget);
            return 1;
        }
    }

    
    {
        CServerGame game;
        game.Start(gameData, &navMesh);
        UnitId idSpawnedUnit = game.GetState()->SpawnUnit(gameData, "stormcaller", {100, -100});
        UnitId idMovingUnit = game.GetState()->SpawnUnit(gameData, "stormcaller", {0, -100});
        game.Input(CMoveAttemptEvent(idMovingUnit, {200, 0, -100}, 0));

        game.Tick();
        if (game.GetState()->GetNavigation(idMovingUnit)->path.size() != 3) {
            printf("FAILED: freshly spawned unit does not block correctly\n");
            return 1;
        }
    
        while (game.GetState()->CurrentTick < 100) {
            game.Tick();
        }

        if (game.GetState()->GetTransform(idSpawnedUnit)->GetPosition() != Vector3(100, 0, -100)) {
            printf("FAILED: freshly spawned unit moves\n");
            return 1;
        }

        if (game.GetState()->GetTransform(idMovingUnit)->GetPosition() == Vector3(0, 0, -100)) {
            printf("FAILED: move-ordered unit did not move\n");
            return 1;
        }
        
    }

    {
        CServerGame game;
        game.Start(gameData, &navMesh);
        UnitId idMovingUnit = game.GetState()->SpawnUnit(gameData, "minion", {100, -750});
        UnitId idAttackedUnit = game.GetState()->SpawnUnit(gameData, "stormcaller", {1000, -850});
        game.GetState()->AddTeam(idMovingUnit)->eTeam = Team::TEAM_1;
        BasicAttackComponent_t* pAttack = game.GetState()->AddBasicAttack(idMovingUnit);
        pAttack->fRange = 200;
        pAttack->fAttackPoint = 500;
        pAttack->fAttackTime = 1000;
        game.GetState()->AddTeam(idAttackedUnit)->eTeam = Team::TEAM_2;

        while (game.GetState()->CurrentTick < 63) {
            game.Tick();
        }

        if (game.GetState()->GetBasicAttack(idMovingUnit)->optCurrentAttack.has_value()) {
            printf("FAILED: attacking too early at tick %lld\n", game.GetState()->CurrentTick);
            return 1;
        }
        
        game.Tick();
        
        if (!game.GetState()->GetBasicAttack(idMovingUnit)->optCurrentAttack.has_value()) {
            printf("FAILED: no attack yet at tick %lld\n", game.GetState()->CurrentTick);
            return 1;
        }

        while (game.GetState()->CurrentTick < 145) {
            game.Tick();
        }

        Vector3 vec3AttackStartPos = game.GetState()->GetTransform(idMovingUnit)->GetPosition();
        if (game.GetState()->GetBasicAttack(idMovingUnit)->optCurrentAttack.value().eState != EAttackState::ATTACKING) {
            printf("FAILED: not attacking yet at tick %lld\n", game.GetState()->CurrentTick);
            return 1;
        }

        while (game.GetState()->CurrentTick < 175) {
            game.Tick();
        }

        if (game.GetState()->GetTransform(idMovingUnit)->GetPosition() != vec3AttackStartPos) {
            printf("FAILED: unit moved while attacking!\n");
            return 1;
        }
    }

    {
        CServerGame game;
        game.Start(gameData, &navMesh);
        UnitId idMovingUnit = game.GetState()->SpawnUnit(gameData, "minion", {600, -750});
        UnitId idBlockingUnit = game.GetState()->SpawnUnit(gameData, "stormcaller", {1100, -800});
        UnitId idAttackedUnit = game.GetState()->SpawnUnit(gameData, "stormcaller", {1150, -750});
        game.GetState()->AddTeam(idMovingUnit)->eTeam = Team::TEAM_1;
        BasicAttackComponent_t* pAttack = game.GetState()->AddBasicAttack(idMovingUnit);
        pAttack->fRange = 200;
        pAttack->fAttackPoint = 500;
        pAttack->fAttackTime = 1000;
        game.GetState()->AddTeam(idAttackedUnit)->eTeam = Team::TEAM_2;
        game.Input(CMoveAttemptEvent(idBlockingUnit, {1100, 0, -750}, 0));
        game.Input(CMoveAttemptEvent(idAttackedUnit, {1900, 0, -750}, 0));

        while (game.GetState()->CurrentTick < 93) {
            game.Tick();
        }

        Vector3 vec3MoverPos = game.GetState()->GetTransform(idMovingUnit)->GetPosition();

        while (game.GetState()->CurrentTick < 96) {
            game.Tick();
        }

        if (vec3MoverPos == game.GetState()->GetTransform(idMovingUnit)->GetPosition()) {
            printf("FAILED: attacking unit stuck on blocker\n");
            return 1;
        }
    }

    {
        CServerGame game;
        game.Start(gameData, &navMesh);
        UnitId idMissile = game.GetState()->SpawnUnit(gameData, "missile", {100, -100});
        game.GetState()->GetMissile(idMissile)->vec3Target = Vector3(900, 0, -900);

        game.Tick();

        if (game.GetState()->GetTransform(idMissile)->GetPosition() == Vector3(100, 0, -100)) {
            printf("FAILED: missile ain't moving\n");
            return 1;
        }

        while (game.GetState()->CurrentTick < 56) {
            game.Tick();
        }
        
        if (game.GetState()->GetTransform(idMissile)->GetPosition() == Vector3(900, 0, -900)) {
            printf("FAILED: missile has arrived too early\n");
            return 1;
        }
        game.Tick();
        if (game.GetState()->GetTransform(idMissile)->GetPosition() != Vector3(900, 0, -900)) {
            printf("FAILED: missile has not arrived at target after %lld ticks\n", game.GetState()->CurrentTick);
            return 1;
        }
        game.Tick();
        if (game.GetState()->GetMissile(idMissile) != nullptr) {
            printf("FAILED: missile has not been removed after %lld ticks\n", game.GetState()->CurrentTick);
            return 1;
        }
    }

    {
        CServerGame game;
        game.Start(gameData, &navMesh);
        UnitId idMissile = game.GetState()->SpawnUnit(gameData, "missile", {100, -100});
        game.GetState()->AddTeam(idMissile)->eTeam = Team::TEAM_1;
        UnitId idEnemy = game.GetState()->SpawnUnit(gameData, "stormcaller", {200, -100});
        game.GetState()->AddTeam(idEnemy)->eTeam = Team::TEAM_2;

        game.GetState()->GetMissile(idMissile)->vec3Target = Vector3(900, 0, -1100);

        if (game.GetState()->GetHealth(idEnemy)->nHealth != game.GetState()->GetHealth(idEnemy)->nMaxHealth) {
            printf("FAILED: enemy does not have full hp\n");
            return 1;
        }
        while (game.GetState()->CurrentTick < 4) {
            game.Tick();
        }
        
        if (game.GetState()->GetHealth(idEnemy)->nHealth == game.GetState()->GetHealth(idEnemy)->nMaxHealth) {
            printf("FAILED: missile has not dealt damage\n");
            return 1;
        }

        if (game.GetState()->GetMissile(idMissile) != nullptr) {
            printf("FAILED: missile has not been removed after hitting\n");
            return 1;
        }
    }

    {
        CServerGame game;
        game.Start(gameData, &navMesh);
        UnitId idStormcaller = game.GetState()->SpawnUnit(gameData, "stormcaller", {100, -100});
        SpellTargetInfo info;
        info.target_point = Vector3(900, 0, -100);
        game.Input(CSpellAttemptCastEvent(idStormcaller, info, 0));

        while (game.GetState()->CurrentTick < 8) {
            game.Tick();
        }
        if (game.GetState()->GetAllMissile().empty()) {
            printf("FAILED: missile has not been spawned after spell cast\n");
            return 1;
        }
        UnitId idMissile = game.GetState()->GetAllMissile().begin()->first;
        if (game.GetState()->GetMissile(idMissile)->vec3Target != info.target_point) {
            printf("FAILED: missile targeted wrong\n");
            return 1;
        }
        if (game.GetState()->GetMissile(idMissile)->idOwner != idStormcaller) {
            printf("FAILED: missile owner wrong\n");
            return 1;
        }
        if (game.GetState()->GetTransform(idMissile)->GetPosition() != Vector3(100, 0, -100)) {
            printf("FAILED: missile spawned at wrong location\n");
            return 1;
        }
        while (game.GetState()->CurrentTick < 48) {
            game.Tick();
        }
        if (game.GetState()->GetTransform(idMissile)->GetPosition() != Vector3(900, 0, -100)) {
            printf("FAILED: missile has not arrived by tick %lld\n", game.GetState()->CurrentTick);
            return 1;
        }
        game.Tick();
        if (game.GetState()->GetMissile(idMissile) != nullptr) {
            printf("FAILED: missile has not been removed by %lld\n", game.GetState()->CurrentTick);
            return 1;
        }
    }
    
    {
        CServerGame game;
        game.Start(gameData, &navMesh);
        UnitId idStormcaller = game.GetState()->SpawnUnit(gameData, "stormcaller", {100, -100});
        UnitId idEnemy = game.GetState()->SpawnUnit(gameData, "stormcaller", {500, -100});
        game.GetState()->AddTeam(idStormcaller)->eTeam = Team::TEAM_1;
        game.GetState()->AddTeam(idEnemy)->eTeam = Team::TEAM_2;
        SpellTargetInfo info;
        info.target_point = Vector3(900, 0, -100);
        game.Input(CSpellAttemptCastEvent(idStormcaller, info, 0));

        if (game.GetState()->GetHealth(idEnemy)->nHealth != game.GetState()->GetHealth(idEnemy)->nMaxHealth) {
            printf("FAILED: enemy not spawned with max health\n");
            return 1;
        }

        while (game.GetState()->CurrentTick < 8) {
            game.Tick();
        }
        if (game.GetState()->GetAllMissile().empty()) {
            printf("FAILED: missile has not been spawned after spell cast\n");
            return 1;
        }
        UnitId idMissile = game.GetState()->GetAllMissile().begin()->first;
        if (game.GetState()->GetMissile(idMissile)->vec3Target != info.target_point) {
            printf("FAILED: missile targeted wrong\n");
            return 1;
        }
        if (game.GetState()->GetMissile(idMissile)->idOwner != idStormcaller) {
            printf("FAILED: missile owner wrong\n");
            return 1;
        }
        if (game.GetState()->GetTransform(idMissile)->GetPosition() != Vector3(100, 0, -100)) {
            printf("FAILED: missile spawned at wrong location\n");
            return 1;
        }
        while (game.GetState()->CurrentTick < 25) {
            game.Tick();
        }

        if (game.GetState()->GetHealth(idEnemy)->nHealth == game.GetState()->GetHealth(idEnemy)->nMaxHealth) {
            printf("FAILED: enemy not hit by missile\n");
            return 1;
        }
        if (game.GetState()->GetMissile(idMissile) != nullptr) {
            printf("FAILED: missile has not been removed by %lld\n", game.GetState()->CurrentTick);
            return 1;
        }
    }

    printf("PASSED: all tests passed\n");

    return 0;
}