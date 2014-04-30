# This macro looks for static library versions of a list of libraries
# STATIC - the output list of static libraries
# LIBRARIES - the input list of libraries you would like as static
# FALSE(default)/TRUE - No import libraries. This option only has effect on windows platforms. 
#                       If FALSE(default), then import libraries will be returned as static along with a warning.
#                       If TRUE, then import libraries will be ignored and only fully static libraries will be returned.
#
# Example usage:
#    LOCATE_STATIC_LIBRARIES (MYLIBS_S "${MYLIBS}")
#      or
#    LOCATE_STATIC_LIBRARIES (MYLIBS_S "${MYLIBS}" TRUE) - will not allow import libraries on windows platforms.
#
# NOTE: When you call this script, make sure you quote the argument to LIBRARIES if it is a list!

macro (LOCATE_STATIC_LIBRARIES STATIC LIBRARIES) #FALSE/TRUE)
	unset (${STATIC})

	if (TOADLET_PLATFORM_WIN32)
		# Are import libraries acceptable?
		set (NO_IMPORT FALSE)
		if (${ARGC} EQUAL 3)
			if (${ARGV2})
				set (NO_IMPORT TRUE)
			endif (${ARGV2})
		endif (${ARGC} EQUAL 3)
	
		# On windows we use the 'lib' tool to try and determine if we have found a fully static or an import library
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
					message("Using:${LIBEXE} to list:${LIBRARY} and dump to file: libout-${LIBNAME}")
					get_filename_component (LIBNAME ${LIBRARY} NAME_WE)
					execute_process (
						COMMAND ${LIBEXE} /list ${LIBRARY} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} OUTPUT_FILE libout-${LIBNAME}
					)
					file (READ ${CMAKE_CURRENT_BINARY_DIR}/libout-${LIBNAME} LIBOUT)
					message("Read found libout=|${LIBOUT}| when scanning:|${CMAKE_CURRENT_BINARY_DIR}/libout-${LIBNAME}")
					string (FIND ${LIBOUT} ".dll" HASDLL)

					if (${HASDLL} GREATER -1)
						# If a path to a .dll is present, this is an import library
						if (${NO_IMPORT})
							message (STATUS "WARNING: A fully static build of ${LIBRARY} could not be found. ${STATIC} will be incomplete.")
						else (${NO_IMPORT})
							message (STATUS "WARNING: ${LIBRARY} is an IMPORT library. The corresponding .dll(s) will be required.")
							set (${STATIC} ${${STATIC}} ${LIBRARY})
						endif (${NO_IMPORT})
					else (${HASDLL} GREATER -1)
						# We have found a fully static library
						set (${STATIC} ${${STATIC}} ${LIBRARY})
					endif (${HASDLL} GREATER -1)
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
