# collect paths
get_property(dist_dir GLOBAL PROPERTY dist_dir)
get_property(out_dir GLOBAL PROPERTY out_dir)
set(server_out_dir ${out_dir}/"server")

add_executable(server
	src/Buff.cpp
	src/Game.cpp
	src/LobbyState.cpp
	src/Main.cpp
	src/Missile.cpp
	src/network-manager.cpp
	src/Server.cpp
	src/spell-cast-api.cpp
	
	src/components/SpellCastComponent.cpp
	src/components/TransformComponent.cpp

	src/data/server-data-loader.cpp
	
	src/events/damage-event.cpp
	src/events/heal-event.cpp
	src/events/spell-cast-event.cpp
	src/events/spell-cast-start-event.cpp

	src/game/wave-manager.cpp
	src/game/server-game-state.cpp

	src/systems/ai-system.cpp
	src/systems/attack-system.cpp
	src/systems/damage-system.cpp
	src/systems/inventory-system.cpp
	src/systems/NavigationSystem.cpp
	src/systems/NetworkSystem.cpp
	src/systems/movement-system.cpp
	src/systems/points-system.cpp
	src/systems/respawn-system.cpp
	src/systems/spawn-system.cpp
	src/systems/spell-system.cpp
	src/systems/trigger-system.cpp
)

target_compile_definitions(server PUBLIC NOMINMAX)
# target_precompile_headers(server PRIVATE src/platform.h)

set_directory_properties(PROPERTIES COMPILE_DEFINITIONS_DEBUG "_DEBUG")

target_link_directories(server PRIVATE ${CMAKE_SOURCE_DIR}/vendor/lib)
target_include_directories(server PRIVATE src ${CMAKE_SOURCE_DIR}/vendor/include)

target_link_libraries(server PUBLIC common)

if(WIN32)
    if(NOT EXISTS ${CMAKE_SOURCE_DIR}/_out/server/data)
        add_custom_command(TARGET server POST_BUILD COMMAND cmd /c mklink /J \"${CMAKE_SOURCE_DIR}/_out/server/data\" \"${CMAKE_SOURCE_DIR}/resources/data\")
	endif()
	add_custom_command(TARGET server POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE_DIR:server>/server.exe ${server_out_dir}/server.exe)
endif()

if(UNIX)
endif()

add_custom_command(TARGET server POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different  ${CMAKE_SOURCE_DIR}/resources/config_server/ ${server_out_dir}/cfg/)