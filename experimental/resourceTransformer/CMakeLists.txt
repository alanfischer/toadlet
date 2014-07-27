# Standard setup
cmake_minimum_required (VERSION 2.6)
project (resourceTransfomer CXX C)

# Custom cmake scripts
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${resourceTransfomer_SOURCE_DIR}/../../cmake/Modules/")

# Find toadlet
find_package (toadlet REQUIRED)

# Includes
include_directories (${resourceTransfomer_SOURCE_DIR} ${TOADLET_INCLUDE_DIR})

# Source files
set (SRC
	mongoose.c
	ResourceTransformer.cpp
)

# Headers
set (H
	mongoose.h
	ResourceTransformer.h
)

set (RES
)

# Executable
add_executable (resourceTransfomer ${SRC} ${H})

target_link_libraries (resourceTransfomer ${TOADLET_LIBRARIES})

foreach (R ${RES})
	add_custom_command (TARGET resourceTransfomer PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${resourceTransfomer_SOURCE_DIR}/${R} ${resourceTransfomer_BINARY_DIR})
endforeach (R ${RES})

install (TARGETS resourceTransfomer DESTINATION ${resourceTransfomer_BINARY_DIR})
