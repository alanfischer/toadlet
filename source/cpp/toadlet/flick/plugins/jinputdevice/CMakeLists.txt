# Source files for toadlet_flick_jinputdevice
set (FLICK_JINPUTDEVICE_SRC
	JInputDevice.cpp
	us_toadlet_flick.cpp
)


# Headers
set (FLICK_JINPUTDEVICE_HEADERS
	JInputDevice.h
)


source_group ("Source Files\\\\plugins\\\\jinputdevice" FILES ${FLICK_JINPUTDEVICE_SRC})
source_group ("Header Files\\\\plugins\\\\jinputdevice" FILES ${FLICK_JINPUTDEVICE_HEADERS})
set (FLICK_JINPUTDEVICE_SRC ${FLICK_JINPUTDEVICE_SRC})
set (FLICK_JINPUTDEVICE_HEADERS ${FLICK_JINPUTDEVICE_HEADERS})


# Dynamic library
add_library (toadlet_flick_jinputdevice SHARED ${FLICK_JINPUTDEVICE_SRC} ${FLICK_JINPUTDEVICE_HEADERS})
set_target_properties (toadlet_flick_jinputdevice PROPERTIES ${DYNAMIC_LIB_PROPS})
target_link_libraries (toadlet_flick_jinputdevice toadlet_flick)
install (TARGETS toadlet_flick_jinputdevice DESTINATION ${LIB_INSTALL_DIR})


# Static library
add_library (toadlet_flick_jinputdevice_s STATIC ${FLICK_JINPUTDEVICE_SRC} ${FLICK_JINPUTDEVICE_HEADERS})
set_target_properties (toadlet_flick_jinputdevice_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_flick_jinputdevice_s toadlet_flick_s)
install (TARGETS toadlet_flick_jinputdevice_s DESTINATION ${LIB_INSTALL_DIR})

