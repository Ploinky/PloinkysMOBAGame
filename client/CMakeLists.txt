function(copy_asset_folder)
    add_custom_command(TARGET client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different $<TARGET_PROPERTY:assets,BINARY_DIR>/${ARGV0} $<TARGET_FILE_DIR:client>/${ARGV0}
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

add_executable(client WIN32
    src/Animation.cpp
    src/Armature.cpp
    src/AudioSystem.cpp
    src/Camera.cpp
    src/CharacterSelectState.cpp
    src/Client.cpp
    src/client-asset-manager.cpp
    src/ClientNetworkManager.cpp
    src/ConnectingState.cpp
    src/Game.cpp
    src/GameObject.cpp
    src/GLBFileLoader.cpp
    src/GuiButton.cpp
    src/GuiCheckbox.cpp
    src/GuiSelector.cpp
    src/GuiServerElement.cpp
    src/GuiSimplePane.cpp
    src/GuiTextfield.cpp
    src/LoadingState.cpp
    src/Lobby.cpp
    src/Main.cpp
    src/MainMenu.cpp
    src/Material.cpp
    src/Mesh.cpp
    src/Model.cpp
    src/ParticleEmitter.cpp
    src/ParticleEffect.cpp
    src/Renderer.cpp
    src/ServerBrowser.cpp
    src/Settings.cpp
    src/SettingsMenu.cpp
    src/VBox.cpp
    src/Vertex.cpp
    src/Window.cpp

    src/core/audio/audio-engine.cpp

    src/core/graphics/d3d11-graphics-engine.cpp

    src/game/systems/animation-system.cpp
    src/game/systems/particle-system.cpp

    Resources/Cursor.cur
    Resources/cursor1.cur
    Resources/Hover.cur
    Resources/HoverAlly.cur
    Resources/icon1.ico
    Resources/Resource.rc
)

target_compile_definitions(client PUBLIC NOMINMAX)
# target_compile_definitions(client PUBLIC NAV_DEBUG)
target_precompile_headers(client PRIVATE src/platform.h)

target_include_directories(client PRIVATE src ${CMAKE_SOURCE_DIR}/vendor/include)
target_link_directories(client PRIVATE ${CMAKE_SOURCE_DIR}/vendor/lib)

target_link_libraries(client PUBLIC common d3d11 dwrite d2d1 xaudio2 steam_api64 glfw3)

add_custom_command(TARGET client POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/vendor/lib/steam_api64.dll $<TARGET_FILE_DIR:client>/steam_api64.dll)
add_custom_command(TARGET client POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/steam_appid.txt $<TARGET_FILE_DIR:client>/steam_appid.txt)

pak_folder(Buildings/Tower Buildings/Tower.pak)
pak_folder(Generic Generic.pak)
if(WIN32)
    pak_folder(Shaders Shaders.pak)
endif()

pak_folder(Maps/map1 Maps/Map1.pak)
pak_folder(Persons/_Generic Persons/_Generic.pak)
pak_folder(Persons/ChessPerson Persons/ChessPerson.pak)
pak_folder(Persons/Minion Persons/Minion.pak)
pak_folder(UI/Buttons/MenuButton UI/Buttons/MenuButton.pak)
pak_folder(UI/MoveTo UI/MoveTo.pak)
pak_folder(characters/stormcaller characters/stormcaller.pak)

add_custom_target(pak_file_target
    DEPENDS ${PAK_FILES}
)

add_dependencies(pak_file_target
    assets
)

add_dependencies(client
    pak_file_target
)