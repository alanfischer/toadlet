# tmshmodifier

# Includes
set (TMSHMODIFIER_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR} ${TOADLET_INCLUDE_DIR})
include_directories (${TMSHMODIFIER_INCLUDES})

# Libraries
set (TMSHMODIFIER_LIBS ${TOADLET_EGG_LIB} ${TOADLET_PEEPER_LIB} ${TOADLET_RIBBIT_LIB} ${TOADLET_TADPOLE_LIB})

# Source files
set (TMSHMODIFIER_SRC
	main.cpp
)

# Executable
add_executable (tmshmodifier ${TMSHMODIFIER_SRC})
target_link_libraries (tmshmodifier ${TMSHMODIFIER_LIBS})
install (TARGETS tmshmodifier DESTINATION bin)

