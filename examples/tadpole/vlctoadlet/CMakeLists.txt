# Standard setup
cmake_minimum_required (VERSION 2.6)
project (vlctoadlet CXX C)

# Custom cmake scripts
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${vlctoadlet_SOURCE_DIR}/../../../cmake/Modules/")

# Find toadlet
find_package (toadlet REQUIRED)

# Includes
include_directories (${vlctoadlet_SOURCE_DIR} ${TOADLET_INCLUDE_DIR})

# Libraries
set (LIBS ${TOADLET_EGG_LIB_D} ${TOADLET_PEEPER_LIB_D} ${TOADLET_RIBBIT_LIB_D} ${TOADLET_TADPOLE_LIB_D} ${TOADLET_PAD_LIB_D} vlc vlccore)

# Source files
set (SRC
	../../main/toadletMain.cpp
	VLCToadlet.cpp
)

# Headers
set (H
	VLCToadlet.h
)

# Executable
add_executable (vlctoadlet ${SRC} ${H})
target_link_libraries (vlctoadlet ${LIBS})
install (TARGETS vlctoadlet DESTINATION ${vlctoadlet_BINARY_DIR})
