add_custom_command(
	OUTPUT ${TEMP_BUILD_DIR}/Persons/Minion/minion.glb
	COMMAND blender -b ${CMAKE_CURRENT_SOURCE_DIR}/minion.blend --background --python ${CMAKE_SOURCE_DIR}/Tool/blender_glb_export.py -- ${TEMP_BUILD_DIR}/Persons/Minion/minion.glb
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/minion.blend
	COMMENT "Exporting minion.glb"
)


add_custom_command(OUTPUT ${DIST_BUILD_DIR}/Game/$<CONFIG>/Persons/Minion.pak
	DEPENDS ${TEMP_BUILD_DIR}/Persons/Minion/minion.glb
	
	COMMAND ${CMAKE_COMMAND} -E make_directory ${DIST_BUILD_DIR}/Game/$<CONFIG>/Persons
	COMMAND ${CMAKE_COMMAND} -E make_directory Persons/Minion

	COMMAND PakUtil Persons/Minion ${DIST_BUILD_DIR}/Game/$<CONFIG>/Persons/Minion.pak

	WORKING_DIRECTORY ${TEMP_BUILD_DIR}
	COMMENT "Packing Minion.pak"
)

add_custom_target(Minion
	DEPENDS ${DIST_BUILD_DIR}/Game/$<CONFIG>/Persons/Minion.pak
)

add_dependencies(GameAssets Minion)
