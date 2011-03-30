# Setup Versions
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/Version.h)


# Source files for toadlet_peeper
set (PEEPER_SRC
	BackableBuffer.cpp
	BackableVertexFormat.cpp
	BackablePixelBufferRenderTarget.cpp
	BackableTexture.cpp
	IndexBufferAccessor.cpp
	IndexData.cpp
	SectionTexture.cpp
	SequenceTexture.cpp
	TextureStage.cpp
	VertexBufferAccessor.cpp
	VertexData.cpp
	Version.cpp
)


# Headers
set (PEEPER_HEADERS
	BackableBuffer.h
	BackableVertexFormat.h
	BackablePixelBufferRenderTarget.h
	BackableTexture.h
	BackableTextureMipPixelBuffer.h
	BlendState.h
	Buffer.h
	BufferDestroyedListener.h
	CapabilityState.h
	DepthState.h
	FogState.h
	IndexBuffer.h
	IndexBufferAccessor.h
	IndexData.h
	MaterialState.h
	LightState.h
	Program.h
	Query.h
	RenderTarget.h
	Renderer.h
	SectionTexture.h
	SequenceTexture.h
	Shader.h
	PixelBuffer.h
	PixelBufferRenderTarget.h
	PointState.h
	RenderStateSet.h
	Texture.h
	TextureBlend.h
	TextureStage.h
	Types.h
	VertexBuffer.h
	VertexBufferAccessor.h
	VertexData.h
	VertexFormat.h
	Viewport.h
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
		set (TOADLET_HAS_D3DM ON PARENT_SCOPE)
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
		set (TOADLET_HAS_D3D9 ON PARENT_SCOPE)
		add_subdirectory (plugins/d3d9renderer)
	else (D3D9_FOUND)
		message ("Direct 3D 9 was not found. The peeper_d3d9renderer will not be built")
	endif (D3D9_FOUND)
	find_package (D3D10)
	if (D3D10_FOUND)
		set (TOADLET_HAS_D3D10 ON PARENT_SCOPE)
		add_subdirectory (plugins/d3d10renderer)
	else (D3D10_FOUND)
		message ("Direct 3D 10 was not found. The peeper_d3d10renderer will not be built")
	endif (D3D10_FOUND)
	find_package (D3D11)
	if (D3D11_FOUND)
		set (TOADLET_HAS_D3D11 ON PARENT_SCOPE)
		add_subdirectory (plugins/d3d11renderer)
	else (D3D11_FOUND)
		message ("Direct 3D 11 was not found. The peeper_d3d11renderer will not be built")
	endif (D3D11_FOUND)
	find_package (GLEW)
	if (GLEW_FOUND)
		set (TOADLET_HAS_OPENGL ON PARENT_SCOPE)
		add_subdirectory (plugins/glrenderer)
	else (GLEW_FOUND)
		message ("GLEW was not found. The peeper_glrenderer will not be built")
	endif (GLEW_FOUND)
elseif (TOADLET_PLATFORM_IPHONE)
	# Both iPhoneOS and iPhoneSimulator always have opengles
	set (TOADLET_HAS_OPENGL ON PARENT_SCOPE)
	add_subdirectory (plugins/glrenderer)
elseif (TOADLET_PLATFORM_OSX)
	# OSX requires we find OpenGL and GLEW
	find_package (OPENGL)
	find_package (GLEW)
	if (OPENGL_FOUND AND GLEW_FOUND)
		set (TOADLET_HAS_OPENGL ON PARENT_SCOPE)
		add_subdirectory (plugins/glrenderer)
	else (OPENGL_FOUND AND GLEW_FOUND)
		message ("OpenGL and/or GLEW was not found. The peeper_glrenderer will not be built")
	endif (OPENGL_FOUND AND GLEW_FOUND)
elseif (TOADLET_PLATFORM_POSIX)
	# On posix we find opengl dynamically provided we have glew
	find_package (GLEW)
	if (GLEW_FOUND)
		set (TOADLET_HAS_OPENGL ON PARENT_SCOPE)
		add_subdirectory (plugins/glrenderer)
	else (GLEW_FOUND)
		message ("GLEW was not found. The peeper_glrenderer will not be built")
	endif (GLEW_FOUND)
endif (TOADLET_PLATFORM_WINCE)

