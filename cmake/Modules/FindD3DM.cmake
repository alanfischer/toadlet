# Attempt to find the D3DM libraries
# Defines:
#
#  D3DM_FOUND		- system has D3DM
#  D3DM_INCLUDE_PATH	- path to the D3DM headers

set (D3DM_FOUND "NO")

if (WIN32)
	# Add default search paths for each supported winmo SDK
	find_path (D3DM_INCLUDE_PATH d3dm.h
		"C:/Program Files/Windows Mobile 6 SDK/PocketPC/Include/Armv4i"
		DOC "Path to the d3dm.h file"
	)

	if (D3DM_INCLUDE_PATH)
		set (D3DM_FOUND "YES")
	endif (D3DM_INCLUDE_PATH)

	mark_as_advanced (D3DM_INCLUDE_PATH)
endif (WIN32)

if (D3DM_FOUND)
	if (NOT D3DM_FIND_QUITELY)
		message (STATUS "Direct3D Mobile found at ${D3DM_INCLUDE_PATH}")
	endif (NOT D3DM_FIND_QUITELY)
else (D3DM_FOUND)
	if (D3DM_FIND_REQUIRED)
		message (FATAL_ERROR "Could NOT find Direct3D Mobile")
	endif (D3DM_FIND_REQUIRED)
	if (NOT D3DM_FIND_QUITELY)
		message (STATUS "Could NOT find Direct3D Mobile")
	endif (NOT D3DM_FIND_QUITELY)
endif (D3DM_FOUND)

