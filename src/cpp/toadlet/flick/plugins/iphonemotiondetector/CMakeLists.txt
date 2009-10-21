# Source files for toadlet_flick_iphonemotiondetector
set (IPHONEMOTIONDETECTOR_SRC
	IPhoneMotionDetector.mm
)


# Headers
set (IPHONEMOTIONDETECTOR_HEADERS
	IPhoneMotionDetector.h
)


# Dynamic library
# Not available on iPhone platforms


# Static library
find_library (UIKIT_LIBS UIKit)
add_library (toadlet_flick_iphonemotiondetector_s STATIC ${IPHONEMOTIONDETECTOR_SRC} ${IPHONEMOTIONDETECTOR_HEADERS})
set_target_properties (toadlet_flick_iphonemotiondetector_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_flick_iphonemotiondetector_s objc ${UIKIT_LIBS} toadlet_flick_s)
install (TARGETS toadlet_flick_iphonemotiondetector_s DESTINATION lib)

