# Standard setup
cmake_minimum_required (VERSION 2.6)
project (logo CXX C)

# Custom cmake scripts
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${logo_SOURCE_DIR}/../../cmake/Modules/")

# Find toadlet
find_package (toadlet REQUIRED)

# Includes
include_directories (${logo_SOURCE_DIR} ${TOADLET_INCLUDE_DIR})

# Libraries
set (LIBS ${TOADLET_LIBRARIES})

# Source files
set (SRC
	../../main/toadletApp.cpp
	../../main/toadletMain.cpp
	Logo.cpp
)

# Headers
set (H
	Logo.h
)

# Executable
add_executable (logo ${SRC} ${H})
target_link_libraries (logo ${LIBS})
install (TARGETS logo DESTINATION ${logo_BINARY_DIR})
