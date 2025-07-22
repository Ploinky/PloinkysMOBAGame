link_directories($(CMAKE_SOURCE_DIR)/Bin ${CMAKE_SOURCE_DIR}/vendor/steam/redistributable_bin/win64)

function(copy_asset_folder)
    add_custom_command(TARGET server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different $<TARGET_PROPERTY:assets,BINARY_DIR>/${ARGV0} $<TARGET_FILE_DIR:server>/${ARGV0}
    )
endfunction()

add_executable(server
	src/Buff.cpp
	src/EventManager.cpp
	src/Game.cpp
	src/GameState.cpp
	src/LobbyState.cpp
	src/Main.cpp
	src/Missile.cpp
	src/NavigationSystem.cpp
	src/NetworkManager.cpp
	src/Server.cpp

	src/GameObject/GameObject.cpp
	src/GameObject/MovementComponent.cpp
	src/GameObject/NavigationComponent.cpp
	src/GameObject/SpellCastComponent.cpp
	src/GameObject/TransformComponent.cpp

	src/GameObject/Spell/ThrowFootball.cpp

	src/Networking/NetworkSystem.cpp
)
set_directory_properties(PROPERTIES COMPILE_DEFINITIONS_DEBUG "_DEBUG")

target_include_directories(server PRIVATE src ${CMAKE_SOURCE_DIR}/vendor/steam/public)
target_link_libraries(server PUBLIC steam_api64 common)

if(WIN32)
	add_custom_command(TARGET server POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/vendor/steam/redistributable_bin/win64/steam_api64.dll $<TARGET_FILE_DIR:server>/steam_api64.dll)
endif()

add_custom_command(TARGET server
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/steam_appid.txt $<TARGET_FILE_DIR:server>/steam_appid.txt)

add_custom_command(
	OUTPUT ${CMAKE_SOURCE_DIR}/Dist/Server/$<CONFIG>/Maps/map1/map1.nvm
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/Dist/Server/$<CONFIG>/Maps/map1
	COMMAND blender ${CMAKE_SOURCE_DIR}/Assets/Maps/map1/map1.blend --background --python ${CMAKE_SOURCE_DIR}/Tool/blender_nvm_export.py -- ${CMAKE_SOURCE_DIR}/Dist/Server/$<CONFIG>/Maps/map1/map1.nvm
	DEPENDS ${CMAKE_SOURCE_DIR}/Assets/Maps/map1/map1.blend
)

copy_asset_folder(Maps)