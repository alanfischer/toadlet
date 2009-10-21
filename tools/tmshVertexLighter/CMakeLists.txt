# tmshvertexlighter

# Includes
set (TMSHVERTEXLIGHTER_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR} ${TOADLET_INCLUDE_DIR})
include_directories (${TMSHVERTEXLIGHTER_INCLUDES})

# Libraries
set (TMSHVERTEXLIGHTER_LIBS ${TOADLET_EGG_LIB} ${TOADLET_PEEPER_LIB} ${TOADLET_RIBBIT_LIB} ${TOADLET_TADPOLE_LIB})

# Source files
set (TMSHVERTEXLIGHTER_SRC
	VertexLighter.cpp
	main.cpp
)

# Headers
set (TMSHVERTEXLIGHTER_H
	VertexLighter.h
)

# Executable
add_executable (tmshvertexlighter ${TMSHVERTEXLIGHTER_SRC} ${TMSHVERTEXLIGHTER_H})
target_link_libraries (tmshvertexlighter ${TMSHVERTEXLIGHTER_LIBS})
install (TARGETS tmshvertexlighter DESTINATION bin)

