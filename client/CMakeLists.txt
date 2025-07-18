link_directories($(CMAKE_SOURCE_DIR)/Bin ${CMAKE_SOURCE_DIR}/vendor/steam/redistributable_bin/win64)

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
)

target_include_directories(client PRIVATE src ${CMAKE_SOURCE_DIR}/vendor/steam/public)

target_link_libraries(client PUBLIC steam_api64 common d3d11 dwrite d2d1 xaudio2)