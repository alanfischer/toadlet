# Setup Versions
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/Version.h)
install (FILES  ${CMAKE_CURRENT_BINARY_DIR}/Version.h DESTINATION include/toadlet/pad)


# Source files for toadlet_pad
set (PAD_SRC
	BaseApplication.cpp
	Version.cpp
)

set (PAD_PLATFORM_ANDROID_JAPPLICATION_SRC
	platform/android/JApplication.cpp
	platform/android/us_toadlet_pad.cpp
)

set (PAD_PLATFORM_ANDROID_ANDROIDAPPLICATION_SRC
	platform/android/AndroidApplication.cpp
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
	Applet.h
	BaseApplication.h
	Application.h
	PlatformApplication.h
	Version.h
)

set (PAD_PLATFORM_ANDROID_JAPPLICATION_HEADERS
	platform/android/JApplication.h
)

set (PAD_PLATFORM_ANDROID_ANDROIDAPPLICATION_HEADERS
	platform/android/AndroidApplication.h
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
if (TOADLET_PLATFORM_ANDROID)
	if (ANDROID_NDK_API_LEVEL LESS 9)
		source_group ("Source Files\\\\platform\\\\android" FILES ${PAD_PLATFORM_ANDROID_JAPPLICATION_SRC})
		source_group ("Header Files\\\\platform\\\\android" FILES ${PAD_PLATFORM_ANDROID_JAPPLICATION_HEADERS})
		set (PAD_SRC ${PAD_SRC} ${PAD_PLATFORM_ANDROID_JAPPLICATION_SRC})
		set (PAD_HEADERS ${PAD_HEADERS} ${PAD_PLATFORM_ANDROID_JAPPLICATION_HEADERS})
	else (ANDROID_NDK_API_LEVEL LESS 9)
		source_group ("Source Files\\\\platform\\\\android" FILES ${PAD_PLATFORM_ANDROID_ANDROIDAPPLICATION_SRC})
		source_group ("Header Files\\\\platform\\\\android" FILES ${PAD_PLATFORM_ANDROID_ANDROIDAPPLICATION_HEADERS})
		set (PAD_SRC ${PAD_SRC} ${PAD_PLATFORM_ANDROID_ANDROIDAPPLICATION_SRC})
		set (PAD_HEADERS ${PAD_HEADERS} ${PAD_PLATFORM_ANDROID_ANDROIDAPPLICATION_HEADERS})
	endif (ANDROID_NDK_API_LEVEL LESS 9)
elseif (TOADLET_PLATFORM_WIN32)
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
endif (TOADLET_PLATFORM_ANDROID)


# Dependencies for toadlet_pad
if (TOADLET_PLATFORM_ANDROID)
	set (PAD_LIBS ${PAD_LIBS} toadlet_peeper_glrenderdevice toadlet_ribbit_jaudiodevice toadlet_flick_jinputdevice)
	set (PAD_LIBS_S ${PAD_LIBS_S} toadlet_peeper_glrenderdevice_s toadlet_ribbit_jaudiodevice_s toadlet_flick_jinputdevice_s)
elseif (TOADLET_PLATFORM_WINCE)
	if (TOADLET_HAS_D3DM)
		set (PAD_LIBS ${PAD_LIBS} toadlet_peeper_d3dmrenderdevice)
		set (PAD_LIBS_S ${PAD_LIBS_S} toadlet_peeper_d3dmrenderdevice_s)
	endif (TOADLET_HAS_D3DM)
	set (PAD_LIBS ${PAD_LIBS} toadlet_peeper_glrenderdevice toadlet_ribbit_mmaudiodevice)
	set (PAD_LIBS_S ${PAD_LIBS_S} toadlet_peeper_glrenderdevice_s toadlet_ribbit_mmaudiodevice_s)
elseif (TOADLET_PLATFORM_WIN32)
	set (PAD_LIBS toadlet_ribbit_mmaudiodevice toadlet_flick_win32joydevice)
	set (PAD_LIBS_S toadlet_ribbit_mmaudiodevice_s toadlet_flick_win32joydevice_s)
	if (TOADLET_HAS_OPENAL)
		set (PAD_LIBS ${PAD_LIBS} toadlet_ribbit_alaudiodevice)
		set (PAD_LIBS_S ${PAD_LIBS_S} toadlet_ribbit_alaudiodevice_s)
	endif (TOADLET_HAS_OPENAL)
	if (TOADLET_HAS_D3D9)
		set (PAD_LIBS ${PAD_LIBS} toadlet_peeper_d3d9renderdevice)
		set (PAD_LIBS_S ${PAD_LIBS_S} toadlet_peeper_d3d9renderdevice_s)
	endif (TOADLET_HAS_D3D9)
	if (TOADLET_HAS_D3D10)
		set (PAD_LIBS ${PAD_LIBS} toadlet_peeper_d3d10renderdevice)
		set (PAD_LIBS_S ${PAD_LIBS_S} toadlet_peeper_d3d10renderdevice_s)
	endif (TOADLET_HAS_D3D10)
	if (TOADLET_HAS_D3D11)
		set (PAD_LIBS ${PAD_LIBS} toadlet_peeper_d3d11renderdevice)
		set (PAD_LIBS_S ${PAD_LIBS_S} toadlet_peeper_d3d11renderdevice_s)
	endif (TOADLET_HAS_D3D11)
	if (TOADLET_HAS_OPENGL)
		set (PAD_LIBS ${PAD_LIBS} toadlet_peeper_glrenderdevice)
		set (PAD_LIBS_S ${PAD_LIBS_S} toadlet_peeper_glrenderdevice_s)
	endif (TOADLET_HAS_OPENGL)
elseif (TOADLET_PLATFORM_IOS)
	find_library (UIKIT_LIBS UIKit)
	set (PAD_LIBS objc ${UIKIT_LIBS} toadlet_peeper_glrenderdevice toadlet_ribbit_alaudiodevice)
	set (PAD_LIBS_S objc ${UIKIT_LIBS} toadlet_peeper_glrenderdevice_s toadlet_ribbit_alaudiodevice_s)
elseif (TOADLET_PLATFORM_OSX)
	find_library (APPKIT_LIBS AppKit)
	if (TOADLET_HAS_OPENAL)
		set (PAD_LIBS toadlet_ribbit_alaudiodevice)
		set (PAD_LIBS_S toadlet_ribbit_alaudiodevice_s)
	endif (TOADLET_HAS_OPENAL)
	if (TOADLET_HAS_OPENGL)
		set (PAD_LIBS ${PAD_LIBS} objc ${APPKIT_LIBS} toadlet_peeper_glrenderdevice)
		set (PAD_LIBS_S ${PAD_LIBS_S} objc ${APPKIT_LIBS} toadlet_peeper_glrenderdevice_s)
	endif (TOADLET_HAS_OPENGL)
elseif (TOADLET_PLATFORM_POSIX)
	# HACK: we are using a custom patched version of FindX11 until the xf86vmode library is included in the mainline
	find_package(X11XVM)
	if (X11_xf86vmode_FOUND)
		include_directories (${X11_INCLUDE_DIR} ${X11_xf86vmode_INCLUDE_PATH})
		set (PAD_LIBS  ${X11_LIBRARIES} ${X11_Xxf86vm_LIB})
		set (PAD_LIBS_S ${X11_LIBRARIES} ${X11_Xxf86vm_LIB})
		if (TOADLET_HAS_OPENAL)
			set (PAD_LIBS ${PAD_LIBS} toadlet_ribbit_alaudiodevice)
			set (PAD_LIBS_S ${PAD_LIBS_S} toadlet_ribbit_alaudiodevice_s)
		endif (TOADLET_HAS_OPENAL)
		if (TOADLET_HAS_OPENGL)
			set (PAD_LIBS ${PAD_LIBS} toadlet_peeper_glrenderdevice)
			set (PAD_LIBS_S ${PAD_LIBS_S} toadlet_peeper_glrenderdevice_s)
		endif (TOADLET_HAS_OPENGL)
	else (X11_xf86vmode_FOUND)
		message ("libXxf86vm was not found, so pad will not be built.")
		return ()
	endif (X11_xf86vmode_FOUND)
endif (TOADLET_PLATFORM_ANDROID)


# Give the headers and namespaces parent scope for inclusion into the master pad.h file
set (PAD_HEADERS ${PAD_HEADERS} PARENT_SCOPE)
set (PAD_NAMESPACES ${PAD_NAMESPACES} PARENT_SCOPE)


# Dynamic Library
if (NOT TOADLET_PLATFORM_IOS)
	add_library (toadlet_pad SHARED ${PAD_SRC} ${PAD_HEADERS})
	set_target_properties (toadlet_pad PROPERTIES ${DYNAMIC_LIB_PROPS})
	target_link_libraries (toadlet_pad toadlet_flick toadlet_tadpole ${PAD_LIBS})
	install (TARGETS toadlet_pad DESTINATION ${LIB_INSTALL_DIR})
endif (NOT TOADLET_PLATFORM_IOS)


# Static Library
add_library (toadlet_pad_s STATIC ${PAD_SRC} ${PAD_HEADERS})
set_target_properties (toadlet_pad_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_pad_s toadlet_flick_s toadlet_tadpole_s ${PAD_LIBS_S})
install (TARGETS toadlet_pad_s DESTINATION ${LIB_INSTALL_DIR})

