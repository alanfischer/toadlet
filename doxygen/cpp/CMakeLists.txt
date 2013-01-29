# Setup doxygen with cmake defines
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.cmake ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

# Create doxygen
add_custom_target (cppdoxygen ALL COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Generating API documentation with Doxygen" VERBATIM) 

# Install doxygen
install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/cpp DESTINATION doc/toadlet-doxygen OPTIONAL PATTERN "*.md5" EXCLUDE PATTERN "*.map" EXCLUDE)

