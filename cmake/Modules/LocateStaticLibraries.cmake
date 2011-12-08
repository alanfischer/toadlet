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
		# On windows we use the 'lib' tool to try and determine if we have found a fully static library
		find_program (LIBEXE lib)

		if (EXISTS ${LIBEXE})
			foreach (LIBRARY ${LIBRARIES})
				# If a .dll was passed in, try the .lib extension
				get_filename_component (SUFFIX ${LIBRARY} EXT)
				if (${SUFFIX} STREQUAL ".dll")
					string (REPLACE ".dll" ".lib" LIBRARY ${LIBRARY})
				endif (${SUFFIX} STREQUAL ".dll")

				if (EXISTS ${LIBRARY})
					# Use the lib.exe tool to dump the .lib contents to a file and search for the string ".dll" in that file
					get_filename_component (LIBNAME ${LIBRARY} NAME_WE)
					execute_process (
						COMMAND ${LIBEXE} /list ${LIBRARY} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} OUTPUT_FILE libout-${LIBNAME}
					)
					file (READ ${CMAKE_CURRENT_BINARY_DIR}/libout-${LIBNAME} LIBOUT)
					string (FIND ${LIBOUT} ".dll" HASDLL)

					# If a path to a .dll is present, this is an import library
					if (${HASDLL} GREATER -1)
						message (STATUS "WARNING: ${LIBRARY} is an IMPORT library. The corresponding .dll(s) will be required.")
					endif (${HASDLL} GREATER -1)
					set (${STATIC} ${${STATIC}} ${LIBRARY})
				else (EXISTS ${LIBRARY})
					message (STATUS "WARNING: A static build of ${LIBRARY} could not be found. ${STATIC} will be incomplete.")
				endif (EXISTS ${LIBRARY})
			endforeach (LIBRARY ${LIBRARIES})
		else (EXISTS ${LIBEXE})
			message (STATUS "WARNING: 'lib.exe' could not be found, libraries:${LIBRARIES} will be assumed static")
			set (${STATIC} ${LIBRARIES})
		endif (EXISTS ${LIBEXE})
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
