enable_language("RC")

add_executable(Game WIN32
	Source/Animation.cpp
	Source/AnimationComponent.cpp
	Source/Armature.cpp
	Source/AudioSystem.cpp
	Source/Camera.cpp
	Source/Client.cpp
	Source/ClientNetworkManager.cpp
	Source/Direct3d.cpp
	Source/Game.cpp
	Source/GameObject.cpp
	Source/GLBFileLoader.cpp
	Source/GLBShader.cpp
	Source/GuiButton.cpp
	Source/GuiCheckbox.cpp
	Source/GuiSelector.cpp
	Source/GuiServerElement.cpp
	Source/GuiSimplePane.cpp
	Source/GuiTextfield.cpp
	Source/LoadingState.cpp
	Source/Lobby.cpp
	Source/Main.cpp
	Source/MainMenu.cpp
	Source/ParticleEmitter.cpp
	Source/ParticleShader.cpp
	Source/ParticleSystem.cpp
	Source/Renderer.cpp
	Source/ServerBrowser.cpp
	Source/Settings.cpp
	Source/SettingsMenu.cpp
	Source/VBox.cpp
	Source/Vertex.cpp
	Source/Window.cpp

	Resources/Client.rc
)

target_include_directories(Game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source)

# statically linked included libraries
target_link_libraries(Game PRIVATE Common PloinkysJSONLibrary SteamAPI)

# dynamically linked libraries
target_link_libraries(Game PUBLIC steam_api64 d3d11 d2d1 dwrite)

set_target_properties(Game
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Dist/Game
)

add_custom_target(print_config ALL
	# Prints "Config is Debug" in this single-config case
	COMMAND ${CMAKE_COMMAND} -E echo "Config is $<CONFIG>"
	VERBATIM
)

add_custom_command(TARGET Game POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/Bin/steam_api64.dll ${CMAKE_SOURCE_DIR}/Dist/Game/$<CONFIG>/steam_api64.dll)

add_custom_command(TARGET Game
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/Bin/steam_appid.txt ${CMAKE_SOURCE_DIR}/Dist/Game/$<CONFIG>/steam_appid.txt
)

add_subdirectory(Shaders)

add_dependencies(Game CompileShaders GameAssets)