# Attempt to find the zzip header
# Defines:
#
#  ZZIP_FOUND			- system has zzip
#  ZZIP_INCLUDE_PATH	- path to the zzip header

set (ZZIP_FOUND "NO")

find_path (ZZIP_INCLUDE_DIR NAMES zzip.h zzip/zzip.h)
find_library (ZZIP_LIBRARY NAMES zzip zziplib)

if (ZZIP_INCLUDE_DIR AND ZZIP_LIBRARY)
	set(ZZIP_FOUND TRUE)
endif (ZZIP_INCLUDE_DIR AND ZZIP_LIBRARY)

if (ZZIP_FOUND)
	if (NOT ZZIP_FIND_QUITELY)
		message (STATUS "zzip found")
	endif (NOT ZZIP_FIND_QUITELY)
else (ZZIP_FOUND)
	if (ZZIP_FIND_QUITELY)
		message (FATAL_ERROR "Could NOT find zzip")
	endif (ZZIP_FIND_QUITELY)
	if (NOT ZZIP_FIND_QUITELY)
		message (STATUS "Could NOT find zzip")
	endif (NOT ZZIP_FIND_QUITELY)
endif (ZZIP_FOUND)

