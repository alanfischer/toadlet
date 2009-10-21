# Source files for toadlet_flick_htcmotiondetector
set (HTCMOTIONDETECTOR_SRC
	HTCMotionDetector.cpp
)


# Header files
set (HTCMOTIONDETECTOR_HEADERS
	HTCMotionDetector.h
)


# Dynamic library
add_library (toadlet_flick_htcmotiondetector SHARED ${HTCMOTIONDETECTOR_SRC} ${HTCMOTIONDETECTOR_HEADERS})
set_target_properties (toadlet_flick_htcmotiondetector PROPERTIES ${DYNAMIC_LIB_PROPS})
target_link_libraries (toadlet_flick_htcmotiondetector toadlet_flick)
install (TARGETS toadlet_flick_htcmotiondetector DESTINATION lib)


# Static library
add_library (toadlet_flick_htcmotiondetector_s STATIC ${HTCMOTIONDETECTOR_SRC} ${HTCMOTIONDETECTOR_HEADERS})
set_target_properties (toadlet_flick_htcmotiondetector_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_flick_htcmotiondetector_s toadlet_flick_s)
install (TARGETS toadlet_flick_htcmotiondetector_s DESTINATION lib)

