add_custom_command(
	OUTPUT ${TEMP_BUILD_DIR}/Buildings/Tower/tower.glb
	COMMAND blender -b ${CMAKE_CURRENT_SOURCE_DIR}/tower.blend --background --python ${CMAKE_SOURCE_DIR}/Tool/blender_glb_export.py -- ${TEMP_BUILD_DIR}/Buildings/Tower/tower.glb
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tower.blend
	COMMENT "Exporting tower.glb"
)

add_custom_COMMAND(OUTPUT ${DIST_BUILD_DIR}/Game/$<CONFIG>/Buildings/Tower.pak
	DEPENDS ${TEMP_BUILD_DIR}/Buildings/Tower/tower.glb

	COMMAND ${CMAKE_COMMAND} -E make_directory ${DIST_BUILD_DIR}/Game/$<CONFIG>/Buildings
	COMMAND ${CMAKE_COMMAND} -E make_directory Buildings/Tower
	
	COMMAND PakUtil Buildings/Tower ${DIST_BUILD_DIR}/Game/$<CONFIG>/Buildings/Tower.pak
	WORKING_DIRECTORY ${TEMP_BUILD_DIR}
	COMMENT "Packing tower"
)

add_custom_target(Tower
	DEPENDS ${DIST_BUILD_DIR}/Game/$<CONFIG>/Buildings/Tower.pak
)

add_dependencies(GameAssets Tower)
