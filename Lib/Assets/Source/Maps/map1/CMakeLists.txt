build_map(Maps/map1/map1.blend Maps/map1/map1.nvm)

add_custom_command(
	OUTPUT ${TEMP_BUILD_DIR}/Maps/map1/map1.glb
	COMMAND blender -b ${CMAKE_CURRENT_SOURCE_DIR}/map1.blend --background --python ${CMAKE_SOURCE_DIR}/Tool/blender_glb_export.py -- ${TEMP_BUILD_DIR}/Maps/map1/map1.glb
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/map1.blend
	COMMENT "Exporting map1.glb"
)

add_custom_command(OUTPUT ${DIST_BUILD_DIR}/Game/$<CONFIG>/Maps/Map1.pak
	DEPENDS ${TEMP_BUILD_DIR}/Maps/map1/map1.nvm
	DEPENDS ${TEMP_BUILD_DIR}/Maps/map1/map1.glb

	COMMAND ${CMAKE_COMMAND} -E make_directory Maps/map1
	COMMAND ${CMAKE_COMMAND} -E make_directory ${DIST_BUILD_DIR}/Game/$<CONFIG>/Maps

	COMMAND PakUtil Maps/map1 ${DIST_BUILD_DIR}/Game/$<CONFIG>/Maps/Map1.pak

	COMMENT "Packing map1.pak for Game"
	
	WORKING_DIRECTORY ${TEMP_BUILD_DIR}
)

add_custom_command(OUTPUT ${DIST_BUILD_DIR}/Server/$<CONFIG>/Maps/Map1.pak
	DEPENDS ${TEMP_BUILD_DIR}/Maps/map1/map1.nvm

	COMMAND ${CMAKE_COMMAND} -E make_directory Maps/map1
	COMMAND ${CMAKE_COMMAND} -E make_directory ${DIST_BUILD_DIR}/Server/$<CONFIG>/Maps

	COMMAND PakUtil Maps/map1 ${DIST_BUILD_DIR}/Server/$<CONFIG>/Maps/Map1.pak

	COMMENT "Packing map1.pak for Server"
	
	WORKING_DIRECTORY ${TEMP_BUILD_DIR}
)

add_custom_target(GameMap1
	DEPENDS ${DIST_BUILD_DIR}/Game/$<CONFIG>/Maps/Map1.pak
)

add_custom_target(ServerMap1
	DEPENDS ${DIST_BUILD_DIR}/Server/$<CONFIG>/Maps/Map1.pak
)

add_dependencies(GameAssets GameMap1)
add_dependencies(ServerAssets ServerMap1)
