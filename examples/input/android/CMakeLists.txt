# Standard setup
cmake_minimum_required (VERSION 2.6)
project (input CXX C)

# Custom cmake scripts
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${input_SOURCE_DIR}/../../../cmake/Modules/")

# Find toadlet
find_package (toadlet REQUIRED)

# Includes
include_directories (${input_SOURCE_DIR} ${TOADLET_INCLUDE_DIR})

# Libraries
set (LIBS ${TOADLET_LIBRARIES} GLESv1_CM GLESv2 log)
if (${ANDROID_NDK_API_LEVEL} GREATER 8)
	set (LIBS ${LIBS} EGL android)
endif (${ANDROID_NDK_API_LEVEL} GREATER 8)

# Source files
set (SRC
	jni/InputImpl.cpp
	../src/Input.cpp
)

# Headers
set (H
	../src/Input.h
)

# Executable
add_library (input SHARED ${SRC} ${H})
target_link_libraries (input ${LIBS})
install (TARGETS input DESTINATION ${input_SOURCE_DIR}/libs/${NDK_NAME_ARCH})
install (FILES ${TOADLET_LIBRARIES} DESTINATION ${input_SOURCE_DIR}/libs/${NDK_NAME_ARCH})
if (${ANDROID_NDK_API_LEVEL} LESS 9)
	install (FILES ${JTOADLET_FLICK_JAR} ${JTOADLET_PEEPER_JAR} ${JTOADLET_RIBBIT_JAR} ${JTOADLET_TADPOLE_JAR} ${JTOADLET_PAD_JAR} DESTINATION ${input_SOURCE_DIR}/libs)
endif (${ANDROID_NDK_API_LEVEL} LESS 9)
