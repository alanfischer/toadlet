# Source files for toadlet_ribbit_jaudiodevice
set (RIBBIT_JAUDIODEVICE_SRC
	JAudioBuffer.cpp
	JAudio.cpp
	JAudioDevice.cpp
)


# Headers
set (RIBBIT_JAUDIODEVICE_HDR
	JAudioBuffer.h
	JAudio.h
	JAudioDevice.h
)


source_group ("Source Files\\\\plugins\\\\jaudiodevice" FILES ${RIBBIT_JAUDIODEVICE_SRC})
source_group ("Header Files\\\\plugins\\\\jaudiodevice" FILES ${RIBBIT_JAUDIODEVICE_HDR})
set (RIBBIT_JAUDIODEVICE_SRC ${RIBBIT_JAUDIODEVICE_SRC})
set (RIBBIT_JAUDIODEVICE_HDR ${RIBBIT_JAUDIODEVICE_HDR})


# Dynamic library
if (TOADLET_BUILD_DYNAMIC)
	add_library (toadlet_ribbit_jaudiodevice SHARED ${RIBBIT_JAUDIODEVICE_SRC} ${RIBBIT_JAUDIODEVICE_HDR})
	set_target_properties (toadlet_ribbit_jaudiodevice PROPERTIES ${DYNAMIC_LIB_PROP})
	target_compile_definitions (toadlet_ribbit_jaudiodevice ${DYNAMIC_LIB_DEFS})
	target_compile_options (toadlet_ribbit_jaudiodevice ${DYNAMIC_LIB_FLAG})
	target_link_libraries (toadlet_ribbit_jaudiodevice ${DYNAMIC_LIB_LINK} toadlet_ribbit)
	install (TARGETS toadlet_ribbit_jaudiodevice DESTINATION ${LIB_INSTALL_DIR} COMPONENT ribbit)
	set (RIBBIT_INSTALL ${RIBBIT_INSTALL} toadlet_ribbit_jaudiodevice PARENT_SCOPE)
endif (TOADLET_BUILD_DYNAMIC)


# Static library
if (TOADLET_BUILD_STATIC)
	add_library (toadlet_ribbit_jaudiodevice_s STATIC ${RIBBIT_JAUDIODEVICE_SRC} ${RIBBIT_JAUDIODEVICE_HDR})
	set_target_properties (toadlet_ribbit_jaudiodevice_s PROPERTIES ${STATIC_LIB_PROP})
	target_compile_definitions (toadlet_ribbit_jaudiodevice_s ${STATIC_LIB_DEFS})
	target_compile_options (toadlet_ribbit_jaudiodevice_s ${STATIC_LIB_FLAG})
	target_link_libraries (toadlet_ribbit_jaudiodevice_s ${STATIC_LIB_LINK} toadlet_ribbit_s)
	install (TARGETS toadlet_ribbit_jaudiodevice_s DESTINATION ${LIB_INSTALL_DIR} COMPONENT ribbit)
	set (RIBBIT_INSTALL ${RIBBIT_INSTALL} toadlet_ribbit_jaudiodevice_s PARENT_SCOPE)
endif (TOADLET_BUILD_STATIC)

