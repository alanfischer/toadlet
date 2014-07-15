# Setup Versions
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/Version.h)


# Source files for toadlet_pad
set (PAD_SRC
	BaseApplication.cpp
	ConsoleApplication.cpp
	Version.cpp
)

set (PAD_PLATFORM_ANDROID_JAPPLICATION_SRC
	platform/android/JApplication.cpp
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
set (PAD_HDR
	Applet.h
	Application.h
	BaseApplication.h
	ConsoleApplication.h
	PlatformApplication.h
	Version.h
)

set (PAD_PLATFORM_ANDROID_JAPPLICATION_HDR
	platform/android/JApplication.h
)

set (PAD_PLATFORM_ANDROID_ANDROIDAPPLICATION_HDR
	platform/android/AndroidApplication.h
)

set (PAD_PLATFORM_WIN32_HDR
	platform/win32/Win32Application.h
)

set (PAD_PLATFORM_OSX_HDR
	platform/osx/OSXApplication.h
)

set (PAD_PLATFORM_X11_HDR
	platform/x11/X11Application.h
)


# Platform specific files
if (TOADLET_PLATFORM_ANDROID)
	source_group ("Source Files\\\\platform\\\\android" FILES ${PAD_PLATFORM_ANDROID_JAPPLICATION_SRC})
	source_group ("Header Files\\\\platform\\\\android" FILES ${PAD_PLATFORM_ANDROID_JAPPLICATION_HDR})
	set (PAD_SRC ${PAD_SRC} ${PAD_PLATFORM_ANDROID_JAPPLICATION_SRC})
	set (PAD_HDR ${PAD_HDR} ${PAD_PLATFORM_ANDROID_JAPPLICATION_HDR})
	
	if (ANDROID_NDK_API_LEVEL GREATER 8)
		source_group ("Source Files\\\\platform\\\\android" FILES ${PAD_PLATFORM_ANDROID_ANDROIDAPPLICATION_SRC})
		source_group ("Header Files\\\\platform\\\\android" FILES ${PAD_PLATFORM_ANDROID_ANDROIDAPPLICATION_HDR})
		set (PAD_SRC ${PAD_SRC} ${PAD_PLATFORM_ANDROID_ANDROIDAPPLICATION_SRC})
		set (PAD_HDR ${PAD_HDR} ${PAD_PLATFORM_ANDROID_ANDROIDAPPLICATION_HDR})
	endif (ANDROID_NDK_API_LEVEL GREATER 8)
elseif (TOADLET_PLATFORM_WIN32)
	source_group ("Source Files\\\\platform\\\\win32" FILES ${PAD_PLATFORM_WIN32_SRC})
	source_group ("Header Files\\\\platform\\\\win32" FILES ${PAD_PLATFORM_WIN32_HDR})
	set (PAD_SRC ${PAD_SRC} ${PAD_PLATFORM_WIN32_SRC})
	set (PAD_HDR ${PAD_HDR} ${PAD_PLATFORM_WIN32_HDR})
elseif (TOADLET_PLATFORM_OSX)
	source_group ("Source Files\\\\platform\\\\osx" FILES ${PAD_PLATFORM_OSX_SRC})
	source_group ("Header Files\\\\platform\\\\osx" FILES ${PAD_PLATFORM_OSX_HDR})
	set (PAD_SRC ${PAD_SRC} ${PAD_PLATFORM_OSX_SRC})
	set (PAD_HDR ${PAD_HDR} ${PAD_PLATFORM_OSX_HDR})	
elseif (TOADLET_PLATFORM_POSIX)
	source_group ("Source Files\\\\platform\\\\posix" FILES ${PAD_PLATFORM_POSIX_SRC})
	source_group ("Header Files\\\\platform\\\\posix" FILES ${PAD_PLATFORM_POSIX_HDR})
	set (PAD_SRC ${PAD_SRC} ${PAD_PLATFORM_X11_SRC})
	set (PAD_HDR ${PAD_HDR} ${PAD_PLATFORM_X11_HDR})
endif (TOADLET_PLATFORM_ANDROID)


# Dependencies for toadlet_pad
if (TOADLET_PLATFORM_ANDROID)
	set (PAD_LIBS ${PAD_LIBS} toadlet_peeper_gles1renderdevice toadlet_peeper_gles2renderdevice)
	set (PAD_LIBS_S ${PAD_LIBS_S} toadlet_peeper_gles1renderdevice_s toadlet_peeper_gles2renderdevice_s)

	set (PAD_LIBS ${PAD_LIBS} toadlet_ribbit_jaudiodevice toadlet_flick_jinputdevice)
	set (PAD_LIBS_S ${PAD_LIBS_S} toadlet_ribbit_jaudiodevice_s toadlet_flick_jinputdevice_s)
	
	if (ANDROID_NDK_API_LEVEL GREATER 8)
		set (PAD_LIBS ${PAD_LIBS} toadlet_flick_androidsensordevice android)
		set (PAD_LIBS_S ${PAD_LIBS_S} toadlet_flick_androidsensordevice android)
	endif (ANDROID_NDK_API_LEVEL GREATER 8)
elseif (TOADLET_PLATFORM_WINCE)
	if (TOADLET_HAS_D3DM)
		set (PAD_LIBS ${PAD_LIBS} toadlet_peeper_d3dmrenderdevice)
		set (PAD_LIBS_S ${PAD_LIBS_S} toadlet_peeper_d3dmrenderdevice_s)
	endif (TOADLET_HAS_D3DM)
	set (PAD_LIBS ${PAD_LIBS} toadlet_peeper_gles1renderdevice toadlet_ribbit_mmaudiodevice)
	set (PAD_LIBS_S ${PAD_LIBS_S} toadlet_peeper_gles1renderdevice_s toadlet_ribbit_mmaudiodevice_s)
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
	set (PAD_LIBS objc ${UIKIT_LIBS} toadlet_peeper_gles1renderdevice toadlet_peeper_gles2renderdevice toadlet_ribbit_alaudiodevice)
	set (PAD_LIBS_S objc ${UIKIT_LIBS} toadlet_peeper_gles1renderdevice_s toadlet_peeper_gles2renderdevice_s toadlet_ribbit_alaudiodevice_s)
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
elseif (TOADLET_PLATFORM_EMSCRIPTEN)
elseif (TOADLET_PLATFORM_POSIX)
	# HACK: we are using a custom patched version of FindX11 until the xf86vmode library is included in the mainline
	find_package(X11)
	if (X11_FOUND)
		include_directories (${X11_INCLUDE_DIR} ${X11_INCLUDE_PATH})
		set (PAD_LIBS  ${X11_LIBRARIES})
		set (PAD_LIBS_S ${X11_LIBRARIES}})
		if (TOADLET_HAS_OPENAL)
			set (PAD_LIBS ${PAD_LIBS} toadlet_ribbit_alaudiodevice)
			set (PAD_LIBS_S ${PAD_LIBS_S} toadlet_ribbit_alaudiodevice_s)
		endif (TOADLET_HAS_OPENAL)
		if (TOADLET_HAS_OPENGL)
			set (PAD_LIBS ${PAD_LIBS} toadlet_peeper_glrenderdevice)
			set (PAD_LIBS_S ${PAD_LIBS_S} toadlet_peeper_glrenderdevice_s)
		endif (TOADLET_HAS_OPENGL)
	else (X11_FOUND)
		message ("libX11 was not found, so pad will not be built.")
		return ()
	endif (X11_FOUND)
endif (TOADLET_PLATFORM_ANDROID)


# Give the headers and namespaces parent scope for inclusion into the master pad.h file
set (PAD_HDR ${PAD_HDR} PARENT_SCOPE)
set (PAD_NAMESPACES ${PAD_NAMESPACES} PARENT_SCOPE)


# If we are using swig create the wrapper file from the interface files
if (SWIG_FOUND AND PAD_SWIG_INT)
	swig_wrap_files (pad_wrap.cpp pad ${PAD_SWIG_INT} "${PAD_SWIG_INT_DEPS}" "${PAD_SWIG_INT_OUTPUTS}")	
	set (PAD_SRC ${PAD_SRC} pad_wrap.cpp)
endif (SWIG_FOUND AND PAD_SWIG_INT)


# For individual component installation 
set (PAD_INSTALL)


# Dynamic Library
if (TOADLET_BUILD_DYNAMIC)
	add_library (toadlet_pad SHARED ${PAD_SRC} ${PAD_HDR})
	set_target_properties (toadlet_pad PROPERTIES ${DYNAMIC_LIB_PROP})
	target_compile_definitions (toadlet_pad PUBLIC ${DYNAMIC_LIB_DEFS})
	target_compile_options (toadlet_pad PUBLIC ${DYNAMIC_LIB_FLAG})
	target_link_libraries (toadlet_pad ${DYNAMIC_LIB_LINK} toadlet_flick toadlet_tadpole ${PAD_LIBS})	
	install (TARGETS toadlet_pad DESTINATION ${LIB_INSTALL_DIR} COMPONENT pad)
	set (PAD_INSTALL ${PAD_INSTALL} toadlet_pad)
endif (TOADLET_BUILD_DYNAMIC)


# Static Library
if (TOADLET_BUILD_STATIC)
	add_library (toadlet_pad_s STATIC ${PAD_SRC} ${PAD_HDR})
	set_target_properties (toadlet_pad_s PROPERTIES ${STATIC_LIB_PROP})
	target_compile_definitions (toadlet_pad_s PUBLIC ${STATIC_LIB_DEFS})
	target_compile_options (toadlet_pad_s PUBLIC ${STATIC_LIB_FLAG})
	target_link_libraries (toadlet_pad_s ${STATIC_LIB_LINK} toadlet_flick_s toadlet_tadpole_s ${PAD_LIBS_S})
	install (TARGETS toadlet_pad_s DESTINATION ${LIB_INSTALL_DIR} COMPONENT pad)
	set (PAD_INSTALL ${PAD_INSTALL} toadlet_pad_s)

	# HACK: install the library on iOS. This will disappear once cmake issue:0012506 is resolved. 
	if (TOADLET_PLATFORM_IOS)
	    install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/\$ENV{CONFIGURATION}\$ENV{EFFECTIVE_PLATFORM_NAME}/ DESTINATION ${LIB_INSTALL_DIR} COMPONENT pad FILES_MATCHING PATTERN "*.a")
	endif (TOADLET_PLATFORM_IOS)
endif (TOADLET_BUILD_STATIC)


# Rule to install only pad
add_custom_target (pad_install
	COMMAND ${CMAKE_COMMAND} -DBUILD_TYPE=$<CONFIGURATION> -DCOMPONENT=required -P ${PROJECT_BINARY_DIR}/cmake_install.cmake
	COMMAND ${CMAKE_COMMAND} -DBUILD_TYPE=$<CONFIGURATION> -DCOMPONENT=egg -P ${PROJECT_BINARY_DIR}/cmake_install.cmake
	COMMAND ${CMAKE_COMMAND} -DBUILD_TYPE=$<CONFIGURATION> -DCOMPONENT=flick -P ${PROJECT_BINARY_DIR}/cmake_install.cmake
	COMMAND ${CMAKE_COMMAND} -DBUILD_TYPE=$<CONFIGURATION> -DCOMPONENT=tadpole -P ${PROJECT_BINARY_DIR}/cmake_install.cmake
	DEPENDS ${PAD_INSTALL}
)


