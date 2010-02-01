# Standard setup
cmake_minimum_required (VERSION 2.6)
project (simplesync CXX C)

# Custom cmake scripts
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${simplesync_SOURCE_DIR}/../../../cmake/Modules/")

# Find toadlet
find_package (toadlet REQUIRED)

# Includes
include_directories (${CMAKE_CURRENT_SOURCE_DIR} ${TOADLET_INCLUDE_DIR})

# Libraries
set (LIBS ${TOADLET_EGG_LIB_D} ${TOADLET_HOP_LIB_D} ${TOADLET_PEEPER_LIB_D} ${TOADLET_KNOT_LIB_D} ${TOADLET_RIBBIT_LIB_D} ${TOADLET_TADPOLE_LIB_D} ${TOADLET_TADPOLE_HOP_LIB_D} ${TOADLET_PAD_LIB_D})

# Source files
set (SRC
	SimpleSync.cpp
)

# Executable
add_executable (simplesync ${SRC})
target_link_libraries (simplesync ${LIBS})
install (TARGETS simplesync DESTINATION ${simplesync_BINARY_DIR})

