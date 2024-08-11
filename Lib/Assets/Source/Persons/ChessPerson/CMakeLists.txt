add_copy_command(Persons/ChessPerson/blast_area.pts Persons/ChessPerson/blast_area.pts)
add_copy_command(Persons/ChessPerson/blast_area.pte Persons/ChessPerson/blast_area.pte)
add_copy_command(Persons/ChessPerson/blast_area_2.pte Persons/ChessPerson/blast_area_2.pte)
add_copy_command(Persons/ChessPerson/blast_area.png Persons/ChessPerson/blast_area.png)
add_copy_command(Persons/ChessPerson/heal_person.pts Persons/ChessPerson/heal_person.pts)
add_copy_command(Persons/ChessPerson/heal_person.pte Persons/ChessPerson/heal_person.pte)
add_copy_command(Persons/ChessPerson/particle.png Persons/ChessPerson/particle.png)

add_custom_command(
	OUTPUT ${TEMP_BUILD_DIR}/Persons/ChessPerson/chess_person.glb
	COMMAND blender -b ${CMAKE_CURRENT_SOURCE_DIR}/chess_person.blend --background --python ${CMAKE_SOURCE_DIR}/Tool/blender_glb_export.py -- ${TEMP_BUILD_DIR}/Persons/ChessPerson/chess_person.glb
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/chess_person.blend
	COMMENT "Exporting chess_person.glb"
)

add_custom_command(
	OUTPUT ${TEMP_BUILD_DIR}/Persons/ChessPerson/missile.glb
	COMMAND blender -b ${CMAKE_CURRENT_SOURCE_DIR}/missile.blend --background --python ${CMAKE_SOURCE_DIR}/Tool/blender_glb_export.py -- ${TEMP_BUILD_DIR}/Persons/ChessPerson/missile.glb
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/missile.blend
	COMMENT "Exporting missile.glb"
)

add_custom_command(OUTPUT ${DIST_BUILD_DIR}/Game/$<CONFIG>/Persons/ChessPerson.pak
	DEPENDS ${TEMP_BUILD_DIR}/Persons/ChessPerson/chess_person.glb
	DEPENDS ${TEMP_BUILD_DIR}/Persons/ChessPerson/missile.glb
	DEPENDS ${TEMP_BUILD_DIR}/Persons/ChessPerson/blast_area.pts
	DEPENDS ${TEMP_BUILD_DIR}/Persons/ChessPerson/blast_area.pte
	DEPENDS ${TEMP_BUILD_DIR}/Persons/ChessPerson/blast_area_2.pte
	DEPENDS ${TEMP_BUILD_DIR}/Persons/ChessPerson/blast_area.png
	DEPENDS ${TEMP_BUILD_DIR}/Persons/ChessPerson/heal_person.pts
	DEPENDS ${TEMP_BUILD_DIR}/Persons/ChessPerson/heal_person.pte
	DEPENDS ${TEMP_BUILD_DIR}/Persons/ChessPerson/particle.png

	COMMAND ${CMAKE_COMMAND} -E make_directory ${DIST_BUILD_DIR}/Game/$<CONFIG>/Persons
	COMMAND ${CMAKE_COMMAND} -E make_directory Persons/ChessPerson
	
	COMMAND PakUtil Persons/ChessPerson ${DIST_BUILD_DIR}/Game/$<CONFIG>/Persons/ChessPerson.pak
	WORKING_DIRECTORY ${TEMP_BUILD_DIR}
	
	COMMENT "Packing ChessPerson"
)

add_custom_target(ChessPerson
	DEPENDS ${DIST_BUILD_DIR}/Game/$<CONFIG>/Persons/ChessPerson.pak
)

add_dependencies(GameAssets ChessPerson)