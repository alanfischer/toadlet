set (FFMPEG_FOUND "NO")

find_path (FFMPEG_INCLUDE_DIR libavcodec/avcodec.h)
find_library (AVCODEC_LIBRARY NAMES avcodec)
find_library (AVFORMAT_LIBRARY NAMES avformat)
find_library (AVUTIL_LIBRARY NAMES avutil)
find_library (SWSCALE_LIBRARY NAMES swscale)

mark_as_advanced (FFMPEG_INCLUDE_DIR AVCODEC_LIBRARY)

message (STATUS ${FFMPEG_INCLUDE_DIR})
message (STATUS ${FFMPEG_LIBRARY})

if (FFMPEG_INCLUDE_DIR AND FFMPEG_LIBRARY)
	set (FFMPEG_FOUND "YES")
endif (FFMPEG_INCLUDE_DIR AND FFMPEG_LIBRARY)

if (FFMPEG_FOUND)
	if (NOT FFMPEG_FIND_QUITELY)
		message (STATUS "FFMPEG found")
	endif (NOT FFMPEG_FIND_QUITELY)
else (FFMPEG_FOUND)
	if (FFMPEG_FIND_REQUIRED)
		message (FATAL_ERROR "Could NOT find FFMPEG")
	endif (FFMPEG_FIND_REQUIRED)
	if (NOT FFMPEG_FIND_QUITELY)
		message (STATUS "Could NOT find FFMPEG")
	endif (NOT FFMPEG_FIND_QUITELY)
endif (FFMPEG_FOUND)

