add_copy_command(UI/MoveTo/move_to.pts UI/MoveTo/move_to.pts)
add_copy_command(UI/MoveTo/move_to.pte UI/MoveTo/move_to.pte)
add_copy_command(UI/MoveTo/move_to.png UI/MoveTo/move_to.png)

add_custom_command(OUTPUT ${DIST_BUILD_DIR}/Game/$<CONFIG>/UI/MoveTo.pak
	DEPENDS ${TEMP_BUILD_DIR}/UI/MoveTo/move_to.pts
	DEPENDS ${TEMP_BUILD_DIR}/UI/MoveTo/move_to.pte
	DEPENDS ${TEMP_BUILD_DIR}/UI/MoveTo/move_to.png

	COMMAND ${CMAKE_COMMAND} -E make_directory ${DIST_BUILD_DIR}/Game/$<CONFIG>/UI
	COMMAND ${CMAKE_COMMAND} -E make_directory UI/MoveTo
	
	COMMAND PakUtil UI/MoveTo ${DIST_BUILD_DIR}/Game/$<CONFIG>/UI/MoveTo.pak
	WORKING_DIRECTORY ${TEMP_BUILD_DIR}

	COMMENT "Packing MoveTo.pak"
)

add_custom_target(MoveTo
	DEPENDS ${DIST_BUILD_DIR}/Game/$<CONFIG>/UI/MoveTo.pak
)

add_dependencies(GameAssets MoveTo)