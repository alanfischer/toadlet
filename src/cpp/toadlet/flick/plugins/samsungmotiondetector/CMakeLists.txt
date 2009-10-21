# Source files for toadlet_flick_samsungmotiondetector
set (SAMSUNGMOTIONDETECTOR_SRC
	SamsungMotionDetector.cpp
)


# Headers
set (SAMSUNGMOTIONDETECTOR_HEADERS
	SamsungMotionDetector.h
)


# Dynamic library
add_library (toadlet_flick_samsungmotiondetector SHARED ${SAMSUNGMOTIONDETECTOR_SRC} ${SAMSUNGMOTIONDETECTOR_HEADERS})
set_target_properties (toadlet_flick_samsungmotiondetector PROPERTIES ${DYNAMIC_LIB_PROPS})
target_link_libraries (toadlet_flick_samsungmotiondetector toadlet_flick)
install (TARGETS toadlet_flick_samsungmotiondetector DESTINATION lib)


# Static library
add_library (toadlet_flick_samsungmotiondetector_s STATIC ${SAMSUNGMOTIONDETECTOR_SRC} ${SAMSUNGMOTIONDETECTOR_HEADERS})
set_target_properties (toadlet_flick_samsungmotiondetector_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_flick_samsungmotiondetector_s toadlet_flick_s)
install (TARGETS toadlet_flick_samsungmotiondetector_s DESTINATION lib)

