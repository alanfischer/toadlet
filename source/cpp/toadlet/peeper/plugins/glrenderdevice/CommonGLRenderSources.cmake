# Source files for the different glrenderdevices
set (PEEPER_GLRENDERDEVICE_PLATFORM_EAGL_SRC
	../glrenderdevice/platform/eagl/EAGLRenderTarget.mm
)

set (PEEPER_GLRENDERDEVICE_PLATFORM_EGL_SRC
	../glrenderdevice/platform/egl/EGLPBufferRenderTarget.cpp
	../glrenderdevice/platform/egl/EGLWindowRenderTarget.cpp
	../glrenderdevice/platform/egl/EGLRenderTarget.cpp
)

set (PEEPER_GLRENDERDEVICE_PLATFORM_GLESEM_SRC
	../glrenderdevice/platform/egl/glesem/glesem.c
)

set (PEEPER_GLRENDERDEVICE_PLATFORM_GLX_SRC
	../glrenderdevice/platform/glx/GLXPBufferRenderTarget.cpp
	../glrenderdevice/platform/glx/GLXRenderTarget.cpp
	../glrenderdevice/platform/glx/GLXWindowRenderTarget.cpp
)

set (PEEPER_GLRENDERDEVICE_PLATFORM_JGL_SRC
	../glrenderdevice/platform/jgl/JGLWindowRenderTarget.cpp
	../glrenderdevice/platform/jgl/JGLWindowRenderTarget.cpp
)

set (PEEPER_GLRENDERDEVICE_PLATFORM_NSGL_SRC
	../glrenderdevice/platform/nsgl/NSGLRenderTarget.mm
)

set (PEEPER_GLRENDERDEVICE_PLATFORM_WGL_SRC
	../glrenderdevice/platform/wgl/WGLPBufferRenderTarget.cpp
	../glrenderdevice/platform/wgl/WGLRenderTarget.cpp
	../glrenderdevice/platform/wgl/WGLWindowRenderTarget.cpp
)


# Headers
set (PEEPER_GLRENDERDEVICE_PLATFORM_EAGL_HEADERS
	../glrenderdevice/platform/eagl/EAGLRenderTarget.h
)

set (PEEPER_GLRENDERDEVICE_PLATFORM_EGL_HEADERS
	../glrenderdevice/platform/egl/EGLPBufferRenderTarget.h
	../glrenderdevice/platform/egl/EGLWindowRenderTarget.h
	../glrenderdevice/platform/egl/EGLRenderTarget.h
)

set (PEEPER_GLRENDERDEVICE_PLATFORM_GLESEM_HEADERS
	../glrenderdevice/platform/egl/glesem/glesem.h
)

set (PEEPER_GLRENDERDEVICE_PLATFORM_GLX_HEADERS
	../glrenderdevice/platform/glx/GLXPBufferRenderTarget.h
	../glrenderdevice/platform/glx/GLXRenderTarget.h
	../glrenderdevice/platform/glx/GLXWindowRenderTarget.h
)

set (PEEPER_GLRENDERDEVICE_PLATFORM_JGL_HEADERS
	../glrenderdevice/platform/jgl/JGLWindowRenderTarget.h
)

set (PEEPER_GLRENDERDEVICE_PLATFORM_NSGL_HEADERS
	../glrenderdevice/platform/nsgl/NSGLRenderTarget.h
)

set (PEEPER_GLRENDERDEVICE_PLATFORM_WGL_HEADERS
	../glrenderdevice/platform/wgl/WGLPBufferRenderTarget.h
	../glrenderdevice/platform/wgl/WGLRenderTarget.h
	../glrenderdevice/platform/wgl/WGLWindowRenderTarget.h
)


# Platform specific files
if (TOADLET_PLATFORM_ANDROID)
	source_group ("Source Files\\\\platform\\\\jgl" FILES ${PEEPER_GLRENDERDEVICE_PLATFORM_JGL_SRC})
	source_group ("Header Files\\\\platform\\\\jgl" FILES ${PEEPER_GLRENDERDEVICE_PLATFORM_JGL_HEADERS})
	set (PEEPER_GLRENDERDEVICE_SRC ${PEEPER_GLRENDERDEVICE_PLATFORM_JGL_SRC})
	set (PEEPER_GLRENDERDEVICE_HEADERS ${PEEPER_GLRENDERDEVICE_PLATFORM_JGL_HEADERS})
	if(EGL)
		source_group ("Source Files\\\\platform\\\\egl" FILES ${PEEPER_GLRENDERDEVICE_PLATFORM_EGL_SRC})
		source_group ("Header Files\\\\platform\\\\egl" FILES ${PEEPER_GLRENDERDEVICE_PLATFORM_EGL_HEADERS})
		set (PEEPER_GLRENDERDEVICE_SRC ${PEEPER_GLRENDERDEVICE_PLATFORM_EGL_SRC})
		set (PEEPER_GLRENDERDEVICE_HEADERS ${PEEPER_GLRENDERDEVICE_PLATFORM_EGL_HEADERS})
	endif (EGL)
elseif (TOADLET_PLATFORM_IOS)
	source_group ("Source Files\\\\platform\\\\eagl" FILES ${PEEPER_GLRENDERDEVICE_PLATFORM_EAGL_SRC})
	source_group ("Header Files\\\\platform\\\\eagl" FILES ${PEEPER_GLRENDERDEVICE_PLATFORM_EAGL_HEADERS})
	set (PEEPER_GLRENDERDEVICE_SRC ${PEEPER_GLRENDERDEVICE_PLATFORM_EAGL_SRC})
	set (PEEPER_GLRENDERDEVICE_HEADERS ${PEEPER_GLRENDERDEVICE_PLATFORM_EAGL_HEADERS})
elseif (TOADLET_PLATFORM_OSX)
	source_group ("Source Files\\\\platform\\\\nsgl" FILES ${PEEPER_GLRENDERDEVICE_PLATFORM_NSGL_SRC})
	source_group ("Header Files\\\\platform\\\\nsgl" FILES ${PEEPER_GLRENDERDEVICE_PLATFORM_NSGL_HEADERS})
	set (PEEPER_GLRENDERDEVICE_SRC ${PEEPER_GLRENDERDEVICE_PLATFORM_NSGL_SRC})
	set (PEEPER_GLRENDERDEVICE_HEADERS ${PEEPER_GLRENDERDEVICE_PLATFORM_NSGL_HEADERS})
elseif (TOADLET_PLATFORM_EMSCRIPTEN)
	source_group ("Source Files\\\\platform\\\\egl" FILES ${PEEPER_GLRENDERDEVICE_PLATFORM_EGL_SRC})
	source_group ("Header Files\\\\platform\\\\egl" FILES ${PEEPER_GLRENDERDEVICE_PLATFORM_EGL_HEADERS})
	set (PEEPER_GLRENDERDEVICE_SRC ${PEEPER_GLRENDERDEVICE_PLATFORM_EGL_SRC})
	set (PEEPER_GLRENDERDEVICE_HEADERS ${PEEPER_GLRENDERDEVICE_PLATFORM_EGL_HEADERS})
elseif (TOADLET_PLATFORM_POSIX)
	source_group ("Source Files\\\\platform\\\\glx" FILES ${PEEPER_GLRENDERDEVICE_PLATFORM_GLX_SRC})
	source_group ("Header Files\\\\platform\\\\glx" FILES ${PEEPER_GLRENDERDEVICE_PLATFORM_GLX_HEADERS})
	set (PEEPER_GLRENDERDEVICE_SRC ${PEEPER_GLRENDERDEVICE_PLATFORM_GLX_SRC})
	set (PEEPER_GLRENDERDEVICE_HEADERS ${PEEPER_GLRENDERDEVICE_PLATFORM_GLX_HEADERS})
elseif (TOADLET_PLATFORM_WINCE)
	source_group ("Source Files\\\\platform\\\\egl" FILES ${PEEPER_GLRENDERDEVICE_PLATFORM_EGL_SRC} ${PEEPER_GLRENDERDEVICE_PLATFORM_GLESEM_SRC})
	source_group ("Header Files\\\\platform\\\\egl" FILES ${PEEPER_GLRENDERDEVICE_PLATFORM_EGL_HEADERS} ${PEEPER_GLRENDERDEVICE_PLATFORM_GLESEM_HEADERS})
	set (PEEPER_GLRENDERDEVICE_SRC ${PEEPER_GLRENDERDEVICE_PLATFORM_EGL_SRC} ${PEEPER_GLRENDERDEVICE_PLATFORM_GLESEM_SRC})
	set (PEEPER_GLRENDERDEVICE_HEADERS ${PEEPER_GLRENDERDEVICE_PLATFORM_EGL_HEADERS} ${PEEPER_GLRENDERDEVICE_PLATFORM_GLESEM_HEADERS})
elseif (TOADLET_PLATFORM_WIN32)
	source_group ("Source Files\\\\platform\\\\wgl" FILES ${PEEPER_GLRENDERDEVICE_PLATFORM_WGL_SRC})
	source_group ("Header Files\\\\platform\\\\wgl" FILES ${PEEPER_GLRENDERDEVICE_PLATFORM_WGL_HEADERS})
	set (PEEPER_GLRENDERDEVICE_SRC ${PEEPER_GLRENDERDEVICE_PLATFORM_WGL_SRC})
	set (PEEPER_GLRENDERDEVICE_HEADERS ${PEEPER_GLRENDERDEVICE_PLATFORM_WGL_HEADERS})
endif (TOADLET_PLATFORM_ANDROID)
