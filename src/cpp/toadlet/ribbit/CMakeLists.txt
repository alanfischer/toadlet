# Setup Versions
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Version.h.in ${CMAKE_CURRENT_BINARY_DIR}/Version.h)


# Source files for toadlet_ribbit
set (RIBBIT_SRC
	AudioFormatConversion.cpp
	Version.cpp
)

set (RIBBIT_DECODERS_OGGVORBISDECODER_SRC
	plugins/decoders/oggvorbisdecoder/OggVorbisDecoder.cpp
)

set (RIBBIT_DECODERS_WAVEDECODER_SRC
	plugins/decoders/wavedecoder/WaveDecoder.cpp
)

set (RIBBIT_DECODERS_SIDDECODER_SRC
	plugins/decoders/siddecoder/SIDDecoder.cpp
)


# Headers
set (RIBBIT_HEADERS
	Audio.h
	AudioBuffer.h
	AudioFormatConversion.h
	AudioPlayer.h
	AudioStream.h
	CapabilitySet.h
	Types.h
	Version.h
)

set (RIBBIT_DECODERS_OGGVORBISDECODER_HEADERS
	plugins/decoders/oggvorbisdecoder/OggVorbisDecoder.h
)

set (RIBBIT_DECODERS_WAVEDECODER_HEADERS
	plugins/decoders/wavedecoder/WaveDecoder.h
)

set (RIBBIT_DECODERS_SIDDECODER_HEADERS
	plugins/decoders/siddecoder/SIDDecoder.h
)


source_group ("Source Files\\\\decoders\\\\wavedecoder" FILES ${RIBBIT_DECODERS_WAVEDECODER_SRC})
source_group ("Header Files\\\\decoders\\\\wavedecoder" FILES ${RIBBIT_DECODERS_WAVEDECODER_HEADERS})

set (RIBBIT_SRC ${RIBBIT_SRC} ${RIBBIT_DECODERS_WAVEDECODER_SRC})
set (RIBBIT_HEADERS ${RIBBIT_HEADERS} ${RIBBIT_DECODERS_WAVEDECODER_HEADERS})

# OSX has an issue with linking ppc/i386 versions of oggvorbis currently
if (NOT TOADLET_PLATFORM_OSX)
	find_package (OggVorbis)
endif (NOT TOADLET_PLATFORM_OSX)

if (OGGVORBIS_FOUND)
	set (TOADLET_HAS_OGGVORBIS ON PARENT_SCOPE)
		
	source_group ("Source Files\\\\decoders\\\\oggvorbisdecoder" FILES ${RIBBIT_DECODERS_OGGVORBISDECODER_SRC})
	source_group ("Header Files\\\\decoders\\\\oggvorbisdecoder" FILES ${RIBBIT_DECODERS_OGGVORBISDECODER_HEADERS})
	set (RIBBIT_SRC ${RIBBIT_SRC} ${RIBBIT_DECODERS_OGGVORBISDECODER_SRC})
	set (RIBBIT_HEADERS ${RIBBIT_HEADERS} ${RIBBIT_DECODERS_OGGVORBISDECODER_HEADERS})
	include_directories (${OGGVORBIS_INCLUDES})

	# Dynamic libs
	set (RIBBIT_LIBS ${RIBBIT_LIBS} ${OGGVORBIS_LIBRARIES})

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
	set (RIBBIT_DEFS_S ${RIBBIT_DEFS_S} TOADLET_LIBOGG_NAME="${OGG_LIBRARY}" TOADLET_LIBVORBIS_NAME="${VORBIS_LIBRARY}" TOADLET_LIBVORBISFILE_NAME="${VORBISFILE_LIBRARY}" TOADLET_LIBVORBISENC_NAME="${VORBISENC_LIBRARY}")
	set (RIBBIT_LIBS_S ${RIBBIT_LIBS_S} libogg_s libvorbis_s libvorbisfile_s libvorbisenc_s)
else (OGGVORBIS_FOUND)
	message ("ribbit will not have Ogg Vorbis support")
endif (OGGVORBIS_FOUND)

find_package (SidPlay2)
if (SIDPLAY2_FOUND)
endif (SIDPLAY2_FOUND)

# Dynamic library
if (NOT TOADLET_PLATFORM_IPHONE)
	add_library (toadlet_ribbit SHARED ${RIBBIT_SRC} ${RIBBIT_HEADERS})
	set_target_properties (toadlet_ribbit PROPERTIES ${DYNAMIC_LIB_PROPS})
	set_property (TARGET toadlet_ribbit APPEND PROPERTY COMPILE_DEFINITIONS ${RIBBIT_DEFS})
	target_link_libraries (toadlet_ribbit toadlet_egg ${RIBBIT_LIBS})
	install (TARGETS toadlet_ribbit DESTINATION lib)
endif (NOT TOADLET_PLATFORM_IPHONE)


# Static library
add_library (toadlet_ribbit_s STATIC ${RIBBIT_SRC} ${RIBBIT_HEADERS})
set_target_properties (toadlet_ribbit_s PROPERTIES ${STATIC_LIB_PROPS})
set_property (TARGET toadlet_ribbit_s APPEND PROPERTY COMPILE_DEFINITIONS ${RIBBIT_DEFS_S})
target_link_libraries (toadlet_ribbit_s toadlet_egg_s ${RIBBIT_LIBS_S})
install (TARGETS toadlet_ribbit_s DESTINATION lib)


# Platform specific modules
if (NOT TOADLET_PLATFORM_WINCE AND TOADLET_PLATFORM_WIN32 OR TOADLET_PLATFORM_POSIX OR TOADLET_PLATFORM_IPHONE)
	if (TOADLET_PLATFORM_OSX)
		# Right now OSX cannot use the alem plugin tool, so we have to search for openal
		find_package (OpenAL)
		if (OPENAL_FOUND)
			set (TOADLET_HAS_OPENAL ON PARENT_SCOPE)
			add_subdirectory (plugins/alplayer)
		else (OPENAL_FOUND)
			message ("OpenAL was not found. The ribbit alplayer will not be built")
		endif (OPENAL_FOUND)
	else (TOADLET_PLATFORM_OSX)
		set (TOADLET_HAS_OPENAL ON PARENT_SCOPE)
		add_subdirectory (plugins/alplayer)
	endif (TOADLET_PLATFORM_OSX)
endif (NOT TOADLET_PLATFORM_WINCE AND TOADLET_PLATFORM_WIN32 OR TOADLET_PLATFORM_POSIX OR TOADLET_PLATFORM_IPHONE)
if (TOADLET_PLATFORM_WINCE OR TOADLET_PLATFORM_WIN32)
	add_subdirectory (plugins/mmplayer)
endif (TOADLET_PLATFORM_WINCE OR TOADLET_PLATFORM_WIN32)

