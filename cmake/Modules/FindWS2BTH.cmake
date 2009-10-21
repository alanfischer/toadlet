# Attempt to find the WS2BTH header
# Defines:
#
#  WS2BTH_FOUND		- system has ws2bth.h
#  WS2BTH_INCLUDE_PATH	- path to the ws2bth.h header

set (WS2BTH_FOUND "NO")

if (WIN32)
	if (WINCE)
		# Add default search paths for each supported winmo SDK
		find_path (WS2BTH_INCLUDE_PATH ws2bth.h
			"C:/Program Files/Windows Mobile 6 SDK/PocketPC/Include/Armv4i"
			DOC "Path to the ws2bth.h file"
		)
	else (WINCE)
		find_path (WS2BTH_INCLUDE_PATH ws2bth.h
			"C:/Program Files/Microsoft Visual Studio 8/VC/include"
			DOC "Path to the ws2bth.h file"
		)
	endif (WINCE)

	if (WS2BTH_INCLUDE_PATH)
		set (WS2BTH_FOUND "YES")
	endif (WS2BTH_INCLUDE_PATH)

	mark_as_advanced (WS2BTH_INCLUDE_PATH)
endif (WIN32)

if (WS2BTH_FOUND)
	if (NOT WS2BTH_FIND_QUITELY)
		message (STATUS "ws2bth.h found at ${WS2BTH_INCLUDE_PATH}")
	endif (NOT WS2BTH_FIND_QUITELY)
else (WS2BTH_FOUND)
	if (WS2BTH_FIND_REQUIRED)
		message (FATAL_ERROR "Could NOT find ws2bth.h")
	endif (WS2BTH_FIND_REQUIRED)
	if (NOT WS2BTH_FIND_QUITELY)
		message (STATUS "Could NOT find ws2bth.h")
	endif (NOT WS2BTH_FIND_QUITELY)
endif (WS2BTH_FOUND)

