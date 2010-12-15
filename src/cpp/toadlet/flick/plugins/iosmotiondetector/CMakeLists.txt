# Source files for toadlet_flick_iphonemotiondetector
set (IOSMOTIONDETECTOR_SRC
	IOSMotionDetector.mm
)


# Headers
set (IOSMOTIONDETECTOR_HEADERS
	IOSMotionDetector.h
)


# Dynamic library
# Not available on iPhone platforms


# Static library
find_library (UIKIT_LIBS UIKit)
add_library (toadlet_flick_iosmotiondetector_s STATIC ${IOSMOTIONDETECTOR_SRC} ${IOSMOTIONDETECTOR_HEADERS})
set_target_properties (toadlet_flick_iosmotiondetector_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_flick_iosmotiondetector_s objc ${UIKIT_LIBS} toadlet_flick_s)
install (TARGETS toadlet_flick_iosmotiondetector_s DESTINATION lib)

