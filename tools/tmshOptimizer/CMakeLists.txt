# tmshoptimizer

# Includes
set (TMSHOPTIMIZER_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR} ${TOADLET_INCLUDE_DIR})
include_directories (${TMSHOPTIMIZER_INCLUDES})

# Libraries
set (TMSHOPTIMIZER_LIBS ${TOADLET_EGG_LIB} ${TOADLET_PEEPER_LIB} ${TOADLET_RIBBIT_LIB} ${TOADLET_TADPOLE_LIB})

# Source files
set (TMSHOPTIMIZER_SRC
	MeshOptimizer.cpp
	main.cpp
)

# Headers
set (TMSHOPTIMIZER_H
	MeshOptimizer.h
)

# Executable
add_executable (tmshoptimizer ${TMSHOPTIMIZER_SRC} ${TMSHOPTIMIZER_H})
target_link_libraries (tmshoptimizer ${TMSHOPTIMIZER_LIBS})
install (TARGETS tmshoptimizer DESTINATION bin)

