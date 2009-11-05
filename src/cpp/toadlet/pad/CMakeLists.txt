# Setup Versions
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Version.h.in ${CMAKE_CURRENT_BINARY_DIR}/Version.h)


# Source files for toadlet_pad
set (PAD_SRC
	Version.cpp
)

set (PAD_PLATFORM_WIN32_SRC
	platform/win32/Win32Application.cpp
)

set (PAD_PLATFORM_OSX_SRC
	platform/osx/OSXApplication.mm
)

set (PAD_PLATFORM_X11_SRC
	platform/x11/X11Application.cpp
)


# Headers
set (PAD_HEADERS
	ApplicationListener.h
	BaseApplication.h
	Application.h
	Version.h
)

set (PAD_PLATFORM_WIN32_HEADERS
	platform/win32/Win32Application.h
)

set (PAD_PLATFORM_OSX_HEADERS
	platform/osx/OSXApplication.h
)

set (PAD_PLATFORM_X11_HEADERS
	platform/x11/X11Application.h
)


# Platform specific files
if (TOADLET_PLATFORM_WIN32)
	source_group ("Source Files\\\\platform\\\\win32" FILES ${PAD_PLATFORM_WIN32_SRC})
	source_group ("Header Files\\\\platform\\\\win32" FILES ${PAD_PLATFORM_WIN32_HEADERS})
	set (PAD_SRC ${PAD_SRC} ${PAD_PLATFORM_WIN32_SRC})
	set (PAD_HEADERS ${PAD_HEADERS} ${PAD_PLATFORM_WIN32_HEADERS})
elseif (TOADLET_PLATFORM_OSX)
	source_group ("Source Files\\\\platform\\\\osx" FILES ${PAD_PLATFORM_OSX_SRC})
	source_group ("Header Files\\\\platform\\\\osx" FILES ${PAD_PLATFORM_OSX_HEADERS})
	set (PAD_SRC ${PAD_SRC} ${PAD_PLATFORM_OSX_SRC})
	set (PAD_HEADERS ${PAD_HEADERS} ${PAD_PLATFORM_OSX_HEADERS})	
elseif (TOADLET_PLATFORM_POSIX)
	source_group ("Source Files\\\\platform\\\\posix" FILES ${PAD_PLATFORM_POSIX_SRC})
	source_group ("Header Files\\\\platform\\\\posix" FILES ${PAD_PLATFORM_POSIX_HEADERS})
	set (PAD_SRC ${PAD_SRC} ${PAD_PLATFORM_X11_SRC})
	set (PAD_HEADERS ${PAD_HEADERS} ${PAD_PLATFORM_X11_HEADERS})
endif (TOADLET_PLATFORM_WIN32)


# Dependcies for toadlet_pad
include_directories (${OPENGL_INCLUDES} ${GLEW_INCLUDE_DIR})
if (TOADLET_PLATFORM_WINCE)
	set (PAD_LIBS toadlet_peeper_d3d9renderer toadlet_peeper_glrenderer toadlet_ribbit_win32player)
	set (PAD_LIBS_S toadlet_peeper_d3d9renderer_s toadlet_peeper_glrenderer_s toadlet_ribbit_win32player_s)
elseif (TOADLET_PLATFORM_WIN32)
	set (PAD_LIBS toadlet_ribbit_win32player toadlet_ribbit_alplayer)
	set (PAD_LIBS_S toadlet_ribbit_win32player_s toadlet_ribbit_alplayer_s)
	if (TOADLET_HAS_DIRECT3D9)
		set (PAD_LIBS ${PAD_LIBS} toadlet_peeper_d3d9renderer)
		set (PAD_LIBS_S ${PAD_LIBS_S} toadlet_peeper_d3d9renderer_s)
	endif (TOADLET_HAS_DIRECT3D9)
	if (TOADLET_HAS_OPENGL)
		set (PAD_LIBS ${PAD_LIBS} toadlet_peeper_glrenderer)
		set (PAD_LIBS_S ${PAD_LIBS_S} toadlet_peeper_glrenderer_s)
	endif (TOADLET_HAS_OPENGL)
elseif (TOADLET_PLATFORM_IPHONE)
	find_library (UIKIT_LIBS UIKit)
	set (PAD_LIBS objc ${UIKIT_LIBS} toadlet_peeper_glrenderer toadlet_ribbit_alplayer)
	set (PAD_LIBS_S objc ${UIKIT_LIBS} toadlet_peeper_glrenderer_s toadlet_ribbit_alplayer_s)
elseif (TOADLET_PLATFORM_OSX)
	find_library (APPKIT_LIBS AppKit)
	set (PAD_LIBS objc ${APPKIT_LIBS} toadlet_peeper_glrenderer toadlet_ribbit_alplayer)
	set (PAD_LIBS_S objc ${APPKIT_LIBS} toadlet_peeper_glrenderer_s toadlet_ribbit_alplayer_s)
elseif (TOADLET_PLATFORM_POSIX)
	# HACK: we are using a custom patched version of FindX11 until the xf86vmode library is included in the mainline
	find_package(X11XVM REQUIRED)
	if (X11_xf86vmode_FOUND)
		include_directories (${X11_INCLUDE_DIR} ${X11_xf86vmode_INCLUDE_PATH})
		set (PAD_LIBS toadlet_peeper_glrenderer toadlet_ribbit_alplayer ${X11_LIBRARIES} ${X11_Xxf86vm_LIB})
		set (PAD_LIBS_S toadlet_peeper_glrenderer_s toadlet_ribbit_alplayer_s ${X11_LIBRARIES} ${X11_Xxf86vm_LIB})
	else (X11_xf86vmode_FOUND)
		message ("The libXxf86vm was not found, so pad will not be built.")
	endif (X11_xf86vmode_FOUND)
endif (TOADLET_PLATFORM_WINCE)


# Dynamic Library
if (NOT TOADLET_PLATFORM_IPHONE)
	add_library (toadlet_pad SHARED ${PAD_SRC} ${PAD_HEADERS})
	set_target_properties (toadlet_pad PROPERTIES ${DYNAMIC_LIB_PROPS})
	target_link_libraries (toadlet_pad toadlet_tadpole ${PAD_LIBS})
	install (TARGETS toadlet_pad DESTINATION lib)
endif (NOT TOADLET_PLATFORM_IPHONE)


# Static Library
add_library (toadlet_pad_s STATIC ${PAD_SRC} ${PAD_HEADERS})
set_target_properties (toadlet_pad_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_pad_s toadlet_tadpole_s ${PAD_LIBS_S})
install (TARGETS toadlet_pad_s DESTINATION lib)

