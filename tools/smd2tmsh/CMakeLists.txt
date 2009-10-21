# smd2tmsh

# Includes
set (SMD2TMSH_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR} ${TOADLET_INCLUDE_DIR})
include_directories (${SMD2TMSH_INCLUDES})

# Libraries
set (SMD2TMSH_LIBS ${TOADLET_EGG_LIB} ${TOADLET_PEEPER_LIB} ${TOADLET_RIBBIT_LIB} ${TOADLET_TADPOLE_LIB})

# Source files
set (SMD2TMSH_SRC
	SMDConverter.cpp
	main.cpp
)

# Headers
set (SMD2TMSH_H
	SMDConverter.h
)

# Executable
add_executable (smd2tmsh ${SMD2TMSH_SRC} ${SMD2TMSH_H})
target_link_libraries (smd2tmsh ${SMD2TMSH_LIBS})
install (TARGETS smd2tmsh DESTINATION bin)

