# Attempt to find the GLEW package
# Defines:
#  
#  GLEW_FOUND        - we have GLEW
#  GLEW_INCLUDE_DIR  - include directory
#  GLEW_LIBRARY_DIR  - library directory
#  GLEW_LIBRARY      - actual name of the glew library
#   

set (GLEW_FOUND "NO")

if (WIN32)
	set (HEADERS
		GL/glew.h
		GL/wglew.h
	)
	set (INCLUDE_PATHS
		"C:/Program Files/"
		"C:/Program Files/glew/include"
		"C:/glew/include"
	)
	set (LIBRARIES
		glew
		glew32
	)
	set (LIBRARY_PATHS
		"C:/Program Files/"
		"C:/Program Files/glew/lib"
		"C:/glew/lib"
	)
else (WIN32)
	set (HEADERS
		GL/glew.h
		GL/glxew.h
		GL/wglew.h
	)
	set (INCLUDE_PATHS
		/usr/include
		/usr/local/include
	)
	set (LIBRARIES
		glew
		GLEW
	)
	set (LIBRARY_PATHS
		/usr/lib
		/usr/local/lib
	)
endif( WIN32 )

find_path (GLEW_INCLUDE_DIR ${HEADERS} PATHS ${INCLUDE_PATHS})

find_library (GLEW_LIBRARY NAMES ${LIBRARIES} PATHS ${LIBRARY_PATHS})

get_filename_component (GLEW_LIBRARY_DIR ${GLEW_LIBRARY} PATH CACHE) 

mark_as_advanced (GLEW_INCLUDE_DIR GLEW_LIBRARY_DIR GLEW_LIBRARY)

if (GLEW_INCLUDE_DIR AND GLEW_LIBRARY)
	set (GLEW_FOUND "YES")
endif (GLEW_INCLUDE_DIR AND GLEW_LIBRARY)

if (GLEW_FOUND)
	if (NOT GLEW_FIND_QUITELY)
		message (STATUS "GLEW found")
	endif (NOT GLEW_FIND_QUITELY)
else (GLEW_FOUND)
	if (GLEW_FIND_REQUIRED)
		message (FATAL_ERROR "Could NOT find GLEW")
	endif (GLEW_FIND_REQUIRED)
	if (NOT GLEW_FIND_QUITELY)
		message (STATUS "Could NOT find GLEW")
	endif (NOT GLEW_FIND_QUITELY)
endif (GLEW_FOUND)

