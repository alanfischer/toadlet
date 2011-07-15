# Setup Versions
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/Version.h)


# Source files for toadlet_ribbit
set (RIBBIT_SRC
	AudioFormatConversion.cpp
	Version.cpp
)

set (RIBBIT_DECODER_OGGVORBISDECODER_SRC
	decoder/OggVorbisDecoder.cpp
)

set (RIBBIT_DECODER_WAVEDECODER_SRC
	decoder/WaveDecoder.cpp
)

set (RIBBIT_DECODER_SIDDECODER_SRC
	decoder/SIDDecoder.cpp
)


# Headers
set (RIBBIT_HEADERS
	Audio.h
	AudioBuffer.h
	AudioFormat.h
	AudioFormatConversion.h
	AudioDevice.h
	AudioStream.h
	CapabilitySet.h
	Types.h
	Version.h
)

set (RIBBIT_DECODER_OGGVORBISDECODER_HEADERS
	decoder/OggVorbisDecoder.h
)

set (RIBBIT_DECODER_WAVEDECODER_HEADERS
	decoder/WaveDecoder.h
)

set (RIBBIT_DECODER_SIDDECODER_HEADERS
	decoder/SIDDecoder.h
)


# Bundle source and header files
source_group ("Source Files\\\\decoder" FILES ${RIBBIT_DECODER_WAVEDECODER_SRC})
source_group ("Header Files\\\\decoder" FILES ${RIBBIT_DECODER_WAVEDECODER_HEADERS})


# Sources, headers and namespaces
set (RIBBIT_SRC ${RIBBIT_SRC} ${RIBBIT_DECODER_WAVEDECODER_SRC})
set (RIBBIT_HEADERS ${RIBBIT_HEADERS} ${RIBBIT_DECODER_WAVEDECODER_HEADERS})
set (RIBBIT_NAMESPACES ${RIBBIT_NAMESPACES} decoder)


find_package (OggVorbis)
if (OGGVORBIS_FOUND)
	set (TOADLET_HAS_OGGVORBIS ON PARENT_SCOPE)
		
	source_group ("Source Files\\\\decoder" FILES ${RIBBIT_DECODER_OGGVORBISDECODER_SRC})
	source_group ("Header Files\\\\decoder" FILES ${RIBBIT_DECODER_OGGVORBISDECODER_HEADERS})
	set (RIBBIT_SRC ${RIBBIT_SRC} ${RIBBIT_DECODER_OGGVORBISDECODER_SRC})
	set (RIBBIT_HEADERS ${RIBBIT_HEADERS} ${RIBBIT_DECODER_OGGVORBISDECODER_HEADERS})
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

find_package (Sidplay)
if (SIDPLAY_FOUND)
	set (TOADLET_HAS_SIDPLAY ON PARENT_SCOPE)

	add_definitions(-DSIDPLAY_VERSION=${SIDPLAY_VERSION})
	
	source_group ("Source Files\\\\decoder" FILES ${RIBBIT_DECODER_SIDDECODER_SRC})
	source_group ("Header Files\\\\decoder" FILES ${RIBBIT_DECODER_SIDDECODER_HEADERS})
	set (RIBBIT_SRC ${RIBBIT_SRC} ${RIBBIT_DECODER_SIDDECODER_SRC})
	set (RIBBIT_HEADERS ${RIBBIT_HEADERS} ${RIBBIT_DECODER_SIDDECODER_HEADERS})
	include_directories (${SIDPLAY_INCLUDES})

	# Dynamic libs
	set (RIBBIT_LIBS ${RIBBIT_LIBS} ${SIDPLAY_LIBRARY})

	# Static libs are ugly since we add references to external static libraries in toadlet
	set (RIBBIT_DEFS_S ${RIBBIT_DEFS_S} TOADLET_LIBSIDPLAY_NAME="${SIDPLAY_LIBRARY}")
	set (RIBBIT_LIBS_S ${RIBBIT_LIBS_S} libsidplay_s)

	# Resid is only used with sidplay 2
	if (${SIDPLAY_VERSION} EQUAL 2)
		set (RIBBIT_LIBS ${RIBBIT_LIBS} ${RESID_LIBRARY})
		set (RIBBIT_DEFS_S ${RIBBIT_DEFS_S} TOADLET_LIBRESID_NAME="${RESID_LIBRARY}")
	endif (${SIDPLAY_VERSION} EQUAL 2)
endif (SIDPLAY_FOUND)


# Give the headers and namespaces parent scope for inclusion into the master ribbit.h file
set (RIBBIT_HEADERS ${RIBBIT_HEADERS} PARENT_SCOPE)
set (RIBBIT_NAMESPACES ${RIBBIT_NAMESPACES} PARENT_SCOPE)


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
			add_subdirectory (plugins/alaudiodevice)
		else (OPENAL_FOUND)
			message ("OpenAL was not found. The ribbit alaudiodevice will not be built")
		endif (OPENAL_FOUND)
	else (TOADLET_PLATFORM_OSX)
		set (TOADLET_HAS_OPENAL ON PARENT_SCOPE)
		add_subdirectory (plugins/alaudiodevice)
	endif (TOADLET_PLATFORM_OSX)
endif (NOT TOADLET_PLATFORM_WINCE AND TOADLET_PLATFORM_WIN32 OR TOADLET_PLATFORM_POSIX OR TOADLET_PLATFORM_IPHONE)
if (TOADLET_PLATFORM_WINCE OR TOADLET_PLATFORM_WIN32)
	add_subdirectory (plugins/mmaudiodevice)
endif (TOADLET_PLATFORM_WINCE OR TOADLET_PLATFORM_WIN32)

