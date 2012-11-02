# Source files for toadlet_ribbit_mmaudiodevice
set (RIBBIT_MMAUDIODEVICE_SRC
	MMAudioBuffer.cpp
	MMAudio.cpp
	MMAudioDevice.cpp
)


# Headers
set (RIBBIT_MMAUDIODEVICE_HDR
	MMAudioBuffer.h
	MMAudio.h
	MMAudioDevice.h
)


source_group ("Source Files\\\\plugins\\\\mmaudiodevice" FILES ${RIBBIT_MMAUDIODEVICE_SRC})
source_group ("Header Files\\\\plugins\\\\mmaudiodevice" FILES ${RIBBIT_MMAUDIODEVICE_HDR})
set (RIBBIT_MMAUDIODEVICE_SRC ${RIBBIT_MMAUDIODEVICE_SRC})
set (RIBBIT_MMAUDIODEVICE_HDR ${RIBBIT_MMAUDIODEVICE_HDR})


# Dynamic library
if (TOADLET_BUILD_DYNAMIC)
	add_library (toadlet_ribbit_mmaudiodevice SHARED ${RIBBIT_MMAUDIODEVICE_SRC} ${RIBBIT_MMAUDIODEVICE_HDR})
	set_target_properties (toadlet_ribbit_mmaudiodevice PROPERTIES ${DYNAMIC_LIB_PROPS})
	target_link_libraries (toadlet_ribbit_mmaudiodevice toadlet_ribbit)
	install (TARGETS toadlet_ribbit_mmaudiodevice DESTINATION ${LIB_INSTALL_DIR} COMPONENT ribbit)
	set (RIBBIT_INSTALL ${RIBBIT_INSTALL} toadlet_ribbit_mmaudiodevice PARENT_SCOPE)
endif (TOADLET_BUILD_DYNAMIC)


# Static library
if (TOADLET_BUILD_STATIC)
	add_library (toadlet_ribbit_mmaudiodevice_s STATIC ${RIBBIT_MMAUDIODEVICE_SRC} ${RIBBIT_MMAUDIODEVICE_HDR})
	set_target_properties (toadlet_ribbit_mmaudiodevice_s PROPERTIES ${STATIC_LIB_PROPS})
	target_link_libraries (toadlet_ribbit_mmaudiodevice_s toadlet_ribbit_s)
	install (TARGETS toadlet_ribbit_mmaudiodevice_s DESTINATION ${LIB_INSTALL_DIR} COMPONENT ribbit)
	set (RIBBIT_INSTALL ${RIBBIT_INSTALL} toadlet_ribbit_mmaudiodevice_s PARENT_SCOPE)
endif (TOADLET_BUILD_STATIC)

