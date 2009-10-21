# Source files for toadlet_peeper
set (PEEPER_SRC
	AnimatedTexture.cpp
	Buffer.cpp
	IndexBuffer.cpp
	IndexBufferAccessor.cpp
	IndexData.cpp
	Light.cpp
	Program.cpp
	RenderTarget.cpp
	RenderTexture.cpp
	Shader.cpp
	Texture.cpp
	TextureSection.cpp
	TextureSequence.cpp
	TextureStage.cpp
	VertexBuffer.cpp
	VertexBufferAccessor.cpp
	VertexData.cpp
	VertexElement.cpp
	VertexFormat.cpp
	Version.cpp
)


# Headers
set (PEEPER_HEADERS
	AnimatedTexture.h
	Blend.h
	Buffer.h
	CapabilitySet.h
	Color.h
	Colors.h
	IndexBuffer.h
	IndexBufferAccessor.h
	IndexData.h
	Light.h
	LightEffect.h
	Program.h
	RenderContext.h
	RenderTarget.h
	RenderTexture.h
	Renderer.h
	Shader.h
	StatisticsSet.h
	Texture.h
	TextureBlend.h
	TextureSection.h
	TextureSequence.h
	TextureStage.h
	Types.h
	VertexBuffer.h
	VertexBufferAccessor.h
	VertexData.h
	VertexElement.h
	VertexFormat.h
	Viewport.h
	Visual.h
	Version.h
)


# Dynamic library
if (NOT TOADLET_PLATFORM_IPHONE)
	add_library (toadlet_peeper SHARED ${PEEPER_SRC} ${PEEPER_HEADERS})
	set_target_properties (toadlet_peeper PROPERTIES ${DYNAMIC_LIB_PROPS})
	target_link_libraries (toadlet_peeper toadlet_egg)
	install (TARGETS toadlet_peeper DESTINATION lib)
endif (NOT TOADLET_PLATFORM_IPHONE)


# Static library
add_library (toadlet_peeper_s STATIC ${PEEPER_SRC} ${PEEPER_HEADERS})
set_target_properties (toadlet_peeper_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_peeper_s toadlet_egg_s)
install (TARGETS toadlet_peeper_s DESTINATION lib)


# Platform specific modules
if (TOADLET_PLATFORM_WINCE)	
	find_package (D3DM)
	if (D3DM_FOUND)
		set (TOADLET_HAS_DIRECT3DMOBILE ON PARENT_SCOPE)
		add_subdirectory (plugins/d3dmrenderer)
	else (D3DM_FOUND)		
		message ("Direct 3D mobile was not found. The peeper_d3dmrenderer will not be built")
	endif (D3DM_FOUND)
	# Wince always has opengles
	set (TOADLET_HAS_OPENGL ON PARENT_SCOPE)
	add_subdirectory (plugins/glrenderer)
elseif (TOADLET_PLATFORM_WIN32)
	find_package (D3D9)
	if (D3D9_FOUND)
		set (TOADLET_HAS_DIRECT3D9 ON PARENT_SCOPE)
		add_subdirectory (plugins/d3d9renderer)
	else (D3D9_FOUND)
		message ("Direct 3D 9 was not found. The peeper_d3d9renderer will not be built")
	endif (D3D9_FOUND)
	find_package (OpenGL)
	if (OPENGL_FOUND)
		find_package (GLEW)
		if (GLEW_FOUND)
			set (TOADLET_HAS_OPENGL ON PARENT_SCOPE)
			add_subdirectory (plugins/glrenderer)
		else (GLEW_FOUND)
			message ("GLEW was not found. The peeper_glrenderer will not be built")
		endif (GLEW_FOUND)
	else (OPENGL_FOUND)
		message ("OpenGL was not found. The peeper_glrenderer will not be built")
	endif (OPENGL_FOUND)
elseif (TOADLET_PLATFORM_IPHONE)
	# Both iPhoneOS and iPhoneSimulator always have opengles
	set (TOADLET_HAS_OPENGL ON PARENT_SCOPE)
	add_subdirectory (plugins/glrenderer)
elseif (TOADLET_PLATFORM_POSIX)
	find_package (OpenGL)
	if (OPENGL_FOUND)
		find_package (GLEW)
		if (GLEW_FOUND)
			set (TOADLET_HAS_OPENGL ON PARENT_SCOPE)
			add_subdirectory (plugins/glrenderer)
		else (GLEW_FOUND)
			message ("GLEW was not found. The peeper_glrenderer will not be built")
		endif (GLEW_FOUND)
	else (OPENGL_FOUND)
		message ("OpenGL was not found. The peeper_glrenderer will not be built")
	endif (OPENGL_FOUND)
endif (TOADLET_PLATFORM_WINCE)

