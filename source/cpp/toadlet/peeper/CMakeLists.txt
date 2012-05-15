# Setup Versions
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/Version.h)


# Source files for toadlet_peeper
set (PEEPER_SRC
	BackableBuffer.cpp
	BackableVertexFormat.cpp
	BackablePixelBufferRenderTarget.cpp
	BackableRenderState.cpp
	BackableShaderState.cpp
	BackableShader.cpp
	BackableTexture.cpp
	IndexBufferAccessor.cpp
	IndexData.cpp
	TextureFormatConversion.cpp
	VariableBufferFormat.cpp
	VertexBufferAccessor.cpp
	VertexData.cpp
	Version.cpp
)


# Headers
set (PEEPER_HEADERS
	BackableBuffer.h
	BackableVertexFormat.h
	BackablePixelBufferRenderTarget.h
	BackableTextureMipPixelBuffer.h
	BackableRenderState.h
	BackableShaderState.h
	BackableShader.h
	BackableTexture.h
	BlendState.h
	Buffer.h
	VariableBuffer.h
	VariableBufferFormat.h
	DepthState.h
	FogState.h
	IndexBuffer.h
	IndexBufferAccessor.h
	IndexData.h
	MaterialState.h
	LightState.h
	Query.h
	PixelBuffer.h
	PixelBufferRenderTarget.h
	PointState.h
	RasterizerState.h
	RenderDevice.h
	RenderCaps.h
	RenderState.h
	RenderTarget.h
	SamplerState.h
	Shader.h
	ShaderState.h
	Texture.h
	TextureFormat.h
	TextureFormatConversion.h
	TextureState.h
	Types.h
	VertexBuffer.h
	VertexBufferAccessor.h
	VertexData.h
	VertexFormat.h
	Viewport.h
	Version.h
	WindowRenderTargetFormat.h
)


# Give the headers and namespaces parent scope for inclusion into the master peeper.h file
set (PEEPER_HEADERS ${PEEPER_HEADERS} PARENT_SCOPE)
set (PEEPER_NAMESPACES ${PEEPER_NAMESPACES} PARENT_SCOPE)


# If we are using swig create the wrapper file from the interface files
if (SWIG_FOUND AND TOADLET_PEEPER_INT)
	swig_wrap_files (peeper_wrap.cpp "${TOADLET_PEEPER_INT}")	
	set (PEEPER_SRC ${PEEPER_SRC} peeper_wrap.cpp)
endif (SWIG_FOUND AND TOADLET_PEEPER_INT)


# Dynamic library
if (NOT TOADLET_PLATFORM_IOS)
	add_library (toadlet_peeper SHARED ${PEEPER_SRC} ${PEEPER_HEADERS})
	set_target_properties (toadlet_peeper PROPERTIES ${DYNAMIC_LIB_PROPS})
	target_link_libraries (toadlet_peeper toadlet_egg)
	install (TARGETS toadlet_peeper DESTINATION ${LIB_INSTALL_DIR})
endif (NOT TOADLET_PLATFORM_IOS)


# Static library
add_library (toadlet_peeper_s STATIC ${PEEPER_SRC} ${PEEPER_HEADERS})
set_target_properties (toadlet_peeper_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_peeper_s toadlet_egg_s)
install (TARGETS toadlet_peeper_s DESTINATION ${LIB_INSTALL_DIR})

# HACK: install the library on iOS. This will disappear once cmake issue:0012506 is resolved. 
if (TOADLET_PLATFORM_IOS)
    install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/\$ENV{CONFIGURATION}\$ENV{EFFECTIVE_PLATFORM_NAME}/ DESTINATION ${LIB_INSTALL_DIR} FILES_MATCHING PATTERN "*.a")
endif (TOADLET_PLATFORM_IOS)


# Platform specific modules
if (TOADLET_PLATFORM_ANDROID)
	find_library (EGL EGL)
	find_library (GLESV1 GLESv1_CM)
	find_library (GLESV2 GLESv2)
	if (ANDROID_NDK_API_LEVEL LESS 9)
		if (GLESV1)
			set (GLESV1_LIBS ${GLESV1})
			set (GLESV2_LIBS ${GLESV2})
		endif (GLESV1)
	else (ANDROID_NDK_API_LEVEL LESS 9)
		if (GLESV1 AND EGL)
			set (GLESV1_LIBS ${GLESV1} ${EGL})
			set (GLESV2_LIBS ${GLESV2} ${EGL})
		endif (GLESV1 AND EGL)
	endif (ANDROID_NDK_API_LEVEL LESS 9)
	if (GLESV1_LIBS)
		set (TOADLET_HAS_OPENGL ON PARENT_SCOPE)
		add_subdirectory (plugins/gles1renderdevice)
	else (GLESV1_LIBS)
		message ("GLESv1_LIBS were not found. The peeper_gles1renderdevice will not be built")
	endif (GLESV1_LIBS)
	if (GLESV2_LIBS)
		set (TOADLET_HAS_OPENGL ON PARENT_SCOPE)
		add_subdirectory (plugins/gles2renderdevice)
	else (GLESV2_LIBS)
		message ("GLESv2_LIBS were not found. The peeper_gles2renderdevice will not be built")
	endif (GLESV2_LIBS)
elseif (TOADLET_PLATFORM_WINCE)	
	find_package (D3DM)
	if (D3DM_FOUND)
		set (TOADLET_HAS_D3DM ON PARENT_SCOPE)
		add_subdirectory (plugins/d3dmrenderdevice)
	else (D3DM_FOUND)
		message ("Direct 3D mobile was not found. The peeper_d3dmrenderdevice will not be built")
	endif (D3DM_FOUND)
	# Wince always has opengles
	set (TOADLET_HAS_OPENGL ON PARENT_SCOPE)
	add_subdirectory (plugins/gles1renderdevice)
elseif (TOADLET_PLATFORM_WIN32)
	find_package (D3D9)
	if (D3D9_FOUND)
		set (TOADLET_HAS_D3D9 ON PARENT_SCOPE)
		add_subdirectory (plugins/d3d9renderdevice)
	else (D3D9_FOUND)
		message ("Direct 3D 9 was not found. The peeper_d3d9renderdevice will not be built")
	endif (D3D9_FOUND)
	find_package (D3D10)
	if (D3D10_FOUND)
		set (TOADLET_HAS_D3D10 ON PARENT_SCOPE)
		add_subdirectory (plugins/d3d10renderdevice)
	else (D3D10_FOUND)
		message ("Direct 3D 10 was not found. The peeper_d3d10renderdevice will not be built")
	endif (D3D10_FOUND)
	find_package (D3D11)
	if (D3D11_FOUND)
		set (TOADLET_HAS_D3D11 ON PARENT_SCOPE)
		add_subdirectory (plugins/d3d11renderdevice)
	else (D3D11_FOUND)
		message ("Direct 3D 11 was not found. The peeper_d3d11renderdevice will not be built")
	endif (D3D11_FOUND)
	find_package (GLEW)
	if (GLEW_FOUND)
		set (TOADLET_HAS_OPENGL ON PARENT_SCOPE)
		add_subdirectory (plugins/glrenderdevice)
	else (GLEW_FOUND)
		message ("GLEW was not found. The peeper_glrenderdevice will not be built")
	endif (GLEW_FOUND)
elseif (TOADLET_PLATFORM_IOS)
	# Both iPhoneOS and iPhoneSimulator always have gles
	set (TOADLET_HAS_OPENGL ON PARENT_SCOPE)
	add_subdirectory (plugins/gles1renderdevice)
	add_subdirectory (plugins/gles2renderdevice)
elseif (TOADLET_PLATFORM_OSX)
	# OSX requires we find OpenGL and GLEW
	find_package (OPENGL)
	find_package (GLEW)
	if (OPENGL_FOUND AND GLEW_FOUND)
		set (TOADLET_HAS_OPENGL ON PARENT_SCOPE)
		add_subdirectory (plugins/glrenderdevice)
	else (OPENGL_FOUND AND GLEW_FOUND)
		message ("OpenGL and/or GLEW was not found. The peeper_glrenderdevice will not be built")
	endif (OPENGL_FOUND AND GLEW_FOUND)
elseif (TOADLET_PLATFORM_POSIX)
	# On posix we find opengl dynamically provided we have glew
	find_package (GLEW)
	if (GLEW_FOUND)
		set (TOADLET_HAS_OPENGL ON PARENT_SCOPE)
		add_subdirectory (plugins/glrenderdevice)
	else (GLEW_FOUND)
		message ("GLEW was not found. The peeper_glrenderdevice will not be built")
	endif (GLEW_FOUND)
endif (TOADLET_PLATFORM_ANDROID)

