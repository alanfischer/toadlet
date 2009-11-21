# Standard setup
cmake_minimum_required (VERSION 2.6)
project (performance CXX C)

# Custom cmake scripts
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${performance_SOURCE_DIR}/../../../cmake/Modules/")

# Find toadlet
find_package (toadlet REQUIRED)

# Includes
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/src/cpp ${TOADLET_INCLUDE_DIR})

# Libraries
set (LIBS ${TOADLET_EGG_LIB} ${TOADLET_PEEPER_LIB} ${TOADLET_RIBBIT_LIB} ${TOADLET_TADPOLE_LIB} ${TOADLET_PAD_LIB})

# Source files
set (SRC
	src/cpp/Performance.cpp
)

# Headers
set (H
	src/cpp/Performance.h
)

# Executable
add_executable (performance ${SRC} ${H})
target_link_libraries (performance ${LIBS})
install (TARGETS performance DESTINATION ${performance_BINARY_DIR})
