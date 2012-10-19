# Source files for toadlet_flick_htcmotiondevice
set (HTCMOTIONDEVICE_SRC
	HTCMotionDevice.cpp
)


# Header files
set (HTCMOTIONDEVICE_HEADERS
	HTCMotionDevice.h
)


# Dynamic library
add_library (toadlet_flick_htcmotiondevice SHARED ${HTCMOTIONDEVICE_SRC} ${HTCMOTIONDEVICE_HEADERS})
set_target_properties (toadlet_flick_htcmotiondevice PROPERTIES ${DYNAMIC_LIB_PROPS})
target_link_libraries (toadlet_flick_htcmotiondevice toadlet_flick)
install (TARGETS toadlet_flick_htcmotiondevice DESTINATION ${LIB_INSTALL_DIR})


# Static library
add_library (toadlet_flick_htcmotiondevice_s STATIC ${HTCMOTIONDEVICE_SRC} ${HTCMOTIONDEVICE_HEADERS})
set_target_properties (toadlet_flick_htcmotiondevice_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_flick_htcmotiondevice_s toadlet_flick_s)
install (TARGETS toadlet_flick_htcmotiondevice_s DESTINATION ${LIB_INSTALL_DIR})

