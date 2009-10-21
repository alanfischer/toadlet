# tmshviewer

# Includes
set (TMSHVIEWER_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR} ${TOADLET_INCLUDE_DIR})
include_directories (${TMSHVIEWER_INCLUDES})

# Libraries
set (TMSHVIEWER_LIBS ${TOADLET_EGG_LIB} ${TOADLET_PEEPER_LIB} ${TOADLET_RIBBIT_LIB} ${TOADLET_TADPOLE_LIB} ${TOADLET_PAD_LIB})

# Source files
set (TMSHVIEWER_SRC
	Viewer.cpp
	main.cpp
)

# Headers
set (TMSHVIEWER_H
	Viewer.h
)

# Executable
add_executable (tmshviewer ${TMSHVIEWER_SRC} ${TMSHVIEWER_H})
target_link_libraries (tmshviewer ${TMSHVIEWER_LIBS})
install (TARGETS tmshviewer DESTINATION bin)

