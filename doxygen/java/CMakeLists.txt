# Force swig to run before generating the java doxygen
swig_wrap_files (flick_wrap.cpp flick ${FLICK_SWIG_INT} "${FLICK_SWIG_INT_DEPS}" "${FLICK_SWIG_INT_OUTPUTS}")
swig_wrap_files (peeper_wrap.cpp peeper ${PEEPER_SWIG_INT} "${PEEPER_SWIG_INT_DEPS}" "${PEEPER_SWIG_INT_OUTPUTS}")
swig_wrap_files (ribbit_wrap.cpp ribbit ${RIBBIT_SWIG_INT} "${RIBBIT_SWIG_INT_DEPS}" "${RIBBIT_SWIG_INT_OUTPUTS}")
swig_wrap_files (tadpole_wrap.cpp tadpole ${TADPOLE_SWIG_INT} "${TADPOLE_SWIG_INT_DEPS}" "${TADPOLE_SWIG_INT_OUTPUTS}")
swig_wrap_files (pad_wrap.cpp pad ${PAD_SWIG_INT} "${PAD_SWIG_INT_DEPS}" "${PAD_SWIG_INT_OUTPUTS}")	

# Setup doxygen with cmake defines
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.cmake ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

# Create doxygen
add_custom_target (javadoxygen ALL COMMAND ${DOXYGEN_EXECUTABLE} 
	${CMAKE_CURRENT_BINARY_DIR}/Doxyfile 
	DEPENDS flick_wrap.cpp peeper_wrap.cpp ribbit_wrap.cpp tadpole_wrap.cpp pad_wrap.cpp
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} 
	COMMENT "Generating API documentation with Doxygen" VERBATIM
) 

# Install doxygen
install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/java DESTINATION doc/toadlet-doxygen OPTIONAL PATTERN "*.md5" EXCLUDE PATTERN "*.map" EXCLUDE)

