# Source files for toadlet_peeper_glrenderdevice
set (PEEPER_GLRENDERDEVICE_SRC
	GLBuffer.cpp
	GLRenderDevice.cpp
	GLRenderState.cpp
	GLSLShader.cpp
	GLSLShaderState.cpp
	GLSLVertexLayout.cpp
	GLTexture.cpp
	GLTextureMipPixelBuffer.cpp
	GLVertexFormat.cpp
)
if (NOT TOADLET_PLATFORM_WINCE)
	set (PEEPER_GLRENDERDEVICE_SRC ${PEEPER_GLRENDERDEVICE_SRC} GLFBORenderTarget.cpp GLFBOPixelBuffer.cpp)
endif (NOT TOADLET_PLATFORM_WINCE)
if (NOT TOADLET_PLATFORM_WINCE AND NOT TOADLET_PLATFORM_IOS)
	set (PEEPER_GLRENDERDEVICE_SRC ${PEEPER_GLRENDERDEVICE_SRC} GLQuery.cpp)
endif (NOT TOADLET_PLATFORM_WINCE AND NOT TOADLET_PLATFORM_IOS)

set (PEEPER_GLRENDERDEVICE_PLATFORM_EAGL_SRC
	platform/eagl/EAGLRenderTarget.mm
)

set (PEEPER_GLRENDERDEVICE_PLATFORM_EGL_SRC
	platform/egl/EGLPBufferRenderTarget.cpp
	platform/egl/EGLWindowRenderTarget.cpp
	platform/egl/EGLRenderTarget.cpp
)

set (PEEPER_GLRENDERDEVICE_PLATFORM_GLESEM_SRC
	platform/egl/glesem/glesem.c
)

set (PEEPER_GLRENDERDEVICE_PLATFORM_GLX_SRC
	platform/glx/GLXPBufferRenderTarget.cpp
	platform/glx/GLXRenderTarget.cpp
	platform/glx/GLXWindowRenderTarget.cpp
)

set (PEEPER_GLRENDERDEVICE_PLATFORM_JGL_SRC
	platform/jgl/JGLWindowRenderTarget.cpp
)

set (PEEPER_GLRENDERDEVICE_PLATFORM_NSGL_SRC
	platform/nsgl/NSGLRenderTarget.mm
)

set (PEEPER_GLRENDERDEVICE_PLATFORM_WGL_SRC
	platform/wgl/WGLPBufferRenderTarget.cpp
	platform/wgl/WGLRenderTarget.cpp
	platform/wgl/WGLWindowRenderTarget.cpp
)


# Headers
set (PEEPER_GLRENDERDEVICE_HEADERS
	GLBuffer.h
	GLIncludes.h
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

if (NOT TOADLET_PLATFORM_WINCE)
	set (PEEPER_GLRENDERDEVICE_HEADERS ${PEEPER_GLRENDERDEVICE_HEADERS} GLFBORenderTarget.h GLFBOPixelBuffer.h)
endif (NOT TOADLET_PLATFORM_WINCE)
if (NOT TOADLET_PLATFORM_WINCE AND NOT TOADLET_PLATFORM_IOS)
	set (PEEPER_GLRENDERDEVICE_HEADERS ${PEEPER_GLRENDERDEVICE_HEADERS} GLQuery.h)
endif (NOT TOADLET_PLATFORM_WINCE AND NOT TOADLET_PLATFORM_IOS)

set (PEEPER_GLRENDERDEVICE_PLATFORM_EAGL_HEADERS
	platform/eagl/EAGLRenderTarget.h
)

set (PEEPER_GLRENDERDEVICE_PLATFORM_EGL_HEADERS
	platform/egl/EGLPBufferRenderTarget.h
	platform/egl/EGLWindowRenderTarget.h
	platform/egl/EGLRenderTarget.h
)

set (PEEPER_GLRENDERDEVICE_PLATFORM_GLESEM_HEADERS
	platform/egl/glesem/glesem.h
)

set (PEEPER_GLRENDERDEVICE_PLATFORM_GLX_HEADERS
	platform/glx/GLXPBufferRenderTarget.h
	platform/glx/GLXRenderTarget.h
	platform/glx/GLXWindowRenderTarget.h
)

set (PEEPER_GLRENDERDEVICE_PLATFORM_JGL_HEADERS
	platform/jgl/JGLWindowRenderTarget.h
)

set (PEEPER_GLRENDERDEVICE_PLATFORM_NSGL_HEADERS
	platform/nsgl/NSGLRenderTarget.h
)

set (PEEPER_GLRENDERDEVICE_PLATFORM_WGL_HEADERS
	platform/wgl/WGLPBufferRenderTarget.h
	platform/wgl/WGLRenderTarget.h
	platform/wgl/WGLWindowRenderTarget.h
)


# Platform specific files
if (TOADLET_PLATFORM_ANDROID)
	# Android NDK api levels less than 9 require the java Android SDK, and thus use jgl
	if (ANDROID_NDK_API_LEVEL LESS 9)
		source_group ("Source Files\\\\platform\\\\jgl" FILES ${PEEPER_GLRENDERDEVICE_PLATFORM_JGL_SRC})
		source_group ("Header Files\\\\platform\\\\jgl" FILES ${PEEPER_GLRENDERDEVICE_PLATFORM_JGL_HEADERS})
		set (PEEPER_GLRENDERDEVICE_SRC ${PEEPER_GLRENDERDEVICE_SRC} ${PEEPER_GLRENDERDEVICE_PLATFORM_JGL_SRC})
		set (PEEPER_GLRENDERDEVICE_HEADERS ${PEEPER_GLRENDERDEVICE_HEADERS} ${PEEPER_GLRENDERDEVICE_PLATFORM_JGL_HEADERS})
	else (ANDROID_NDK_API_LEVEL LESS 9)
		source_group ("Source Files\\\\platform\\\\egl" FILES ${PEEPER_GLRENDERDEVICE_PLATFORM_EGL_SRC})
		source_group ("Header Files\\\\platform\\\\egl" FILES ${PEEPER_GLRENDERDEVICE_PLATFORM_EGL_HEADERS})
		set (PEEPER_GLRENDERDEVICE_SRC ${PEEPER_GLRENDERDEVICE_SRC} ${PEEPER_GLRENDERDEVICE_PLATFORM_EGL_SRC})
		set (PEEPER_GLRENDERDEVICE_HEADERS ${PEEPER_GLRENDERDEVICE_HEADERS} ${PEEPER_GLRENDERDEVICE_PLATFORM_EGL_HEADERS})
	endif (ANDROID_NDK_API_LEVEL LESS 9)
elseif (TOADLET_PLATFORM_IOS)
	source_group ("Source Files\\\\platform\\\\eagl" FILES ${PEEPER_GLRENDERDEVICE_PLATFORM_EAGL_SRC})
	source_group ("Header Files\\\\platform\\\\eagl" FILES ${PEEPER_GLRENDERDEVICE_PLATFORM_EAGL_HEADERS})
	set (PEEPER_GLRENDERDEVICE_SRC ${PEEPER_GLRENDERDEVICE_SRC} ${PEEPER_GLRENDERDEVICE_PLATFORM_EAGL_SRC})
	set (PEEPER_GLRENDERDEVICE_HEADERS ${PEEPER_GLRENDERDEVICE_HEADERS} ${PEEPER_GLRENDERDEVICE_PLATFORM_EAGL_HEADERS})
elseif (TOADLET_PLATFORM_OSX)
	source_group ("Source Files\\\\platform\\\\nsgl" FILES ${PEEPER_GLRENDERDEVICE_PLATFORM_NSGL_SRC})
	source_group ("Header Files\\\\platform\\\\nsgl" FILES ${PEEPER_GLRENDERDEVICE_PLATFORM_NSGL_HEADERS})
	set (PEEPER_GLRENDERDEVICE_SRC ${PEEPER_GLRENDERDEVICE_SRC} ${PEEPER_GLRENDERDEVICE_PLATFORM_NSGL_SRC})
	set (PEEPER_GLRENDERDEVICE_HEADERS ${PEEPER_GLRENDERDEVICE_HEADERS} ${PEEPER_GLRENDERDEVICE_PLATFORM_NSGL_HEADERS})
elseif (TOADLET_PLATFORM_POSIX)
	source_group ("Source Files\\\\platform\\\\glx" FILES ${PEEPER_GLRENDERDEVICE_PLATFORM_GLX_SRC})
	source_group ("Header Files\\\\platform\\\\glx" FILES ${PEEPER_GLRENDERDEVICE_PLATFORM_GLX_HEADERS})
	set (PEEPER_GLRENDERDEVICE_SRC ${PEEPER_GLRENDERDEVICE_SRC} ${PEEPER_GLRENDERDEVICE_PLATFORM_GLX_SRC})
	set (PEEPER_GLRENDERDEVICE_HEADERS ${PEEPER_GLRENDERDEVICE_HEADERS} ${PEEPER_GLRENDERDEVICE_PLATFORM_GLX_HEADERS})
elseif (TOADLET_PLATFORM_WINCE)
	source_group ("Source Files\\\\platform\\\\egl" FILES ${PEEPER_GLRENDERDEVICE_PLATFORM_EGL_SRC} ${PEEPER_GLRENDERDEVICE_PLATFORM_GLESEM_SRC})
	source_group ("Header Files\\\\platform\\\\egl" FILES ${PEEPER_GLRENDERDEVICE_PLATFORM_EGL_HEADERS} ${PEEPER_GLRENDERDEVICE_PLATFORM_GLESEM_HEADERS})
	set (PEEPER_GLRENDERDEVICE_SRC ${PEEPER_GLRENDERDEVICE_SRC} ${PEEPER_GLRENDERDEVICE_PLATFORM_EGL_SRC} ${PEEPER_GLRENDERDEVICE_PLATFORM_GLESEM_SRC})
	set (PEEPER_GLRENDERDEVICE_HEADERS ${PEEPER_GLRENDERDEVICE_HEADERS} ${PEEPER_GLRENDERDEVICE_PLATFORM_EGL_HEADERS} ${PEEPER_GLRENDERDEVICE_PLATFORM_GLESEM_HEADERS})
elseif (TOADLET_PLATFORM_WIN32)
	source_group ("Source Files\\\\platform\\\\wgl" FILES ${PEEPER_GLRENDERDEVICE_PLATFORM_WGL_SRC})
	source_group ("Header Files\\\\platform\\\\wgl" FILES ${PEEPER_GLRENDERDEVICE_PLATFORM_WGL_HEADERS})
	set (PEEPER_GLRENDERDEVICE_SRC ${PEEPER_GLRENDERDEVICE_SRC} ${PEEPER_GLRENDERDEVICE_PLATFORM_WGL_SRC})
	set (PEEPER_GLRENDERDEVICE_HEADERS ${PEEPER_GLRENDERDEVICE_HEADERS} ${PEEPER_GLRENDERDEVICE_PLATFORM_WGL_HEADERS})
endif (TOADLET_PLATFORM_ANDROID)


# Dependencies
if (TOADLET_PLATFORM_ANDROID)
	if (ANDROID_NDK_API_LEVEL LESS 9)
		set (PEEPER_GLRENDERDEVICE_LIBS ${GLESV1_LIBS})
	else (ANDROID_NDK_API_LEVEL LESS 9)
		set (PEEPER_GLRENDERDEVICE_LIBS ${GLESV1_LIBS} android)
	endif (ANDROID_NDK_API_LEVEL LESS 9)
elseif (TOADLET_PLATFORM_IOS)
	find_library (UIKIT_LIBS UIKit)
	set (PEEPER_GLRENDERDEVICE_LIBS objc ${UIKIT_LIBS})
elseif (TOADLET_PLATFORM_OSX)
	find_library (APPKIT_LIBS AppKit)
	include_directories (${OPENGL_INCLUDE_DIR})
	set (PEEPER_GLRENDERDEVICE_LIBS objc ${APPKIT_LIBS} ${OPENGL_LIBRARY})
elseif (TOADLET_PLATFORM_WINCE)
	set (PEEPER_GLRENDERDEVICE_DEFS "GLESEM_BUILD") 
	set (PEEPER_GLRENDERDEVICE_DEFS_S "GLESEM_BUILD" "GLESEM_STATIC")
endif (TOADLET_PLATFORM_ANDROID)


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
if (NOT TOADLET_PLATFORM_IOS)
	add_library (toadlet_peeper_glrenderdevice SHARED ${PEEPER_GLRENDERDEVICE_SRC} ${PEEPER_GLRENDERDEVICE_HEADERS})
	set_target_properties (toadlet_peeper_glrenderdevice PROPERTIES ${DYNAMIC_LIB_PROPS})
	set_property (TARGET toadlet_peeper_glrenderdevice APPEND PROPERTY COMPILE_DEFINITIONS ${PEEPER_GLRENDERDEVICE_DEFS})
	target_link_libraries (toadlet_peeper_glrenderdevice toadlet_peeper ${PEEPER_GLRENDERDEVICE_LIBS})
	install (TARGETS toadlet_peeper_glrenderdevice DESTINATION ${LIB_INSTALL_DIR})
endif (NOT TOADLET_PLATFORM_IOS)


# Don't even bother to build the static plugin if we do not have a static GLEW
if (GLEW_FOUND AND NOT GLEW_LIBRARY_S)
	# Static Library needs a static glew library
	# If we do not find one, do not build the static plugin
	message (STATUS "WARNING: GLEW is needed but a static glew was not found,")
	message (STATUS "WARNING: toadlet_peeper_glrenderdevice_s will not be built")
	return()
endif (GLEW_FOUND AND NOT GLEW_LIBRARY_S)


# Static library
add_library (toadlet_peeper_glrenderdevice_s STATIC ${PEEPER_GLRENDERDEVICE_SRC} ${PEEPER_GLRENDERDEVICE_HEADERS})
set_target_properties (toadlet_peeper_glrenderdevice_s PROPERTIES ${STATIC_LIB_PROPS})
set_property (TARGET toadlet_peeper_glrenderdevice_s APPEND PROPERTY COMPILE_DEFINITIONS ${PEEPER_GLRENDERDEVICE_DEFS_S})
target_link_libraries (toadlet_peeper_glrenderdevice_s toadlet_peeper_s ${PEEPER_GLRENDERDEVICE_LIBS})
MERGE_STATIC_LIBRARIES (toadlet_peeper_glrenderdevice_s "${PEEPER_GLRENDERDEVICE_LIBS_S}")
install (TARGETS toadlet_peeper_glrenderdevice_s DESTINATION ${LIB_INSTALL_DIR})
