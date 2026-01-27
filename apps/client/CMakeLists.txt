# collect paths
get_property(dist_dir GLOBAL PROPERTY dist_dir)
get_property(out_dir GLOBAL PROPERTY out_dir)
set(client_out_dir ${out_dir}/client)


# shader compilation functions
function(compile_vertex_shader)
    add_custom_command(OUTPUT ${client_out_dir}/shaders/${ARGV0}.cso
        COMMAND FXC ${CMAKE_CURRENT_SOURCE_DIR}/shaders/${ARGV0}.hlsl /T vs_5_0 /Fo ${client_out_dir}/shaders/${ARGV0}.cso /Zi
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/${ARGV0}.hlsl
    )
    list(APPEND SHADER_FILES ${client_out_dir}/shaders/${ARGV0}.cso)
    set(SHADER_FILES ${SHADER_FILES} PARENT_SCOPE)
endfunction()

function(compile_pixel_shader)
    add_custom_command(OUTPUT ${client_out_dir}/shaders/${ARGV0}.cso
        COMMAND FXC ${CMAKE_CURRENT_SOURCE_DIR}/shaders/${ARGV0}.hlsl /T ps_5_0 /Fo ${client_out_dir}/shaders/${ARGV0}.cso /Zi
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/${ARGV0}.hlsl
    )
    list(APPEND SHADER_FILES ${client_out_dir}/shaders/${ARGV0}.cso)
    set(SHADER_FILES ${SHADER_FILES} PARENT_SCOPE)
endfunction()


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
    
    src/core/input/input-manager.cpp
    src/core/network/enet-network-engine.cpp

    src/game/client-game-state.cpp

    src/game/systems/animation-system.cpp
    src/game/systems/particle-system.cpp
    src/game/systems/health-system.cpp
    
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
    add_custom_command(TARGET client POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE_DIR:client>/client.exe ${client_out_dir}/client.exe)
    target_link_libraries(client PUBLIC d3d11 dwrite d2d1 xaudio2 glfw3)

    if(NOT EXISTS ${CMAKE_SOURCE_DIR}/_out/client/data)
        add_custom_command(TARGET client POST_BUILD COMMAND cmd /c mklink /J \"${CMAKE_SOURCE_DIR}/_out/client/data\" \"${CMAKE_SOURCE_DIR}/resources/assets_raw\")
    endif()
endif()

if(UNIX)
    target_link_libraries(client PUBLIC common glfw GL glut)
endif()
    
add_custom_command(TARGET client POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different  ${CMAKE_SOURCE_DIR}/resources/config_client/ ${client_out_dir}/cfg/)

# shaders
if(WIN32)
    compile_vertex_shader(glb.vert)
    compile_pixel_shader(glb.pixel)
    compile_vertex_shader(flat_unlit.vert)
    compile_pixel_shader(flat_unlit.pixel)
    compile_vertex_shader(particle.vert)
    compile_pixel_shader(particle.pixel)
    compile_pixel_shader(texture.pixel)
endif()

add_custom_target(shader_target
    DEPENDS ${SHADER_FILES}
)
add_dependencies(client 
shader_target)