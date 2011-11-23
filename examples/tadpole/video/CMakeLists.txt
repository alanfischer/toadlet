# Standard setup
cmake_minimum_required (VERSION 2.6)
project (video CXX C)

# Custom cmake scripts
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${video_SOURCE_DIR}/../../../cmake/Modules/")

# Find toadlet
find_package (toadlet REQUIRED)

# Includes
include_directories (${video_SOURCE_DIR} ${TOADLET_INCLUDE_DIR})

# Libraries
set (LIBS ${TOADLET_LIBRARIES})

# Source files
set (SRC
	../../main/toadletApp.cpp
	../../main/toadletMain.cpp
	Video.cpp
)

# Headers
set (H
	Video.h
)

# Executable
add_executable (video ${SRC} ${H})
target_link_libraries (video ${LIBS})
install (TARGETS video DESTINATION ${video_BINARY_DIR})
