# Standard setup
cmake_minimum_required (VERSION 2.6)
project (particles CXX C)

# Custom cmake scripts
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${particles_SOURCE_DIR}/../../cmake/Modules/")

# Find toadlet
find_package (toadlet REQUIRED)

# Includes
include_directories (${particles_SOURCE_DIR} ${TOADLET_INCLUDE_DIR})

# Source files
set (SRC
	../../main/toadletApp.cpp
	../../main/toadletMain.cpp
	Particles.cpp
)

# Headers
set (H
	Particles.h
)

set (RES
	../data/fancyGlow.png
	../data/sparkle.png
)

# Executable
add_executable (particles ${SRC} ${H})

if (EMSCRIPTEN)
	set (CMAKE_C_FLAGS "-Wno-warn-absolute-paths")
	set (CMAKE_CXX_FLAGS "-Wno-warn-absolute-paths")
	target_link_libraries (particles ${TOADLET_LIBRARIES_S} "--embed-file fancyGlow.png" "--embed-file sparkle.png")
	set_target_properties( particles PROPERTIES OUTPUT_NAME particles.html ) 
else (EMSCRIPTEN)
	target_link_libraries (particles ${TOADLET_LIBRARIES})
endif (EMSCRIPTEN)

foreach (R ${RES})
	add_custom_command (TARGET particles PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${particles_SOURCE_DIR}/${R} ${particles_BINARY_DIR})
endforeach (R ${RES})

install (TARGETS particles DESTINATION ${particles_BINARY_DIR})
