# This macro creates a true static library bundle
# TARGET - the output library, or target, that you wish to contain all of the object files
# LIBRARIES - a list of all of the static libraries you want merged into the TARGET

macro (MERGE_STATIC_LIBRARIES TARGET LIBRARIES)
	if (TOADLET_PLATFORM_WIN32)
		# On Windows you must add aditional formatting to the LIBRARIES variable as a single string for the windows libtool
		string (REPLACE ";" " " LIBS "${LIBRARIES}")
		string (REPLACE "/" "\\" LIBS "${LIBS}")
		set_property (TARGET ${TARGET} APPEND PROPERTY STATIC_LIBRARY_FLAGS "${LIBS}")
	elseif (TOADLET_PLATFORM_OSX)
		# iOS and OSX platforms need slighly less formatting
		string (REPLACE ";" " " LIBS "${LIBRARIES}")
		set_property (TARGET ${TARGET} APPEND PROPERTY STATIC_LIBRARY_FLAGS "${LIBS}")
	elseif (TOADLET_PLATFORM_POSIX)
		# Posix platforms, including Android, require manual merging of static libraries via a special script
		set (LIBRARIES ${LIBRARIES})

		if (NOT CMAKE_BUILD_TYPE)
			message (FATAL_ERROR "To use the MergeStaticLibraries script on Posix systems, you MUST define your CMAKE_BUILD_TYPE")
		endif (NOT CMAKE_BUILD_TYPE)

		# We need the debug postfix on posix systems for the merge script
		string (TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
		if (${BUILD_TYPE} STREQUAL "DEBUG")
			get_target_property (TARGETLOC ${TARGET} LOCATION_DEBUG)
		else (${BUILD_TYPE} STREQUAL "DEBUG")
			get_target_property (TARGETLOC ${TARGET} LOCATION)
		endif (${BUILD_TYPE} STREQUAL "DEBUG")

		# Setup the static library merge script
		configure_file (
			${PROJECT_SOURCE_DIR}/cmake/Modules/PosixMergeStaticLibraries.cmake.in 
			${CMAKE_CURRENT_BINARY_DIR}/PosixMergeStaticLibraries-${TARGET}.cmake @ONLY
		)
		add_custom_command (TARGET ${TARGET} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/PosixMergeStaticLibraries-${TARGET}.cmake
		)
	endif (TOADLET_PLATFORM_WIN32)
endmacro (MERGE_STATIC_LIBRARIES)
