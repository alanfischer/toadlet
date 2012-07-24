# Make all platform specific decisions for glrenderdevice
include (CommonGLRenderSources.cmake)


# Source files
set (PEEPER_GLRENDERDEVICE_SRC
	${PEEPER_GLRENDERDEVICE_SRC}
	GLBuffer.cpp
	GLFBORenderTarget.cpp
	GLFBOPixelBuffer.cpp
	GLRenderDevice.cpp
	GLRenderState.cpp
	GLSLShader.cpp
	GLSLShaderState.cpp
	GLSLVertexLayout.cpp
	GLQuery.cpp
	GLTexture.cpp
	GLTextureMipPixelBuffer.cpp
	GLVertexFormat.cpp
)


# Headers
set (PEEPER_GLRENDERDEVICE_HEADERS
	${PEEPER_GLRENDERDEVICE_HEADERS}
	GLBuffer.h
	GLFBORenderTarget.h
	GLFBOPixelBuffer.h
	GLIncludes.h
	GLQuery.h
	GLRenderDevice.h
	GLRenderState.h
	GLRenderTarget.h
	GLSLShader.h
	GLSLShaderState.h
	GLSLVertexLayout.h
	GLTexture.h
	GLTextureMipPixelBuffer.h
	GLPixelBuffer.h
	GLVertexFormat.h
)


# Dependencies
if (TOADLET_PLATFORM_OSX)
	find_library (APPKIT_LIBS AppKit)
	include_directories (${OPENGL_INCLUDE_DIR})
	set (PEEPER_GLRENDERDEVICE_LIBS objc ${APPKIT_LIBS} ${OPENGL_LIBRARY})
endif (TOADLET_PLATFORM_OSX)


# On platforms that use GLEW, include it and try to setup a static build
if (GLEW_FOUND)
	include_directories (${GLEW_INCLUDE_DIR})
	set (PEEPER_GLRENDERDEVICE_LIBS ${PEEPER_GLRENDERDEVICE_LIBS} ${GLEW_LIBRARY})
	LOCATE_STATIC_LIBRARIES (GLEW_LIBRARY_S "${GLEW_LIBRARY}")
	if (GLEW_LIBRARY_S)
		set (PEEPER_GLRENDERDEVICE_LIBS_S ${GLEW_LIBRARY_S})
	endif (GLEW_LIBRARY_S)
endif (GLEW_FOUND)


# Dynamic library
if (TOADLET_BUILD_DYNAMIC)
	add_library (toadlet_peeper_glrenderdevice SHARED ${PEEPER_GLRENDERDEVICE_SRC} ${PEEPER_GLRENDERDEVICE_HEADERS})
	set_target_properties (toadlet_peeper_glrenderdevice PROPERTIES ${DYNAMIC_LIB_PROPS})
	set_property (TARGET toadlet_peeper_glrenderdevice APPEND PROPERTY COMPILE_DEFINITIONS ${PEEPER_GLRENDERDEVICE_DEFS})
	target_link_libraries (toadlet_peeper_glrenderdevice toadlet_peeper ${PEEPER_GLRENDERDEVICE_LIBS})
	install (TARGETS toadlet_peeper_glrenderdevice DESTINATION ${LIB_INSTALL_DIR})
endif (TOADLET_BUILD_DYNAMIC)


# Don't even bother to build the static plugin if we do not have a static GLEW
if (GLEW_FOUND AND NOT GLEW_LIBRARY_S)
	# Static Library needs a static glew library
	# If we do not find one, do not build the static plugin
	message (STATUS "WARNING: GLEW is needed but a static glew was not found,")
	message (STATUS "WARNING: toadlet_peeper_glrenderdevice_s will not be built")
	return()
endif (GLEW_FOUND AND NOT GLEW_LIBRARY_S)


# Static library
if (TOADLET_BUILD_STATIC)
	add_library (toadlet_peeper_glrenderdevice_s STATIC ${PEEPER_GLRENDERDEVICE_SRC} ${PEEPER_GLRENDERDEVICE_HEADERS})
	set_target_properties (toadlet_peeper_glrenderdevice_s PROPERTIES ${STATIC_LIB_PROPS})
	set_property (TARGET toadlet_peeper_glrenderdevice_s APPEND PROPERTY COMPILE_DEFINITIONS ${PEEPER_GLRENDERDEVICE_DEFS_S})
	target_link_libraries (toadlet_peeper_glrenderdevice_s toadlet_peeper_s ${PEEPER_GLRENDERDEVICE_LIBS})
	MERGE_STATIC_LIBRARIES (toadlet_peeper_glrenderdevice_s "${PEEPER_GLRENDERDEVICE_LIBS_S}")
	install (TARGETS toadlet_peeper_glrenderdevice_s DESTINATION ${LIB_INSTALL_DIR})

	# HACK: install the library on iOS. This will disappear once cmake issue:0012506 is resolved. 
	if (TOADLET_PLATFORM_IOS)
	    install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/\$ENV{CONFIGURATION}\$ENV{EFFECTIVE_PLATFORM_NAME}/ DESTINATION ${LIB_INSTALL_DIR} FILES_MATCHING PATTERN "*.a")
	endif (TOADLET_PLATFORM_IOS)
endif (TOADLET_BUILD_STATIC)

