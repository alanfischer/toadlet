# Standard setup

cmake_minimum_required (VERSION 2.6)
project (logo CXX C)

# Custom cmake scripts
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${logo_SOURCE_DIR}/../../../../cmake/Modules/")

# Find toadlet
find_package (toadlet REQUIRED)
find_package (mxml REQUIRED)

# Includes
include_directories (${logo_SOURCE_DIR} ${TOADLET_INCLUDE_DIR})

# Libraries
set (LIBS ${TOADLET_PAD_LIB_S} ${TOADLET_TADPOLE_LIB_S} ${TOADLET_PEEPER_GLRENDERDEVICE_LIB_S} ${TOADLET_PEEPER_LIB_S} ${TOADLET_RIBBIT_LIB_S} ${TOADLET_EGG_LIB_S})
# Add required android static libs
set (LIBS ${LIBS} ${MXML_LIBRARY} GLESv1_CM log)

message ( ${TOADLET_EGG_LIB_S} )
message ( ${TOADLET_TADPOLE_LIB_S} )

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
	install (FILES ${JTOADLET_PEEPER_JAR} ${JTOADLET_PAD_JAR} DESTINATION ${logo_SOURCE_DIR}/libs)
endif (${ANDROID_NDK_API_LEVEL} LESS 9)

