# Standard setup
cmake_minimum_required (VERSION 2.6)
project (randisle CXX C)

# Custom cmake scripts
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${randisle_SOURCE_DIR}/../../../cmake/Modules/")

# Find toadlet
find_package (toadlet REQUIRED)

# Includes
include_directories (${randisle_SOURCE_DIR} ${TOADLET_INCLUDE_DIR})

# Libraries
set (LIBS ${TOADLET_LIBRARIES})

# Source files
set (SRC
	../../main/toadletApp.cpp
	../../main/toadletMain.cpp
	src/BranchSystem.cpp
	src/HUD.cpp
	src/PathClimber.cpp
	src/Resources.cpp
	src/Sky.cpp
	src/RandIsle.cpp
	src/TreeSystem.cpp
)

# Headers
set (H
	src/BranchSystem.h
	src/GroundProjector.h
	src/HUD.h
	src/PathClimber.h
	src/PathClimberListener.h
	src/Path.h
	src/Resources.h
	src/Sky.h
	src/SmoothFollower.h
	src/RandIsle.h
	src/TreeSystem.h
)

# Executable
add_executable (randisle ${SRC} ${H})
target_link_libraries (randisle ${LIBS})
install (TARGETS randisle DESTINATION bin)
