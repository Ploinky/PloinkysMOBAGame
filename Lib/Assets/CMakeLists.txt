function(build_map INPUT OUTPUT_NVM)
	add_custom_command(
		OUTPUT ${TEMP_BUILD_DIR}/${OUTPUT_NVM}

		COMMAND blender Source/${INPUT} --background --python ${CMAKE_SOURCE_DIR}/Tool/blender_nvm_export.py -- ${TEMP_BUILD_DIR}/${OUTPUT_NVM}

		WORKING_DIRECTORY ${ASSET_SOURCE_DIR}

		DEPENDS ${ASSET_SOURCE_DIR}/Source/${INPUT}
	)
endfunction()

function(add_copy_command INPUT OUTPUT)
	add_custom_command(
		OUTPUT ${TEMP_BUILD_DIR}/${OUTPUT}
		COMMAND ${CMAKE_COMMAND} -E copy ${ASSET_SOURCE_DIR}/Source/${INPUT} ${TEMP_BUILD_DIR}/${OUTPUT}
		DEPENDS ${ASSET_SOURCE_DIR}/Source/${INPUT}
		COMMENT "Copying ${TEMP_BUILD_DIR}/${OUTPUT}"
	)
endfunction()

function(pak_folder INPUT OUTPUT)
	add_custom_command(
		OUTPUT ${CMAKE_SOURCE_DIR}/Dist/Game/${OUTPUT}
		COMMAND PakUtil ${INPUT} ${CMAKE_SOURCE_DIR}/Dist/Game/${OUTPUT}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Temp
	)
endfunction()

set(ASSET_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(GameAssets
	COMMENT "Creating GameAssets"
)
add_custom_target(ServerAssets
	COMMENT "Creating ServerAssets"
)

add_subdirectory(Source)

