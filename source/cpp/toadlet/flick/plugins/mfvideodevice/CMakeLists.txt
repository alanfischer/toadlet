# Source files for toadlet_flick_mfvideodevice
set (MFVIDEODEVICE_SRC
	MFVideoDevice.cpp
)


# Headers
set (MFVIDEODEVICE_HEADERS
	MFVideoDevice.h
)

include_directories (${MEDIAFOUNDATION_INCLUDE_DIR})

# Dynamic library
if (TOADLET_BUILD_DYNAMIC)
	add_library (toadlet_flick_mfvideodevice SHARED ${MFVIDEODEVICE_SRC} ${MFVIDEODEVICE_HEADERS})
	set_target_properties (toadlet_flick_mfvideodevice PROPERTIES ${DYNAMIC_LIB_PROPS})
	set_property (TARGET toadlet_flick_mfvideodevice APPEND PROPERTY COMPILE_DEFINITIONS ${MFVIDEODEVICE_DEFS})
	target_link_libraries (toadlet_flick_mfvideodevice toadlet_flick ${MEDIAFOUNDATION_LIBRARIES})
	install (TARGETS toadlet_flick_mfvideodevice DESTINATION ${LIB_INSTALL_DIR})
endif (TOADLET_BUILD_DYNAMIC)

# Static library
if (TOADLET_BUILD_STATIC)
	add_library (toadlet_flick_mfvideodevice_s STATIC ${MFVIDEODEVICE_SRC} ${MFVIDEODEVICE_HEADERS})
	set_target_properties (toadlet_flick_mfvideodevice_s PROPERTIES ${STATIC_LIB_PROPS})
	target_link_libraries (toadlet_flick_mfvideodevice_s toadlet_flick_s)
	install (TARGETS toadlet_flick_mfvideodevice_s DESTINATION ${LIB_INSTALL_DIR})
endif (TOADLET_BUILD_STATIC)

