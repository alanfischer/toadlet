# Setup Versions
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/Version.h)


# Source files for toadlet_ribbit
set (RIBBIT_SRC
	AudioFormatConversion.cpp
	Version.cpp
)


# Headers
set (RIBBIT_HDR
	Audio.h
	AudioBuffer.h
	AudioCaps.h
	AudioFormat.h
	AudioFormatConversion.h
	AudioDevice.h
	AudioStream.h
	BaseAudio.h
	BaseAudioBuffer.h
	BaseAudioDevice.h
	Types.h
	Version.h
)


# Give the headers and namespaces parent scope for inclusion into the master ribbit.h file
set (RIBBIT_HDR ${RIBBIT_HDR} PARENT_SCOPE)
set (RIBBIT_NAMESPACES ${RIBBIT_NAMESPACES} PARENT_SCOPE)


# If we are using swig create the wrapper file from the interface files
if (SWIG_FOUND AND RIBBIT_SWIG_INT)
	swig_wrap_files (ribbit_wrap.cpp ribbit ${RIBBIT_SWIG_INT} "${RIBBIT_SWIG_INT_DEPS}" "${RIBBIT_SWIG_INT_OUTPUTS}")
	set (RIBBIT_SRC ${RIBBIT_SRC} ribbit_wrap.cpp)
endif (SWIG_FOUND AND RIBBIT_SWIG_INT)


# For individual component installation 
set (RIBBIT_INSTALL)


# Dynamic library
if (TOADLET_BUILD_DYNAMIC)
	add_library (toadlet_ribbit SHARED ${RIBBIT_SRC} ${RIBBIT_HDR})
	set_target_properties (toadlet_ribbit PROPERTIES ${DYNAMIC_LIB_PROPS})
	set_property (TARGET toadlet_ribbit APPEND PROPERTY COMPILE_DEFINITIONS ${RIBBIT_DEFS})
	target_link_libraries (toadlet_ribbit toadlet_egg)
	install (TARGETS toadlet_ribbit DESTINATION ${LIB_INSTALL_DIR} COMPONENT ribbit)
	set (RIBBIT_INSTALL ${RIBBIT_INSTALL} toadlet_ribbit)
endif (TOADLET_BUILD_DYNAMIC)


# Static library
if (TOADLET_BUILD_STATIC)
	add_library (toadlet_ribbit_s STATIC ${RIBBIT_SRC} ${RIBBIT_HDR})
	set_target_properties (toadlet_ribbit_s PROPERTIES ${STATIC_LIB_PROPS})
	set_property (TARGET toadlet_ribbit_s APPEND PROPERTY COMPILE_DEFINITIONS ${RIBBIT_DEFS_S})
	target_link_libraries (toadlet_ribbit_s toadlet_egg_s)
	install (TARGETS toadlet_ribbit_s DESTINATION ${LIB_INSTALL_DIR} COMPONENT ribbit)
	set (RIBBIT_INSTALL ${RIBBIT_INSTALL} toadlet_ribbit_s)

	# HACK: install the library on iOS. This will disappear once cmake issue:0012506 is resolved. 
	if (TOADLET_PLATFORM_IOS)
	    install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/\$ENV{CONFIGURATION}\$ENV{EFFECTIVE_PLATFORM_NAME}/ DESTINATION ${LIB_INSTALL_DIR} COMPONENT ribbit FILES_MATCHING PATTERN "*.a")
	endif (TOADLET_PLATFORM_IOS)
endif (TOADLET_BUILD_STATIC)


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


# Rule to install only ribbit
add_custom_target (ribbit_install
	COMMAND ${CMAKE_COMMAND} -DBUILD_TYPE=$<CONFIGURATION> -DCOMPONENT=required -P ${PROJECT_BINARY_DIR}/cmake_install.cmake
	COMMAND ${CMAKE_COMMAND} -DBUILD_TYPE=$<CONFIGURATION> -DCOMPONENT=egg -P ${PROJECT_BINARY_DIR}/cmake_install.cmake
	COMMAND ${CMAKE_COMMAND} -DBUILD_TYPE=$<CONFIGURATION> -DCOMPONENT=ribbit -P ${PROJECT_BINARY_DIR}/cmake_install.cmake
	DEPENDS ${RIBBIT_INSTALL}
)
