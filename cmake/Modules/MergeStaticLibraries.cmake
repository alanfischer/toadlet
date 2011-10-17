# This macro creates a true static library bundle
# TARGET - the output library, or target, that you wish to contain all of the object files
# LIBRARIES - a list of all of the static libraries you want merged into the TARGET

macro (MERGE_STATIC_LIBRARIES TARGET LIBRARIES)
	if (TOADLET_PLATFORM_ANDROID)
		# Android platforms are like Posix, so merge all objects into a single archive
		set (LIBRARIES ${LIBRARIES})
		get_target_property (TARGETLOC ${TARGET} LOCATION)
		configure_file (
			${PROJECT_SOURCE_DIR}/cmake/Modules/PosixMergeStaticLibraries.cmake.in 
			${CMAKE_CURRENT_BINARY_DIR}/PosixMergeStaticLibraries${TARGET}.cmake @ONLY
		)
		add_custom_command (TARGET ${TARGET} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/PosixMergeStaticLibraries-${TARGET}.cmake
		)
	elseif (TOADLET_PLATFORM_WIN32)
		# On Windows you must directly format the LIBRARIES variable as a single string for the windows libtool
		string (REPLACE ";" " " LIBS "${LIBRARIES}")
		string (REPLACE "/" "\\" LIBS "${LIBS}")
		set_property (TARGET ${TARGET} APPEND PROPERTY STATIC_LIBRARY_FLAGS "${LIBS}")
	elseif (TOADLET_PLATFORM_OSX)
		# OSX is similar in that we need to clean up the library list for libtool
		string (REPLACE ";" " " LIBS "${LIBRARIES}")
		set_property (TARGET ${TARGET} APPEND PROPERTY STATIC_LIBRARY_FLAGS "${LIBS}")
	elseif (TOADLET_PLATFORM_POSIX)
		# On Posix use a configured script to merge all objects into a single archive
		set (LIBRARIES ${LIBRARIES})
		get_target_property (TARGETLOC ${TARGET} LOCATION)
		configure_file (
			${PROJECT_SOURCE_DIR}/cmake/Modules/PosixMergeStaticLibraries.cmake.in 
			${CMAKE_CURRENT_BINARY_DIR}/PosixMergeStaticLibraries-${TARGET}.cmake @ONLY
		)
		add_custom_command (TARGET ${TARGET} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/PosixMergeStaticLibraries-${TARGET}.cmake
		)
	endif (TOADLET_PLATFORM_ANDROID)
endmacro (MERGE_STATIC_LIBRARIES)
