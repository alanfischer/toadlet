# Build java doxygen if requested
find_package (Doxygen)
if (DOXYGEN_FOUND AND DOXYGEN_DOT_FOUND)
	configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.cmake ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
	add_custom_target (javadoxygen ALL COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Generating API documentation with Doxygen" VERBATIM) 
	install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/java DESTINATION doc/toadlet-doxygen OPTIONAL PATTERN "*.md5" EXCLUDE PATTERN "*.map" EXCLUDE)
else (DOXYGEN_FOUND AND DOXYGEN_DOT_FOUND)
	message ("Doxygen and/or dot not found. Java Doxygen docs will not be built")	
endif (DOXYGEN_FOUND AND DOXYGEN_DOT_FOUND)

