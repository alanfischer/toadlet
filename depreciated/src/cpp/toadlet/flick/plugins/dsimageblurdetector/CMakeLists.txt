# Source files for toadlet_flick_dsimageblurdetector
set (DSIMAGEBLURDETECTOR_SRC
	DSImageBlurDetector.cpp
	PropertyBag.cpp
	dump.cpp
)


# Header files
set (DSIMAGEBLURDETECTOR_HEADERS
	DSImageBlurDetector.h
	DSIncludes.h
	PropertyBag.h
	dump.h
	dumpuids.h
)


# Special flags
add_definitions ("/Zc:wchar_t-")


# Dynamic library
add_library (toadlet_flick_dsimageblurdetector SHARED ${DSIMAGEBLURDETECTOR_SRC} ${DSIMAGEBLURDETECTOR_HEADERS})
set_target_properties (toadlet_flick_dsimageblurdetector PROPERTIES ${DYNAMIC_LIB_PROPS})
target_link_libraries (toadlet_flick_dsimageblurdetector toadlet_flick)
install (TARGETS toadlet_flick_dsimageblurdetector DESTINATION lib)


# Static library
add_library (toadlet_flick_dsimageblurdetector_s STATIC ${DSIMAGEBLURDETECTOR_SRC} ${DSIMAGEBLURDETECTOR_HEADERS})
set_target_properties (toadlet_flick_dsimageblurdetector_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_flick_dsimageblurdetector_s toadlet_flick_s)
install (TARGETS toadlet_flick_dsimageblurdetector_s DESTINATION lib)

