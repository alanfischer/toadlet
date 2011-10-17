# This macro creates a true static library bundle
# TARGET - the output library, or target, that you wish to contain all of the object files
# LIBRARIES - a list of all of the static libraries you want merged into the TARGET

macro (TOADLET_MERGE_STATIC_LIBRARIES TARGET LIBRARIES)
	if (TOADLET_PLATFORM_WIN32)
		# To use STATIC_LIBRARY_FLAGS you mush directly format the LIBRARIES variable as a single string for the windows libtool
		string (REPLACE ";" " " LIBS "${LIBRARIES}")
		string (REPLACE "/" "\\" LIBS "${LIBRARIES}")
		set_property (TARGET ${TARGET} APPEND PROPERTY STATIC_LIBRARY_FLAGS ${LIBS})
	elseif (TOADLET_PLATFORM_OSX)
		# OSX platforms work just as the cmake documentation would suggest
		foreach (LIB ${LIBRARIES})
			set (PROPS ${PROPS} STATIC_LIBRARY_FLAGS ${LIB}) 
		endforeach (LIB)
		set_target_properties (${TARGET} PROPERTIES ${PROPS})
	elseif (TOADLET_PLATFORM_POSIX)
		# Use a configured script that unarchives the target and all libraries and remerges them into a single library
		#get_target_property (DIR ${TARGET} LOCATION)
		configure_file (PosixMergeStaticLibraries.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/PosixMergeStaticLibraries${TARGET}.cmake @ONLY)
		add_custom_command (TARGET ${TARGET} POST_BUILD
			#COMMAND rm -f ${DIR}
			COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/PosixMergeStaticLibraries${TARGET}.cmake
		)
	else
		message (FATAL_ERROR "Toadlet platform undefined. Toadlet will not be built")
	endif (TOADLET_PLATFORM_WIN32)
endmacro (TOADLET_MERGE_STATIC_LIBRARIES)
