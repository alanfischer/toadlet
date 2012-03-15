set (FFMPEG_FOUND "NO")
set (FFMPEG_LIBRARIES "")

if (WIN32)
	set (HDR_SEARCH_PATHS
        "C:/Program Files/ffmpeg/include"
	)
	set (LIB_SEARCH_PATHS
        "C:/Program Files/ffmpeg/lib"
	)
	set (EXE_SEARCH_PATHS
        "C:/Program Files/ffmpeg/bin"
	)
else (WIN32)
	set (HDR_SEARCH_PATHS
        /usr/include
        /usr/local/include
	)
	set (LIB_SEARCH_PATHS
        /usr/lib
        /usr/local/lib
	)
	set (EXE_SEARCH_PATHS
        /usr/bin
        /usr/local/bin
	)
endif (WIN32)

find_program (FFMPEG_EXE NAMES ffmpeg ffmpeg.exe PATHS ${EXE_SEARCH_PATHS})
find_path (FFMPEG_INCLUDE_DIR libavcodec/avcodec.h PATHS ${HDR_SEARCH_PATHS})

# NOTE: These are listed in the order acceptable for static linking
set (FFMPEG_LIBS
	avdevice
	avfilter
	avformat
	avcodec
	postproc
	swresample
	swscale
	avutil
)

foreach (FFMPEG_LIB ${FFMPEG_LIBS})
	string (TOUPPER ${FFMPEG_LIB} FFMPEG_LIB_VAR)

	if (NOT ${FFMPEG_LIB_VAR}_FOUND)
		find_library (${FFMPEG_LIB_VAR}_LIBRARY NAMES ${FFMPEG_LIB} PATHS ${LIB_SEARCH_PATHS})
		if (${FFMPEG_LIB_VAR}_LIBRARY)
			set (FFMPEG_LIBRARIES ${FFMPEG_LIBRARIES} ${${FFMPEG_LIB_VAR}_LIBRARY})
		
			if (WIN32)
				# On windows we try to use the 'lib' tool to locate the appropriate ffmpeg dlls
				find_program (LIBEXE lib)
				if (EXISTS ${LIBEXE})
					# Use the lib.exe tool to dump the .lib contents to a file and search for "libraryname-number.dll" in that file
					# If found, set DLL_NAME to that specific dll
					get_filename_component (LIBNAME ${${FFMPEG_LIB_VAR}_LIBRARY} NAME_WE)
					execute_process (
						COMMAND ${LIBEXE} /list ${${FFMPEG_LIB_VAR}_LIBRARY} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} OUTPUT_FILE libout-${LIBNAME}
					)
					file (READ ${CMAKE_CURRENT_BINARY_DIR}/libout-${LIBNAME} LIBOUT)
					string (REGEX MATCH "${LIBNAME}(-[0-9]*)?\\.dll" DLL_NAME ${LIBOUT})
					file (REMOVE ${CMAKE_CURRENT_BINARY_DIR}/libout-${LIBNAME})
				endif (EXISTS ${LIBEXE})
				if (NOT DLL_NAME)
					set (DLL_NAME ${FFMPEG_LIB})
				endif (NOT DLL_NAME)				
				
				# Setup the find_library suffixes to find dlls
				set (CMAKE_FIND_LIBRARY_SUFFIXES_DEFAULT ${CMAKE_FIND_LIBRARY_SUFFIXES})
				set (CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_SHARED_MODULE_SUFFIX} ${CMAKE_SHARED_LIBRARY_SUFFIX})
				
				# DLL's are often in executable locations so use the find_program command
				find_program (${FFMPEG_LIB_VAR}_DLL NAMES ${DLL_NAME} PATHS ${LIB_SEARCH_PATHS} ENV PATH)
				if (${FFMPEG_LIB_VAR}_DLL)
					set (FFMPEG_DLLS ${FFMPEG_DLLS} ${${FFMPEG_LIB_VAR}_DLL})
				endif (${FFMPEG_LIB_VAR}_DLL)
				mark_as_advanced (${FFMPEG_LIB_VAR}_DLL)
				
				# Restore the standard find_library suffixes
				set (CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES_DEFAULT})
			endif (WIN32)			
		endif (${FFMPEG_LIB_VAR}_LIBRARY)
		
		mark_as_advanced (${FFMPEG_LIB_VAR}_LIBRARY)
	endif (NOT ${FFMPEG_LIB_VAR}_FOUND)
endforeach (FFMPEG_LIB)

mark_as_advanced (FFMPEG_EXE FFMPEG_INCLUDE_DIR FFMPEG_LIBRARIES FFMPEG_DLLS)

set (FFMPEG_EXE ${FFMPEG_EXE} CACHE FILEPATH "FFmpeg executable" FORCE)
set (FFMPEG_INCLUDE_DIR ${FFMPEG_INCLUDE_DIR} CACHE PATH "FFmpeg include folder" FORCE)
set (FFMPEG_LIBRARIES ${FFMPEG_LIBRARIES} CACHE FILEPATH "All FFmpeg related libraries" FORCE)
if (WIN32)
	set (FFMPEG_DLLS ${FFMPEG_DLLS} CACHE FILEPATH "All FFmpeg related dlls" FORCE)
endif (WIN32)

if (FFMPEG_EXE AND FFMPEG_INCLUDE_DIR AND FFMPEG_LIBRARIES)
	set (FFMPEG_FOUND "YES")
endif (FFMPEG_EXE AND FFMPEG_INCLUDE_DIR AND FFMPEG_LIBRARIES)

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

