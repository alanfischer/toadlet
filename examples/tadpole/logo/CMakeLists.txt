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
	find_package(MXML)
	add_library (libmxml_s STATIC IMPORTED)
	string (REPLACE ".so" ".a" MXML_A ${MXML_LIBRARY})
	if (EXISTS ${MXML_A})
		set (MXML_LIBRARY ${MXML_A})
	endif (EXISTS ${MXML_A})
        set_property (TARGET libmxml_s PROPERTY IMPORTED_LOCATION ${MXML_LIBRARY})
	set (STATIC_LIB_PROPS ${STATIC_LIB_PROPS} STATIC_LIBRARY_FLAGS ${MXML_LIBRARY})


	find_library (UIKIT_LIBS UIKit)
	find_library (OPENAL_LIBS OpenAL)
	find_library (OPENGLES_LIBS OpenGLES)
	find_library (FOUNDATION_LIBS Foundation)
	find_library (COREGRAPHICS_LIBS CoreGraphics)
	find_library (QUARTZCORE_LIBS QuartzCore)
	set (LIBS ${TOADLET_EGG_LIB_S} ${TOADLET_PEEPER_LIB_S} ${TOADLET_RIBBIT_LIB_S} ${TOADLET_TADPOLE_LIB_S} ${TOADLET_FLICK_LIB_S} ${TOADLET_PAD_LIB_S} ${TOADLET_PEEPER_GLRENDERER_LIB_S} ${TOADLET_FLICK_IOSMOTIONDETECTOR_LIB_S} ${TOADLET_RIBBIT_ALPLAYER_LIB_S} ${UIKIT_LIBS} ${OPENGLES_LIBS} ${OPENAL_LIBS} ${FOUNDATION_LIBS} ${COREGRAPHICS_LIBS}  ${QUARTZCORE_LIBS} libmxml_s)
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
add_executable (logo MACOSX_BUNDLE ${SRC} ${H})
target_link_libraries (logo ${LIBS})
install (TARGETS logo DESTINATION ${logo_BINARY_DIR})
