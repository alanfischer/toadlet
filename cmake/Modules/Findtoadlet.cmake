# Attempt to find toadlet
# Defines:
#
#  TOADLET_FOUND	- system has toadlet
#  TOADLET_INCLUDE_DIR	- path to the toadlet include directory
#  TOADLET_LIBRARY_DIR	- path to the toadlet library installation dir
#
# Define each toadlet library individually, both dynamic (no suffix) and static (_S) versions, and release (no suffix) and debug (_D):
#  TOADLET_EGG_LIB
#  TOADLET_FLICK_LIB
#  TOADLET_FLICK_HTCMOTIONDEVICE_LIB
#  TOADLET_FLICK_IOSMOTIONDEVICE_LIB
#  TOADLET_FLICK_SAMSUNGMOTIONDEVICE_LIB
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
#  TOADLET_TADPOLE_LIB
#  TOADLET_TADPOLE_HOP_LIB
#  TOADLET_PAD_LIB
# 
# Caveats:
# The generator will be queried for the build configurations (debug or optimized) and the proper library types assigned.
# If release libraries are not found, they will be set to the debug libraries.
# If debug libraries are not found, they will be set to the release libraries.
# Static and dynamic libraries, however, are totally separate.

set (TOADLET_FOUND "NO")

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
	toadlet_flick_htcmotiondevice
	toadlet_flick_iosmotiondevice
	toadlet_flick_samsungmotiondevice
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
	toadlet_tadpole
	toadlet_tadpole_hop
	toadlet_pad
)

# Search for and assign the toadlet libraries
foreach (TOADLET_LIB ${TOADLET_LIB_BASENAMES})
	# Capitalize the library variable name
	string (TOUPPER ${TOADLET_LIB} TOADLET_LIB_VAR)

	# Find dynamic release, dynamic debug, static release and then static debug
	find_library (${TOADLET_LIB_VAR}_LIB NAMES ${TOADLET_LIB} PATHS ${LIBRARY_SEARCH_PATHS})
	find_library (${TOADLET_LIB_VAR}_LIB_D NAMES ${TOADLET_LIB}_d PATHS ${LIBRARY_SEARCH_PATHS})
	find_library (${TOADLET_LIB_VAR}_LIB_S NAMES ${TOADLET_LIB}_s PATHS ${LIBRARY_SEARCH_PATHS})
	find_library (${TOADLET_LIB_VAR}_LIB_SD NAMES ${TOADLET_LIB}_sd PATHS ${LIBRARY_SEARCH_PATHS})

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
	endif (${TOADLET_LIB_VAR}_LIB AND ${TOADLET_LIB_VAR}_LIB_D)
	if (${TOADLET_LIB_VAR}_LIB_S AND ${TOADLET_LIB_VAR}_LIB_SD)
		set (${TOADLET_LIB_VAR}_LIB_S optimized ${${TOADLET_LIB_VAR}_LIB_S} debug ${${TOADLET_LIB_VAR}_LIB_SD} CACHE STRING "Both libraries" FORCE)
	endif (${TOADLET_LIB_VAR}_LIB_S AND ${TOADLET_LIB_VAR}_LIB_SD)

	# If only release libraries are found, assign them to the debug libraries
	if (${TOADLET_LIB_VAR}_LIB AND NOT ${TOADLET_LIB_VAR}_LIB_D)
		set (${TOADLET_LIB_VAR}_LIB_D ${${TOADLET_LIB_VAR}_LIB} CACHE FILEPATH "Path to a library" FORCE)
	endif (${TOADLET_LIB_VAR}_LIB AND NOT ${TOADLET_LIB_VAR}_LIB_D)
	if (${TOADLET_LIB_VAR}_LIB_S AND NOT ${TOADLET_LIB_VAR}_LIB_SD)
		set (${TOADLET_LIB_VAR}_LIB_SD ${${TOADLET_LIB_VAR}_LIB_S} CACHE FILEPATH "Path to a library" FORCE)
	endif (${TOADLET_LIB_VAR}_LIB_S AND NOT ${TOADLET_LIB_VAR}_LIB_SD)
	
	# If only debug libraries are found, assign them to the release libraries
	if (${TOADLET_LIB_VAR}_LIB_D AND NOT ${TOADLET_LIB_VAR}_LIB)
		set (${TOADLET_LIB_VAR}_LIB ${${TOADLET_LIB_VAR}_LIB_D} CACHE FILEPATH "Path to a library" FORCE)
	endif (${TOADLET_LIB_VAR}_LIB_D AND NOT ${TOADLET_LIB_VAR}_LIB)
	if (${TOADLET_LIB_VAR}_LIB_SD AND NOT ${TOADLET_LIB_VAR}_LIB_S)
		set (${TOADLET_LIB_VAR}_LIB_S ${${TOADLET_LIB_VAR}_LIB_SD} CACHE FILEPATH "Path to a library" FORCE)
	endif (${TOADLET_LIB_VAR}_LIB_SD AND NOT ${TOADLET_LIB_VAR}_LIB_S)

	mark_as_advanced (${TOADLET_LIB_VAR}_LIB ${TOADLET_LIB_VAR}_LIB_D ${TOADLET_LIB_VAR}_LIB_S ${TOADLET_LIB_VAR}_LIB_SD)
endforeach (TOADLET_LIB)

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

