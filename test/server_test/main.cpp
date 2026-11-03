#include "common/data/game-data.h"
#include "game/server-game.h"
#include "data/server-data-loader.h"

int main(int argc, char* argv[]) {
    NavMesh navMesh = NavMesh();
    navMesh.LoadFromData({
        "v 0.0 0.0 0.0", "v 1000.0 0.0 0.0", "v 2000.0 0.0 0.0",
        "v 0.0 0.0 -1000.0", "v 1000.0 0.0 -1000.0", "v 2000.0 0.0 -1000.0",
        "v 1000.0 0.0 -2000.0", "v 2000.0 0.0 -2000.0",
        "f 0 1 4", "f 0 4 3",
        "f 1 2 5", "f 1 5 4",
        "f 4 5 7", "f 4 7 6"
    });

    CServerDataLoader loader;
    CGameData gameData = loader.LoadManifest();
    CServerGame game;
    game.Start(gameData, &navMesh);

    for(int i = 0; i < 5; i++) {
        game.Tick();
    }

    if (game.GetState()->CurrentTick != 5) {
        printf("FAILED: expected currentTick = 5, got %lld\n", game.GetState()->CurrentTick);
        return 1;
    }

    printf("PASSED: all tests passed\n");
    return 0;
}