# Attempt to find the D3D9 libraries
# Defines:
#
#  D3D9_FOUND		- system has D3D9
#  D3D9_INCLUDE_PATH 	- path to the D3D9 headers
#  D3D9_LIBRARIES	- path to the D3D9 libraries
#  D3D9_LIB		- d3d9.lib
#  D3DX9_LIB		- d3dx9.lib
#  DXERR9_LIB		- dxerr9.lib

set (D3D9_FOUND "NO")

if (WIN32)
	find_path (D3D9_INCLUDE_PATH d3d9.h
		"$ENV{DXSDK_DIR}/Include"
		NO_DEFAULT_PATH
		DOC "Path to the d3d9.h file"
	)

	if (D3D9_INCLUDE_PATH)
		if (CMAKE_CL_64)
			set (D3D9_LIB_PATH x64)
		else (CMAKE_CL_64)
			set (D3D9_LIB_PATH x86)
		endif (CMAKE_CL_64)

		find_library (D3D9_LIB d3d9
			"$ENV{DXSDK_DIR}/Lib/${D3D9_LIB_PATH}"
			NO_DEFAULT_PATH
			DOC "Path to the d3d9.lib file"
		)

		find_library (D3DX9_LIB d3dx9
			"$ENV{DXSDK_DIR}/Lib/${D3D9_LIB_PATH}"
			NO_DEFAULT_PATH
			DOC "Path to the d3d9x.lib file"
		)

		find_library (DXERR9_LIB NAMES dxerr dxerr9
			"$ENV{DXSDK_DIR}/Lib/${D3D9_LIB_PATH}"
			NO_DEFAULT_PATH
			DOC "Path to the dxerr9x.lib file"
		)

		if (D3D9_LIB AND D3DX9_LIB AND DXERR9_LIB)
			set (D3D9_FOUND "YES")
		endif (D3D9_LIB AND D3DX9_LIB AND DXERR9_LIB)
	endif (D3D9_INCLUDE_PATH)

	mark_as_advanced (D3D9_INCLUDE_PATH D3D9_LIB D3DX9_LIB DXERR9_LIB)
endif (WIN32)

if (D3D9_FOUND)
	set (D3D9_LIBRARIES ${D3D9_LIB} ${D3DX9_LIB} ${DXERR9_LIB})
	if (NOT D3D9_FIND_QUIETLY)		
		message (STATUS "D3D9 headers found at ${D3D9_INCLUDE_PATH}")
	endif (NOT D3D9_FIND_QUIETLY)
else (D3D9_FOUND)
	if (D3D9_FIND_REQUIRED)
		message (FATAL_ERROR "Could NOT find Direct3D9")
	endif (D3D9_FIND_REQUIRED)
	if (NOT D3D9_FIND_QUIETLY)
		message (STATUS "Could NOT find Direct3D9")
	endif (NOT D3D9_FIND_QUIETLY)
endif (D3D9_FOUND)

