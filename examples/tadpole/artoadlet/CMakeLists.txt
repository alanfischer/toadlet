# Standard setup
cmake_minimum_required (VERSION 2.6)
project (artoadlet CXX C)

# Custom cmake scripts
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${artoadlet_SOURCE_DIR}/../../../cmake/Modules/")

# Find toadlet
find_package (toadlet REQUIRED)

# Includes
include_directories (${artoadlet_SOURCE_DIR} ${TOADLET_INCLUDE_DIR} "C:/Program Files/Microsoft SDKs/Windows/v7.1/Include")

# Libraries
set (LIBS ${TOADLET_LIBRARIES} "C:/Program Files/Microsoft SDKs/Windows/v7.1/Lib/mfreadwrite.lib")

# Source files
set (SRC
	../../main/toadletApp.cpp
	../../main/toadletMain.cpp
	src/ARToadlet.cpp
	src/MFVideoDevice.cpp
)

# Headers
set (H
	src/ARToadlet.h
	src/MFVideoDevice.h
	src/VideoDevice.h
)

# Executable
add_executable (artoadlet ${SRC} ${H})
target_link_libraries (artoadlet ${LIBS})
install (TARGETS artoadlet DESTINATION bin)
