#pragma once

#include <memory>

#include <common/AssetManager.h>
#include <data/server-game-data.h>
#include <common/vendor/pugixml.hpp>

class CServerDataLoader : public AssetManager {
public:
    CServerGameData LoadManifest();

private:
    std::shared_ptr<AssetManager> m_pAssetManager;

    pugi::xml_document LoadXMLFile(std::string strFileName);

    CCharacterData LoadCharacter(pugi::xml_node& characterNode);
    CAbilityData LoadAbility(pugi::xml_node& abilityNode);
};