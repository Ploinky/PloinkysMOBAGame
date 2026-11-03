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

    printf("PASSED: all tests passed\n");

    return 0;
}