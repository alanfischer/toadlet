# Standard setup
cmake_minimum_required (VERSION 2.6)
project (convextest CXX C)

# Custom cmake scripts
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${convextest_SOURCE_DIR}/../../../cmake/Modules/")

# Find toadlet
find_package (toadlet REQUIRED)

# Includes
include_directories (${CMAKE_CURRENT_SOURCE_DIR} ${TOADLET_INCLUDE_DIR})

# Libraries
set (LIBS ${TOADLET_EGG_LIB} ${TOADLET_PEEPER_LIB} ${TOADLET_RIBBIT_LIB} ${TOADLET_TADPOLE_LIB} ${TOADLET_PAD_LIB})

# Source files
set (SRC
	ConvexTest.cpp
)

# Headers
set (H
	ConvexTest.h
)

# Executable
add_executable (convextest ${SRC} ${H})
target_link_libraries (convextest ${LIBS})
install (TARGETS convextest DESTINATION ${convextest_BINARY_DIR})
