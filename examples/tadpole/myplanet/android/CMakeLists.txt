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
set (LIBS ${TOADLET_LIBRARIES})

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
install (TARGETS myplanet DESTINATION ${myplanet_SOURCE_DIR}/libs/${NDK_NAME_ARCH})
if (${ANDROID_NDK_API_LEVEL} LESS 9)
	install (FILES ${JTOADLET_PEEPER_JAR} ${JTOADLET_PAD_JAR} DESTINATION ${myplanet_SOURCE_DIR}/libs)
endif (${ANDROID_NDK_API_LEVEL} LESS 9)
