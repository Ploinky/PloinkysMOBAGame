#include "server-data-loader.h"

#include <iostream>

#include <common/vendor/pugixml.hpp>
#include <common/logger.h>

CServerGameData CServerDataLoader::LoadManifest(std::shared_ptr<AssetManager> pAssetManager) {
    CServerGameData gameData{};

    std::vector<uint8_t> mnfData = pAssetManager->LoadFile("mnf.xml");

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_buffer(mnfData.data(), mnfData.size(), 116u, pugi::encoding_utf8);

    if(result.status != pugi::xml_parse_status::status_ok) {
        Logger::FormatErr("Failed to load data manifest file");
        return {};
    }

    pugi::xml_node gameDataNode = doc.child("game_data");

    if(gameDataNode) {
        pugi::xml_node characters = gameDataNode.child("characters");

        if(characters) {
            for(pugi::xml_node character : characters.children()) {
                std::string id = character.attribute("id").as_string();
            }
        }
    }
    
    return gameData;
}