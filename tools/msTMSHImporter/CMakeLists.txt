# msTMSHImporter tool

# Windows Platforms Only!
if (TOADLET_PLATFORM_WIN32)
	# Find Milkshape 3D and it's SDK
        find_package (Milkshape3D REQUIRED)

	# Includes
	set (MSTMSHEXPORTER_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR} ${TOADLET_INCLUDE_DIR} ${MS3D_SDK_DIR})
	include_directories (${MSTMSHEXPORTER_INCLUDES})

	# Libraries
	set (MSTMSHEXPORTER_LIBS ${MS3D_SDK_LIB} ${TOADLET_EGG_LIB_S} ${TOADLET_PEEPER_LIB_S} ${TOADLET_RIBBIT_LIB_S} ${TOADLET_TADPOLE_LIB_S})

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
	install (TARGETS msTMSHImporter DESTINATION ${MS3D_DIR})
endif (TOADLET_PLATFORM_WIN32)

