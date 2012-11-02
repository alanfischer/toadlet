# Source files for toadlet_ribbit_alaudiodevice
set (RIBBIT_ALAUDIODEVICE_SRC
	ALAudioBuffer.cpp
	ALAudio.cpp
	ALAudioDevice.cpp
)

set (RIBBIT_ALAUDIODEVICE_ALEM_SRC
	alem/alem.c
)


# Headers
set (RIBBIT_ALAUDIODEVICE_HDR
	ALAudioBuffer.h
	ALAudio.h
	ALIncludes.h
	ALAudioDevice.h
)

set (RIBBIT_ALAUDIODEVICE_ALEM_HDR
	alem/alem.h
)


# Platform specific files
if (TOADLET_PLATFORM_OSX OR TOADLET_PLATFORM_IOS)
	find_library (COREAUDIO_LIBS CoreAudio)
	find_library (COREFOUNDATION_LIBS CoreFoundation)
	find_library (AUDIOTOOLBOX_LIBS AudioToolbox)
	if (COREAUDIO_LIBS AND COREFOUNDATION_LIBS AND AUDIOTOOLBOX_LIBS)
		include_directories (${OPENAL_INCLUDE_DIR})
		set (RIBBIT_ALAUDIODEVICE_LIBS ${RIBBIT_ALAUDIODEVICE_LIBS} ${COREAUDIO_LIBS} ${COREFOUNDATION_LIBS} ${AUDIOTOOLBOX_LIBS} ${OPENAL_LIBRARY})
	else (COREAUDIO_LIBS AND COREFOUNDATION_LIBS AND AUDIOTOOLBOX_LIBS)
		message ("One of CoreAudio, CoreFoundation, or AudioToolbox were not found, so the ribbit_alaudiodevice will not be built")
	endif (COREAUDIO_LIBS AND COREFOUNDATION_LIBS AND AUDIOTOOLBOX_LIBS)
elseif (TOADLET_PLATFORM_WIN32 OR TOADLET_PLATFORM_POSIX)
	set (RIBBIT_ALAUDIODEVICE_DEFS "ALEM_BUILD")
	set (RIBBIT_ALAUDIODEVICE_DEFS_S "ALEM_BUILD" "ALEM_STATIC")
	source_group ("Source Files\\\\alem" FILES ${RIBBIT_ALAUDIODEVICE_ALEM_SRC})
	source_group ("Header Files\\\\alem" FILES ${RIBBIT_ALAUDIODEVICE_ALEM_HDR})
	set (RIBBIT_ALAUDIODEVICE_SRC ${RIBBIT_ALAUDIODEVICE_SRC} ${RIBBIT_ALAUDIODEVICE_ALEM_SRC})
	set (RIBBIT_ALAUDIODEVICE_HDR ${RIBBIT_ALAUDIODEVICE_HDR} ${RIBBIT_ALAUDIODEVICE_ALEM_HDR})
endif (TOADLET_PLATFORM_OSX OR TOADLET_PLATFORM_IOS)


# Dynamic library
if (TOADLET_BUILD_DYNAMIC)
	add_library (toadlet_ribbit_alaudiodevice SHARED ${RIBBIT_ALAUDIODEVICE_SRC} ${RIBBIT_ALAUDIODEVICE_HDR})
	set_target_properties (toadlet_ribbit_alaudiodevice PROPERTIES ${DYNAMIC_LIB_PROPS})
	set_property (TARGET toadlet_ribbit_alaudiodevice APPEND PROPERTY COMPILE_DEFINITIONS ${RIBBIT_ALAUDIODEVICE_DEFS})
	target_link_libraries (toadlet_ribbit_alaudiodevice toadlet_ribbit ${RIBBIT_ALAUDIODEVICE_LIBS})
	install (TARGETS toadlet_ribbit_alaudiodevice DESTINATION ${LIB_INSTALL_DIR} COMPONENT ribbit)
	set (RIBBIT_INSTALL ${RIBBIT_INSTALL} toadlet_ribbit_alaudiodevice PARENT_SCOPE)
endif (TOADLET_BUILD_DYNAMIC)


# Static library
if (TOADLET_BUILD_STATIC)
	add_library (toadlet_ribbit_alaudiodevice_s STATIC ${RIBBIT_ALAUDIODEVICE_SRC} ${RIBBIT_ALAUDIODEVICE_HDR})
	set_target_properties (toadlet_ribbit_alaudiodevice_s PROPERTIES ${STATIC_LIB_PROPS})
	set_property (TARGET toadlet_ribbit_alaudiodevice_s APPEND PROPERTY COMPILE_DEFINITIONS ${RIBBIT_ALAUDIODEVICE_DEFS_S})
	target_link_libraries (toadlet_ribbit_alaudiodevice_s toadlet_ribbit_s ${RIBBIT_ALAUDIODEVICE_LIBS})
	install (TARGETS toadlet_ribbit_alaudiodevice_s DESTINATION ${LIB_INSTALL_DIR} COMPONENT ribbit)
	set (RIBBIT_INSTALL ${RIBBIT_INSTALL} toadlet_ribbit_alaudiodevice_s PARENT_SCOPE)

	# HACK: install the library on iOS. This will disappear once cmake issue:0012506 is resolved. 
	if (TOADLET_PLATFORM_IOS)
	    install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/\$ENV{CONFIGURATION}\$ENV{EFFECTIVE_PLATFORM_NAME}/ DESTINATION ${LIB_INSTALL_DIR} COMPONENT ribbit FILES_MATCHING PATTERN "*.a")
	endif (TOADLET_PLATFORM_IOS)
endif (TOADLET_BUILD_STATIC)

