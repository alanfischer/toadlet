# Setup Versions
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Version.h.in ${CMAKE_CURRENT_BINARY_DIR}/Version.h


# Source files for toadlet_ribbit
set (RIBBIT_SRC
	Version.cpp
)


# Headers
set (RIBBIT_HEADERS
	Audio.h
	AudioBuffer.h
	AudioPlayer.h
	AudioStream.h
	CapabilitySet.h
	Types.h
	Version.h
)


# Dynamic library
if (NOT TOADLET_PLATFORM_IPHONE)
	add_library (toadlet_ribbit SHARED ${RIBBIT_SRC} ${RIBBIT_HEADERS})
	set_target_properties (toadlet_ribbit PROPERTIES ${DYNAMIC_LIB_PROPS})
	target_link_libraries (toadlet_ribbit toadlet_egg)
	install (TARGETS toadlet_ribbit DESTINATION lib)
endif (NOT TOADLET_PLATFORM_IPHONE)


# Static library
add_library (toadlet_ribbit_s STATIC ${RIBBIT_SRC} ${RIBBIT_HEADERS})
set_target_properties (toadlet_ribbit_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_ribbit_s toadlet_egg_s)
install (TARGETS toadlet_ribbit_s DESTINATION lib)


# Platform specific modules
if (NOT TOADLET_PLATFORM_WINCE AND TOADLET_PLATFORM_WIN32 OR TOADLET_PLATFORM_POSIX OR TOADLET_PLATFORM_IPHONE)
	find_package (OpenAL)
	if (OPENAL_FOUND)
		set (TOADLET_HAS_OPENAL ON PARENT_SCOPE)
		add_subdirectory (plugins/alplayer)
	else (OPENAL_FOUND)
		message ("OpenAL was not found. The ribbit_alplayer will not be built")
	endif (OPENAL_FOUND)
endif (NOT TOADLET_PLATFORM_WINCE AND TOADLET_PLATFORM_WIN32 OR TOADLET_PLATFORM_POSIX OR TOADLET_PLATFORM_IPHONE)
if (TOADLET_PLATFORM_WINCE OR TOADLET_PLATFORM_WIN32)
	add_subdirectory (plugins/win32player)
	# Audiere is currently unavailable due to lack of a detection script
	#add_subdirectory (plugins/audiereplayer)
endif (TOADLET_PLATFORM_WINCE OR TOADLET_PLATFORM_WIN32)

