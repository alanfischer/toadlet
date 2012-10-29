# Standard setup
cmake_minimum_required (VERSION 2.6)
project (randisle CXX C)

# Custom cmake scripts
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${randisle_SOURCE_DIR}/../../../../cmake/Modules/")

# Find toadlet
find_package (toadlet REQUIRED)

# Includes
include_directories (${randisle_SOURCE_DIR} ${TOADLET_INCLUDE_DIR})

# Libraries
set (LIBS ${TOADLET_LIBRARIES} GLESv1_CM GLESv2 log)
if (${ANDROID_NDK_API_LEVEL} GREATER 8)
	set (LIBS ${LIBS} EGL android)
endif (${ANDROID_NDK_API_LEVEL} GREATER 8)

# Source files
set (SRC
	jni/RandIsleImpl.cpp
	../src/BranchSystem.cpp
	../src/HUD.cpp
	../src/PathClimber.cpp
	../src/RandIsle.cpp
	../src/Resources.cpp
	../src/Sky.cpp
	../src/TreeSystem.cpp
)

# Headers
set (H
	../src/RandIsle.h
)

# Executable
add_library (randisle SHARED ${SRC} ${H})
target_link_libraries (randisle ${LIBS})
install (TARGETS randisle DESTINATION ${randisle_SOURCE_DIR}/libs/${NDK_NAME_ARCH})
install (FILES ${TOADLET_LIBRARIES} DESTINATION ${randisle_SOURCE_DIR}/libs/${NDK_NAME_ARCH})
if (${ANDROID_NDK_API_LEVEL} LESS 9)
	install (FILES ${JTOADLET_FLICK_JAR} ${JTOADLET_PEEPER_JAR} ${JTOADLET_RIBBIT_JAR} ${JTOADLET_TADPOLE_JAR} ${JTOADLET_PAD_JAR} DESTINATION ${randisle_SOURCE_DIR}/libs)
endif (${ANDROID_NDK_API_LEVEL} LESS 9)
