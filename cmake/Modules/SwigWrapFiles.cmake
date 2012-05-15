# This simple macro uses swig to wrap interface files
# OUTPUT - the output .cpp file 
# FILES - a list of all of the interface files you want wrapped into the output .cpp file
# 
# Required variables: 
#   SWIG_EXECUTABLE = points to the location of the swig exe
#   SWIG_LANGUAGE = the output language (java/objc/etc)
#
# Optional:
#   SWIG_INCLUDES = additional include paths for swig
#
# Example use:
#   SWIG_WRAP_FILES(my_wrap.cpp "${MY_FILES}")
#
# NOTE: When you call this script, make sure you quote the argument to FILES if it is a list!

macro (SWIG_WRAP_FILES OUTPUT FILES)
	add_custom_command (OUTPUT ${OUTPUT}
		COMMAND ${SWIG_EXECUTABLE} -${SWIG_LANGUAGE} -I${SWIG_INCLUDES} -c++ -outcurrentdir -o ${OUTPUT} ${FILES}
	)
endmacro (SWIG_WRAP_FILES)

