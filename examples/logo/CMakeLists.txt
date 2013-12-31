# Standard setup
cmake_minimum_required (VERSION 2.6)
project (logo CXX C)

# Custom cmake scripts
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${logo_SOURCE_DIR}/../../cmake/Modules/")

# Find toadlet
find_package (toadlet REQUIRED)

# Includes
include_directories (${logo_SOURCE_DIR} ${TOADLET_INCLUDE_DIR})

# Source files
set (SRC
	../../main/toadletApp.cpp
	../../main/toadletMain.cpp
	Logo.cpp
)

# Headers
set (H
	Logo.h
)

set (RES
	../data/lt.tmsh
)

# Executable
add_executable (logo ${SRC} ${H})

if (EMSCRIPTEN)
	set (CMAKE_C_FLAGS "-Wno-warn-absolute-paths")
	set (CMAKE_CXX_FLAGS "-Wno-warn-absolute-paths")
	target_link_libraries (logo ${TOADLET_LIBRARIES_S} "--embed-file lt.tmsh")
	set_target_properties( logo PROPERTIES OUTPUT_NAME logo.html ) 
else (EMSCRIPTEN)
	target_link_libraries (logo ${TOADLET_LIBRARIES})
endif (EMSCRIPTEN)

foreach (R ${RES})
	add_custom_command (TARGET logo PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${logo_SOURCE_DIR}/${R} ${logo_BINARY_DIR})
endforeach (R ${RES})

install (TARGETS logo DESTINATION ${logo_BINARY_DIR})
