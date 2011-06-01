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
set (LIBS ${TOADLET_EGG_LIB} ${TOADLET_PEEPER_LIB} ${TOADLET_RIBBIT_LIB} ${TOADLET_TADPOLE_LIB} ${TOADLET_PAD_LIB} ${TOADLET_HOP_LIB} ${TOADLET_TADPOLE_HOP_LIB})

# Source files
set (SRC
	../../main/toadletMain.cpp
	src/Acorn.cpp
	src/HUD.cpp
	src/PathClimber.cpp
	src/Resources.cpp
	src/Sky.cpp
	src/RandIsle.cpp
	src/Tree.cpp
)

# Headers
set (H
	src/Acorn.h
	src/BranchSystem.h
	src/GroundProjector.h
	src/HUD.h
	src/PathClimber.h
	src/PathClimberListener.h
	src/PathSystem.h
	src/Resources.h
	src/Sky.h
	src/SmoothFollower.h
	src/RandIsle.h
	src/Tree.h
)

# Executable
add_executable (randisle ${SRC} ${H})
target_link_libraries (randisle ${LIBS})
install (TARGETS randisle DESTINATION ${randisle_BINARY_DIR})
