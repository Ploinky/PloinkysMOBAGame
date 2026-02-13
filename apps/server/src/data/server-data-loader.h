#pragma once

#include <memory>

#include <common/AssetManager.h>
#include <common/data/game-data.h>

class CServerDataLoader : public AssetManager {
public:
    CGameData LoadManifest();
    CTriggerData LoadTriggerData(pugi::xml_node node);

private:
    std::shared_ptr<AssetManager> m_pAssetManager;

};