# This file lists 2 macros for generating swig wrapper files

# swig_wrap_setup(LANGUAGE PREFIX INCLUDES)
#  NOTE: This must be called at least once before using the swig_wrap_files() macro.
#  
#  Arguments:
#   LANGUAGE - language of the bindings being generated (ex: java/objc)
#   PREFIX - a package prefix (ex: my.package.is.) NOTE: set to NO if empty
#   INCLUDES - a list of include directories for the bindings
#
#  Examples:
#   SWIG_WRAP_SETUP(java com.my. /path/to/binding/includes)
#   SWIG_WRAP_SETUP(objc NO /path/to/binding/includes)

macro (SWIG_WRAP_SETUP LANGUAGE PREFIX INCLUDES)
	if (NOT SWIG_EXECUTABLE)
		message (FATAL_ERROR "The SWIG_EXECUTABLE is not set. Did using find_package(SWIG) fail?")
	endif (NOT SWIG_EXECUTABLE)
	
	set (SWIG_WRAP_LANGUAGE ${LANGUAGE} CACHE INTERNAL "Swig wrap language" FORCE)
	set (SWIG_WRAP_INCLUDES ${INCLUDES} CACHE INTERNAL "Swig wrap include path" FORCE)
	
	# The prefix may be empty, and we must redefine prefix for this comparison to work properly
	set (PREFIX ${PREFIX})
	if (PREFIX)
		set (SWIG_WRAP_PREFIX ${PREFIX} CACHE INTERNAL "Swig wrap package prefix" FORCE)
	endif (PREFIX)
	
	set (SWIG_WRAP_FILES_SETUP ON CACHE INTERNAL "Swig wrap files is setup" FORCE)
	message (STATUS "SWIG_WRAP_FILES() has been setup")
endmacro (SWIG_WRAP_SETUP)


# swig_wrap_files(OUTPUT PACKAGE INTERFACE)
#  This uses swig to wrap interface file to an output.
#  NOTE: swig_wrap_setup() MUST be called before this macro may be used.
#
#  Arguments:
#   OUTPUT - the output file name
#   PACKAGE - package name you wish to output
#   INTERFACE - the interface file you are using
#
#  Example:
#   SWIG_WRAP_FILES(my_wrap.cpp packagename /path/to/my/interface.i)

macro (SWIG_WRAP_FILES OUTPUT PACKAGE INTERFACE)
	if (NOT SWIG_WRAP_FILES_SETUP)
		message (FATAL_ERROR "The SWIG_WRAP_SETUP() macro has not been called. You must setup before using SWIG_WRAP_FILES().")
	endif (NOT SWIG_WRAP_FILES_SETUP)
	
	add_custom_command (OUTPUT ${OUTPUT}
		COMMAND ${SWIG_EXECUTABLE} -${SWIG_WRAP_LANGUAGE} -I${SWIG_WRAP_INCLUDES} -package ${SWIG_WRAP_PREFIX}${PACKAGE} -c++ -outcurrentdir -o ${OUTPUT} ${INTERFACE}
	)
endmacro (SWIG_WRAP_FILES)
