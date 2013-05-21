# Attempt to find the gdiplus/Imaging  header
# Defines:
#
#  GDIPLUS_FOUND		- system has gdiplus/Imaging
#  GDIPLUS_INCLUDE_PATH	- path to the gdiplus/Imaging header
#  GDIPLUS_LIBRARY		- path to the gdiplus library

set (GDIPLUS_FOUND "NO")

# Win32 Only
if (WIN32)
	if (WINCE)
		# Add default search paths for each supported winmo SDK
		find_path (GDIPLUS_INCLUDE_PATH GdiplusPixelFormats.h PATHS
			"C:/Program Files/Windows Mobile 6 SDK/PocketPC/Include/Armv4i"
			NO_DEFAULT_PATH
		)
		find_library (GDIPLUS_LIBRARY NAMES gdiplus)
	else (WINCE)
		find_path (GDIPLUS_INCLUDE_PATH GdiplusPixelFormats.h PATHS
			"C:/Program Files/Microsoft Visual Studio 8/VC/include"
			"C:/Program Files (x86)/Microsoft SDKs/Windows/v7.0A/Include"
		)
		find_library (GDIPLUS_LIBRARY NAMES gdiplus)
	endif (WINCE)

	if (GDIPLUS_INCLUDE_PATH AND GDIPLUS_LIBRARY)
		set (GDIPLUS_FOUND "YES")
	endif (GDIPLUS_INCLUDE_PATH AND GDIPLUS_LIBRARY)

	mark_as_advanced (GDIPLUS_INCLUDE_PATH GDIPLUS_LIBRARY)
endif (WIN32)

if (GDIPLUS_FOUND)
	if (NOT GDIPLUS_FIND_QUITELY)
		message (STATUS "Gdiplus Found")
	endif (NOT GDIPLUS_FIND_QUITELY)
else (GDIPLUS_FOUND)
	if (GDIPLUS_FIND_QUITELY)
		message (FATAL_ERROR "Could NOT find Gdiplus")
	endif (GDIPLUS_FIND_QUITELY)
	if (NOT GDIPLUS_FIND_QUITELY)
		message (STATUS "Could NOT find Gdiplus")
	endif (NOT GDIPLUS_FIND_QUITELY)
endif (GDIPLUS_FOUND)

