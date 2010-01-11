# tmsh2m3g

# Includes
set (TMSH2M3G_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR} ${TOADLET_INCLUDE_DIR})
include_directories (${TMSH2M3G_INCLUDES})

# Libraries
set (TMSH2M3G_LIBS ${TOADLET_EGG_LIB} ${TOADLET_PEEPER_LIB} ${TOADLET_RIBBIT_LIB} ${TOADLET_TADPOLE_LIB})

# Source files
set (TMSH2M3G_SRC
	M3GConverter.cpp
	main.cpp
	../shared/Adler32.cpp
	../shared/actc/tc.c
	../shared/VertexCompression.cpp
	../shared/NvTriStrip/NvTriStrip.cpp
	../shared/NvTriStrip/NvTriStripObjects.cpp
	../shared/MeshPreprocess.cpp
)

# Headers
set (TMSH2M3G_H
	M3GConverter.h
	../shared/MeshPreprocess.h
	../shared/actc/tc.h
	../shared/NvTriStrip/VertexCache.h
	../shared/NvTriStrip/NvTriStripObjects.h
	../shared/NvTriStrip/NvTriStrip.h
	../shared/Adler32.h
	../shared/VertexCompression.h
)

# Executable
add_executable (tmsh2m3g ${TMSH2M3G_SRC} ${TMSH2M3G_H})
target_link_libraries (tmsh2m3g ${TMSH2M3G_LIBS})
install (TARGETS tmsh2m3g DESTINATION bin)

