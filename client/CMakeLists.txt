link_directories($(CMAKE_SOURCE_DIR)/Bin ${CMAKE_SOURCE_DIR}/vendor/steam/redistributable_bin/win64)


function(copy_asset_folder)
    add_custom_command(TARGET client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different $<TARGET_PROPERTY:assets,BINARY_DIR>/${ARGV0} $<TARGET_FILE_DIR:client>/${ARGV0}
    )
endfunction()

function(compile_vertex_shader)
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Shaders/${ARGV0}.cso
        COMMAND FXC ${CMAKE_CURRENT_SOURCE_DIR}/Shaders/${ARGV0}.hlsl /T vs_5_0 /Fo ${CMAKE_CURRENT_BINARY_DIR}/Shaders/${ARGV0}.cso
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Shaders/${ARGV0}.hlsl
    )
    list(APPEND SHADER_FILES ${CMAKE_CURRENT_BINARY_DIR}/Shaders/${ARGV0}.cso)
    set(SHADER_FILES ${SHADER_FILES} PARENT_SCOPE)
endfunction()

function(compile_pixel_shader)
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Shaders/${ARGV0}.cso
        COMMAND FXC ${CMAKE_CURRENT_SOURCE_DIR}/Shaders/${ARGV0}.hlsl /T ps_5_0 /Fo ${CMAKE_CURRENT_BINARY_DIR}/Shaders/${ARGV0}.cso
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Shaders/${ARGV0}.hlsl
    )
    list(APPEND SHADER_FILES ${CMAKE_CURRENT_BINARY_DIR}/Shaders/${ARGV0}.cso)
    set(SHADER_FILES ${SHADER_FILES} PARENT_SCOPE)
endfunction()

add_executable(client WIN32
    src/Animation.cpp
    src/AnimationComponent.cpp
    src/Armature.cpp
    src/AudioSystem.cpp
    src/Camera.cpp
    src/CharacterSelectState.cpp
    src/Client.cpp
    src/ClientNetworkManager.cpp
    src/ConnectingState.cpp
    src/Direct3D.cpp
    src/FlatUnlitShader.cpp
    src/Game.cpp
    src/GameObject.cpp
    src/GLBFileLoader.cpp
    src/GLBShader.cpp
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
    src/MovementComponent.cpp
    src/ParticleEmitter.cpp
    src/ParticleShader.cpp
    src/ParticleSystem.cpp
    src/Renderer.cpp
    src/ServerBrowser.cpp
    src/Settings.cpp
    src/SettingsMenu.cpp
    src/VBox.cpp
    src/Vertex.cpp
    src/Window.cpp

    Resources/Cursor.cur
    Resources/cursor1.cur
    Resources/Hover.cur
    Resources/HoverAlly.cur
    Resources/icon1.ico
    Resources/Resource.rc
)


target_include_directories(client PRIVATE src ${CMAKE_SOURCE_DIR}/vendor/steam/public)

target_link_libraries(client PUBLIC steam_api64 common d3d11 dwrite d2d1 xaudio2)

add_custom_command(TARGET client POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/vendor/steam/redistributable_bin/win64/steam_api64.dll $<TARGET_FILE_DIR:client>/steam_api64.dll)
add_custom_command(TARGET client POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/steam_appid.txt $<TARGET_FILE_DIR:client>/steam_appid.txt)

copy_asset_folder(Buildings)
copy_asset_folder(Generic)
copy_asset_folder(Maps)
copy_asset_folder(Persons)
copy_asset_folder(UI)

compile_vertex_shader(GLBVertexShader)
compile_pixel_shader(GLBPixelShader)
compile_vertex_shader(flat_unlit_vs)
compile_pixel_shader(flat_unlit_ps)
compile_vertex_shader(particle_vs)

add_custom_target(shaders
    DEPENDS ${SHADER_FILES}
)
add_dependencies(client
    assets
    shaders
)