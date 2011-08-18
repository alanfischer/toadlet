set (CG_FOUND "NO")

find_path (CG_INCLUDE_DIR Cg/cg.h PATHS
	/usr/include
	/usr/local/include
	"C:/Program Files/NVIDIA Corporation/Cg/include"
)
find_library (CG_LIBRARY NAMES cg PATHS 
	/usr/lib
	/usr/local/lib
	"C:/Program Files/NVIDIA Corporation/Cg/lib"
)
find_library (CGGL_LIBRARY NAMES cgGL PATHS 
	/usr/lib
	/usr/local/lib
	"C:/Program Files/NVIDIA Corporation/Cg/lib"
)
find_library (CGD3D8_LIBRARY NAMES cgD3D8 PATHS 
	/usr/lib
	/usr/local/lib
	"C:/Program Files/NVIDIA Corporation/Cg/lib"
)

find_library (CGD3D9_LIBRARY NAMES cgD3D9 PATHS 
	/usr/lib
	/usr/local/lib
	"C:/Program Files/NVIDIA Corporation/Cg/lib"
)

find_library (CGD3D10_LIBRARY NAMES cgD3D10 PATHS 
	/usr/lib
	/usr/local/lib
	"C:/Program Files/NVIDIA Corporation/Cg/lib"
)

find_library (CGD3D11_LIBRARY NAMES cgD3D11 PATHS 
	/usr/lib
	/usr/local/lib
	"C:/Program Files/NVIDIA Corporation/Cg/lib"
)

mark_as_advanced (CG_INCLUDE_DIR CG_LIBRARY)

message (STATUS ${CG_INCLUDE_DIR})
message (STATUS ${CG_LIBRARY})

if (CG_INCLUDE_DIR AND CG_LIBRARY)
	set (CG_FOUND "YES")
endif (CG_INCLUDE_DIR AND CG_LIBRARY)

if (CG_FOUND)
	if (NOT CG_FIND_QUITELY)
		message (STATUS "CG found")
	endif (NOT CG_FIND_QUITELY)
else (CG_FOUND)
	if (CG_FIND_REQUIRED)
		message (FATAL_ERROR "Could NOT find CG")
	endif (CG_FIND_REQUIRED)
	if (NOT CG_FIND_QUITELY)
		message (STATUS "Could NOT find CG")
	endif (NOT CG_FIND_QUITELY)
endif (CG_FOUND)

