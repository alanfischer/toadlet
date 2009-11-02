# Attempt to find Milkshape3D and it's SDK
# Defines:
#  
#  MS3D_FOUND	- we have Milkshape and the SDK
#  MS3D_SDK_DIR	- sdk include dir
#  MS3D_SDK_LIB	- sdk library file
#  MS3D_DIR	- installation library directory

set (MS3D_FOUND "NO")

# Milkshape3D is win32 only
if (WIN32)
	set (MS_PATHS
		"C:/Program Files/MiklShape 3D 1.8.5"
		"C:/Program Files/MiklShape 3D 1.8.4"
		"C:/Program Files/MiklShape 3D 1.8.3"
		"C:/Program Files/MiklShape 3D 1.8.2"
		"C:/Program Files/MiklShape 3D 1.8.1"
	)
	
	set (MS_HEADERS
		msLib.h
		msPlugIn.h
	)

	find_program (MS3D_EXE ms3d.exe PATHS ${MS_PATHS})

	get_filename_component (MS3D_LIB_DIR ${MS3D_EXE} PATH)

	find_path (MS3D_SDK_DIR ${MS_HEADERS} PATHS ${MS3D_LIB_DIR}/msLib)

	find_library (MS3D_SDK_LIB msModelLib PATHS ${MS3D_SDK_DIR} ${MS3D_SDK_DIR}/lib)

	mark_as_advanced (MS3D_SDK_DIR MS3D_SDK_LIB MS3D_DIR)

	if (MS3D_SDK_LIB AND MS3D_SDK_DIR)
		set (MS3D_FOUND "YES")
	endif (MS3D_SDK_LIB AND MS3D_SDK_DIR)
endif( WIN32 )

if (MS3D_FOUND)
	if (NOT MS3D_FIND_QUITELY)
		message (STATUS "Milkshape3D SDK found")
	endif (NOT MS3D_FIND_QUITELY)
else (MS3D_FOUND)
	if (MS3D_FIND_REQUIRED)
		message (FATAL_ERROR "Could NOT find Milkshape3D SDK")
	endif (MS3D_FIND_REQUIRED)
	if (NOT MS3D_FIND_QUITELY)
		message (STATUS "Could NOT find Milkshape3D SDK")
	endif (NOT MS3D_FIND_QUITELY)
endif (MS3D_FOUND)

