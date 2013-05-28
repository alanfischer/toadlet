# Standard setup
cmake_minimum_required (VERSION 2.8)
project (toadletexamples CXX C)


# Custom cmake scripts
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${toadletexamples_SOURCE_DIR}/../cmake/Modules/")


# Setup platforms
if (WIN32)
	set (TOADLET_PLATFORM_WIN32 ON)
elseif (UNIX)
	set (TOADLET_PLATFORM_POSIX ON)
	if (APPLE) 
		set (TOADLET_PLATFORM_OSX ON)
	endif (APPLE)
endif (WIN32)


# Find toadlet
find_package (toadlet REQUIRED)


# Add the example subdirectories
add_subdirectory (input)
add_subdirectory (logo)
add_subdirectory (particles)
add_subdirectory (randisle)
