# tmsh2bac

# Includes
set (TMSH2BAC_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR} ${TOADLET_INCLUDE_DIR})
include_directories (${TMSH2BAC_INCLUDES})

# Libraries
set (TMSH2BAC_LIBS ${TOADLET_EGG_LIB} ${TOADLET_PEEPER_LIB} ${TOADLET_RIBBIT_LIB} ${TOADLET_TADPOLE_LIB})

# Source files
set (TMSH2BAC_SRC
	BACConverter.cpp
	main.cpp
)

# Headers
set (TMSH2BAC_H
	BACConverter.h
)

# Executable
add_executable (tmsh2bac ${TMSH2BAC_SRC} ${TMSH2BAC_H})
target_link_libraries (tmsh2bac ${TMSH2BAC_LIBS})
install (TARGETS tmsh2bac DESTINATION bin)

