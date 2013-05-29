# Standard setup
cmake_minimum_required (VERSION 2.6)
project (input CXX C)

# Custom cmake scripts
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${input_SOURCE_DIR}/../../cmake/Modules/")

# Find toadlet
find_package (toadlet REQUIRED)

# Includes
include_directories (${input_SOURCE_DIR} ${TOADLET_INCLUDE_DIR})

# Libraries
set (LIBS ${TOADLET_LIBRARIES})

# Source files
set (SRC
	../../main/toadletApp.cpp
	../../main/toadletMain.cpp
	src/Input.cpp
)

# Headers
set (H
	src/Input.h
)

# Executable
add_executable (input ${SRC} ${H})
target_link_libraries (input ${LIBS})
install (TARGETS input DESTINATION ${input_BINARY_DIR})
