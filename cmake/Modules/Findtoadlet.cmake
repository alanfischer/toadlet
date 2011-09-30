# Attempt to find toadlet
# Defines:
#
#  TOADLET_FOUND	- system has toadlet
#  TOADLET_INCLUDE_DIR	- path to the toadlet include directory
#  TOADLET_LIBRARY_DIR	- path to the toadlet library installation dir
#  TOADLET_LIBRARIES	- sum of all toadlet libraries found for each type: dynamic (no suffix), static (_S), release (no suffix), debug (_D)
#
# Define each toadlet library individually: dynamic (no suffix), static (_S), release (no suffix), debug (_D)
# Each library, when at least one of the above is found, sets TOADLET_${LIBNAME}_FOUND = YES
#  TOADLET_EGG_LIB
#  TOADLET_FLICK_LIB
#  TOADLET_FLICK_IOSMOTIONDEVICE_LIB
#  TOADLET_FLICK_WIN32JOYDEVICE_LIB
#  TOADLET_HOP_LIB
#  TOADLET_KNOT_LIB
#  TOADLET_PEEPER_LIB
#  TOADLET_PEEPER_GLRENDERDEVICE_LIB
#  TOADLET_PEEPER_D3D9RENDERDEVICE_LIB
#  TOADLET_PEEPER_D3DMRENDERDEVICE_LIB
#  TOADLET_PEEPER_D3D10RENDERDEVICE_LIB
#  TOADLET_PEEPER_D3D11RENDERDEVICE_LIB
#  TOADLET_RIBBIT_LIB
#  TOADLET_RIBBIT_ALAUIODEVICE_LIB
#  TOADLET_RIBBIT_MMAUIODEVICE_LIB
#  TOADLET_RIBBIT_JAUIODEVICE_LIB
#  TOADLET_TADPOLE_LIB
#  TOADLET_TADPOLE_HOP_LIB
#  TOADLET_PAD_LIB
# Android builds with ndk api level < 9 only
#  TOADLET_PEEPER_JAR
#  TOADLET_PAD_JAR
# 
# Caveats:
# The generator will be queried for the build configurations (debug or optimized) and the proper library types assigned.
# If release libraries are not found, they will be set to the debug libraries.
# If debug libraries are not found, they will be set to the release libraries.
# Static and dynamic libraries, however, are totally separate.

set (TOADLET_FOUND "NO")
unset (TOADLET_LIBRARIES)
unset (TOADLET_LIBRARIES_S)
unset (TOADLET_LIBRARIES_D)
unset (TOADLET_LIBRARIES_SD)

# Default search paths
if (WIN32)
	set (HEADER_SEARCH_PATHS
		"C:/Program Files"
		"C:/Program Files/toadlet"
	)
	set (LIBRARY_SEARCH_PATHS
		"C:/Program Files"
		"C:/Program Files/toadlet"
	)
else (WIN32)
	set (HEADER_SEARCH_PATHS
		/usr/include
		/usr/local/include

	)
	set (LIBRARY_SEARCH_PATHS
		/usr/lib 
		/usr/local/lib
	)
endif (WIN32)

# Find the toadlet header file
find_path (TOADLET_INCLUDE_DIR toadlet/toadlet.h PATHS ${HEADER_SEARCH_PATHS})

# List of toadlet library basenames
set (TOADLET_LIB_BASENAMES
	toadlet_egg
	toadlet_flick
	toadlet_flick_iosmotiondevice
	toadlet_flick_win32joydevice
	toadlet_hop
	toadlet_knot
	toadlet_peeper
	toadlet_peeper_glrenderdevice
	toadlet_peeper_d3d9renderdevice
	toadlet_peeper_d3dmrenderdevice
	toadlet_peeper_d3d10renderdevice
	toadlet_peeper_d3d11renderdevice
	toadlet_ribbit
	toadlet_ribbit_alaudiodevice
	toadlet_ribbit_mmaudiodevice
	toadlet_ribbit_jaudiodevice
	toadlet_tadpole
	toadlet_tadpole_hop
	toadlet_pad
)

# Android specifics
if (ANDROID) 
	if (${ANDROID_NDK_API_LEVEL} LESS 9)
		set (LIBRARY_SEARCH_SUFFIXES ${NDK_NAME_ARCH})
		set (TOADLET_JAR_BASENAMES jtoadlet_peeper jtoadlet_ribbit jtoadlet_pad)

		# Search for and assign the android jar files
		foreach (TOADLET_JAR ${TOADLET_JAR_BASENAMES})
			string (TOUPPER ${TOADLET_JAR} TOADLET_JAR_VAR)

			# Only search for a library if we haven't already
			# Otherwise the debug/release assignment settings below will cause trouble with multiple runs
			if (NOT ${TOADLET_JAR_VAR}_FOUND)
				# Find release and debug
				find_file (${TOADLET_JAR_VAR}_JAR NAMES ${TOADLET_JAR}.jar PATHS ${LIBRARY_SEARCH_PATHS} PATH_SUFFIXES lib)
				find_file (${TOADLET_JAR_VAR}_JAR_D NAMES ${TOADLET_JAR}_d.jar PATHS ${LIBRARY_SEARCH_PATHS} PATH_SUFFIXES lib)

				# If only release libraries are found, assign them to the debug libraries
				if (${TOADLET_JAR_VAR}_JAR AND NOT ${TOADLET_JAR_VAR}_JAR_D)
					set (${TOADLET_JAR_VAR}_JAR_D ${${TOADLET_JAR_VAR}_JAR} CACHE FILEPATH "Path to a jar" FORCE)
					set (${TOADLET_JAR_VAR}_FOUND "YES")
				endif (${TOADLET_JAR_VAR}_JAR AND NOT ${TOADLET_JAR_VAR}_JAR_D)
				
				# If only debug libraries are found, assign them to the release libraries
				if (${TOADLET_JAR_VAR}_JAR_D AND NOT ${TOADLET_JAR_VAR}_JAR)
					set (${TOADLET_JAR_VAR}_JAR ${${TOADLET_JAR_VAR}_JAR_D} CACHE FILEPATH "Path to a jar" FORCE)
					set (${TOADLET_JAR_VAR}_FOUND "YES")
				endif (${TOADLET_JAR_VAR}_JAR_D AND NOT ${TOADLET_JAR_VAR}_JAR)
				
				set (${TOADLET_JAR_VAR}_FOUND ${${TOADLET_JAR_VAR}_FOUND} CACHE BOOL "Jar ${TOADLET_JAR} found flag" FORCE)
				mark_as_advanced (${TOADLET_JAR_VAR}_JAR ${TOADLET_JAR_VAR}_JAR_D ${TOADLET_JAR_VAR}_FOUND)
			endif (NOT ${TOADLET_JAR_VAR}_FOUND)
		endforeach (TOADLET_JAR)
	endif (${ANDROID_NDK_API_LEVEL} LESS 9)
endif (ANDROID)

# Search for and assign the toadlet libraries
foreach (TOADLET_LIB ${TOADLET_LIB_BASENAMES})
	string (TOUPPER ${TOADLET_LIB} TOADLET_LIB_VAR)

	# Only search for a library if we haven't already
	# Otherwise the debug/release assignment settings below will cause trouble with multiple runs
	if (NOT ${TOADLET_LIB_VAR}_FOUND)
		# Find dynamic release, dynamic debug, static release and then static debug
		find_library (${TOADLET_LIB_VAR}_LIB NAMES ${TOADLET_LIB} PATHS ${LIBRARY_SEARCH_PATHS} PATH_SUFFIXES ${LIBRARY_SEARCH_SUFFIXES})
		find_library (${TOADLET_LIB_VAR}_LIB_D NAMES ${TOADLET_LIB}_d PATHS ${LIBRARY_SEARCH_PATHS} PATH_SUFFIXES ${LIBRARY_SEARCH_SUFFIXES})
		find_library (${TOADLET_LIB_VAR}_LIB_S NAMES ${TOADLET_LIB}_s PATHS ${LIBRARY_SEARCH_PATHS} PATH_SUFFIXES ${LIBRARY_SEARCH_SUFFIXES})
		find_library (${TOADLET_LIB_VAR}_LIB_SD NAMES ${TOADLET_LIB}_sd PATHS ${LIBRARY_SEARCH_PATHS} PATH_SUFFIXES ${LIBRARY_SEARCH_SUFFIXES})

		# The first toadlet library we find sets the TOADLET_LIBRARY_DIR path
		if (NOT TOADLET_LIBRARY_DIR)
			if (${TOADLET_LIB_VAR}_LIB)
				get_filename_component (TOADLET_LIBRARY_DIR ${${TOADLET_LIB_VAR}_LIB} PATH CACHE)
			elseif(${TOADLET_LIB_VAR}_LIB_D)
				get_filename_component (TOADLET_LIBRARY_DIR ${${TOADLET_LIB_VAR}_LIB_D} PATH CACHE)
			elseif(${TOADLET_LIB_VAR}_LIB_S)
				get_filename_component (TOADLET_LIBRARY_DIR ${${TOADLET_LIB_VAR}_LIB_S} PATH CACHE)
			elseif(${TOADLET_LIB_VAR}_LIB_SD)
				get_filename_component (TOADLET_LIBRARY_DIR ${${TOADLET_LIB_VAR}_LIB_SD} PATH CACHE)
			endif (${TOADLET_LIB_VAR}_LIB)
		endif (NOT TOADLET_LIBRARY_DIR)

		# If both versions are found, set configuration specific libraries
		if (${TOADLET_LIB_VAR}_LIB AND ${TOADLET_LIB_VAR}_LIB_D)
			set (${TOADLET_LIB_VAR}_LIB optimized ${${TOADLET_LIB_VAR}_LIB} debug ${${TOADLET_LIB_VAR}_LIB_D} CACHE STRING "Both libraries" FORCE)
			set (${TOADLET_LIB_VAR}_FOUND "YES")
			set (TOADLET_LIBRARIES ${TOADLET_LIBRARIES} ${${TOADLET_LIB_VAR}_LIB})
			set (TOADLET_LIBRARIES_D ${TOADLET_LIBRARIES_D} ${${TOADLET_LIB_VAR}_LIB_D})
		endif (${TOADLET_LIB_VAR}_LIB AND ${TOADLET_LIB_VAR}_LIB_D)
		if (${TOADLET_LIB_VAR}_LIB_S AND ${TOADLET_LIB_VAR}_LIB_SD)
			set (${TOADLET_LIB_VAR}_LIB_S optimized ${${TOADLET_LIB_VAR}_LIB_S} debug ${${TOADLET_LIB_VAR}_LIB_SD} CACHE STRING "Both libraries" FORCE)
			set (${TOADLET_LIB_VAR}_FOUND "YES")
			set (TOADLET_LIBRARIES_S ${TOADLET_LIBRARIES_S} ${${TOADLET_LIB_VAR}_LIB_S})
			set (TOADLET_LIBRARIES_SD ${TOADLET_LIBRARIES_SD} ${${TOADLET_LIB_VAR}_LIB_SD})
		endif (${TOADLET_LIB_VAR}_LIB_S AND ${TOADLET_LIB_VAR}_LIB_SD)

		# If only release libraries are found, assign them to the debug libraries
		if (${TOADLET_LIB_VAR}_LIB AND NOT ${TOADLET_LIB_VAR}_LIB_D)
			set (${TOADLET_LIB_VAR}_LIB_D ${${TOADLET_LIB_VAR}_LIB} CACHE FILEPATH "Path to a library" FORCE)
			set (${TOADLET_LIB_VAR}_FOUND "YES")
			set (TOADLET_LIBRARIES ${TOADLET_LIBRARIES} ${${TOADLET_LIB_VAR}_LIB})
			set (TOADLET_LIBRARIES_D ${TOADLET_LIBRARIES_D} ${${TOADLET_LIB_VAR}_LIB_D})
		endif (${TOADLET_LIB_VAR}_LIB AND NOT ${TOADLET_LIB_VAR}_LIB_D)
		if (${TOADLET_LIB_VAR}_LIB_S AND NOT ${TOADLET_LIB_VAR}_LIB_SD)
			set (${TOADLET_LIB_VAR}_LIB_SD ${${TOADLET_LIB_VAR}_LIB_S} CACHE FILEPATH "Path to a library" FORCE)
			set (${TOADLET_LIB_VAR}_FOUND "YES")
			set (TOADLET_LIBRARIES_S ${TOADLET_LIBRARIES_S} ${${TOADLET_LIB_VAR}_LIB_S})
			set (TOADLET_LIBRARIES_SD ${TOADLET_LIBRARIES_SD} ${${TOADLET_LIB_VAR}_LIB_SD})
		endif (${TOADLET_LIB_VAR}_LIB_S AND NOT ${TOADLET_LIB_VAR}_LIB_SD)
		
		# If only debug libraries are found, assign them to the release libraries
		if (${TOADLET_LIB_VAR}_LIB_D AND NOT ${TOADLET_LIB_VAR}_LIB)
			set (${TOADLET_LIB_VAR}_LIB ${${TOADLET_LIB_VAR}_LIB_D} CACHE FILEPATH "Path to a library" FORCE)
			set (${TOADLET_LIB_VAR}_FOUND "YES")
			set (TOADLET_LIBRARIES ${TOADLET_LIBRARIES} ${${TOADLET_LIB_VAR}_LIB})
			set (TOADLET_LIBRARIES_D ${TOADLET_LIBRARIES_D} ${${TOADLET_LIB_VAR}_LIB_D})
		endif (${TOADLET_LIB_VAR}_LIB_D AND NOT ${TOADLET_LIB_VAR}_LIB)
		if (${TOADLET_LIB_VAR}_LIB_SD AND NOT ${TOADLET_LIB_VAR}_LIB_S)
			set (${TOADLET_LIB_VAR}_LIB_S ${${TOADLET_LIB_VAR}_LIB_SD} CACHE FILEPATH "Path to a library" FORCE)
			set (${TOADLET_LIB_VAR}_FOUND "YES")
			set (TOADLET_LIBRARIES_S ${TOADLET_LIBRARIES_S} ${${TOADLET_LIB_VAR}_LIB_S})
			set (TOADLET_LIBRARIES_SD ${TOADLET_LIBRARIES_SD} ${${TOADLET_LIB_VAR}_LIB_SD})
		endif (${TOADLET_LIB_VAR}_LIB_SD AND NOT ${TOADLET_LIB_VAR}_LIB_S)
		
		set (${TOADLET_LIB_VAR}_FOUND ${${TOADLET_LIB_VAR}_FOUND} CACHE BOOL "Library ${TOADLET_LIB} found flag" FORCE)
		mark_as_advanced (${TOADLET_LIB_VAR}_LIB ${TOADLET_LIB_VAR}_LIB_D ${TOADLET_LIB_VAR}_LIB_S ${TOADLET_LIB_VAR}_LIB_SD ${TOADLET_LIB_VAR}_FOUND)
	endif (NOT ${TOADLET_LIB_VAR}_FOUND)
endforeach (TOADLET_LIB)

# Cache the toadlet libraries values
set (TOADLET_LIBRARIES ${TOADLET_LIBRARIES} CACHE FILEPATH "Toadlet libraries" FORCE)
set (TOADLET_LIBRARIES_S ${TOADLET_LIBRARIES_S} CACHE FILEPATH "Toadlet libraries (static)" FORCE)
set (TOADLET_LIBRARIES_D ${TOADLET_LIBRARIES_D} CACHE FILEPATH "Toadlet libraries (debug)" FORCE)
set (TOADLET_LIBRARIES_SD ${TOADLET_LIBRARIES_SD} CACHE FILEPATH "Toadlet libraries (static debug)" FORCE)

# Consider toadlet found if we have the header file and a library path
if (TOADLET_INCLUDE_DIR AND TOADLET_LIBRARY_DIR)
	set (TOADLET_FOUND "YES")
endif (TOADLET_INCLUDE_DIR AND TOADLET_LIBRARY_DIR)

# Standard settings
if (TOADLET_FOUND)
	if (NOT TOADLET_FIND_QUITELY)
		message (STATUS "toadet found")
	endif (NOT TOADLET_FIND_QUITELY)
else (TOADLET_FOUND)
	if (TOADLET_FIND_REQUIRED)
		message (FATAL_ERROR "Could NOT find toadlet")
	endif (TOADLET_FIND_REQUIRED)
	if (NOT TOADLET_FIND_QUITELY)
		message (STATUS "Could NOT find toadlet")
	endif (NOT TOADLET_FIND_QUITELY)
endif (TOADLET_FOUND)

