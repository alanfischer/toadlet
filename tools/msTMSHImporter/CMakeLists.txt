# msTMSHImporter tool

# Windows Platforms Only!
if (TOADLET_PLATFORM_WIN32)
	# Includes
	set (MS_INCLUDE "C:/Program Files/MilkShape 3D 1.8.4/msLib")
	set (MSTMSHEXPORTER_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR} ${TOADLET_INCLUDE_DIR} ${MS_INCLUDE})
	include_directories (${MSTMSHEXPORTER_INCLUDES})

	# Libraries
	set (MS_MODEL_LIB "NO")
	find_program (MS_EXE ms3d PATHS "C:/Program Files/MilkShape 3D 1.8.4")
	get_filename_component (MS_LIB ${MS_EXE} PATH)
	find_library (MSMODELLIB msModelLib PATHS "C:/Program Files/MilkShape 3D 1.8.4/mslib/lib")
	message ("MSMODELLIB=${MSMODELLIB}")
	set (MSTMSHEXPORTER_LIBS ${MS_MODEL_LIB} ${TOADLET_EGG_LIB_S} ${TOADLET_PEEPER_LIB_S} ${TOADLET_RIBBIT_LIB_S} ${TOADLET_TADPOLE_LIB_S})
	if (NOT MSMODELLIB)
		message (FATAL_ERROR "Could NOT find Milkshape SDK")
	endif (NOT MSMODELLIB)

	# Source files
	set (MSTMSHIMPORTER_SRC
		DlgMessage.cpp
		DlgOptions.cpp
		StdAfx.cpp
		msPlugInImpl.cpp
	)

	# Headers
	set (MSTMSHIMPORTER_H
		DlgMessage.h
		DlgOptions.h
		StdAfx.h
		msPlugInImpl.h
	)

	# Library
	add_library (msTMSHImporter ${MSTMSHIMPORTER_SRC} ${MSTMSHIMPORTER_H})
	set_target_properties (msTMSHImporter PROPERTIES COMPILE_FLAGS "/MT")
	target_link_libraries (msTMSHImporter ${MSTMSHIMPORTER_LIBS})
	install (TARGETS msTMSHImporter DESTINATION ${MS_LIB})
endif (TOADLET_PLATFORM_WIN32)

