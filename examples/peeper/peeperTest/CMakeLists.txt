# Standard setup
cmake_minimum_required (VERSION 2.6)
project (peepertest CXX C)

# Custom cmake scripts
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${peepertest_SOURCE_DIR}/../../../cmake/Modules/")

# Find toadlet
find_package (toadlet REQUIRED)

# Includes
include_directories (${CMAKE_CURRENT_SOURCE_DIR} ${TOADLET_INCLUDE_DIR})

# Libraries
set (LIBS ${TOADLET_EGG_LIB} ${TOADLET_PEEPER_LIB} ${TOADLET_RIBBIT_LIB} ${TOADLET_TADPOLE_LIB} ${TOADLET_PAD_LIB})

# Source files
set (SRC
	PeeperTest.cpp
)

# Executable
add_executable (peepertest ${SRC})
target_link_libraries (peepertest ${LIBS})
install (TARGETS peepertest DESTINATION ${peepertest_BINARY_DIR})

