cmake_minimum_required (VERSION 2.6)
project (GLEW C)

# Platform specific dependencies
# This includes all header files from the source tree (SOURCE_DIR) and the cmake build tree (BINARY_DIR)
set (GLEW_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Actually include the directories for CMake to use
include_directories (${GLEW_INCLUDES})

# Source files
set (GLEW_SRC
	src/glew.c
)

# Headers
set (GLEW_H
	include/GL/glew.h
)

# Static library
add_library (GLEW STATIC ${GLEW_SRC} ${GLEW_H})

# Install
install (TARGETS GLEW DESTINATION lib)
install (DIRECTORY ${PROJECT_SOURCE_DIR}/include/GL/ DESTINATION include/GL FILES_MATCHING PATTERN "*.h")
