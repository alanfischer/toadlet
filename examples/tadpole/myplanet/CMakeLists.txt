# Standard setup
cmake_minimum_required (VERSION 2.6)
project (myplanet CXX C)

# Custom cmake scripts
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${myplanet_SOURCE_DIR}/../../../cmake/Modules/")

# Find toadlet
find_package (toadlet REQUIRED)

# Includes
include_directories (${myplanet_SOURCE_DIR} ${TOADLET_INCLUDE_DIR} ${VLC_INCLUDE_DIR})

# Libraries
set (LIBS ${TOADLET_LIBRARIES})

# Source files
set (SRC
	../../main/toadletApp.cpp
	../../main/toadletMain.cpp
	MyPlanet.cpp
)

# Headers
set (H
	MyPlanet.h
)

# Executable
add_executable (myplanet ${SRC} ${H})
target_link_libraries (myplanet ${LIBS})
install (TARGETS myplanet DESTINATION ${myplanet_BINARY_DIR})
