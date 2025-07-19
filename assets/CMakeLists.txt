# copy function to directly copy files into output directory
# used for ui elements, sounds ... basically anything that doesn't have to be built i guess
# only takes one argument, the path of the file relative to the /assets directory
function(copy_file)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${ARGV0}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0} ${CMAKE_CURRENT_BINARY_DIR}/${ARGV0}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0}
    )

    list(APPEND COPY_OUTPUTS ${CMAKE_CURRENT_BINARY_DIR}/${ARGV0})
    set(COPY_OUTPUTS ${COPY_OUTPUTS} PARENT_SCOPE)
endfunction()

# export the specified blend file into the specified glb file
function(export_glb)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${ARGV1}
        COMMAND blender ${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0} --background --python ${CMAKE_SOURCE_DIR}/tools/blender_glb_export.py -- ${CMAKE_CURRENT_BINARY_DIR}/${ARGV1}
        DEPENDS ${CMAKE_SOURCE_DIR}/tools/blender_glb_export.py ${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0}
    )
    list(APPEND GLB_EXPORTS ${CMAKE_CURRENT_BINARY_DIR}/${ARGV1})
    set(GLB_EXPORTS ${GLB_EXPORTS} PARENT_SCOPE)
endfunction()

# export the specified blend file into the specified nvm file
function(export_nvm)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${ARGV1}
        COMMAND blender ${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0} --background --python ${CMAKE_SOURCE_DIR}/tools/blender_nvm_export.py -- ${CMAKE_CURRENT_BINARY_DIR}/${ARGV1}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0} ${CMAKE_SOURCE_DIR}/tools/blender_nvm_export.py
    )
    list(APPEND NVM_EXPORTS ${CMAKE_CURRENT_BINARY_DIR}/${ARGV1})
    set(NVM_EXPORTS ${NVM_EXPORTS} PARENT_SCOPE)
endfunction()

copy_file(Generic/missing_texture.bmp)
copy_file(UI/ChampUI.png)
copy_file(UI/HealthBar.png)
copy_file(UI/Buttons/MenuButton/MenuButton.bmp)
copy_file(UI/Buttons/MenuButton/MenuButtonClick.wav)
copy_file(UI/MoveTo/move_to.png)
copy_file(UI/MoveTo/move_to.pte)
copy_file(UI/MoveTo/move_to.pts)

copy_file(Persons/_Generic/AbilityIcon.bmp)

# Chess person
copy_file(Persons/ChessPerson/blast_area_2.pte)
copy_file(Persons/ChessPerson/blast_area.png)
copy_file(Persons/ChessPerson/blast_area.pte)
copy_file(Persons/ChessPerson/blast_area.pts)
copy_file(Persons/ChessPerson/heal_person.pts)
copy_file(Persons/ChessPerson/heal_person.pte)
copy_file(Persons/ChessPerson/particle.png)
export_glb(Persons/ChessPerson/chess_person.blend Persons/ChessPerson/chess_person.glb)
export_glb(Persons/ChessPerson/missile.blend Persons/ChessPerson/missile.glb)

# Minion
export_glb(Persons/Minion/minion.blend Persons/Minion/minion.glb)

# Map1
export_glb(Maps/map1/map1.blend Maps/map1/map1.glb)
export_nvm(Maps/map1/map1.blend Maps/map1/map1.nvm)

# Tower
export_glb(Buildings/Tower/tower.blend Buildings/Tower/tower.glb)

add_custom_target(assets
    DEPENDS ${COPY_OUTPUTS} ${GLB_EXPORTS} ${NVM_EXPORTS}
)