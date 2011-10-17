# This macro creates a true static library bundle
# TARGET - the output library, or target, that you wish to contain all of the object files
# LIBRARIES - a list of all of the static libraries you want merged into the TARGET

macro (TOADLET_MERGE_STATIC_LIBRARIES TARGET LIBRARIES)
	if (TOADLET_PLATFORM_ANDROID)
		# Android platforms are like Posix, so 
		configure_file (PosixMergeStaticLibraries.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/PosixMergeStaticLibraries${TARGET}.cmake @ONLY)
		add_custom_command (TARGET ${TARGET} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/PosixMergeStaticLibraries${TARGET}.cmake
		)
	elseif (TOADLET_PLATFORM_WIN32)
		# To use STATIC_LIBRARY_FLAGS on Windows you must directly format the LIBRARIES variable as a single string for the windows libtool
		string (REPLACE ";" " " LIBS "${LIBRARIES}")
		string (REPLACE "/" "\\" LIBS "${LIBS}")
		set_property (TARGET ${TARGET} APPEND PROPERTY STATIC_LIBRARY_FLAGS "${LIBS}")
	elseif (TOADLET_PLATFORM_OSX)
		# OSX platforms work just as the cmake documentation would suggest
		foreach (LIB ${LIBRARIES})
			set (PROPS ${PROPS} STATIC_LIBRARY_FLAGS ${LIB})
		endforeach (LIB)
		set_target_properties (${TARGET} PROPERTIES ${PROPS})
	elseif (TOADLET_PLATFORM_POSIX)
		# On Posix use a configured script to merge all objects into a single archive
		configure_file (PosixMergeStaticLibraries.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/PosixMergeStaticLibraries${TARGET}.cmake @ONLY)
		add_custom_command (TARGET ${TARGET} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/PosixMergeStaticLibraries${TARGET}.cmake
		)
	else (TOADLET_PLATFORM_ANDROID)
		message (FATAL_ERROR "Toadlet platform undefined. Toadlet will not be built")
	endif (TOADLET_PLATFORM_ANDROID)
endmacro (TOADLET_MERGE_STATIC_LIBRARIES)
