# msTMSHImporter tool

# Find Milkshape 3D and it's SDK
find_package (Milkshape3D)

if (MS3D_FOUND)
	# Includes
	set (MSTMSHIMPORTER_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR} ${TOADLET_INCLUDE_DIR} ${MS3D_SDK_DIR})
	include_directories (${MSTMSHIMPORTER_INCLUDES})

	# Libraries
	set (MSTMSHIMPORTER_LIBS ${MS3D_SDK_LIB} ${TOADLET_EGG_LIB_S} ${TOADLET_PEEPER_LIB_S} ${TOADLET_RIBBIT_LIB_S} ${TOADLET_TADPOLE_LIB_S})

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
	add_library (msTMSHImporter SHARED ${MSTMSHIMPORTER_SRC} ${MSTMSHIMPORTER_H})
	set_target_properties (msTMSHImporter PROPERTIES COMPILE_FLAGS "/MT")
	target_link_libraries (msTMSHImporter ${MSTMSHIMPORTER_LIBS})
	install (TARGETS msTMSHImporter DESTINATION ${MS3D_DIR})
endif (MS3D_FOUND)

