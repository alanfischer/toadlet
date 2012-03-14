# Setup Versions
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/Version.h)


# Source files for toadlet_flick
set (FLICK_SRC
	FilteredMotionDevice.cpp
	Version.cpp
)


# Header files for flick
set (FLICK_HEADERS
	FilteredMotionDevice.h
	InputData.h
	InputDevice.h
	InputDeviceListener.h
	Types.h
	VideoDevice.h
	VideoDeviceListener.h
	Version.h
)


# Give the headers and namespaces parent scope for inclusion into the master flick.h file
set (FLICK_HEADERS ${FLICK_HEADERS} PARENT_SCOPE)
set (FLICK_NAMESPACES ${FLICK_NAMESPACES} PARENT_SCOPE)


# Dynamic library
if (NOT TOADLET_PLATFORM_IOS)
	add_library (toadlet_flick SHARED ${FLICK_SRC} ${FLICK_HEADERS})
	set_target_properties (toadlet_flick PROPERTIES ${DYNAMIC_LIB_PROPS})
	target_link_libraries (toadlet_flick toadlet_egg)
	install (TARGETS toadlet_flick DESTINATION ${LIB_INSTALL_DIR})
endif (NOT TOADLET_PLATFORM_IOS)


# Static library
add_library (toadlet_flick_s STATIC ${FLICK_SRC} ${FLICK_HEADERS})
set_target_properties (toadlet_flick_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_flick_s toadlet_egg_s)
install (TARGETS toadlet_flick_s DESTINATION ${LIB_INSTALL_DIR})

# HACK: install the library on iOS. This will disappear once cmake issue:0012506 is resolved. 
if (TOADLET_PLATFORM_IOS)
    install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/\$ENV{CONFIGURATION}\$ENV{EFFECTIVE_PLATFORM_NAME}/ DESTINATION ${LIB_INSTALL_DIR} FILES_MATCHING PATTERN "*.a")
endif (TOADLET_PLATFORM_IOS)


# Platform specific modules
if (TOADLET_PLATFORM_WIN32)
	add_subdirectory (plugins/win32joydevice)
	find_package (MediaFoundation)
	if (MEDIAFOUNDATION_FOUND)
		add_subdirectory (plugins/mfvideodevice)
	endif (MEDIAFOUNDATION_FOUND)
elseif (TOADLET_PLATFORM_ANDROID)
	if (ANDROID_NDK_API_LEVEL LESS 9)
		add_subdirectory (plugins/jinputdevice)
	else (ANDROID_NDK_API_LEVEL LESS 9)
		add_subdirectory (plugins/androidsensordevice)
	endif (ANDROID_NDK_API_LEVEL LESS 9)
elseif (TOADLET_PLATFORM_IOS)
	add_subdirectory (plugins/iosmotiondevice)
endif (TOADLET_PLATFORM_WIN32)

