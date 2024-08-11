function(compile_shader SHADER_NAME SHADER_TYPE)
	add_custom_command(
		OUTPUT ${CMAKE_SOURCE_DIR}/Temp/Shaders/${SHADER_NAME}.cso
		COMMAND fxc ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_NAME}.hlsl /T ${SHADER_TYPE} /Fo ${CMAKE_SOURCE_DIR}/Temp/Shaders/${SHADER_NAME}.cso /Zi
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_NAME}.hlsl
		COMMENT "Compiling shader ${SHADER_NAME}"
	)
endfunction()

compile_shader(FontPixelShader ps_5_0)
compile_shader(FontVertexShader vs_5_0)
compile_shader(GLBVertexShader vs_5_0)
compile_shader(GLBPixelShader ps_5_0)
compile_shader(GUIPixelShader ps_5_0)
compile_shader(GUIVertexShader vs_5_0)
compile_shader(particle_vs vs_5_0)
compile_shader(pixel_ps ps_5_0)
compile_shader(skinned_textured_vs vs_5_0)
compile_shader(test_vs vs_5_0)
compile_shader(texture_ps ps_5_0)
compile_shader(texture_vs vs_5_0)
compile_shader(vertex_vs vs_5_0)


add_custom_COMMAND(OUTPUT ${DIST_BUILD_DIR}/Game/$<CONFIG>/Shaders.pak
	DEPENDS ${CMAKE_SOURCE_DIR}/Temp/Shaders/test_vs.cso
	DEPENDS ${CMAKE_SOURCE_DIR}/Temp/Shaders/FontPixelShader.cso
	DEPENDS ${CMAKE_SOURCE_DIR}/Temp/Shaders/FontVertexShader.cso
	DEPENDS ${CMAKE_SOURCE_DIR}/Temp/Shaders/GUIPixelShader.cso
	DEPENDS ${CMAKE_SOURCE_DIR}/Temp/Shaders/GUIVertexShader.cso
	DEPENDS ${CMAKE_SOURCE_DIR}/Temp/Shaders/particle_vs.cso
	DEPENDS ${CMAKE_SOURCE_DIR}/Temp/Shaders/pixel_ps.cso
	DEPENDS ${CMAKE_SOURCE_DIR}/Temp/Shaders/skinned_textured_vs.cso
	DEPENDS ${CMAKE_SOURCE_DIR}/Temp/Shaders/texture_ps.cso
	DEPENDS ${CMAKE_SOURCE_DIR}/Temp/Shaders/texture_vs.cso
	DEPENDS ${CMAKE_SOURCE_DIR}/Temp/Shaders/vertex_vs.cso
	DEPENDS ${CMAKE_SOURCE_DIR}/Temp/Shaders/GLBPixelShader.cso
	DEPENDS ${CMAKE_SOURCE_DIR}/Temp/Shaders/GLBVertexShader.cso
	DEPENDS ${CMAKE_SOURCE_DIR}/Temp/Shaders/test_vs.cso

	COMMAND PakUtil Shaders ${DIST_BUILD_DIR}/Game/$<CONFIG>/Shaders.pak
	WORKING_DIRECTORY ${TEMP_BUILD_DIR}
	COMMENT "Packing shaders"
)

add_custom_target(CompileShaders
	DEPENDS ${DIST_BUILD_DIR}/Game/$<CONFIG>/Shaders.pak
)