cmake_minimum_required (VERSION 2.6)
project (GLEW C)

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

# Install 
install (DIRECTORY ${PROJECT_SOURCE_DIR}/include/GL/ DESTINATION include/GL FILES_MATCHING PATTERN "*.h")

# Static library
add_library (GLEW STATIC ${GLEW_SRC} ${GLEW_H})
install (TARGETS GLEW DESTINATION lib)

# Posix platfroms build the shared library and link against OpenGL
if (UNIX)
	find_package (OpenGL REQUIRED)
	include_directories (${OPENGL_INCLUDE_DIR})

	add_library (GLEW_DYN SHARED ${GLEW_SRC} ${GLEW_H})
	target_link_libraries (GLEW_DYN ${OPENGL_LIBRARIES})
	set_target_properties (GLEW_DYN PROPERTIES OUTPUT_NAME GLEW)
	install (TARGETS GLEW_DYN DESTINATION lib)
endif (UNIX)
