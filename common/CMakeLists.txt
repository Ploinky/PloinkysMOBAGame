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
	src/PakFile.cpp
	src/projection.cpp
	src/ray.cpp
	src/sphere.cpp
	src/util.cpp
	src/vector2.cpp
	src/vector3.cpp
	src/vector4.cpp
	src/Packets/AttackCommand.cpp
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
	src/Packets/PlayParticlePacket.cpp
	src/Packets/ScoreUpdatePacket.cpp
	src/Packets/ServerInfoPacket.cpp
	src/Packets/SpawnPacket.cpp
	src/Packets/SpellCastStartPacket.cpp
	src/Packets/StopCommand.cpp
	src/Packets/UnitIdlePacket.cpp
	src/Packets/UnitIdPacket.cpp
	src/Packets/UnitMoveIntentionPacket.cpp
	src/Packets/UnitMovePacket.cpp
	src/Packets/UnitStatsPacket.cpp
	
	src/Packets/CharacterSelect/RemainingTimePacket.cpp
)

target_include_directories(common PUBLIC include)

# TODO fix this but not right now pls
target_include_directories(common PRIVATE include/common)