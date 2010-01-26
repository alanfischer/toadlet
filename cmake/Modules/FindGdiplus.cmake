# Attempt to find the gdiplus/Imaging  header
# Defines:
#
#  GDIPLUS_FOUND		- system has gdiplus/Imaging
#  GDIPLUS_INCLUDE_PATH	- path to the gdiplus/Imaging header

set (GDIPLUS_FOUND "NO")

# Win32 Only
if (WIN32)
	if (WINCE)
		# Add default search paths for each supported winmo SDK
		find_path (GDIPLUS_INCLUDE_PATH GdiplusPixelFormats.h PATHS
			"C:/Program Files/Windows Mobile 6 SDK/PocketPC/Include/Armv4i"
			NO_DEFAULT_PATH
		)
	else (WINCE)
		find_path (GDIPLUS_INCLUDE_PATH GdiplusPixelFormats.h PATHS
			"C:/Program Files/Microsoft Visual Studio 8/VC/include"
		)
	endif (WINCE)

	if (GDIPLUS_INCLUDE_PATH)
		set (GDIPLUS_FOUND "YES")
	endif (GDIPLUS_INCLUDE_PATH)

	mark_as_advanced (GDIPLUS_INCLUDE_PATH)
endif (WIN32)

if (GDIPLUS_FOUND)
	if (NOT GDIPLUS_FIND_QUITELY)
		message (STATUS "GdiplusPixelFormats.h found at ${GDIPLUS_INCLUDE_PATH}")
	endif (NOT GDIPLUS_FIND_QUITELY)
else (GDIPLUS_FOUND)
	if (GDIPLUS_FIND_QUITELY)
		message (FATAL_ERROR "Could NOT find GdiplusPixelFormats.h")
	endif (GDIPLUS_FIND_QUITELY)
	if (NOT GDIPLUS_FIND_QUITELY)
		message (STATUS "Could NOT find Imaging.h")
	endif (NOT GDIPLUS_FIND_QUITELY)
endif (GDIPLUS_FOUND)

