function(copy_asset_folder)
    add_custom_command(TARGET server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different $<TARGET_PROPERTY:assets,BINARY_DIR>/${ARGV0} $<TARGET_FILE_DIR:server>/${ARGV0}
    )
endfunction()

function(pak_folder)
    file(GLOB PAK_SOURCE "$<TARGET_PROPERTY:assets,BINARY_DIR>/${ARGV0}/**")
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/${ARGV1}
        COMMAND pakr ${ARGV0} ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/${ARGV1}
        DEPENDS ${PAK_SOURCE}
        WORKING_DIRECTORY $<TARGET_PROPERTY:assets,BINARY_DIR>
    )
    list(APPEND PAK_FILES ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/${ARGV1})
    set(PAK_FILES ${PAK_FILES} PARENT_SCOPE)
endfunction()

add_executable(server
	src/Buff.cpp
	src/Game.cpp
	src/GameState.cpp
	src/LobbyState.cpp
	src/Main.cpp
	src/Missile.cpp
	src/network-manager.cpp
	src/Server.cpp
	src/spell-cast-api.cpp
	
	src/GameObject.cpp
	
	src/components/MovementComponent.cpp
	src/components/NavigationComponent.cpp
	src/components/SpellCastComponent.cpp
	src/components/TransformComponent.cpp

	src/data/server-data-loader.cpp
	
	src/events/damage-event.cpp
	src/events/heal-event.cpp
	src/events/spell-cast-event.cpp
	src/events/spell-cast-start-event.cpp

	src/game/wave-manager.cpp

	src/systems/ai-system.cpp
	src/systems/attack-system.cpp
	src/systems/damage-system.cpp
	src/systems/NavigationSystem.cpp
	src/systems/NetworkSystem.cpp
	src/systems/movement-system.cpp
	src/systems/points-system.cpp
	src/systems/respawn-system.cpp
	src/systems/spell-system.cpp
	src/systems/system.cpp

	src/content/characters/stormcaller/thunderstrike.cpp
)

target_compile_definitions(server PUBLIC NOMINMAX)
# target_precompile_headers(server PRIVATE src/platform.h)

set_directory_properties(PROPERTIES COMPILE_DEFINITIONS_DEBUG "_DEBUG")

target_link_directories(server PRIVATE ${CMAKE_SOURCE_DIR}/vendor/lib)
target_include_directories(server PRIVATE src ${CMAKE_SOURCE_DIR}/vendor/include)

target_link_libraries(server PUBLIC common)

if(WIN32)
	target_link_libraries(server PUBLIC steam_api64)
	add_custom_command(TARGET server POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/vendor/lib/steam_api64.dll $<TARGET_FILE_DIR:server>/steam_api64.dll)
endif()

if(UNIX)
	target_link_libraries(server PUBLIC steam_api)
	add_custom_command(TARGET server POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/vendor/lib/libsteam_api.so $<TARGET_FILE_DIR:server>/libsteam_api.so)

endif()

add_custom_command(TARGET server POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/steam_appid.txt $<TARGET_FILE_DIR:server>/steam_appid.txt)

pak_folder(Maps/map1 Maps/Map1.pak)
pak_folder(mnf.xml mnf.pak)

add_custom_target(server_assets
    DEPENDS ${PAK_FILES}
)

add_dependencies(server_assets
	assets
)

add_dependencies(server server_assets)