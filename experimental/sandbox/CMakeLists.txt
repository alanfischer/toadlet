# Standard setup
cmake_minimum_required (VERSION 2.6)
project (sandbox CXX C)

# Custom cmake scripts
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${sandbox_SOURCE_DIR}/../../cmake/Modules/")

# Find toadlet
find_package (toadlet REQUIRED)

# Includes
include_directories (${sandbox_SOURCE_DIR} ${TOADLET_INCLUDE_DIR})

# Source files
set (SRC
	../../examples/main/toadletApp.cpp
	../../examples/main/toadletMain.cpp
	Sandbox.cpp
)

# Headers
set (H
	Sandbox.h
)

set (RES
)

# Executable
add_executable (sandbox ${SRC} ${H})

target_link_libraries (sandbox ${TOADLET_LIBRARIES})

foreach (R ${RES})
	add_custom_command (TARGET sandbox PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${sandbox_SOURCE_DIR}/${R} ${sandbox_BINARY_DIR})
endforeach (R ${RES})

install (TARGETS sandbox DESTINATION ${sandbox_BINARY_DIR})
