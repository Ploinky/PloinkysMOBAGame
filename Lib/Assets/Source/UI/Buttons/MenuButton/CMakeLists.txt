add_copy_command(UI/Buttons/MenuButton/MenuButton.bmp UI/Buttons/MenuButton/MenuButton.bmp)

add_custom_command(OUTPUT ${DIST_BUILD_DIR}/Game/$<CONFIG>/UI/Buttons/MenuButton.pak
	DEPENDS ${TEMP_BUILD_DIR}/UI/Buttons/MenuButton/MenuButton.bmp

	COMMAND ${CMAKE_COMMAND} -E make_directory ${DIST_BUILD_DIR}/Game/$<CONFIG>/UI/Buttons
	COMMAND ${CMAKE_COMMAND} -E make_directory UI/Buttons/MenuButton
	
	COMMAND PakUtil UI/Buttons/MenuButton ${DIST_BUILD_DIR}/Game/$<CONFIG>/UI/Buttons/MenuButton.pak
	WORKING_DIRECTORY ${TEMP_BUILD_DIR}
)

add_custom_target(MenuButton
	DEPENDS ${DIST_BUILD_DIR}/Game/$<CONFIG>/UI/Buttons/MenuButton.pak
)

add_dependencies(GameAssets MenuButton)
