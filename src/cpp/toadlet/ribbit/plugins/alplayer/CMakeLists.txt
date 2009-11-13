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

set (RIBBIT_DECODERS_OGGVORBISDECODER_SRC
	../decoders/oggvorbisdecoder/OggVorbisDecoder.cpp
)

set (RIBBIT_DECODERS_WAVEDECODER_SRC
	../decoders/wavedecoder/WaveDecoder.cpp
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

set (RIBBIT_DECODERS_OGGVORBISDECODER_HEADERS
	../decoders/oggvorbisdecoder/OggVorbisDecoder.h
)

set (RIBBIT_DECODERS_WAVEDECODER_HEADERS
	../decoders/wavedecoder/WaveDecoder.h
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
	source_group ("Source Files\\\\decoders\\\\wavedecoder" FILES ${RIBBIT_DECODERS_WAVEDECODER_SRC})
	source_group ("Header Files\\\\decoders\\\\wavedecoder" FILES ${RIBBIT_DECODERS_WAVEDECODER_HEADERS})
	source_group ("Source Files\\\\alem" FILES ${RIBBIT_ALPLAYER_ALEM_SRC})
	source_group ("Header Files\\\\alem" FILES ${RIBBIT_ALPLAYER_ALEM_HEADERS})
	set (RIBBIT_ALPLAYER_SRC ${RIBBIT_ALPLAYER_SRC} ${RIBBIT_DECODERS_WAVEDECODER_SRC} ${RIBBIT_ALPLAYER_ALEM_SRC})
	set (RIBBIT_ALPLAYER_HEADERS ${RIBBIT_ALPLAYER_HEADERS} ${RIBBIT_DECODERS_WAVEDECODER_HEADERS} ${RIBBIT_ALPLAYER_ALEM_HEADERS})
	find_package (OggVorbis)
	if (OGGVORBIS_FOUND)
		source_group ("Source Files\\\\decoders\\\\oggvorbisdecoder" FILES ${RIBBIT_DECODERS_OGGVORBISDECODER_SRC})
		source_group ("Header Files\\\\decoders\\\\oggvorbisdecoder" FILES ${RIBBIT_DECODERS_OGGVORBISDECODER_HEADERS})
		set (RIBBIT_ALPLAYER_SRC ${RIBBIT_ALPLAYER_SRC} ${RIBBIT_DECODERS_OGGVORBISDECODER_SRC})
		set (RIBBIT_ALPLAYER_HEADERS ${RIBBIT_ALPLAYER_HEADERS} ${RIBBIT_DECODERS_OGGVORBISDECODER_HEADERS})
		include_directories (${OGGVORBIS_INCLUDES})

		# Dynamic libs
		set (RIBBIT_ALPLAYER_LIBS ${RIBBIT_ALPLAYER_LIBS} ${OGGVORBIS_LIBRARIES})

		# Static libs are ugly since we add references to external static libraries in toadlet
		add_library (libogg_s STATIC IMPORTED)
		add_library (libvorbis_s STATIC IMPORTED)
		add_library (libvorbisfile_s STATIC IMPORTED)
		add_library (libvorbisenc_s STATIC IMPORTED)
		if (TOADLET_PLATFORM_POSIX)
			string (REPLACE ".so" ".a" OGG_A ${OGG_LIBRARY})
			if (EXISTS ${OGG_A})
				set (OGG_LIBRARY ${OGG_A})
			endif (EXISTS ${OGG_A})
			string (REPLACE ".so" ".a" VORBIS_A ${VORBIS_LIBRARY})
			if (EXISTS ${VORBIS_A})
				set (VORBIS_LIBRARY ${VORBIS_A})
			endif (EXISTS ${VORBIS_A})
			string (REPLACE ".so" ".a" VORBISFILE_A ${VORBISFILE_LIBRARY})
			if (EXISTS ${VORBISFILE_A})
				set (VORBISFILE_LIBRARY ${VORBISFILE_A})
			endif (EXISTS ${VORBISFILE_A})
			string (REPLACE ".so" ".a" VORBISENG_A ${VORBISENC_LIBRARY})
			if (EXISTS ${VORBISENC_A})
				set (VORBISENC_LIBRARY ${VORBISENC_A})
			endif (EXISTS ${VORBISENC_A})
		endif (TOADLET_PLATFORM_POSIX)
		set_property (TARGET libogg_s PROPERTY IMPORTED_LOCATION ${OGG_LIBRARY})
		set_property (TARGET libvorbis_s PROPERTY IMPORTED_LOCATION ${VORBIS_LIBRARY})
		set_property (TARGET libvorbisfile_s PROPERTY IMPORTED_LOCATION ${VORBISFILE_LIBRARY})
		set_property (TARGET libvorbisenc_s PROPERTY IMPORTED_LOCATION ${VORBISENC_LIBRARY})
		set (RIBBIT_ALPLAYER_DEFS_S ${RIBBIT_ALPLAYER_DEFS_S} TOADLET_LIBOGG_NAME="${OGG_LIBRARY}" TOADLET_LIBVORBIS_NAME="${VORBIS_LIBRARY}" TOADLET_LIBVORBISFILE_NAME="${VORBISFILE_LIBRARY}" TOADLET_LIBVORBISENC_NAME="${VORBISENC_LIBRARY}")
		set (RIBBIT_ALPLAYER_LIBS_S ${RIBBIT_ALPLAYER_LIBS_S} libogg_s libvorbis_s libvorbisfile_s libvorbisenc_s)
	else (OGGVORBIS_FOUND)
		message ("The ribbit_alplayer will not have Ogg Vorbis support")
	endif (OGGVORBIS_FOUND)
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

