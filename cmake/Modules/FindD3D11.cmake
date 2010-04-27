# Attempt to find the D3D11 libraries
# Defines:
#
#  D3D11_FOUND		- system has D3D11
#  D3D11_INCLUDE_PATH 	- path to the D3D11 headers
#  D3D11_LIBRARIES	- path to the D3D11 libraries
#  D3D11_LIB		- d3d11.lib
#  D3DX11_LIB		- d3dx11.lib
#  DXERR11_LIB		- dxerr11.lib

set (D3D11_FOUND "NO")

if (WIN32)
	find_path (D3D11_INCLUDE_PATH d3d11.h
		"$ENV{DXSDK_DIR}/Include"
		NO_DEFAULT_PATH
		DOC "Path to the d3d11.h file"
	)

	if (D3D11_INCLUDE_PATH)
		if (CMAKE_CL_64)
			set (D3D11_LIB_PATH x64)
		else (CMAKE_CL_64)
			set (D3D11_LIB_PATH x86)
		endif (CMAKE_CL_64)

		find_library (D3D11_LIB d3d11
			"$ENV{DXSDK_DIR}/Lib/${D3D11_LIB_PATH}"
			NO_DEFAULT_PATH
			DOC "Path to the d3d11.lib file"
		)

		find_library (D3DX11_LIB d3dx11
			"$ENV{DXSDK_DIR}/Lib/${D3D11_LIB_PATH}"
			NO_DEFAULT_PATH
			DOC "Path to the d3d11x.lib file"
		)

		find_library (DXERR11_LIB NAMES dxerr 
			PATHS "$ENV{DXSDK_DIR}/Lib/${D3D11_LIB_PATH}"
			NO_DEFAULT_PATH
			DOC "Path to the dxerr11x.lib file"
		)

		if (D3D11_LIB AND D3DX11_LIB AND DXERR11_LIB)
			set (D3D11_FOUND "YES")
		endif (D3D11_LIB AND D3DX11_LIB AND DXERR11_LIB)
	endif (D3D11_INCLUDE_PATH)

	mark_as_advanced (D3D11_INCLUDE_PATH D3D11_LIB D3DX11_LIB DXERR11_LIB)
endif (WIN32)

if (D3D11_FOUND)
	set (D3D11_LIBRARIES ${D3D11_LIB} ${D3DX11_LIB} ${DXERR11_LIB})
	if (NOT D3D11_FIND_QUIETLY)
		message (STATUS "D3D11 headers found at ${D3D11_INCLUDE_PATH}")
	endif (NOT D3D11_FIND_QUIETLY)
else (D3D11_FOUND)
	if (D3D11_FIND_REQUIRED)
		message (FATAL_ERROR "Could NOT find Direct3D11")
	endif (D3D11_FIND_REQUIRED)
	if (NOT D3D11_FIND_QUIETLY)
		message (STATUS "Could NOT find Direct3D11")
	endif (NOT D3D11_FIND_QUIETLY)
endif (D3D11_FOUND)

