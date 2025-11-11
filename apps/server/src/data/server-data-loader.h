#pragma once

#include <memory>

#include <common/AssetManager.h>
#include <data/server-game-data.h>
#include <common/vendor/pugixml.hpp>

class CServerDataLoader {
public:
    CServerDataLoader(std::shared_ptr<AssetManager> pAssetManager);
    CServerGameData LoadManifest();

private:
    std::shared_ptr<AssetManager> m_pAssetManager;

    pugi::xml_document LoadFile(std::string strFileName);

    CCharacterData LoadCharacter(pugi::xml_node& characterNode);
    CAbilityData LoadAbility(std::string charId, pugi::xml_node& abilityNode);
};