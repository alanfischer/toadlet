# msTMSHExporter tool

# Find Milkshape 3D and it's SDK
find_package (Milkshape3D)

# Windows Platforms Only!
if (MS3D_FOUND)
	# Includes
	set (MSTMSHEXPORTER_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR} ${TOADLET_INCLUDE_DIR} ${MS3D_SDK_DIR})
	include_directories (${MSTMSHEXPORTER_INCLUDES})

	# Libraries
	set (MSTMSHEXPORTER_LIBS ${MS3D_SDK_LIB} ${TOADLET_EGG_LIB_S} ${TOADLET_PEEPER_LIB_S} ${TOADLET_RIBBIT_LIB_S} ${TOADLET_TADPOLE_LIB_S})

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
	add_library (msTMSHExporter SHARED ${MSTMSHEXPORTER_SRC} ${MSTMSHEXPORTER_H})
	set_target_properties (msTMSHExporter PROPERTIES COMPILE_FLAGS "/MT")
	target_link_libraries (msTMSHExporter ${MSTMSHEXPORTER_LIBS})
	install (TARGETS msTMSHExporter DESTINATION ${MS3D_DIR})
endif (MS3D_FOUND)

