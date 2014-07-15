# Source files for toadlet_flick_ioslineardevice
set (IOSLINEARDEVICE_SRC
	IOSLinearDevice.mm
)


# Headers
set (IOSLINEARDEVICE_HDR
	IOSLinearDevice.h
)


# Static library
if (TOADLET_BUILD_STATIC)
	find_library (UIKIT_LIBS UIKit)
	add_library (toadlet_flick_ioslineardevice_s STATIC ${IOSLINEARDEVICE_SRC} ${IOSLINEARDEVICE_HDR})
	set_target_properties (toadlet_flick_ioslineardevice_s PROPERTIES ${DYNAMIC_LIB_PROP})
	target_compile_definitions (toadlet_flick_ioslineardevice_s PUBLIC ${DYNAMIC_LIB_DEFS})
	target_compile_options (toadlet_flick_ioslineardevice_s PUBLIC ${DYNAMIC_LIB_FLAG})
	target_link_libraries (toadlet_flick_ioslineardevice_s ${DYNAMIC_LIB_LINK} toadlet_flick_s)
	install (TARGETS toadlet_flick_ioslineardevice_s DESTINATION ${LIB_INSTALL_DIR} COMPONENT flick)
	set (FLICK_INSTALL ${FLICK_INSTALL} toadlet_flick_ioslineardevice_s PARENT_SCOPE)

	# HACK: install the library on iOS. This will disappear once cmake issue:0012506 is resolved. 
	if (TOADLET_PLATFORM_IOS)
	    install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/\$ENV{CONFIGURATION}\$ENV{EFFECTIVE_PLATFORM_NAME}/ DESTINATION ${LIB_INSTALL_DIR} COMPONENT flick FILES_MATCHING PATTERN "*.a")
	endif (TOADLET_PLATFORM_IOS)
endif (TOADLET_BUILD_STATIC)
