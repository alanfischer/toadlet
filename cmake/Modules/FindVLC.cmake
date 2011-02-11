set (VLC_FOUND "NO")

find_path (VLC_INCLUDE_DIR vlc/vlc.h PATHS
	/usr/include
	/usr/local/include
	"C:/Program Files/VideoLAN/VLC/sdk/include"
)
find_library (VLC_LIBRARY NAMES vlc vlccore PATHS 
	/usr/lib 
	/usr/local/lib
	"C:/Program Files/VideoLAN/VLC/sdk/lib"
)

mark_as_advanced (VLC_INCLUDE_DIR VLC_LIBRARY)

message (STATUS ${VLC_INCLUDE_DIR})
message (STATUS ${VLC_LIBRARY})

if (VLC_INCLUDE_DIR AND VLC_LIBRARY)
	set (VLC_FOUND "YES")
endif (VLC_INCLUDE_DIR AND VLC_LIBRARY)

if (VLC_FOUND)
	if (NOT VLC_FIND_QUITELY)
		message (STATUS "VLC found")
	endif (NOT VLC_FIND_QUITELY)
else (VLC_FOUND)
	if (VLC_FIND_REQUIRED)
		message (FATAL_ERROR "Could NOT find VLC")
	endif (VLC_FIND_REQUIRED)
	if (NOT VLC_FIND_QUITELY)
		message (STATUS "Could NOT find VLC")
	endif (NOT VLC_FIND_QUITELY)
endif (VLC_FOUND)

