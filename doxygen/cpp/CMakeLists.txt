# Build cpp doxygen if requested
find_package (Doxygen)
if (DOXYGEN_FOUND AND DOXYGEN_DOT_FOUND)
	configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.cmake ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
	add_custom_target (cppdoxygen COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Generating API documentation with Doxygen" VERBATIM) 
	install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION ${TOADLET_DOXYGEN_INSTALL_PATH}/toadlet-doxygen-cpp OPTIONAL PATTERN "*.md5" EXCLUDE PATTERN "*.map" EXCLUDE)
	message ("C++ Doxygen may be build with the cppdoxygen target")
else (DOXYGEN_FOUND AND DOXYGEN_DOT_FOUND)
	message ("Doxygen and/or dot not found. Cpp Doxygen docs will not be built")	
endif (DOXYGEN_FOUND AND DOXYGEN_DOT_FOUND)

