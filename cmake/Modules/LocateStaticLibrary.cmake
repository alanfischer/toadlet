# This macro looks for static library versions of a target
# TARGET - the output library, or target, that you wish to contain all of the object files
# LIBRARIES - a list of all of the static libraries you want merged into the TARGET

macro (LOCATE_STATIC_LIBRARY LIBRARY STATIC)
	unset (${STATIC})

	if (TOADLET_PLATFORM_WIN32)
		# On windows we can only assume a static version exists
		set (${STATIC} ${LIBRARY})
	else (TOADLET_PLATFORM_WIN32)
		if (TOADLET_PLATFORM_OSX)
			# Both iOS and OSX use the dylib extension
			set (DYNEXT ".dylib")
		else (TOADLET_PLATFORM_OSX)
			# Everyone else uses .so
			set (DYNEXT ".so")
		endif (TOADLET_PLATFORM_OSX)

		get_filename_component (SUFFIX ${LIBRARY} EXT)
                if (${SUFFIX} STREQUAL ".a")
                        # Library is already static
                        set (${STATIC} ${LIBRARY})
                elseif (${SUFFIX} STREQUAL ${DYNEXT})
                        string (REPLACE ${DYNEXT} ".a" LIB_S ${LIBRARY})
                        if (EXISTS ${LIB_S})
                                set (${STATIC} ${LIB_S})
                        else (EXISTS ${LIB_S})
				# Do a last ditch search for a static library
				find_file (LIB_S_FOUND ${LIB_S})
				if (LIB_S_FOUND)
					set (${STATIC} ${LIB_S_FOUND})
				else (LIB_S_FOUND)
					message (STATUS "WARNING: A static build of ${LIBRARY} could not be found")
				endif (LIB_S_FOUND)
                        endif (EXISTS ${LIB_S})
                endif (${SUFFIX} STREQUAL ".a")
	endif (TOADLET_PLATFORM_WIN32)
endmacro (LOCATE_STATIC_LIBRARY)
