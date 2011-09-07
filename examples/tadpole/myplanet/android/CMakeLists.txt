# Standard setup
cmake_minimum_required (VERSION 2.6)
project (myplanet CXX C)

# Custom cmake scripts
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${myplanet_SOURCE_DIR}/../../../../cmake/Modules/")

# Find toadlet
find_package (toadlet REQUIRED)

# Includes
include_directories (${myplanet_SOURCE_DIR} ${TOADLET_INCLUDE_DIR})

# Libraries
set (LIBS ${TOADLET_PAD_LIB_S} ${TOADLET_TADPOLE_LIB_S} ${TOADLET_PEEPER_GLRENDERDEVICE_LIB_S} ${TOADLET_PEEPER_LIB_S} ${TOADLET_RIBBIT_LIB_S} ${TOADLET_EGG_LIB_S} GLESv1_CM log)

message ( ${TOADLET_EGG_LIB_S} )
message ( ${TOADLET_TADPOLE_LIB_S} )

# Source files
set (SRC
	jni/MyPlanetImpl.cpp
	../MyPlanet.cpp
)

# Headers
set (H
	../MyPlanet.h
)

# Executable
add_library (myplanet SHARED ${SRC} ${H})
target_link_libraries (myplanet ${LIBS})
install (TARGETS myplanet DESTINATION ${myplanet_BINARY_DIR})
