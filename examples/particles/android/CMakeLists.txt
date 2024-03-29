# Standard setup
cmake_minimum_required (VERSION 2.6)
project (particles CXX C)

# Custom cmake scripts
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${particles_SOURCE_DIR}/../../../cmake/Modules/")

# Find toadlet
find_package (toadlet REQUIRED)
find_package (mxml REQUIRED)

# Includes
include_directories (${particles_SOURCE_DIR} ${TOADLET_INCLUDE_DIR})

# Libraries
set (LIBS ${TOADLET_LIBRARIES_S} GLESv1_CM log)

# Source files
set (SRC
	jni/ParticlesImpl.cpp
	../Particles.cpp
)

# Headers
set (H
	../Particles.h
)

# Executable
add_library (particles SHARED ${SRC} ${H})
target_link_libraries (particles ${LIBS})
install (TARGETS particles DESTINATION ${particles_SOURCE_DIR}/libs/${NDK_NAME_ARCH})
if (${ANDROID_NDK_API_LEVEL} LESS 9)
	install (FILES ${JTOADLET_PEEPER_JAR} ${JTOADLET_RIBBIT_JAR} ${JTOADLET_PAD_JAR} DESTINATION ${particles_SOURCE_DIR}/libs)
endif (${ANDROID_NDK_API_LEVEL} LESS 9)
