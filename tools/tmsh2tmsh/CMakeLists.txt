# tmsh2tmsh

# Includes
set (TMSH2TMSH_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR} ${TOADLET_INCLUDE_DIR})
include_directories (${TMSH2TMSH_INCLUDES})

# Libraries
set (TMSH2TMSH_LIBS ${TOADLET_EGG_LIB} ${TOADLET_PEEPER_LIB} ${TOADLET_RIBBIT_LIB} ${TOADLET_TADPOLE_LIB})

# Source files
set (TMSH2TMSH_SRC
	main.cpp
)

# Headers
set (TMSH2TMSH_H
)

# Executable
add_executable (tmsh2tmsh ${TMSH2TMSH_SRC} ${TMSH2TMSH_H})
target_link_libraries (tmsh2tmsh ${TMSH2TMSH_LIBS})
install (TARGETS tmsh2tmsh DESTINATION bin)

