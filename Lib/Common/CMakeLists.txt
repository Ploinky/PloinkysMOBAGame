add_library(Common
	Source/AssetManager.cpp
	Source/calculation.cpp
	Source/circle.cpp
	Source/collision.cpp
	Source/component_registry.cpp
	Source/logger.cpp
	Source/matrix.cpp
	Source/navigation.cpp
	Source/networking.cpp
	Source/PakFile.cpp
	Source/projection.cpp
	Source/ray.cpp
	Source/sphere.cpp
	Source/util.cpp
	Source/vector2.cpp
	Source/vector3.cpp
	Source/vector4.cpp

	Source/Packets/AttackCommand.cpp
	Source/Packets/AttackStartPacket.cpp
	Source/Packets/CastCommand.cpp
	Source/Packets/CastTargetCommand.cpp
	Source/Packets/CooldownPacket.cpp
	Source/Packets/DespawnPacket.cpp
	Source/Packets/GameTickPacket.cpp
	Source/Packets/LobbyGameStartPacket.cpp
	Source/Packets/LobbyReadyCommand.cpp
	Source/Packets/LobbySlotCommand.cpp
	Source/Packets/LobbySlotPacket.cpp
	Source/Packets/MoveCommand.cpp
	Source/Packets/PlayParticlePacket.cpp
	Source/Packets/SpawnPacket.cpp
	Source/Packets/StopCommand.cpp
	Source/Packets/UnitIdlePacket.cpp
	Source/Packets/UnitIdPacket.cpp
	Source/Packets/UnitMovePacket.cpp
	Source/Packets/UnitStatsPacket.cpp
)

target_include_directories(Common PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Include)
target_include_directories(Common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Public)

add_executable(CommonTest
Test/test_main.cpp
)

target_link_libraries(CommonTest Common gtest gtest_main)

add_test(NAME CommonTest COMMAND CommonTest)