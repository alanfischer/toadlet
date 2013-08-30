# Standard setup
cmake_minimum_required (VERSION 2.6)
project (randisle CXX C)

# Custom cmake scripts
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${randisle_SOURCE_DIR}/../../cmake/Modules/")

# Source files
set (SRC
	../../main/toadletApp.cpp
	../../main/toadletMain.cpp
	src/BranchSystem.cpp
	src/Clouds.cpp
	src/HUD.cpp
	src/PathClimber.cpp
	src/Resources.cpp
	src/Sky.cpp
	src/RandIsle.cpp
	src/TreeComponent.cpp
	src/TreeSystem.cpp
)

# Headers
set (H
	src/BranchSystem.h
	src/Clouds.h
	src/HUD.h
	src/PathClimber.h
	src/PathClimberListener.h
	src/Path.h
	src/Resources.h
	src/Sky.h
	src/SmoothFollower.h
	src/RandIsle.h
	src/TreeComponent.h
	src/TreeSystem.h
)

# Find toadlet
find_package (toadlet REQUIRED)

# Includes
include_directories (${randisle_SOURCE_DIR} ${TOADLET_INCLUDE_DIR})

# Executable
add_executable (randisle ${SRC} ${H})
target_link_libraries (randisle ${TOADLET_LIBRARIES_MIXED})

# Add the required resources to the directory necessary for in-editor running
if(WIN32)
	# Toadlet debug dlls
	file (MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Debug)
	foreach (TDLLD ${TOADLET_DLLS_D})
		add_custom_command (TARGET randisle POST_BUILD 
			COMMAND ${CMAKE_COMMAND} -E copy ${TDLLD} ${CMAKE_CURRENT_BINARY_DIR}/Debug
		)
	endforeach (TDLLD ${TOADLET_DLLS_D})

	# Toadlet release dlls
	file (MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Release)
	foreach (TDLLD ${TOADLET_DLLS_D})
		add_custom_command (TARGET randisle POST_BUILD 
			COMMAND ${CMAKE_COMMAND} -E copy ${TDLLD} ${CMAKE_CURRENT_BINARY_DIR}/Release
		)
	endforeach (TDLLD ${TOADLET_DLLS_D})
	
	# Win32 toadlet may need the ffmpeg dlls for in-editor running
	find_package (ffmpeg)
	if (FFMPEG_FOUND)
		foreach (FDLL ${FFMPEG_DLLS})
			add_custom_command (TARGET randisle POST_BUILD 
				COMMAND ${CMAKE_COMMAND} -E copy ${FDLL} "${CMAKE_CURRENT_BINARY_DIR}/\$(CONFIGURATION)"
			)
		endforeach (FDLL ${FFMPEG_DLLS})
	endif (FFMPEG_FOUND)
endif(WIN32)

# Installation
install (TARGETS randisle DESTINATION bin)
