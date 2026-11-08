get_property(out_dir GLOBAL PROPERTY out_dir)

add_library(common
	src/AssetManager.cpp
	src/calculation.cpp
	src/collision.cpp
	src/JSONArray.cpp
	src/JSONObject.cpp
	src/JSONParser.cpp
	src/JSONValue.cpp
	src/logger.cpp
	src/navigation.cpp
	src/ray.cpp
	src/sphere.cpp
	src/util.cpp
	src/vector2.cpp
	src/vector3.cpp
	src/vector4.cpp

	src/Packets/AttackCommand.cpp
	src/Packets/AttackFinishedPacket.cpp
	src/Packets/AttackStartPacket.cpp
	src/Packets/CastCommand.cpp
	src/Packets/CastTargetCommand.cpp
	src/Packets/CooldownPacket.cpp
	src/Packets/DespawnPacket.cpp
	src/Packets/GameEndPacket.cpp
	src/Packets/GameTickPacket.cpp
	src/Packets/LobbyGameStartPacket.cpp
	src/Packets/LobbyPlayerLeftPacket.cpp
	src/Packets/LobbyReadyCommand.cpp
	src/Packets/LobbySlotCommand.cpp
	src/Packets/LobbySlotPacket.cpp
	src/Packets/MoveCommand.cpp
	src/Packets/pick-up-entity-command.cpp
	src/Packets/picked-up-entity-packet.cpp
	src/Packets/PlayParticlePacket.cpp
	src/Packets/round-end-packet.cpp
	src/Packets/round-reset-packet.cpp
	src/Packets/ScoreUpdatePacket.cpp
	src/Packets/ServerInfoPacket.cpp
	src/Packets/SpawnPacket.cpp
	src/Packets/SpellCastStartPacket.cpp
	src/Packets/SpellHitPacket.cpp
	src/Packets/StopCommand.cpp
	src/Packets/team-win-packet.cpp
	src/Packets/unit-death-packet.cpp
	src/Packets/unit-respawn-packet.cpp
	src/Packets/UnitIdlePacket.cpp
	src/Packets/UnitIdPacket.cpp
	src/Packets/UnitMoveIntentionPacket.cpp
	src/Packets/UnitMovePacket.cpp
	src/Packets/UnitStatsPacket.cpp
	src/packets/use-entity-point-command.cpp
	
	src/Packets/CharacterSelect/RemainingTimePacket.cpp

	src/util/frame-timer.cpp

	src/vendor/pugixml.cpp

	${CMAKE_CURRENT_BINARY_DIR}/version.cpp 
)

target_include_directories(common PUBLIC include)

# TODO fix this but not right now pls
target_include_directories(common PRIVATE include/common)

target_link_libraries(common PUBLIC paklib)

target_compile_definitions(common PUBLIC NOMINMAX)

add_custom_target(
	versioner COMMAND ${CMAKE_COMMAND} -DBUILD_TYPE=${CMAKE_BUILD_TYPE} -DVERSION_OUT_DIR=${out_dir} -DVERSION_OUT=${CMAKE_CURRENT_BINARY_DIR}/version.cpp -P ${CMAKE_CURRENT_SOURCE_DIR}/version.cmake
	BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/version.cpp
)
add_dependencies(common versioner)