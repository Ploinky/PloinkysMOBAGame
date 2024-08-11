add_executable(Server
	Source/AreaSpell.cpp
	Source/Attackable.cpp
	Source/Buff.cpp
	Source/Building.cpp
	Source/FootballPerson.cpp
	Source/Game.cpp
	Source/GameObject.cpp
	Source/LobbyState.cpp
	Source/Main.cpp
	Source/Minion.cpp
	Source/MinionSpawner.cpp
	Source/Missile.cpp
	Source/MissileSpell.cpp
	Source/NetworkManager.cpp
	Source/NetworkPeer.cpp
	Source/ParticleSpawner.cpp
	Source/Person.cpp
	Source/Server.cpp
	Source/Spell.cpp

	Source/GameObjectAction/GameObjectActionAttackMove.cpp
	Source/GameObjectAction/GameObjectActionAttackUnit.cpp
	Source/GameObjectAction/GameObjectActionCastSpell.cpp
	Source/GameObjectAction/GameObjectActionMove.cpp
	Source/GameObjectAction/GameObjectActionStop.cpp
)

target_include_directories(Server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Include)
target_link_libraries(Server PRIVATE Common SteamAPI)
target_link_libraries(Server PUBLIC steam_api64)

set_target_properties(Server
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Dist/Server
)

add_custom_command(TARGET Server POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/Bin/steam_api64.dll ${CMAKE_SOURCE_DIR}/Dist/Server/$<CONFIG>/steam_api64.dll)

add_custom_command(TARGET Server
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/Bin/steam_appid.txt ${CMAKE_SOURCE_DIR}/Dist/Server/$<CONFIG>/steam_appid.txt)

add_dependencies(Server ServerAssets)