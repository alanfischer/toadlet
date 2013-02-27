# Setup Versions
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/Version.h)


# Source files for toadlet_flick
set (FLICK_SRC
	FilteredLinearDevice.cpp
	Version.cpp
)


# Header files for flick
set (FLICK_HDR
	BaseInputDevice.h
	FilteredLinearDevice.h
	InputData.h
	InputDevice.h
	InputDeviceListener.h
	Types.h
	Version.h
)


# Give the headers and namespaces parent scope for inclusion into the master flick.h file
set (FLICK_HDR ${FLICK_HDR} PARENT_SCOPE)
set (FLICK_NAMESPACES ${FLICK_NAMESPACES} PARENT_SCOPE)


# If we are using swig create the wrapper file from the interface files
if (SWIG_FOUND AND FLICK_SWIG_INT)
	swig_wrap_files (flick_wrap.cpp flick ${FLICK_SWIG_INT} "${FLICK_SWIG_INT_DEPS}" "${FLICK_SWIG_INT_OUTPUTS}")
	set (FLICK_SRC ${FLICK_SRC} flick_wrap.cpp)
endif (SWIG_FOUND AND FLICK_SWIG_INT)


# Component installation
set (FLICK_INSTALL)


# Dynamic library
if (TOADLET_BUILD_DYNAMIC)
	add_library (toadlet_flick SHARED ${FLICK_SRC} ${FLICK_HDR})
	set_target_properties (toadlet_flick PROPERTIES ${DYNAMIC_LIB_PROPS})
	target_link_libraries (toadlet_flick toadlet_egg)
	install (TARGETS toadlet_flick DESTINATION ${LIB_INSTALL_DIR} COMPONENT flick)
	set (FLICK_INSTALL ${FLICK_INSTALL} toadlet_flick)
endif (TOADLET_BUILD_DYNAMIC)


# Static library
if (TOADLET_BUILD_STATIC)
	add_library (toadlet_flick_s STATIC ${FLICK_SRC} ${FLICK_HDR})
	set_target_properties (toadlet_flick_s PROPERTIES ${STATIC_LIB_PROPS})
	target_link_libraries (toadlet_flick_s toadlet_egg_s)
	install (TARGETS toadlet_flick_s DESTINATION ${LIB_INSTALL_DIR})
	set (FLICK_INSTALL ${FLICK_INSTALL} toadlet_flick_s)

	# HACK: install the library on iOS. This will disappear once cmake issue:0012506 is resolved. 
	if (TOADLET_PLATFORM_IOS)
	    install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/\$ENV{CONFIGURATION}\$ENV{EFFECTIVE_PLATFORM_NAME}/ DESTINATION ${LIB_INSTALL_DIR} COMPONENT flick FILES_MATCHING PATTERN "*.a")
	endif (TOADLET_PLATFORM_IOS)
endif (TOADLET_BUILD_STATIC)


# Platform specific modules
if (TOADLET_PLATFORM_WIN32)
	add_subdirectory (plugins/win32joydevice)
elseif (TOADLET_PLATFORM_ANDROID)
	add_subdirectory (plugins/jinputdevice)
	if (ANDROID_NDK_API_LEVEL GREATER 8)
		add_subdirectory (plugins/androidsensordevice)
	endif (ANDROID_NDK_API_LEVEL GREATER 8)
elseif (TOADLET_PLATFORM_IOS)
	add_subdirectory (plugins/ioslineardevice)
endif (TOADLET_PLATFORM_WIN32)


# Rule to install only flick
add_custom_target (flick_install
	COMMAND ${CMAKE_COMMAND} -DBUILD_TYPE=$<CONFIGURATION> -DCOMPONENT=required -P ${PROJECT_BINARY_DIR}/cmake_install.cmake
	COMMAND ${CMAKE_COMMAND} -DBUILD_TYPE=$<CONFIGURATION> -DCOMPONENT=egg -P ${PROJECT_BINARY_DIR}/cmake_install.cmake
	COMMAND ${CMAKE_COMMAND} -DBUILD_TYPE=$<CONFIGURATION> -DCOMPONENT=flick -P ${PROJECT_BINARY_DIR}/cmake_install.cmake
	DEPENDS ${FLICK_INSTALL}
)
