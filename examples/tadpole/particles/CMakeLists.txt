# Standard setup
cmake_minimum_required (VERSION 2.6)
project (particles CXX C)

# Custom cmake scripts
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${particles_SOURCE_DIR}/../../../cmake/Modules/")

# Find toadlet
find_package (toadlet REQUIRED)

# Includes
include_directories (${particles_SOURCE_DIR} ${TOADLET_INCLUDE_DIR})

# Libraries
set (LIBS ${TOADLET_PAD_LIB_S} ${TOADLET_TADPOLE_LIB_S} ${TOADLET_PEEPER_GLRENDERDEVICE_LIB_S} ${TOADLET_PEEPER_LIB_S} ${TOADLET_RIBBIT_LIB_S} ${TOADLET_EGG_LIB_S} GLESv1_CM log)

message ( ${TOADLET_EGG_LIB_S} )
message ( ${TOADLET_TADPOLE_LIB_S} )

# Source files
set (SRC
#	../../main/toadletApp.cpp
#	../../main/toadletMain.cpp
	android/jni/ParticlesImpl.cpp

	Particles.cpp
)

# Headers
set (H
	Particles.h
)

# Executable
#add_executable (particles ${SRC} ${H})
add_library (particles SHARED ${SRC} ${H})
target_link_libraries (particles ${LIBS})
install (TARGETS particles DESTINATION ${particles_BINARY_DIR})
