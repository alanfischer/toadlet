# Standard setup
cmake_minimum_required (VERSION 2.6)
project (ribbittest CXX C)

# Custom cmake scripts
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${ribbittest_SOURCE_DIR}/../../../cmake/Modules/")

# Find toadlet
find_package (toadlet REQUIRED)

# Includes
include_directories (${CMAKE_CURRENT_SOURCE_DIR} ${TOADLET_INCLUDE_DIR})

# Libraries
set (LIBS ${TOADLET_EGG_LIB} ${TOADLET_PEEPER_LIB} ${TOADLET_RIBBIT_LIB} ${TOADLET_TADPOLE_LIB} ${TOADLET_PAD_LIB})

# Source files
set (SRC
	RibbitTest.cpp
	boing_wav.cpp
)

# Executable
add_executable (ribbittest ${SRC})
target_link_libraries (ribbittest ${LIBS})
install (TARGETS ribbittest DESTINATION ${ribbittest_BINARY_DIR})

