# Standard setup
cmake_minimum_required (VERSION 2.6)
project (toadlettools CXX C)


# Custom cmake scripts
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${toadlettools_SOURCE_DIR}/../cmake/Modules/")


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


# Add the tool subdirectories
add_subdirectory (msTMSHExporter)
add_subdirectory (msTMSHImporter)
add_subdirectory (smd2tmsh)
add_subdirectory (tmsh2bac)
add_subdirectory (tmsh2m3g)
add_subdirectory (tmsh2mmsh)
add_subdirectory (tmshModifier)
add_subdirectory (tmshOptimizer)
add_subdirectory (tmshVertexLighter)
add_subdirectory (tmshViewer)

