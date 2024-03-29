# Source files for toadlet_flick_androidsensordevice
set (FLICK_ANDROIDSENSORDEVICE_SRC
	AndroidSensorDevice.cpp
)


# Headers
set (FLICK_ANDROIDSENSORDEVICE_HDR
	AndroidSensorDevice.h
)


source_group ("Source Files\\\\plugins\\\\androidsensordevice" FILES ${FLICK_ANDROIDSENSORDEVICE_SRC})
source_group ("Header Files\\\\plugins\\\\androidsensordevice" FILES ${FLICK_ANDROIDSENSORDEVICE_HDR})
set (FLICK_ANDROIDSENSORDEVICE_SRC ${FLICK_ANDROIDSENSORDEVICE_SRC})
set (FLICK_ANDROIDSENSORDEVICE_HDR ${FLICK_ANDROIDSENSORDEVICE_HDR})


# Dynamic library
if (TOADLET_BUILD_DYNAMIC)
	add_library (toadlet_flick_androidsensordevice SHARED ${FLICK_ANDROIDSENSORDEVICE_SRC} ${FLICK_ANDROIDSENSORDEVICE_HDR})
	set_target_properties (toadlet_flick_androidsensordevice PROPERTIES ${DYNAMIC_LIB_PROP})
	target_compile_definitions (toadlet_flick_androidsensordevice PUBLIC ${DYNAMIC_LIB_DEFS})
	target_compile_options (toadlet_flick_androidsensordevice PUBLIC ${DYNAMIC_LIB_FLAG})
	target_link_libraries (toadlet_flick_androidsensordevice ${DYNAMIC_LIB_LINK} toadlet_flick android)
	install (TARGETS toadlet_flick_androidsensordevice DESTINATION ${LIB_INSTALL_DIR} COMPONENT flick)
	set (FLICK_INSTALL ${FLICK_INSTALL} toadlet_flick_androidsensordevice PARENT_SCOPE)
endif (TOADLET_BUILD_DYNAMIC)


# Static library
if (TOADLET_BUILD_STATIC)
	add_library (toadlet_flick_androidsensordevice_s STATIC ${FLICK_ANDROIDSENSORDEVICE_SRC} ${FLICK_ANDROIDSENSORDEVICE_HDR})	
	set_target_properties (toadlet_flick_androidsensordevice_s PROPERTIES ${STATIC_LIB_PROP})
	target_compile_definitions (toadlet_flick_androidsensordevice_s PUBLIC ${STATIC_LIB_DEFS})
	target_compile_options (toadlet_flick_androidsensordevice_s PUBLIC ${STATIC_LIB_FLAG})
	target_link_libraries (toadlet_flick_androidsensordevice_s ${STATIC_LIB_LINK} toadlet_flick_s android)
	install (TARGETS toadlet_flick_androidsensordevice_s DESTINATION ${LIB_INSTALL_DIR} COMPONENT flick)
	set (FLICK_INSTALL ${FLICK_INSTALL} toadlet_flick_androidsensordevice_s PARENT_SCOPE)
endif (TOADLET_BUILD_STATIC)

