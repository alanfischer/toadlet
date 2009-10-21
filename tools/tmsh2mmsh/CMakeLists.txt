# tmsh2mmsh

# Includes
set (TMSH2MMSH_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR} ${TOADLET_INCLUDE_DIR})
include_directories (${TMSH2MMSH_INCLUDES})

# Libraries
set (TMSH2MMSH_LIBS ${TOADLET_EGG_LIB} ${TOADLET_PEEPER_LIB} ${TOADLET_RIBBIT_LIB} ${TOADLET_TADPOLE_LIB})

# Source files
set (TMSH2MMSH_SRC
	MicroMeshConverter.cpp
	../shared/MeshPreprocess.cpp
	../shared/actc/tc.c
	../shared/VertexCompression.cpp
	main.cpp
)

# Headers
set (TMSH2MMSH_H
	MicroMeshConverter.h
	../shared/MeshPreprocess.h
	../shared/actc/tc.h
	../shared/VertexCompression.h
)

# Executable
add_executable (tmsh2mmsh ${TMSH2MMSH_SRC} ${TMSH2MMSH_H})
target_link_libraries (tmsh2mmsh ${TMSH2MMSH_LIBS})
install (TARGETS tmsh2mmsh DESTINATION bin)

