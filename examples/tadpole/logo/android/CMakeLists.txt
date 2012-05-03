# Standard setup
cmake_minimum_required (VERSION 2.6)
project (logo CXX C)

# Custom cmake scripts
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${logo_SOURCE_DIR}/../../../../cmake/Modules/")

# Find toadlet
find_package (toadlet REQUIRED)

# Includes
include_directories (${logo_SOURCE_DIR} ${TOADLET_INCLUDE_DIR})

# Libraries
set (LIBS ${TOADLET_LIBRARIES_S} GLESv1_CM GLESv2 log)

# Source files
set (SRC
	jni/LogoImpl.cpp
	../Logo.cpp
)

# Headers
set (H
	../Logo.h
)

# Executable
add_library (logo SHARED ${SRC} ${H})
target_link_libraries (logo ${LIBS})
install (TARGETS logo DESTINATION ${logo_SOURCE_DIR}/libs/${NDK_NAME_ARCH})
if (${ANDROID_NDK_API_LEVEL} LESS 9)
	install (FILES ${JTOADLET_PEEPER_JAR} ${JTOADLET_FLICK_JAR} ${JTOADLET_RIBBIT_JAR} ${JTOADLET_TADPOLE_JAR} ${JTOADLET_PAD_JAR} DESTINATION ${logo_SOURCE_DIR}/libs)
endif (${ANDROID_NDK_API_LEVEL} LESS 9)

