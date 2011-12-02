# This macro looks for static library versions of a list of libraries
# STATIC - the output list of static libraries
# LIBRARIES - the input list of libraries you would like as static
#
# Example usage:
#    LOCATE_STATIC_LIBRARIES (MYLIBS_S "${MYLIBS}")
#
# NOTE: When you call this script, make sure you quote the argument to LIBRARIES if it is a list!

macro (LOCATE_STATIC_LIBRARIES STATIC LIBRARIES)
	unset (${STATIC})

	if (TOADLET_PLATFORM_WIN32)
		# On windows we can only assume a static version exists and let the linker sort it out
		set (${STATIC} ${LIBRARIES})
	else (TOADLET_PLATFORM_WIN32)
		if (TOADLET_PLATFORM_OSX)
			# Both iOS and OSX use the dylib extension
			set (DYNEXT ".dylib")
		else (TOADLET_PLATFORM_OSX)
			# Everyone else uses .so
			set (DYNEXT ".so")
		endif (TOADLET_PLATFORM_OSX)

		foreach (LIBRARY ${LIBRARIES})
			get_filename_component (SUFFIX ${LIBRARY} EXT)
			if (${SUFFIX} STREQUAL ".a")
					# Library is already static
					set (${STATIC} ${${STATIC}} ${LIBRARY})
			elseif (${SUFFIX} STREQUAL ${DYNEXT})
					string (REPLACE ${DYNEXT} ".a" LIB_S ${LIBRARY})
					if (EXISTS ${LIB_S})
							set (${STATIC} ${${STATIC}} ${LIB_S})
					else (EXISTS ${LIB_S})
			# Do a last ditch search for a static library
			find_file (LIB_S_FOUND ${LIB_S})
			if (LIB_S_FOUND)
				set (${STATIC} ${${STATIC}} ${LIB_S_FOUND})
			else (LIB_S_FOUND)
				message (STATUS "WARNING: A static build of ${LIBRARY} could not be found. ${STATIC} will be incomplete.")
			endif (LIB_S_FOUND)
					endif (EXISTS ${LIB_S})
			endif (${SUFFIX} STREQUAL ".a")
		endforeach (LIBRARY ${LIBRARIES})
	endif (TOADLET_PLATFORM_WIN32)
endmacro (LOCATE_STATIC_LIBRARIES)