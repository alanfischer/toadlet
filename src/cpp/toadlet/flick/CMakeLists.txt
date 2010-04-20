# Setup Versions
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Version.h.in ${CMAKE_CURRENT_BINARY_DIR}/Version.h)


# Source files for toadlet_flick
set (FLICK_SRC
	FilteredMotionDetector.cpp
	Version.cpp
)


# Header files for flick
set (FLICK_HEADERS
	FilteredMotionDetector.h
	MotionDetector.h
	MotionDetectorListener.h
	Types.h
	Version.h
)


# Dynamic library
if (NOT TOADLET_PLATFORM_IPHONE)
	add_library (toadlet_flick SHARED ${FLICK_SRC} ${FLICK_HEADERS})
	set_target_properties (toadlet_flick PROPERTIES ${DYNAMIC_LIB_PROPS})
	target_link_libraries (toadlet_flick toadlet_egg)
	install (TARGETS toadlet_flick DESTINATION lib)
endif (NOT TOADLET_PLATFORM_IPHONE)


# Static library
add_library (toadlet_flick_s STATIC ${FLICK_SRC} ${FLICK_HEADERS})
set_target_properties (toadlet_flick_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_flick_s toadlet_egg_s)
install (TARGETS toadlet_flick_s DESTINATION lib)


# Platform specific modules
if (TOADLET_PLATFORM_WINCE)
	add_subdirectory (plugins/htcmotiondetector)
	add_subdirectory (plugins/samsungmotiondetector)
elseif (TOADLET_PLATFORM_IPHONE)
	add_subdirectory (plugins/iphonemotiondetector)
endif (TOADLET_PLATFORM_WINCE)

