# Source files for toadlet_peeper_glrenderer
set (PEEPER_GLRENDERER_SRC
	GLBuffer.cpp
	GLRenderer.cpp
	GLTexture.cpp
	GLTextureMipSurface.cpp
)
if (NOT TOADLET_PLATFORM_WINCE)
	set (PEEPER_GLRENDERER_SRC ${PEEPER_GLRENDERER_SRC} GLFBOSurfaceRenderTarget.cpp GLFBORenderbufferSurface.cpp)
endif (NOT TOADLET_PLATFORM_WINCE)

set (PEEPER_GLRENDERER_PLATFORM_EAGL_SRC
	platform/eagl/EAGLRenderContext.mm
)

set (PEEPER_GLRENDERER_PLATFORM_EGL_SRC
	platform/egl/EGLPBufferSurfaceRenderTarget.cpp
	platform/egl/EGLWindowRenderTarget.cpp
	platform/egl/EGLRenderTarget.cpp
	platform/egl/glesem/glesem.c
)

set (PEEPER_GLRENDERER_PLATFORM_GLX_SRC
	platform/glx/GLXPBufferSurfaceRenderTarget.cpp
	platform/glx/GLXRenderTarget.cpp
	platform/glx/GLXWindowRenderTarget.cpp
)

set (PEEPER_GLRENDERER_PLATFORM_NSGL_SRC
	platform/nsgl/NSGLRenderTarget.mm
)

set (PEEPER_GLRENDERER_PLATFORM_WGL_SRC
	platform/wgl/WGLPBufferSurfaceRenderTarget.cpp
	platform/wgl/WGLRenderTarget.cpp
	platform/wgl/WGLWindowRenderTarget.cpp
)


# Headers
set (PEEPER_GLRENDERER_HEADERS
	GLBuffer.h
	GLIncludes.h
	GLRenderer.h
	GLRenderTarget.h
	GLSLProgramPeer.h
	GLSLShaderPeer.h
	GLTexture.h
	GLTextureMipSurface.h
	GLSurface.h
)
if (NOT TOADLET_PLATFORM_WINCE)
	set (PEEPER_GLRENDERER_HEADERS ${PEEPER_GLRENDERER_HEADERS} GLFBOSurfaceRenderTarget.h GLFBORenderbufferSurface.h)
endif (NOT TOADLET_PLATFORM_WINCE)

set (PEEPER_GLRENDERER_PLATFORM_EAGL_HEADERS
	platform/eagl/EAGLRenderContext.h
)

set (PEEPER_GLRENDERER_PLATFORM_EGL_HEADERS
	platform/egl/EGLPBufferSurfaceRenderTarget.h
	platform/egl/EGLWindowRenderTarget.h
	platform/egl/EGLRenderTarget.h
	platform/egl/glesem/glesem.h
)

set (PEEPER_GLRENDERER_PLATFORM_GLX_HEADERS
	platform/glx/GLXPBufferSurfaceRenderTarget.h
	platform/glx/GLXRenderTarget.h
	platform/glx/GLXWindowRenderTarget.h
)

set (PEEPER_GLRENDERER_PLATFORM_NSGL_HEADERS
	platform/nsgl/NSGLRenderTarget.h
)

set (PEEPER_GLRENDERER_PLATFORM_WGL_HEADERS
	platform/wgl/WGLPBufferSurfaceRenderTarget.h
	platform/wgl/WGLRenderTarget.h
	platform/wgl/WGLWindowRenderTarget.h
)


# Platform specific files
if (TOADLET_PLATFORM_IPHONE)
	source_group ("Source Files\\\\platform\\\\eagl" FILES ${PEEPER_GLRENDERER_PLATFORM_EAGL_SRC})
	source_group ("Header Files\\\\platform\\\\eagl" FILES ${PEEPER_GLRENDERER_PLATFORM_EAGL_HEADERS})
	set (PEEPER_GLRENDERER_SRC ${PEEPER_GLRENDERER_SRC} ${PEEPER_GLRENDERER_PLATFORM_EAGL_SRC})
	set (PEEPER_GLRENDERER_HEADERS ${PEEPER_GLRENDERER_HEADERS} ${PEEPER_GLRENDERER_PLATFORM_EAGL_HEADERS})
elseif (TOADLET_PLATFORM_OSX)
	source_group ("Source Files\\\\platform\\\\nsgl" FILES ${PEEPER_GLRENDERER_PLATFORM_NSGL_SRC})
	source_group ("Header Files\\\\platform\\\\nsgl" FILES ${PEEPER_GLRENDERER_PLATFORM_NSGL_HEADERS})
	set (PEEPER_GLRENDERER_SRC ${PEEPER_GLRENDERER_SRC} ${PEEPER_GLRENDERER_PLATFORM_NSGL_SRC})
	set (PEEPER_GLRENDERER_HEADERS ${PEEPER_GLRENDERER_HEADERS} ${PEEPER_GLRENDERER_PLATFORM_NSGL_HEADERS})
elseif (TOADLET_PLATFORM_POSIX)
	source_group ("Source Files\\\\platform\\\\glx" FILES ${PEEPER_GLRENDERER_PLATFORM_GLX_SRC})
	source_group ("Header Files\\\\platform\\\\glx" FILES ${PEEPER_GLRENDERER_PLATFORM_GLX_HEADERS})
	set (PEEPER_GLRENDERER_SRC ${PEEPER_GLRENDERER_SRC} ${PEEPER_GLRENDERER_PLATFORM_GLX_SRC})
	set (PEEPER_GLRENDERER_HEADERS ${PEEPER_GLRENDERER_HEADERS} ${PEEPER_GLRENDERER_PLATFORM_GLX_HEADERS})
endif (TOADLET_PLATFORM_IPHONE)
if (TOADLET_PLATFORM_WINCE)
	source_group ("Source Files\\\\platform\\\\egl" FILES ${PEEPER_GLRENDERER_PLATFORM_EGL_SRC})
	source_group ("Header Files\\\\platform\\\\egl" FILES ${PEEPER_GLRENDERER_PLATFORM_EGL_HEADERS})
	set (PEEPER_GLRENDERER_SRC ${PEEPER_GLRENDERER_SRC} ${PEEPER_GLRENDERER_PLATFORM_EGL_SRC})
	set (PEEPER_GLRENDERER_HEADERS ${PEEPER_GLRENDERER_HEADERS} ${PEEPER_GLRENDERER_PLATFORM_EGL_HEADERS})
elseif (TOADLET_PLATFORM_WIN32)
	source_group ("Source Files\\\\platform\\\\wgl" FILES ${PEEPER_GLRENDERER_PLATFORM_WGL_SRC})
	source_group ("Header Files\\\\platform\\\\wgl" FILES ${PEEPER_GLRENDERER_PLATFORM_WGL_HEADERS})
	set (PEEPER_GLRENDERER_SRC ${PEEPER_GLRENDERER_SRC} ${PEEPER_GLRENDERER_PLATFORM_WGL_SRC})
	set (PEEPER_GLRENDERER_HEADERS ${PEEPER_GLRENDERER_HEADERS} ${PEEPER_GLRENDERER_PLATFORM_WGL_HEADERS})
endif (TOADLET_PLATFORM_WINCE)


# Dependencies - note that glew has already been found
include_directories (${GLEW_INCLUDE_DIR})

# Dynamic libs
if (TOADLET_PLATFORM_WINCE)
	set (PEEPER_GLRENDERER_DEFS "GLESEM_BUILD") 
elseif (TOADLET_PLATFORM_IPHONEOS OR TOADLET_PLATFORM_IPHONESIMULATOR)
	find_library (UIKIT_LIBS UIKit)
	set (PEEPER_GLRENDERER_LIBS objc ${UIKIT_LIBS})
elseif (TOADLET_PLATFORM_OSX)
	find_library (APPKIT_LIBS AppKit)
	set (PEEPER_GLRENDERER_LIBS objc ${APPKIT_LIBS} ${GLEW_LIBRARY})
else (TOADLET_PLATFORM_WINCE)
	set (PEEPER_GLRENDERER_LIBS ${GLEW_LIBRARY})
endif (TOADLET_PLATFORM_WINCE)

# Static libs are ugly since we add references to certain external static libraries in toadlet
if (TOADLET_PLATFORM_WINCE)
	set (PEEPER_GLRENDERER_DEFS_S "GLESEM_BUILD" "GLESEM_STATIC")
elseif (TOADLET_PLATFORM_IPHONE)
	find_library (UIKIT_LIBS UIKit)
	set (PEEPER_GLRENDERER_LIBS_S objc ${UIKIT_LIBS})
elseif (TOADLET_PLATFORM_OSX)
	find_library (APPKIT_LIBS AppKit)
	set (PEEPER_GLRENDERER_LIBS_S objc ${APPKIT_LIBS} ${GLEW_LIBRARY})
else (TOADLET_PLATFORM_WINCE)
	add_library (libglew_s STATIC IMPORTED)
	if (TOADLET_DEPENDENCY_EXTENSION_NAMES)
		string (REPLACE ".lib" "_s.lib" GLEW_LIBRARY ${GLEW_LIBRARY})
	endif (TOADLET_DEPENDENCY_EXTENSION_NAMES)
	if (TOADLET_PLATFORM_POSIX)
		string (REPLACE ".so" ".a" GLEW_LIBRARY ${GLEW_LIBRARY})
 		if (EXISTS ${GLEW_A})
			set (GLEW_LIBRARY ${GLEW_A})
		endif (EXISTS ${GLEW_A})
	endif (TOADLET_PLATFORM_POSIX)
	set_property (TARGET libglew_s PROPERTY IMPORTED_LOCATION ${GLEW_LIBRARY})
	set (PEEPER_GLRENDERER_DEFS_S TOADLET_LIBGLEW_NAME="${GLEW_LIBRARY}")
	set (PEEPER_GLRENDERER_LIBS_S libglew_s)
endif (TOADLET_PLATFORM_WINCE)


# Dynamic library
if (NOT TOADLET_PLATFORM_IPHONE)
	add_library (toadlet_peeper_glrenderer SHARED ${PEEPER_GLRENDERER_SRC} ${PEEPER_GLRENDERER_HEADERS})
	set_target_properties (toadlet_peeper_glrenderer PROPERTIES ${DYNAMIC_LIB_PROPS})
	set_property (TARGET toadlet_peeper_glrenderer APPEND PROPERTY COMPILE_DEFINITIONS ${PEEPER_GLRENDERER_DEFS})
	target_link_libraries (toadlet_peeper_glrenderer toadlet_peeper ${PEEPER_GLRENDERER_LIBS})
	install (TARGETS toadlet_peeper_glrenderer DESTINATION lib)
endif (NOT TOADLET_PLATFORM_IPHONE)


# Static library
add_library (toadlet_peeper_glrenderer_s STATIC ${PEEPER_GLRENDERER_SRC} ${PEEPER_GLRENDERER_HEADERS})
set_target_properties (toadlet_peeper_glrenderer_s PROPERTIES ${STATIC_LIB_PROPS})
set_property (TARGET toadlet_peeper_glrenderer_s APPEND PROPERTY COMPILE_DEFINITIONS ${PEEPER_GLRENDERER_DEFS_S})
target_link_libraries (toadlet_peeper_glrenderer_s toadlet_peeper_s ${PEEPER_GLRENDERER_LIBS_S})
install (TARGETS toadlet_peeper_glrenderer_s DESTINATION lib)

