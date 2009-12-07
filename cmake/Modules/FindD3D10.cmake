# Attempt to find the D3D10 libraries
# Defines:
#
#  D3D10_FOUND		- system has D3D10
#  D3D10_INCLUDE_PATH 	- path to the D3D10 headers
#  D3D10_LIBRARIES	- path to the D3D10 libraries
#  D3D10_LIB		- d3d10.lib
#  D3DX10_LIB		- d3dx10.lib
#  DXERR10_LIB		- dxerr10.lib

set (D3D10_FOUND "NO")

if (WIN32)
	find_path (D3D10_INCLUDE_PATH d3d10.h
		"$ENV{DXSDK_DIR}/Include"
		NO_DEFAULT_PATH
		DOC "Path to the d3d10.h file"
	)

	if (D3D10_INCLUDE_PATH)
		if (CMAKE_CL_64)
			set (D3D10_LIB_PATH x64)
		else (CMAKE_CL_64)
			set (D3D10_LIB_PATH x86)
		endif (CMAKE_CL_64)

		find_library (D3D10_LIB d3d10
			"$ENV{DXSDK_DIR}/Lib/${D3D10_LIB_PATH}"
			NO_DEFAULT_PATH
			DOC "Path to the d3d10.lib file"
		)

		find_library (D3DX10_LIB d3dx10
			"$ENV{DXSDK_DIR}/Lib/${D3D10_LIB_PATH}"
			NO_DEFAULT_PATH
			DOC "Path to the d3d10x.lib file"
		)

		find_library (DXERR10_LIB NAMES dxerr dxerr10
			PATHS "$ENV{DXSDK_DIR}/Lib/${D3D10_LIB_PATH}"
			NO_DEFAULT_PATH
			DOC "Path to the dxerr10x.lib file"
		)

		if (D3D10_LIB AND D3DX10_LIB AND DXERR10_LIB)
			set (D3D10_FOUND "YES")
		endif (D3D10_LIB AND D3DX10_LIB AND DXERR10_LIB)
	endif (D3D10_INCLUDE_PATH)

	mark_as_advanced (D3D10_INCLUDE_PATH D3D10_LIB D3DX10_LIB DXERR10_LIB)
endif (WIN32)

if (D3D10_FOUND)
	set (D3D10_LIBRARIES ${D3D10_LIB} ${D3DX10_LIB} ${DXERR10_LIB})
	if (NOT D3D10_FIND_QUIETLY)
		message (STATUS "D3D10 headers found at ${D3D10_INCLUDE_PATH}")
	endif (NOT D3D10_FIND_QUIETLY)
else (D3D10_FOUND)
	if (D3D10_FIND_REQUIRED)
		message (FATAL_ERROR "Could NOT find Direct3D10")
	endif (D3D10_FIND_REQUIRED)
	if (NOT D3D10_FIND_QUIETLY)
		message (STATUS "Could NOT find Direct3D10")
	endif (NOT D3D10_FIND_QUIETLY)
endif (D3D10_FOUND)

