#pragma once

#include <memory>

#include <common/AssetManager.h>
#include <data/server-game-data.h>

class CServerDataLoader {
public:
    static CServerGameData LoadManifest(std::shared_ptr<AssetManager> m_pAssetManager);
};