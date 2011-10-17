# This macro looks for static library versions of a target
# TARGET - the output library, or target, that you wish to contain all of the object files
# LIBRARIES - a list of all of the static libraries you want merged into the TARGET

macro (LOCATE_STATIC_LIBRARY LIBRARY STATIC)
	unset (${STATIC})

	if (TOADLET_PLATFORM_ANDROID)
		get_filename_component (SUFFIX ${LIBRARY} EXT)
		if (${SUFFIX} STREQUAL ".a")
			# Library is already static
			set (${STATIC} ${LIBRARY})
		elseif (${SUFFIX} STREQUAL ".so")
			string (REPLACE ".so" ".a" LIB_S ${LIBRARY})
			if (EXISTS ${LIB_S})
				set (${STATIC} ${LIB_S})
			endif (EXISTS ${LIB_S})
		endif (${SUFFIX} STREQUAL ".a")
	elseif (TOADLET_PLATFORM_WIN32)
		# On windows we can only assume a static version exists
		set (${STATIC} ${LIBRARY})
	elseif (TOADLET_PLATFORM_OSX)
		get_filename_component (SUFFIX ${LIBRARY} EXT)
		if (${SUFFIX} STREQUAL ".a")
			# Library is already static
			set (${STATIC} ${LIBRARY})
		elseif (${SUFFIX} STREQUAL ".dylib")
			string (REPLACE ".dylib" ".a" LIB_S ${LIBRARY})
			if (EXISTS ${LIB_S})
				set (${STATIC} ${LIB_S})
			endif (EXISTS ${LIB_S})
		endif (${SUFFIX} STREQUAL ".a")
	elseif (TOADLET_PLATFORM_POSIX)
		get_filename_component (SUFFIX ${LIBRARY} EXT)
		if (${SUFFIX} STREQUAL ".a")
			# Library is already static
			set (${STATIC} ${LIBRARY})
		elseif (${SUFFIX} STREQUAL ".so")
			string (REPLACE ".so" ".a" LIB_S ${LIBRARY})
			if (EXISTS ${LIB_S})
				set (${STATIC} ${LIB_S})
			endif (EXISTS ${LIB_S})
		endif (${SUFFIX} STREQUAL ".a")
	endif (TOADLET_PLATFORM_ANDROID)
endmacro (LOCATE_STATIC_LIBRARY)
