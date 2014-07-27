# Attempt to find toadlet
# Defines:
#
#  TOADLET_FOUND	- system has toadlet
#  TOADLET_INCLUDE_DIR	- path to the toadlet include directory
#  TOADLET_LIBRARY_DIR	- path to the toadlet library installation dir
#  TOADLET_LIBRARIES_MIXED		- sum of all toadlet dynamic libraries in a special optimized/debug separated fashion
#  TOADLET_LIBRARIES_S_MIXED	- sum of all toadlet static libraries in a special optimized/debug separated fashion
#  TOADLET_LIBRARIES	- sum of all toadlet dynamic release libraries
#  TOADLET_LIBRARIES_D	- sum of all toadlet dynamic debug libraries 
#  TOADLET_LIBRARIES_S	- sum of all toadlet static release libraries 
#  TOADLET_LIBRARIES_SD	- sum of all toadlet static debug 
#  TOADLET_DLLS		- WIN32 platforms only - sum of all toadlet release dlls 
#  TOADLET_DLLS_D	- WIN32 platforms only - sum of all toadlet debug dlls
#
# Define each toadlet library individually.
# Each define has 4 versions: dynamic release (no suffix), static release (_S), dynamic debug (_D), static debug (_SD)
# Each library, when at least one of the version is found, sets TOADLET_${LIBNAME}_FOUND = YES
#  TOADLET_EGG_LIB
#  TOADLET_FLICK_LIB
#  TOADLET_HOP_LIB
#  TOADLET_PEEPER_LIB
#  TOADLET_RIBBIT_LIB
#  TOADLET_TADPOLE_LIB
#  TOADLET_TADPOLE_HOP_LIB
#  TOADLET_PAD_LIB
# On Windows platforms, the corresponding DLL's will also be located for each library, including plugins. Debug versions will have (_D).
#  TOADLET_EGG_DLL
#  TOADLET_FLICK_DLL
#  TOADLET_HOP_DLL
#  TOADLET_PEEPER_DLL
#  TOADLET_RIBBIT_DLL
#  TOADLET_TADPOLE_DLL
#  TOADLET_PAD_DLL
# Android builds 
#  JTOADLET_EGG_JAR
#  JTOADLET_FLICK_JAR
#  JTOADLET_PEEPER_JAR
#  JTOADLET_RIBBIT_JAR
#  JTOADLET_TADPOLE_JAR
#  JTOADLET_PAD_JAR
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
		"C:/Program Files/toadlet/include"
	)
	set (LIBRARY_SEARCH_PATHS
		"C:/Program Files/toadlet/lib"
	)
else (WIN32)
	set (HEADER_SEARCH_PATHS
		/usr/include
		/usr/local/include
		${TOADLET_DIR}/include
	)
	set (LIBRARY_SEARCH_PATHS
		/usr/lib 
		/usr/local/lib
		${TOADLET_DIR}/lib
	)
endif (WIN32)

# Find the toadlet header file
find_path (TOADLET_INCLUDE_DIR toadlet/toadlet.h PATHS ${HEADER_SEARCH_PATHS})

# If no global header was found, we may have a sub-library installation.
# Check backwards through the list.
if (NOT TOADLET_INCLUDE_DIR)
		foreach (TOADLET_HEADER pad tadpole ribbit peeper hop flick egg)
			find_path (TOADLET_INCLUDE_DIR toadlet/${TOADLET_HEADER}.h PATHS ${HEADER_SEARCH_PATHS})
			if (TOADLET_INCLUDE_DIR)
				message (STATUS "toadlet sub library installation found for ${TOADLET_HEADER}")
				break()
			endif (TOADLET_INCLUDE_DIR)
		endforeach (TOADLET_HEADER)
endif (NOT TOADLET_INCLUDE_DIR)

# List of toadlet library basenames
# NOTE: These are listed in the order acceptable for static linking
if (EMSCRIPTEN)
	set (TOADLET_LIB_BASENAMES
		toadlet_pad
		toadlet_tadpole
		toadlet_ribbit_alaudiodevice
		toadlet_ribbit
		toadlet_peeper_gles2renderdevice
		toadlet_peeper
		toadlet_hop
		toadlet_flick
		toadlet_egg
	)
else (EMSCRIPTEN)
	set (TOADLET_LIB_BASENAMES
		toadlet_pad
		toadlet_tadpole
		toadlet_ribbit_jaudiodevice
		toadlet_ribbit_mmaudiodevice
		toadlet_ribbit_alaudiodevice
		toadlet_ribbit
		toadlet_peeper_d3dmrenderdevice
		toadlet_peeper_d3d9renderdevice
		toadlet_peeper_d3d10renderdevice
		toadlet_peeper_d3d11renderdevice
		toadlet_peeper_gles1renderdevice
		toadlet_peeper_gles2renderdevice
		toadlet_peeper_glrenderdevice
		toadlet_peeper
		toadlet_hop
		toadlet_flick_androidsensordevice
		toadlet_flick_ioslineardevice
		toadlet_flick_jinputdevice
		toadlet_flick_mfvideodevice
		toadlet_flick_win32joydevice
		toadlet_flick
		toadlet_egg
	)
endif (EMSCRIPTEN)

# Android specifics
if (ANDROID) 
	set (LIBRARY_SEARCH_SUFFIXES ${NDK_NAME_ARCH})

	set (TOADLET_JAR_BASENAMES 
		jtoadlet_egg
		jtoadlet_flick
		jtoadlet_peeper
		jtoadlet_ribbit
		jtoadlet_tadpole
		jtoadlet_pad
	)

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
endif (ANDROID)

# Search for and assign the toadlet libraries
foreach (TOADLET_LIB ${TOADLET_LIB_BASENAMES})
	string (TOUPPER ${TOADLET_LIB} TOADLET_LIB_VAR)

	# Only search for a library if we haven't already
	# Otherwise the debug/release assignment settings below will cause trouble with multiple runs
	if (NOT ${TOADLET_LIB_VAR}_FOUND)
		# Find dynamic release, dynamic debug, static release and then static debug
		if (EMSCRIPTEN)
			set (${TOADLET_LIB_VAR}_LIB "${CMAKE_PREFIX_PATH}/lib/lib${TOADLET_LIB}.a")
			set (${TOADLET_LIB_VAR}_LIB_D "${CMAKE_PREFIX_PATH}/lib/lib${TOADLET_LIB}_d.a")
			set (${TOADLET_LIB_VAR}_LIB_S "${CMAKE_PREFIX_PATH}/lib/lib${TOADLET_LIB}_s.a")
			set (${TOADLET_LIB_VAR}_LIB_SD "${CMAKE_PREFIX_PATH}/lib/lib${TOADLET_LIB}_sd.a")
		else (EMSCRIPTEN)
			find_library (${TOADLET_LIB_VAR}_LIB NAMES ${TOADLET_LIB} PATHS ${LIBRARY_SEARCH_PATHS} PATH_SUFFIXES ${LIBRARY_SEARCH_SUFFIXES})
			find_library (${TOADLET_LIB_VAR}_LIB_D NAMES ${TOADLET_LIB}_d PATHS ${LIBRARY_SEARCH_PATHS} PATH_SUFFIXES ${LIBRARY_SEARCH_SUFFIXES})
			find_library (${TOADLET_LIB_VAR}_LIB_S NAMES ${TOADLET_LIB}_s PATHS ${LIBRARY_SEARCH_PATHS} PATH_SUFFIXES ${LIBRARY_SEARCH_SUFFIXES})
			find_library (${TOADLET_LIB_VAR}_LIB_SD NAMES ${TOADLET_LIB}_sd PATHS ${LIBRARY_SEARCH_PATHS} PATH_SUFFIXES ${LIBRARY_SEARCH_SUFFIXES})
		endif (EMSCRIPTEN)
			
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

		# If both versions are found, set both library types
		if (${TOADLET_LIB_VAR}_LIB AND ${TOADLET_LIB_VAR}_LIB_D)
			set (${TOADLET_LIB_VAR}_LIB ${${TOADLET_LIB_VAR}_LIB} CACHE FILEPATH "Path to dynamic release library" FORCE)
			set (${TOADLET_LIB_VAR}_LIB_D ${${TOADLET_LIB_VAR}_LIB_D} CACHE FILEPATH "Path to dynamic debug library" FORCE)
			set (${TOADLET_LIB_VAR}_FOUND "YES")
			set (TOADLET_LIBRARIES ${TOADLET_LIBRARIES} ${${TOADLET_LIB_VAR}_LIB})
			set (TOADLET_LIBRARIES_D ${TOADLET_LIBRARIES_D} ${${TOADLET_LIB_VAR}_LIB_D})
			set (TOADLET_LIBRARIES_MIXED ${TOADLET_LIBRARIES_MIXED} optimized ${${TOADLET_LIB_VAR}_LIB} debug ${${TOADLET_LIB_VAR}_LIB_D})
		endif (${TOADLET_LIB_VAR}_LIB AND ${TOADLET_LIB_VAR}_LIB_D)
		if (${TOADLET_LIB_VAR}_LIB_S AND ${TOADLET_LIB_VAR}_LIB_SD)
			set (${TOADLET_LIB_VAR}_LIB_S ${${TOADLET_LIB_VAR}_LIB_S} CACHE FILEPATH "Path to static release library" FORCE)
			set (${TOADLET_LIB_VAR}_LIB_SD ${${TOADLET_LIB_VAR}_LIB_SD} CACHE FILEPATH "Path to static debug library" FORCE)
			set (${TOADLET_LIB_VAR}_FOUND "YES")
			set (TOADLET_LIBRARIES_S ${TOADLET_LIBRARIES_S} ${${TOADLET_LIB_VAR}_LIB_S})
			set (TOADLET_LIBRARIES_SD ${TOADLET_LIBRARIES_SD} ${${TOADLET_LIB_VAR}_LIB_SD})
			set (TOADLET_LIBRARIES_S_MIXED ${TOADLET_LIBRARIES_S_MIXED} optimized ${${TOADLET_LIB_VAR}_LIB_S} debug ${${TOADLET_LIB_VAR}_LIB_SD})
		endif (${TOADLET_LIB_VAR}_LIB_S AND ${TOADLET_LIB_VAR}_LIB_SD)

		# If only release libraries are found, assign them to the debug libraries
		if (${TOADLET_LIB_VAR}_LIB AND NOT ${TOADLET_LIB_VAR}_LIB_D)
			set (${TOADLET_LIB_VAR}_LIB ${${TOADLET_LIB_VAR}_LIB} CACHE FILEPATH "Path to dynamic release library" FORCE)
			set (${TOADLET_LIB_VAR}_LIB_D ${${TOADLET_LIB_VAR}_LIB} CACHE FILEPATH "Path to dynamic release (no debug found) library" FORCE)
			set (${TOADLET_LIB_VAR}_FOUND "YES")
			set (TOADLET_LIBRARIES ${TOADLET_LIBRARIES} ${${TOADLET_LIB_VAR}_LIB})
			set (TOADLET_LIBRARIES_D ${TOADLET_LIBRARIES_D} ${${TOADLET_LIB_VAR}_LIB_D})
			set (TOADLET_LIBRARIES_MIXED ${TOADLET_LIBRARIES_MIXED} optimized ${${TOADLET_LIB_VAR}_LIB} debug ${${TOADLET_LIB_VAR}_LIB_D})
		endif (${TOADLET_LIB_VAR}_LIB AND NOT ${TOADLET_LIB_VAR}_LIB_D)
		if (${TOADLET_LIB_VAR}_LIB_S AND NOT ${TOADLET_LIB_VAR}_LIB_SD)
			set (${TOADLET_LIB_VAR}_LIB_S ${${TOADLET_LIB_VAR}_LIB_S} CACHE FILEPATH "Path to static release library" FORCE)
			set (${TOADLET_LIB_VAR}_LIB_SD ${${TOADLET_LIB_VAR}_LIB_S} CACHE FILEPATH "Path to static release (no debug found) library" FORCE)
			set (${TOADLET_LIB_VAR}_FOUND "YES")
			set (TOADLET_LIBRARIES_S ${TOADLET_LIBRARIES_S} ${${TOADLET_LIB_VAR}_LIB_S})
			set (TOADLET_LIBRARIES_SD ${TOADLET_LIBRARIES_SD} ${${TOADLET_LIB_VAR}_LIB_SD})
			set (TOADLET_LIBRARIES_S_MIXED ${TOADLET_LIBRARIES_S_MIXED} optimized ${${TOADLET_LIB_VAR}_LIB_S} debug ${${TOADLET_LIB_VAR}_LIB_SD})
		endif (${TOADLET_LIB_VAR}_LIB_S AND NOT ${TOADLET_LIB_VAR}_LIB_SD)
		
		# If only debug libraries are found, assign them to the release libraries
		if (${TOADLET_LIB_VAR}_LIB_D AND NOT ${TOADLET_LIB_VAR}_LIB)
			set (${TOADLET_LIB_VAR}_LIB_D ${${TOADLET_LIB_VAR}_LIB_D} CACHE FILEPATH "Path to dynamic debug library" FORCE)
			set (${TOADLET_LIB_VAR}_LIB ${${TOADLET_LIB_VAR}_LIB_D} CACHE FILEPATH "Path to dynamic debug (no release found) library" FORCE)
			set (${TOADLET_LIB_VAR}_FOUND "YES")
			set (TOADLET_LIBRARIES ${TOADLET_LIBRARIES} ${${TOADLET_LIB_VAR}_LIB})
			set (TOADLET_LIBRARIES_D ${TOADLET_LIBRARIES_D} ${${TOADLET_LIB_VAR}_LIB_D})
			set (TOADLET_LIBRARIES_MIXED ${TOADLET_LIBRARIES_MIXED} optimized ${${TOADLET_LIB_VAR}_LIB} debug ${${TOADLET_LIB_VAR}_LIB_D})
		endif (${TOADLET_LIB_VAR}_LIB_D AND NOT ${TOADLET_LIB_VAR}_LIB)
		if (${TOADLET_LIB_VAR}_LIB_SD AND NOT ${TOADLET_LIB_VAR}_LIB_S)
			set (${TOADLET_LIB_VAR}_LIB_SD ${${TOADLET_LIB_VAR}_LIB_SD} CACHE FILEPATH "Path to dynamic debug library" FORCE)
			set (${TOADLET_LIB_VAR}_LIB_S ${${TOADLET_LIB_VAR}_LIB_SD} CACHE FILEPATH "Path to dynamic debug (no release found) library" FORCE)
			set (${TOADLET_LIB_VAR}_FOUND "YES")
			set (TOADLET_LIBRARIES_S ${TOADLET_LIBRARIES_S} ${${TOADLET_LIB_VAR}_LIB_S})
			set (TOADLET_LIBRARIES_SD ${TOADLET_LIBRARIES_SD} ${${TOADLET_LIB_VAR}_LIB_SD})
			set (TOADLET_LIBRARIES_S_MIXED ${TOADLET_LIBRARIES_S_MIXED} optimized ${${TOADLET_LIB_VAR}_LIB_S} debug ${${TOADLET_LIB_VAR}_LIB_SD})
		endif (${TOADLET_LIB_VAR}_LIB_SD AND NOT ${TOADLET_LIB_VAR}_LIB_S)
		
		if (WIN32)
			# Setup the find_library suffixes to find dlls
			set (CMAKE_FIND_LIBRARY_SUFFIXES_DEFAULT ${CMAKE_FIND_LIBRARY_SUFFIXES})
			set (CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_SHARED_MODULE_SUFFIX} ${CMAKE_SHARED_LIBRARY_SUFFIX})

			# Look for release and debug dlls
			find_library (${TOADLET_LIB_VAR}_DLL NAMES ${TOADLET_LIB} PATHS ${LIBRARY_SEARCH_PATHS} PATH_SUFFIXES ${LIBRARY_SEARCH_SUFFIXES})
			find_library (${TOADLET_LIB_VAR}_DLL_D NAMES ${TOADLET_LIB}_d PATHS ${LIBRARY_SEARCH_PATHS} PATH_SUFFIXES ${LIBRARY_SEARCH_SUFFIXES})
			
			# If both versions are found, set each to their own
			if (${TOADLET_LIB_VAR}_LIB AND ${TOADLET_LIB_VAR}_LIB_D)
				set (${TOADLET_LIB_VAR}_DLL ${${TOADLET_LIB_VAR}_DLL} CACHE FILEPATH "Path to release dll" FORCE)
				set (${TOADLET_LIB_VAR}_DLL_D ${${TOADLET_LIB_VAR}_DLL_D} CACHE FILEPATH "Path to debug dll" FORCE)
				set (TOADLET_DLLS ${TOADLET_DLLS} ${${TOADLET_LIB_VAR}_DLL})
				set (TOADLET_DLLS_D ${TOADLET_DLLS_D} ${${TOADLET_LIB_VAR}_DLL_D})
			endif (${TOADLET_LIB_VAR}_LIB AND ${TOADLET_LIB_VAR}_LIB_D)
			
			# If only release dlls are found, assign them to the debug dlls
			if (${TOADLET_LIB_VAR}_DLL AND NOT ${TOADLET_LIB_VAR}_DLL_D)
				set (${TOADLET_LIB_VAR}_DLL ${${TOADLET_LIB_VAR}_DLL} CACHE FILEPATH "Path to release dll" FORCE)
				set (${TOADLET_LIB_VAR}_DLL_D ${${TOADLET_LIB_VAR}_DLL} CACHE FILEPATH "Path to release (no debug found) dll" FORCE)
				set (TOADLET_DLLS ${TOADLET_DLLS} ${${TOADLET_LIB_VAR}_DLL})
				set (TOADLET_DLLS_D ${TOADLET_DLLS_D} ${${TOADLET_LIB_VAR}_DLL_D})
			endif (${TOADLET_LIB_VAR}_DLL AND NOT ${TOADLET_LIB_VAR}_DLL_D)
			
			# If only debug dlls are found, assign them to the release dlls
			if (${TOADLET_LIB_VAR}_DLL_D AND NOT ${TOADLET_LIB_VAR}_DLL)
				set (${TOADLET_LIB_VAR}_DLL_D ${${TOADLET_LIB_VAR}_DLL_D} CACHE FILEPATH "Path to debug dll" FORCE)
				set (${TOADLET_LIB_VAR}_DLL ${${TOADLET_LIB_VAR}_DLL_D} CACHE FILEPATH "Path to debug (no release found) dll" FORCE)
				set (TOADLET_DLLS ${TOADLET_DLLS} ${${TOADLET_LIB_VAR}_DLL})
				set (TOADLET_DLLS_D ${TOADLET_DLLS_D} ${${TOADLET_LIB_VAR}_DLL_D})
			endif (${TOADLET_LIB_VAR}_DLL_D AND NOT ${TOADLET_LIB_VAR}_DLL)

			# Restore the standard find_library suffixes
			set (CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES_DEFAULT})
			
			mark_as_advanced (${TOADLET_LIB_VAR}_DLL ${TOADLET_LIB_VAR}_DLL_D)
		endif (WIN32)
		
		set (${TOADLET_LIB_VAR}_FOUND ${${TOADLET_LIB_VAR}_FOUND} CACHE BOOL "Library ${TOADLET_LIB} found flag" FORCE)
		mark_as_advanced (${TOADLET_LIB_VAR}_LIB ${TOADLET_LIB_VAR}_LIB_D ${TOADLET_LIB_VAR}_LIB_S ${TOADLET_LIB_VAR}_LIB_SD ${TOADLET_LIB_VAR}_FOUND)
	endif (NOT ${TOADLET_LIB_VAR}_FOUND)
endforeach (TOADLET_LIB)

# Cache the toadlet libraries values
set (TOADLET_LIBRARIES_MIXED ${TOADLET_LIBRARIES_MIXED} CACHE FILEPATH "Toadlet libraries (dynamic mixed release/debug)" FORCE)
set (TOADLET_LIBRARIES_S_MIXED ${TOADLET_LIBRARIES_S_MIXED} CACHE FILEPATH "Toadlet libraries (static mixed release/debug)" FORCE)
set (TOADLET_LIBRARIES ${TOADLET_LIBRARIES} CACHE FILEPATH "Toadlet libraries (dynamic release)" FORCE)
set (TOADLET_LIBRARIES_S ${TOADLET_LIBRARIES_S} CACHE FILEPATH "Toadlet libraries (static release)" FORCE)
set (TOADLET_LIBRARIES_D ${TOADLET_LIBRARIES_D} CACHE FILEPATH "Toadlet libraries (dynamic debug)" FORCE)
set (TOADLET_LIBRARIES_SD ${TOADLET_LIBRARIES_SD} CACHE FILEPATH "Toadlet libraries (static debug)" FORCE)
if (WIN32)
	set (TOADLET_DLLS ${TOADLET_DLLS} CACHE FILEPATH "Toadlet dlls (release)" FORCE)
	set (TOADLET_DLLS_D ${TOADLET_DLLS_D} CACHE FILEPATH "Toadlet dlls (debug)" FORCE)
endif (WIN32)

# Consider toadlet found if we have the header file and a library path
if (TOADLET_INCLUDE_DIR AND TOADLET_LIBRARY_DIR)
	set (TOADLET_FOUND "YES")
endif (TOADLET_INCLUDE_DIR AND TOADLET_LIBRARY_DIR)

# Standard settings
if (TOADLET_FOUND)
	if (NOT TOADLET_FIND_QUITELY)
		message (STATUS "toadlet found")
	endif (NOT TOADLET_FIND_QUITELY)
else (TOADLET_FOUND)
	if (TOADLET_FIND_REQUIRED)
		message (FATAL_ERROR "Could NOT find toadlet")
	endif (TOADLET_FIND_REQUIRED)
	if (NOT TOADLET_FIND_QUITELY)
		message (STATUS "Could NOT find toadlet")
	endif (NOT TOADLET_FIND_QUITELY)
endif (TOADLET_FOUND)

