add_executable(server_test
    main.cpp
    ${CMAKE_SOURCE_DIR}/apps/server/src/systems/ai-system.cpp
    ${CMAKE_SOURCE_DIR}/apps/server/src/systems/attack-system.cpp
    ${CMAKE_SOURCE_DIR}/apps/server/src/systems/cleanup-system.cpp
    ${CMAKE_SOURCE_DIR}/apps/server/src/systems/damage-system.cpp
    ${CMAKE_SOURCE_DIR}/apps/server/src/systems/inventory-system.cpp
	${CMAKE_SOURCE_DIR}/apps/server/src/systems/missile-system.cpp
    ${CMAKE_SOURCE_DIR}/apps/server/src/systems/movement-system.cpp
    ${CMAKE_SOURCE_DIR}/apps/server/src/systems/NavigationSystem.cpp
    ${CMAKE_SOURCE_DIR}/apps/server/src/systems/points-system.cpp
    ${CMAKE_SOURCE_DIR}/apps/server/src/systems/respawn-system.cpp
    ${CMAKE_SOURCE_DIR}/apps/server/src/systems/spawn-system.cpp
    ${CMAKE_SOURCE_DIR}/apps/server/src/systems/spell-system.cpp
    ${CMAKE_SOURCE_DIR}/apps/server/src/systems/trigger-system.cpp
    ${CMAKE_SOURCE_DIR}/apps/server/src/events/damage-event.cpp
    ${CMAKE_SOURCE_DIR}/apps/server/src/events/heal-event.cpp
    ${CMAKE_SOURCE_DIR}/apps/server/src/events/spell-cast-event.cpp
    ${CMAKE_SOURCE_DIR}/apps/server/src/events/spell-cast-start-event.cpp
    ${CMAKE_SOURCE_DIR}/apps/server/src/components/SpellCastComponent.cpp
    ${CMAKE_SOURCE_DIR}/apps/server/src/components/TransformComponent.cpp
    ${CMAKE_SOURCE_DIR}/apps/server/src/game/server-game-state.cpp
    ${CMAKE_SOURCE_DIR}/apps/server/src/game/server-game.cpp
    ${CMAKE_SOURCE_DIR}/apps/server/src/data/server-data-loader.cpp
    ${CMAKE_SOURCE_DIR}/apps/server/src/spell-cast-api.cpp
)

target_compile_definitions(server_test PUBLIC NOMINMAX)

target_include_directories(server_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/server/src ${CMAKE_SOURCE_DIR}/vendor/include)

target_link_libraries(server_test PUBLIC common)