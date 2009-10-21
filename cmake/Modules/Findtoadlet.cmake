# Attempt to find toadlet
# Defines:
#
#  TOADLET_FOUND	- system has toadlet
#  TOADLET_INCLUDE_DIR	- path to the toadlet include directory
#  TOADLET_LIBRARY_DIR	- path to the toadlet library installation dir
#
# Also defines each toadlet library individually, both dynamic and static (_S) versions. Release chosen first, then debug:
#  TOADLET_EGG_LIB
#  TOADLET_FLICK_LIB
#  TOADLET_FLICK_HTCMOTIONDETECTOR_LIB
#  TOADLET_FLICK_IPHONEMOTIONDETECTOR_LIB
#  TOADLET_FLICK_SAMSUNGMOTIONDETECTOR_LIB
#  TOADLET_HOP_LIB
#  TOADLET_KNOT_LIB
#  TOADLET_PEEPER_LIB
#  TOADLET_PEEPER_GLRENDERER_LIB
#  TOADLET_PEEPER_D3D9RENDERER_LIB
#  TOADLET_RIBBIT_LIB
#  TOADLET_RIBBIT_ALPLAYER_LIB
#  TOADLET_RIBBIT_AUDIEREPLAYER_LIB
#  TOADLET_RIBBIT_WIN32PLAYER_LIB
#  TOADLET_TADPOLE_LIB
#  TOADLET_TADPOLE_HOP_LIB
#  TOADLET_PAD_LIB

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

# Headers
find_path (TOADLET_INCLUDE_DIR toadlet/Config.h PATHS ${HEADER_SEARCH_PATHS})

# Dynamic Libraries
find_library (TOADLET_EGG_LIB NAMES toadlet_egg toadlet_egg_d PATHS ${LIBRARY_SEARCH_PATHS})
find_library (TOADLET_FLICK_LIB NAMES toadlet_flick toadlet_flick_d PATHS ${LIBRARY_SEARCH_PATHS})
find_library (TOADLET_FLICK_HTCMOTIONDETECTOR_LIB NAMES toadlet_flick_htcmotiondetector toadlet_flick_htcmotiondetector_d PATHS ${LIBRARY_SEARCH_PATHS})
find_library (TOADLET_FLICK_IPHONEMOTIONDETECTOR_LIB NAMES toadlet_flick_iphonemotiondetector toadlet_flick_iphonemotiondetector_d PATHS ${LIBRARY_SEARCH_PATHS})
find_library (TOADLET_FLICK_SAMSUNGMOTIONDETECTOR_LIB NAMES toadlet_flick_samsungmotiondetector toadlet_flick_samsungmotiondetector_d PATHS ${LIBRARY_SEARCH_PATHS})
find_library (TOADLET_HOP_LIB NAMES toadlet_hop toadlet_hop_d PATHS ${LIBRARY_SEARCH_PATHS})
find_library (TOADLET_KNOT_LIB NAMES toadlet_knot toadlet_knot_d PATHS ${LIBRARY_SEARCH_PATHS})
find_library (TOADLET_PEEPER_LIB NAMES toadlet_peeper toadlet_peeper_d PATHS ${LIBRARY_SEARCH_PATHS})
find_library (TOADLET_PEEPER_GLRENDERER_LIB NAMES toadlet_peeper_glrenderer toadlet_peeper_glrenderer_d PATHS ${LIBRARY_SEARCH_PATHS})
find_library (TOADLET_PEEPER_D3D9RENDERER_LIB NAMES toadlet_peeper_d3d9renderer toadlet_peeper_d3d9renderer_d PATHS ${LIBRARY_SEARCH_PATHS})
find_library (TOADLET_RIBBIT_LIB NAMES toadlet_ribbit toadlet_ribbit_d PATHS ${LIBRARY_SEARCH_PATHS})
find_library (TOADLET_RIBBIT_ALPLAYER_LIB NAMES toadlet_ribbit_alplayer toadlet_ribbit_alplayer_d PATHS ${LIBRARY_SEARCH_PATHS})
find_library (TOADLET_RIBBIT_AUDIEREPLAYER_LIB NAMES toadlet_ribbit_audiereplayer toadlet_ribbit_audiereplayer_d PATHS ${LIBRARY_SEARCH_PATHS})
find_library (TOADLET_RIBBIT_WIN32PLAYER_LIB NAMES toadlet_ribbit_win32player toadlet_ribbit_win32player_d PATHS ${LIBRARY_SEARCH_PATHS})
find_library (TOADLET_TADPOLE_LIB NAMES toadlet_tadpole toadlet_tadpole_d PATHS ${LIBRARY_SEARCH_PATHS})
find_library (TOADLET_TADPOLE_HOP_LIB NAMES toadlet_tadpole_hop toadlet_tadpole_hop_d PATHS ${LIBRARY_SEARCH_PATHS})
find_library (TOADLET_PAD_LIB NAMES toadlet_pad toadlet_pad_d PATHS ${LIBRARY_SEARCH_PATHS})

# Static Libraries
find_library (TOADLET_EGG_LIB_S NAMES toadlet_egg_s toadlet_egg_sd PATHS ${LIBRARY_SEARCH_PATHS})
find_library (TOADLET_FLICK_LIB_S NAMES toadlet_flick_s toadlet_flick_sd PATHS ${LIBRARY_SEARCH_PATHS})
find_library (TOADLET_FLICK_HTCMOTIONDETECTOR_LIB_S NAMES toadlet_flick_htcmotiondetector_s toadlet_flick_htcmotiondetector_sd PATHS ${LIBRARY_SEARCH_PATHS})
find_library (TOADLET_FLICK_IPHONEMOTIONDETECTOR_LIB_S NAMES toadlet_flick_iphonemotiondetector_s toadlet_flick_iphonemotiondetector_sd PATHS ${LIBRARY_SEARCH_PATHS})
find_library (TOADLET_FLICK_SAMSUNGMOTIONDETECTOR_LIB_S NAMES toadlet_flick_samsungmotiondetector_s toadlet_flick_samsungmotiondetector_sd PATHS ${LIBRARY_SEARCH_PATHS})
find_library (TOADLET_HOP_LIB_S NAMES toadlet_hop_s toadlet_hop_sd PATHS ${LIBRARY_SEARCH_PATHS})
find_library (TOADLET_KNOT_LIB_S NAMES toadlet_knot_s toadlet_knot_sd PATHS ${LIBRARY_SEARCH_PATHS})
find_library (TOADLET_PEEPER_LIB_S NAMES toadlet_peeper_s toadlet_peeper_sd PATHS ${LIBRARY_SEARCH_PATHS})
find_library (TOADLET_PEEPER_GLRENDERER_LIB_S NAMES toadlet_peeper_glrenderer_s toadlet_peeper_glrenderer_sd PATHS ${LIBRARY_SEARCH_PATHS})
find_library (TOADLET_PEEPER_D3D9RENDERER_LIB_S NAMES toadlet_peeper_d3d9renderer_s toadlet_peeper_d3d9renderer_sd PATHS ${LIBRARY_SEARCH_PATHS})
find_library (TOADLET_RIBBIT_LIB_S NAMES toadlet_ribbit_s toadlet_ribbit_sd PATHS ${LIBRARY_SEARCH_PATHS})
find_library (TOADLET_RIBBIT_ALPLAYER_LIB_S NAMES toadlet_ribbit_alplayer_s toadlet_ribbit_alplayer_sd PATHS ${LIBRARY_SEARCH_PATHS})
find_library (TOADLET_RIBBIT_AUDIEREPLAYER_LIB_S NAMES toadlet_ribbit_audiereplayer_s toadlet_ribbit_audiereplayer_sd PATHS ${LIBRARY_SEARCH_PATHS})
find_library (TOADLET_RIBBIT_WIN32PLAYER_LIB_S NAMES toadlet_ribbit_win32player_s toadlet_ribbit_win32player_sd PATHS ${LIBRARY_SEARCH_PATHS})
find_library (TOADLET_TADPOLE_LIB_S NAMES toadlet_tadpole_s toadlet_tadpole_sd PATHS ${LIBRARY_SEARCH_PATHS})
find_library (TOADLET_TADPOLE_HOP_LIB_S NAMES toadlet_tadpole_hop_s toadlet_tadpole_hop_sd PATHS ${LIBRARY_SEARCH_PATHS})
find_library (TOADLET_PAD_LIB_S NAMES toadlet_pad_s toadlet_pad_sd PATHS ${LIBRARY_SEARCH_PATHS})

# Prevent appearing in regular variable list
mark_as_advanced (
	TOADLET_INCLUDE_DIR
	TOADLET_LIBRARY_DIR
	TOADLET_EGG_LIB 
	TOADLET_FLICK_LIB 
	TOADLET_FLICK_HTCMOTIONDETECTOR_LIB 
	TOADLET_FLICK_IPHONEMOTIONDETECTOR_LIB 
	TOADLET_FLICK_SAMSUNGMOTIONDETECTOR_LIB 
	TOADLET_HOP_LIB 
	TOADLET_KNOT_LIB 
	TOADLET_PEEPER_LIB 
	TOADLET_PEEPER_GLRENDERER_LIB 
	TOADLET_PEEPER_D3D9RENDERER_LIB 
	TOADLET_RIBBIT_LIB 
	TOADLET_RIBBIT_ALPLAYER_LIB 
	TOADLET_RIBBIT_AUDIEREPLAYER_LIB 
	TOADLET_RIBBIT_WIN32PLAYER_LIB 
	TOADLET_TADPOLE_LIB 
	TOADLET_TADPOLE_HOP_LIB 
	TOADLET_PAD_LIB 
	TOADLET_EGG_LIB_S 
	TOADLET_FLICK_LIB_S 
	TOADLET_FLICK_HTCMOTIONDETECTOR_LIB_S 
	TOADLET_FLICK_IPHONEMOTIONDETECTOR_LIB_S 
	TOADLET_FLICK_SAMSUNGMOTIONDETECTOR_LIB_S 
	TOADLET_HOP_LIB_S 
	TOADLET_KNOT_LIB_S 
	TOADLET_PEEPER_LIB_S 
	TOADLET_PEEPER_GLRENDERER_LIB_S 
	TOADLET_PEEPER_D3D9RENDERER_LIB_S 
	TOADLET_RIBBIT_LIB_S 
	TOADLET_RIBBIT_ALPLAYER_LIB_S 
	TOADLET_RIBBIT_AUDIEREPLAYER_LIB_S 
	TOADLET_RIBBIT_WIN32PLAYER_LIB_S 
	TOADLET_TADPOLE_LIB_S 
	TOADLET_TADPOLE_HOP_LIB_S 
	TOADLET_PAD_LIB_S 
)

if (TOADLET_INCLUDE_DIR)
	if (TOADLET_EGG_LIB)
		get_filename_component (TOADLET_LIBRARY_DIR ${TOADLET_EGG_LIB} PATH CACHE)
		set (TOADLET_FOUND "YES")
	elseif (TOADLET_EGG_LIB_S)
		get_filename_component (TOADLET_LIBRARY_DIR ${TOADLET_EGG_LIB_S} PATH CACHE)
		set (TOADLET_FOUND "YES")
	endif (TOADLET_EGG_LIB)
endif (TOADLET_INCLUDE_DIR)

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

