# Setup Versions
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/Version.h)
install (FILES  ${CMAKE_CURRENT_BINARY_DIR}/Version.h DESTINATION include/toadlet/ribbit)


# Source files for toadlet_ribbit
set (RIBBIT_SRC
	AudioFormatConversion.cpp
	Version.cpp
)


# Headers
set (RIBBIT_HEADERS
	Audio.h
	AudioBuffer.h
	AudioCaps.h
	AudioFormat.h
	AudioFormatConversion.h
	AudioDevice.h
	AudioStream.h
	Types.h
	Version.h
)


# Give the headers and namespaces parent scope for inclusion into the master ribbit.h file
set (RIBBIT_HEADERS ${RIBBIT_HEADERS} PARENT_SCOPE)
set (RIBBIT_NAMESPACES ${RIBBIT_NAMESPACES} PARENT_SCOPE)


# Dynamic library
if (NOT TOADLET_PLATFORM_IOS)
	add_library (toadlet_ribbit SHARED ${RIBBIT_SRC} ${RIBBIT_HEADERS})
	set_target_properties (toadlet_ribbit PROPERTIES ${DYNAMIC_LIB_PROPS})
	set_property (TARGET toadlet_ribbit APPEND PROPERTY COMPILE_DEFINITIONS ${RIBBIT_DEFS})
	target_link_libraries (toadlet_ribbit toadlet_egg)
	install (TARGETS toadlet_ribbit DESTINATION ${LIB_INSTALL_DIR})
endif (NOT TOADLET_PLATFORM_IOS)


# Static library
add_library (toadlet_ribbit_s STATIC ${RIBBIT_SRC} ${RIBBIT_HEADERS})
set_target_properties (toadlet_ribbit_s PROPERTIES ${STATIC_LIB_PROPS})
set_property (TARGET toadlet_ribbit_s APPEND PROPERTY COMPILE_DEFINITIONS ${RIBBIT_DEFS_S})
target_link_libraries (toadlet_ribbit_s toadlet_egg_s)
install (TARGETS toadlet_ribbit_s DESTINATION ${LIB_INSTALL_DIR})


# Platform specific modules
if (TOADLET_PLATFORM_ANDROID)
	add_subdirectory (plugins/jaudiodevice)
elseif (TOADLET_PLATFORM_WINCE)
	add_subdirectory (plugins/mmaudiodevice)
elseif (TOADLET_PLATFORM_IOS)
	set (TOADLET_HAS_OPENAL ON PARENT_SCOPE)
	add_subdirectory (plugins/alaudiodevice)
elseif (TOADLET_PLATFORM_WIN32)
	add_subdirectory (plugins/mmaudiodevice)
	set (TOADLET_HAS_OPENAL ON PARENT_SCOPE)
	add_subdirectory (plugins/alaudiodevice)
elseif (TOADLET_PLATFORM_OSX)
	# Right now OSX cannot use the alem plugin tool, so we have to search for openal
	find_package (OpenAL)
	if (OPENAL_FOUND)
		set (TOADLET_HAS_OPENAL ON PARENT_SCOPE)
		add_subdirectory (plugins/alaudiodevice)
	else (OPENAL_FOUND)
		message ("OpenAL was not found. The ribbit alaudiodevice will not be built")
	endif (OPENAL_FOUND)
elseif (TOADLET_PLATFORM_POSIX)
	set (TOADLET_HAS_OPENAL ON PARENT_SCOPE)
	add_subdirectory (plugins/alaudiodevice)
endif (TOADLET_PLATFORM_ANDROID)
