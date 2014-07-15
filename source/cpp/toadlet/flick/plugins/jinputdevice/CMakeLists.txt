# Source files for toadlet_flick_jinputdevice
set (FLICK_JINPUTDEVICE_SRC
	JInputDevice.cpp
)


# Headers
set (FLICK_JINPUTDEVICE_HDR
	JInputDevice.h
)


source_group ("Source Files\\\\plugins\\\\jinputdevice" FILES ${FLICK_JINPUTDEVICE_SRC})
source_group ("Header Files\\\\plugins\\\\jinputdevice" FILES ${FLICK_JINPUTDEVICE_HDR})
set (FLICK_JINPUTDEVICE_SRC ${FLICK_JINPUTDEVICE_SRC})
set (FLICK_JINPUTDEVICE_HDR ${FLICK_JINPUTDEVICE_HDR})


# Dynamic library
if (TOADLET_BUILD_DYNAMIC)
	add_library (toadlet_flick_jinputdevice SHARED ${FLICK_JINPUTDEVICE_SRC} ${FLICK_JINPUTDEVICE_HDR})
	set_target_properties (toadlet_flick_jinputdevice PROPERTIES ${DYNAMIC_LIB_PROP})
	target_compile_definitions (toadlet_flick_jinputdevice PUBLIC ${DYNAMIC_LIB_DEFS})
	target_compile_options (toadlet_flick_jinputdevice PUBLIC ${DYNAMIC_LIB_FLAG})
	target_link_libraries (toadlet_flick_jinputdevice ${DYNAMIC_LIB_LINK} toadlet_flick)
	install (TARGETS toadlet_flick_jinputdevice DESTINATION ${LIB_INSTALL_DIR} COMPONENT flick)
	set (FLICK_INSTALL ${FLICK_INSTALL} toadlet_flick_jinputdevice PARENT_SCOPE)
endif (TOADLET_BUILD_DYNAMIC)

# Static library
if (TOADLET_BUILD_STATIC)
	add_library (toadlet_flick_jinputdevice_s STATIC ${FLICK_JINPUTDEVICE_SRC} ${FLICK_JINPUTDEVICE_HDR})
	set_target_properties (toadlet_flick_jinputdevice_s PROPERTIES ${STATIC_LIB_PROP})
	target_compile_definitions (toadlet_flick_jinputdevice_s PUBLIC ${STATIC_LIB_DEFS})
	target_compile_options (toadlet_flick_jinputdevice_s PUBLIC ${STATIC_LIB_FLAG})
	target_link_libraries (toadlet_flick_jinputdevice_s ${STATIC_LIB_LINK} toadlet_flick_s)
	install (TARGETS toadlet_flick_jinputdevice_s DESTINATION ${LIB_INSTALL_DIR} COMPONENT flick)
	set (FLICK_INSTALL ${FLICK_INSTALL} toadlet_flick_jinputdevice_s PARENT_SCOPE)
endif (TOADLET_BUILD_STATIC)

