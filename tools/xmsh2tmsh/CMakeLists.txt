# xmsh2tmsh

# Includes
set (XMSH2TMSH_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR} ${TOADLET_INCLUDE_DIR})
include_directories (${XMSH2TMSH_INCLUDES})

# Libraries
set (XMSH2TMSH_LIBS ${TOADLET_EGG_LIB} ${TOADLET_PEEPER_LIB} ${TOADLET_RIBBIT_LIB} ${TOADLET_TADPOLE_LIB})

# Source files
set (XMSH2TMSH_SRC
	main.cpp
)

# Headers
set (XMSH2TMSH_H
)

# Executable
add_executable (xmsh2tmsh ${XMSH2TMSH_SRC} ${XMSH2TMSH_H})
target_link_libraries (xmsh2tmsh ${XMSH2TMSH_LIBS})
install (TARGETS xmsh2tmsh DESTINATION bin)

