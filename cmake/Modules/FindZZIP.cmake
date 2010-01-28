# Attempt to find the zzip header
# Defines:
#
#  ZZIP_FOUND			- system has zzip
#  ZZIP_INCLUDE_PATH	- path to the zzip header

set (ZZIP_FOUND "NO")

if (WIN32)
	if (WINCE)
		# Add default search paths for each supported winmo SDK
		find_path (ZZIP_INCLUDE_DIR zzip.h PATHS
			"C:/Program Files/Windows Mobile 6 SDK/PocketPC/Include/Armv4i"
			"C:/Program Files/Microsoft Visual Studio 8/SmartDevices/SDK/PocketPC2003/Include"
		)
		find_library (ZZIP_LIBRARY NAMES zzip PATHS
			"C:/Program Files/Windows Mobile 6 SDK/PocketPC/Lib/Armv4i"
			"C:/Program Files/Microsoft Visual Studio 8/SmartDevices/SDK/PocketPC2003/Lib/armv4"
		)
	else (WINCE)
		find_path (ZZIP_INCLUDE_DIR zzip.h
			"C:/Program Files/Microsoft Visual Studio 8/VC/include"
		)
		find_library (ZZIP_LIBRARY NAMES zzip PATHS
			"C:/Program Files/Microsoft Visual Studio 8/VC/lib"
		)
	endif (WINCE)
else (WIN32)
	find_path (ZZIP_INCLUDE_DIR zzip.h PATHS
		/usr/include
		/usr/local/include
	)
	find_library (ZZIP_LIBRARY NAMES zzip PATHS 
		/usr/lib 
		/usr/local/lib
	)
endif (WIN32)


if (ZZIP_FOUND)
	if (NOT ZZIP_FIND_QUITELY)
		message (STATUS "zzip.h found at ${ZZIP_INCLUDE_PATH}")
	endif (NOT GDIPLUS_FIND_QUITELY)
else (ZZIP_FOUND)
	if (ZZIP_FIND_QUITELY)
		message (FATAL_ERROR "Could NOT find zzip.h")
	endif (ZZIP_FIND_QUITELY)
	if (NOT ZZIP_FIND_QUITELY)
		message (STATUS "Could NOT find zzip.h")
	endif (NOT ZZIP_FIND_QUITELY)
endif (ZZIP_FOUND)

