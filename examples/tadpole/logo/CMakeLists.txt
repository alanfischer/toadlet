# Standard setup
cmake_minimum_required (VERSION 2.6)
project (logo CXX C)

# Custom cmake scripts
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${logo_SOURCE_DIR}/../../../cmake/Modules/")

# Find toadlet
find_package (toadlet REQUIRED)

# Includes
include_directories (${logo_SOURCE_DIR} ${TOADLET_INCLUDE_DIR})

# Libraries
if(TOADLET_PLATFORM_IPHONE)
	set (LIBS ${TOADLET_EGG_LIB_S} ${TOADLET_PEEPER_LIB_S} ${TOADLET_RIBBIT_LIB_S} ${TOADLET_TADPOLE_LIB_S} ${TOADLET_PAD_LIB_S} ${TOADLET_PEEPER_GLRENDERER_LIB_S} ${TOADLET_RIBBIT_ALPLAYER_LIB_S})
else(TOADLET_PLATFORM_IPHONE)
	set (LIBS ${TOADLET_EGG_LIB} ${TOADLET_PEEPER_LIB} ${TOADLET_RIBBIT_LIB} ${TOADLET_TADPOLE_LIB} ${TOADLET_PAD_LIB})
endif(TOADLET_PLATFORM_IPHONE)

# Source files
set (SRC
	../../main/toadletMain.cpp
	Logo.cpp
)

# Headers
set (H
	Logo.h
)

# Executable
add_executable (logo ${SRC} ${H})
target_link_libraries (logo ${LIBS})
install (TARGETS logo DESTINATION ${logo_BINARY_DIR})
