# msTMSHExporter tool

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
	find_library (MSMODELLIB msModelLib PATHS "C:/Program Files/MilkShape 3D 1.8.4/msLib/lib")
	message ("MSMODELLIB=${MSMODELLIB}")
	set (MSTMSHEXPORTER_LIBS ${MS_MODEL_LIB} ${TOADLET_EGG_LIB_S} ${TOADLET_PEEPER_LIB_S} ${TOADLET_RIBBIT_LIB_S} ${TOADLET_TADPOLE_LIB_S})
	if (NOT MSMODELLIB)
		message (FATAL_ERROR "Could NOT find Milkshape SDK")
	endif (NOT MSMODELLIB)

	# Source files
	set (MSTMSHEXPORTER_SRC
		StdAfx.cpp
		msPlugInImpl.cpp
	)

	# Headers
	set (MSTMSHEXPORTER_H
		StdAfx.h
		msPlugInImpl.h
	)

	# Library
	add_library (msTMSHExporter ${MSTMSHEXPORTER_SRC} ${MSTMSHEXPORTER_H})
	set_target_properties (msTMSHExporter PROPERTIES COMPILE_FLAGS "/MT")
	target_link_libraries (msTMSHExporter ${MSTMSHEXPORTER_LIBS})
	install (TARGETS msTMSHExporter DESTINATION ${MS_LIB})
endif (TOADLET_PLATFORM_WIN32)

