# Source files for toadlet_flick_iphonemotiondevice
set (IOSMOTIONDEVICE_SRC
	IOSMotionDevice.mm
)


# Headers
set (IOSMOTIONDEVICE_HEADERS
	IOSMotionDevice.h
)


# Dynamic library
# Not available on iPhone platforms


# Static library
find_library (UIKIT_LIBS UIKit)
add_library (toadlet_flick_iosmotiondevice_s STATIC ${IOSMOTIONDEVICE_SRC} ${IOSMOTIONDEVICE_HEADERS})
set_target_properties (toadlet_flick_iosmotiondevice_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_flick_iosmotiondevice_s objc ${UIKIT_LIBS} toadlet_flick_s)
install (TARGETS toadlet_flick_iosmotiondevice_s DESTINATION lib)

