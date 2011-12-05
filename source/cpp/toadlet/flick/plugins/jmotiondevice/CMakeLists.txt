# Source files for toadlet_flick_jmotiondevice
set (FLICK_JMOTIONDEVICE_SRC
	JMotionDevice.cpp
	us_toadlet_flick.cpp
)


# Headers
set (FLICK_JMOTIONDEVICE_HEADERS
	JMotionDevice.h
)


source_group ("Source Files\\\\plugins\\\\jmotiondevice" FILES ${FLICK_JMOTIONDEVICE_SRC})
source_group ("Header Files\\\\plugins\\\\jmotiondevice" FILES ${FLICK_JMOTIONDEVICE_HEADERS})
set (FLICK_JMOTIONDEVICE_SRC ${FLICK_JMOTIONDEVICE_SRC})
set (FLICK_JMOTIONDEVICE_HEADERS ${FLICK_JMOTIONDEVICE_HEADERS})


# Dynamic library
add_library (toadlet_flick_jmotiondevice SHARED ${FLICK_JMOTIONDEVICE_SRC} ${FLICK_JMOTIONDEVICE_HEADERS})
set_target_properties (toadlet_flick_jmotiondevice PROPERTIES ${DYNAMIC_LIB_PROPS})
target_link_libraries (toadlet_flick_jmotiondevice toadlet_flick)
install (TARGETS toadlet_flick_jmotiondevice DESTINATION ${LIB_INSTALL_DIR})


# Static library
add_library (toadlet_flick_jmotiondevice_s STATIC ${FLICK_JMOTIONDEVICE_SRC} ${FLICK_JMOTIONDEVICE_HEADERS})
set_target_properties (toadlet_flick_jmotiondevice_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_flick_jmotiondevice_s toadlet_flick_s)
install (TARGETS toadlet_flick_jmotiondevice_s DESTINATION ${LIB_INSTALL_DIR})

