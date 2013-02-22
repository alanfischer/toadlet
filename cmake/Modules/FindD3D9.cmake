# Attempt to find the D3D9 libraries
# Defines:
#
#  D3D9_FOUND		 - system has D3D9
#  D3D9_INCLUDE_PATH - path to the D3D9 headers
#  D3D9_LIBRARIES	 - path to the D3D9 libraries
#  D3D9_LIB			 - d3d9.lib
#  D3DX9_LIB		 - d3dx9.lib
#  DXERR9_LIB		 - dxerr9.lib

set (D3D9_FOUND "NO")

if (WIN32)
	set (WIN8_SDK_DIR "C:/Program Files (x86)/Windows Kits/8.0")
	set (LEGACY_SDK_DIR "$ENV{DXSDK_DIR}")
	
	if (CMAKE_CL_64)
		set (ARCH x64)
	else (CMAKE_CL_64)
		set (ARCH x86)
	endif (CMAKE_CL_64)

	# Look for the windows 8 sdk
	find_path (D3D9_INCLUDE_PATH 
		NAMES d3d9helper.h
		PATHS "${WIN8_SDK_DIR}/Include/um"
		NO_DEFAULT_PATH
		DOC "Path to the windows 8 d3d9.h file"
	)
	
	if (D3D9_INCLUDE_PATH)
		find_library (D3D9_LIB 
			NAMES d3d9
			PATHS "${WIN8_SDK_DIR}/Lib/win8/um/${ARCH}"
			NO_DEFAULT_PATH
			DOC "Path to the windows 8 d3d9.lib file"
		)
		
		if (D3D9_LIB)
			set (D3D9_FOUND "YES")
			set (D3D9_LIBRARIES ${D3D9_LIB})
			mark_as_advanced (D3D9_INCLUDE_PATH D3D9_LIB)
		endif (D3D9_LIB)
	endif (D3D9_INCLUDE_PATH)
	
	# Otherwise look for legacy installs
	if (NOT D3D9_FOUND)
		set (D3D9_INCLUDE_PATH NOTFOUND)
	
		find_path (D3D9_INCLUDE_PATH 
			NAMES d3d9.h
			PATHS "${LEGACY_SDK_DIR}/Include"
			NO_DEFAULT_PATH
			DOC "Path to the legacy d3d9.h file"
		)
		
		if (D3D9_INCLUDE_PATH)
			find_library (D3D9_LIB 
				NAMES d3d9
				PATHS "${LEGACY_SDK_DIR}/Lib/${ARCH}"
				NO_DEFAULT_PATH
				DOC "Path to the legacy d3d9.lib file"
			)

			find_library (D3DX9_LIB 
				NAMES d3dx9
				PATHS "${LEGACY_SDK_DIR}/Lib/${ARCH}"
				NO_DEFAULT_PATH
				DOC "Path to the legacy d3dx9.lib file"
			)

			find_library (DXERR9_LIB 
				NAMES dxerr dxerr9
				PATHS "${LEGACY_SDK_DIR}/Lib/${ARCH}"
				NO_DEFAULT_PATH
				DOC "Path to the legacy dxerr9x.lib file"
			)

			if (D3D9_LIB AND D3DX9_LIB AND DXERR9_LIB)
				set (D3D9_FOUND "YES")
				set (D3D9_LIBRARIES ${D3D9_LIB} ${D3DX9_LIB} ${DXERR9_LIB})
				mark_as_advanced (D3D9_INCLUDE_PATH D3D9_LIB)
			endif (D3D9_LIB AND D3DX9_LIB AND DXERR9_LIB)
		endif (D3D9_INCLUDE_PATH)
	endif (NOT D3D9_FOUND)
endif (WIN32)

if (D3D9_FOUND)	
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

