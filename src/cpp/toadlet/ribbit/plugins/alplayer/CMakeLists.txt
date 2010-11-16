# Source files for toadlet_ribbit_alplayer
set (RIBBIT_ALPLAYER_SRC
	ALAudioBuffer.cpp
	ALAudio.cpp
	ALPlayer.cpp
)

set (RIBBIT_ALPLAYER_ALEM_SRC
	alem/alem.c
)

set (RIBBIT_ALPLAYER_PLATFORM_OSX_SRC
	platform/osx/CoreAudio.cpp
)

set (RIBBIT_DECODERS_COREAUDIODECODER_SRC
	../decoders/coreaudiodecoder/CoreAudioDecoder.cpp
)


# Headers
set (RIBBIT_ALPLAYER_HEADERS
	ALAudioBuffer.h
	ALAudio.h
	ALIncludes.h
	ALPlayer.h
)

set (RIBBIT_ALPLAYER_ALEM_HEADERS
	alem/alem.h
)

set (RIBBIT_ALPLAYER_PLATFORM_OSX_HEADERS
	platform/osx/CoreAudio.h
)

set (RIBBIT_DECODERS_COREAUDIODECODER_HEADERS
	../decoders/coreaudiodecoder/CoreAudioDecoder.h
)


# Platform specific files
if (TOADLET_PLATFORM_OSX OR TOADLET_PLATFORM_IPHONE)
	find_library (COREAUDIO_LIBS CoreAudio)
	find_library (COREFOUNDATION_LIBS CoreFoundation)
	find_library (AUDIOTOOLBOX_LIBS AudioToolbox)
	if (COREAUDIO_LIBS AND COREFOUNDATION_LIBS AND AUDIOTOOLBOX_LIBS)
		include_directories (${OPENAL_INCLUDE_DIR})
		source_group ("Source Files\\\\platform\\\\osx" FILES ${RIBBIT_ALPLAYER_PLATFORM_OSX_SRC})
		source_group ("Source Files\\\\decoders\\\\coreaudiodecoder" FILES ${RIBBIT_DECODERS_COREAUDIODECODER_SRC})
		source_group ("Header Files\\\\platform\\\\osx" FILES ${RIBBIT_ALPLAYER_PLATFORM_OSX_HEADERS})
		source_group ("Header Files\\\\decoders\\\\coreaudiodecoder" FILES ${RIBBIT_DECODERS_COREAUDIODECODER_HEADERS})
		set (RIBBIT_ALPLAYER_SRC ${RIBBIT_ALPLAYER_SRC} ${RIBBIT_ALPLAYER_PLATFORM_OSX_SRC} ${RIBBIT_DECODERS_COREAUDIODECODER_SRC})
		set (RIBBIT_ALPLAYER_HEADERS ${RIBBIT_ALPLAYER_HEADERS} ${RIBBIT_ALPLAYER_PLATFORM_OSX_HEADERS} ${RIBBIT_DECODERS_COREAUDIODECODER_HEADERS})
		set (RIBBIT_ALPLAYER_LIBS ${RIBBIT_ALPLAYER_LIBS} ${COREAUDIO_LIBS} ${COREFOUNDATION_LIBS} ${AUDIOTOOLBOX_LIBS} ${OPENAL_LIBRARY})
		set (RIBBIT_ALPLAYER_LIBS_S ${RIBBIT_ALPLAYER_LIBS_S} ${COREAUDIO_LIBS} ${COREFOUNDATION_LIBS} ${AUDIOTOOLBOX_LIBS} ${OPENAL_LIBRARY})
	else (COREAUDIO_LIBS AND COREFOUNDATION_LIBS AND AUDIOTOOLBOX_LIBS)
		message ("One of CoreAudio, CoreFoundation, or AudioToolbox were not found, so the ribbit_alplayer will not be built")
	endif (COREAUDIO_LIBS AND COREFOUNDATION_LIBS AND AUDIOTOOLBOX_LIBS)
elseif (TOADLET_PLATFORM_WIN32 OR TOADLET_PLATFORM_POSIX)
	set (RIBBIT_ALPLAYER_DEFS "ALEM_BUILD")
	set (RIBBIT_ALPLAYER_DEFS_S "ALEM_BUILD" "ALEM_STATIC")
	source_group ("Source Files\\\\alem" FILES ${RIBBIT_ALPLAYER_ALEM_SRC})
	source_group ("Header Files\\\\alem" FILES ${RIBBIT_ALPLAYER_ALEM_HEADERS})
	set (RIBBIT_ALPLAYER_SRC ${RIBBIT_ALPLAYER_SRC} ${RIBBIT_ALPLAYER_ALEM_SRC})
	set (RIBBIT_ALPLAYER_HEADERS ${RIBBIT_ALPLAYER_HEADERS} ${RIBBIT_ALPLAYER_ALEM_HEADERS})
endif (TOADLET_PLATFORM_OSX OR TOADLET_PLATFORM_IPHONE)


# Dynamic library
if (NOT TOADLET_PLATFORM_IPHONE)
	add_library (toadlet_ribbit_alplayer SHARED ${RIBBIT_ALPLAYER_SRC} ${RIBBIT_ALPLAYER_HEADERS})
	set_target_properties (toadlet_ribbit_alplayer PROPERTIES ${DYNAMIC_LIB_PROPS})
	set_property (TARGET toadlet_ribbit_alplayer APPEND PROPERTY COMPILE_DEFINITIONS ${RIBBIT_ALPLAYER_DEFS})
	target_link_libraries (toadlet_ribbit_alplayer toadlet_ribbit ${RIBBIT_ALPLAYER_LIBS})
	install (TARGETS toadlet_ribbit_alplayer DESTINATION lib)
endif (NOT TOADLET_PLATFORM_IPHONE)


# Static library
add_library (toadlet_ribbit_alplayer_s STATIC ${RIBBIT_ALPLAYER_SRC} ${RIBBIT_ALPLAYER_HEADERS})
set_target_properties (toadlet_ribbit_alplayer_s PROPERTIES ${STATIC_LIB_PROPS})
set_property (TARGET toadlet_ribbit_alplayer_s APPEND PROPERTY COMPILE_DEFINITIONS ${RIBBIT_ALPLAYER_DEFS_S})
target_link_libraries (toadlet_ribbit_alplayer_s toadlet_ribbit_s ${RIBBIT_ALPLAYER_LIBS_S})
install (TARGETS toadlet_ribbit_alplayer_s DESTINATION lib)

