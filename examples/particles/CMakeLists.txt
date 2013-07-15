# Standard setup
cmake_minimum_required (VERSION 2.6)
project (particles CXX C)

# Custom cmake scripts
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${particles_SOURCE_DIR}/../../cmake/Modules/")

# Find toadlet
find_package (toadlet REQUIRED)

# Includes
include_directories (${particles_SOURCE_DIR} ${TOADLET_INCLUDE_DIR})

# Libraries
set (LIBS ${TOADLET_LIBRARIES})

# Source files
set (SRC
	../../main/toadletApp.cpp
	../../main/toadletMain.cpp
	Particles.cpp
)

# Headers
set (H
	Particles.h
)

# Executable
add_executable (particles ${SRC} ${H})
target_link_libraries (particles ${LIBS})
install (TARGETS particles DESTINATION ${particles_BINARY_DIR})