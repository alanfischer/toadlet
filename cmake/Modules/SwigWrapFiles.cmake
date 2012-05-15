# This simple macro uses swig to wrap interface files
# OUTPUT - the output .cpp file 
# FILES - a list of all of the interface files you want wrapped into the output .cpp file
#
# Example use:
#   SWIG_WRAP_FILES(my_wrap.cpp "${MY_FILES}")
#
# NOTE: When you call this script, make sure you quote the argument to FILES if it is a list!
# NOTE: Both SWIG_EXECUTABLE and SWIG_LANGUAGE must be set to use this macro

macro (SWIG_WRAP_FILES OUTPUT FILES)
	add_custom_command (OUTPUT ${OUTPUT}
		COMMAND ${SWIG_EXECUTABLE} -${SWIG_LANGUAGE} -c++ -o ${OUTPUT} ${FILES}
	)
endmacro (SWIG_WRAP_FILES)

