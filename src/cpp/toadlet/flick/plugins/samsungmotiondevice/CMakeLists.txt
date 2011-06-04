# Source files for toadlet_flick_samsungmotiondevice
set (SAMSUNGMOTIONDEVICE_SRC
	SamsungMotionDevice.cpp
)


# Headers
set (SAMSUNGMOTIONDEVICE_HEADERS
	SamsungMotionDevice.h
)


# Dynamic library
add_library (toadlet_flick_samsungmotiondevice SHARED ${SAMSUNGMOTIONDEVICE_SRC} ${SAMSUNGMOTIONDEVICE_HEADERS})
set_target_properties (toadlet_flick_samsungmotiondevice PROPERTIES ${DYNAMIC_LIB_PROPS})
target_link_libraries (toadlet_flick_samsungmotiondevice toadlet_flick)
install (TARGETS toadlet_flick_samsungmotiondevice DESTINATION lib)


# Static library
add_library (toadlet_flick_samsungmotiondevice_s STATIC ${SAMSUNGMOTIONDEVICE_SRC} ${SAMSUNGMOTIONDEVICE_HEADERS})
set_target_properties (toadlet_flick_samsungmotiondevice_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_flick_samsungmotiondevice_s toadlet_flick_s)
install (TARGETS toadlet_flick_samsungmotiondevice_s DESTINATION lib)

