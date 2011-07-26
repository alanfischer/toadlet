# Setup Versions
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/Version.h)
install (FILES  ${CMAKE_CURRENT_BINARY_DIR}/Version.h DESTINATION include/toadlet/flick)


# Source files for toadlet_flick
set (FLICK_SRC
	FilteredMotionDevice.cpp
	Version.cpp
)


# Header files for flick
set (FLICK_HEADERS
	FilteredMotionDevice.h
	MotionDevice.h
	MotionDeviceListener.h
	Types.h
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
	install (TARGETS toadlet_flick DESTINATION lib)
endif (NOT TOADLET_PLATFORM_IOS)


# Static library
add_library (toadlet_flick_s STATIC ${FLICK_SRC} ${FLICK_HEADERS})
set_target_properties (toadlet_flick_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_flick_s toadlet_egg_s)
install (TARGETS toadlet_flick_s DESTINATION lib)


# Platform specific modules
if (TOADLET_PLATFORM_WINCE)
	add_subdirectory (plugins/htcmotiondevice)
	add_subdirectory (plugins/samsungmotiondevice)
elseif (TOADLET_PLATFORM_IOS)
	add_subdirectory (plugins/iosmotiondevice)
endif (TOADLET_PLATFORM_WINCE)

