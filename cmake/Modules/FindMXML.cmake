# Attempt to find the MiniXML library
# Defines:
#
#  MXML_FOUND		- system has minixml
#  MXML_INCLUDE_DIR	- path to the mxml.h header
#  MXML_LIBRARY		- path to the mxml library

set (MXML_FOUND "NO")

if (ANDROID)
	find_path (MXML_INCLUDE_DIR mxml.h)
	find_library (MXML_LIBRARY NAMES mxml PATH_SUFFIXES ${NDK_NAME_ARCH})
elseif (WIN32)
	if (WINCE)
		# Add default search paths for each supported winmo SDK
		find_path (MXML_INCLUDE_DIR mxml.h PATHS
			"C:/Program Files/Windows Mobile 6 SDK/PocketPC/Include/Armv4i"
			"C:/Program Files/Microsoft Visual Studio 8/SmartDevices/SDK/PocketPC2003/Include"
		)
		find_library (MXML_LIBRARY NAMES mxml mxml1 PATHS
			"C:/Program Files/Windows Mobile 6 SDK/PocketPC/Lib/Armv4i"
			"C:/Program Files/Microsoft Visual Studio 8/SmartDevices/SDK/PocketPC2003/Lib/armv4"
		)
	else (WINCE)
		find_path (MXML_INCLUDE_DIR mxml.h
			"C:/Program Files/Microsoft Visual Studio 8/VC/include"
		)
		find_library (MXML_LIBRARY NAMES mxml mxml1 PATHS
			"C:/Program Files/Microsoft Visual Studio 8/VC/lib"
		)
	endif (WINCE)
else (ANDROID)
	find_path (MXML_INCLUDE_DIR mxml.h PATHS
		/usr/include
		/usr/local/include
	)
	find_library (MXML_LIBRARY NAMES mxml PATHS 
		/usr/lib 
		/usr/local/lib
	)
endif (ANDROID)

mark_as_advanced (MXML_INCLUDE_DIR MXML_LIBRARY)

set (MXML_INCLUDE_DIR ${MXML_INCLUDE_DIR} CACHE PATH "mxml include folder" FORCE)
set (MXML_LIBRARY ${MXML_LIBRARY} CACHE FILEPATH "mxml library" FORCE)

if (MXML_INCLUDE_DIR AND MXML_LIBRARY)
	set (MXML_FOUND "YES")
endif (MXML_INCLUDE_DIR AND MXML_LIBRARY)

if (MXML_FOUND)
	if (NOT MXML_FIND_QUITELY)
		message (STATUS "MiniXML found")
	endif (NOT MXML_FIND_QUITELY)
else (MXML_FOUND)
	if (MXML_FIND_REQUIRED)
		message (FATAL_ERROR "Could NOT find MiniXML")
	endif (MXML_FIND_REQUIRED)
	if (NOT MXML_FIND_QUITELY)
		message (STATUS "Could NOT find MiniXML")
	endif (NOT MXML_FIND_QUITELY)
endif (MXML_FOUND)

