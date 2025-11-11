# collect paths
get_property(dist_dir GLOBAL PROPERTY dist_dir)
get_property(out_dir GLOBAL PROPERTY out_dir)
set(client_out_dir ${out_dir}/"client")

add_executable(client
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
    
    src/game/systems/animation-system.cpp
    src/game/systems/particle-system.cpp
    
    src/core/input/input-manager.cpp
    
    Resources/Cursor.cur
    Resources/cursor1.cur
    Resources/Hover.cur
    Resources/HoverAlly.cur
    Resources/icon1.ico
    Resources/Resource.rc
)

if(UNIX)
    target_sources(client PRIVATE
        src/core/audio/audio-engine-minaudio.cpp
        src/core/graphics/opengl-graphics-engine.cpp
        src/core/platform/platform_linux.cpp
        src/core/window/window-glfw.cpp
    )
endif()

if(WIN32)
    target_sources(client PRIVATE
        src/core/graphics/d3d11-graphics-engine.cpp
        src/core/audio/audio-engine-xaudio.cpp
        src/core/platform/platform_win32.cpp
        src/core/window/window-win32.cpp
    )
endif()

target_compile_definitions(client PUBLIC NOMINMAX)
# target_compile_definitions(client PUBLIC NAV_DEBUG)
target_precompile_headers(client PRIVATE src/platform.h)

target_include_directories(client PRIVATE src ${CMAKE_SOURCE_DIR}/vendor/include)
target_link_directories(client PRIVATE ${CMAKE_SOURCE_DIR}/vendor/lib)

target_link_libraries(client PUBLIC common)

if(WIN32)
    add_custom_command(TARGET client POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/vendor/lib/steam_api64.dll ${client_out_dir}/steam_api64.dll)
    add_custom_command(TARGET client POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE_DIR:client>/client.exe ${client_out_dir}/client.exe)
    target_link_libraries(client PUBLIC d3d11 dwrite d2d1 xaudio2 steam_api64 glfw3)

    if(NOT EXISTS ${CMAKE_SOURCE_DIR}/_out/client/data)
        add_custom_command(TARGET client POST_BUILD COMMAND cmd /c mklink /J \"${CMAKE_SOURCE_DIR}/_out/client/data\" \"${CMAKE_SOURCE_DIR}/assets_raw\")
    endif()
endif()

if(UNIX)
    add_custom_command(TARGET client POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/vendor/lib/libsteam_api.so $<TARGET_FILE_DIR:client>/libsteam_api.so)
    target_link_libraries(client PUBLIC common steam_api glfw GL glut)
endif()
    
add_custom_command(TARGET client POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/steam_appid.txt ${client_out_dir}/steam_appid.txt)
