# Attempt to find the Sidplay/2 library
# Defines:
#
#  SIDPLAY_FOUND		- system has minixml
#  SIDPLAY_INCLUDE_DIR	- path to the sidplay headers
#  SIDPLAY_LIBRARY		- path to the sidplay library
#  SIDPLAY_VERSION		- Sidplay version (1 or 2)
#  RESID_LIBRARY		- path to the resid library
#  HARDSID_LIBRARY		- path to the hardsid library

set (SIDPLAY_FOUND "NO")

mark_as_advanced (SIDPLAY_INCLUDE_DIR SIDPLAY_LIBRARY)

find_library (HARDSID_LIBRARY NAMES hardsid_builder)
find_library (RESID_LIBRARY NAMES resid_builder)

# We need to at least have the resid builder
if (RESID_LIBRARY)
	# Look for sidplay 2
	find_path (SIDPLAY_INCLUDE_DIR sidplay/sidplay2.h)
	find_library (SIDPLAY_LIBRARY NAMES sidplay2)
	if (SIDPLAY_INCLUDE_DIR AND SIDPLAY_LIBRARY)
		set (SIDPLAY_FOUND "YES")
		set (SIDPLAY_VERSION 2)
	else (SIDPLAY_INCLUDE_DIR AND SIDPLAY_LIBRARY)
		# Otherwise look for sidplay 1
		find_path (SIDPLAY_INCLUDE_DIR sidplay/player.h)
		find_library (SIDPLAY_LIBRARY NAMES sidplay)
		if (SIDPLAY_INCLUDE_DIR AND SIDPLAY_LIBRARY)
			set (SIDPLAY_FOUND "YES")
			set (SIDPLAY_VERSION 1)
		endif (SIDPLAY_INCLUDE_DIR AND SIDPLAY_LIBRARY)
	endif (SIDPLAY_INCLUDE_DIR AND SIDPLAY_LIBRARY)
endif (RESID_LIBRARY)

if (SIDPLAY_FOUND)
	if (NOT SIDPLAY_FIND_QUITELY)
		message (STATUS "Sidplay found version ${SIDPLAY_VERSION}")
	endif (NOT SIDPLAY_FIND_QUITELY)
else (SIDPLAY_FOUND)
	if (SIDPLAY_FIND_REQUIRED)
		message (FATAL_ERROR "Could NOT find Sidplay")
	endif (SIDPLAY_FIND_REQUIRED)
	if (NOT SIDPLAY_FIND_QUITELY)
		message (STATUS "Could NOT find Sidplay")
	endif (NOT SIDPLAY_FIND_QUITELY)
endif (SIDPLAY_FOUND)
