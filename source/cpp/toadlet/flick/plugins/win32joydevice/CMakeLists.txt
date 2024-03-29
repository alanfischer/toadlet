# Source files for toadlet_flick_win32joydevice
set (WIN32JOYDEVICE_SRC
	Win32JoyDevice.cpp
)


# Headers
set (WIN32JOYDEVICE_HDR
	Win32JoyDevice.h
)


# Dynamic library
if (TOADLET_BUILD_DYNAMIC)
	add_library (toadlet_flick_win32joydevice SHARED ${WIN32JOYDEVICE_SRC} ${WIN32JOYDEVICE_HDR})
	set_target_properties (toadlet_flick_win32joydevice PROPERTIES ${DYNAMIC_LIB_PROP})
	target_compile_definitions (toadlet_flick_win32joydevice PUBLIC ${DYNAMIC_LIB_DEFS})
	target_compile_options (toadlet_flick_win32joydevice PUBLIC ${DYNAMIC_LIB_FLAG})
	target_link_libraries (toadlet_flick_win32joydevice ${DYNAMIC_LIB_LINK} toadlet_flick)
	install (TARGETS toadlet_flick_win32joydevice DESTINATION ${LIB_INSTALL_DIR} COMPONENT flick)
	set (FLICK_INSTALL ${FLICK_INSTALL} toadlet_flick_win32joydevice PARENT_SCOPE)
endif (TOADLET_BUILD_DYNAMIC)

# Static library
if (TOADLET_BUILD_STATIC)
	add_library (toadlet_flick_win32joydevice_s STATIC ${WIN32JOYDEVICE_SRC} ${WIN32JOYDEVICE_HDR})
	set_target_properties (toadlet_flick_win32joydevice_s PROPERTIES ${STATIC_LIB_PROP})
	target_compile_definitions (toadlet_flick_win32joydevice_s PUBLIC ${STATIC_LIB_DEFS})
	target_compile_options (toadlet_flick_win32joydevice_s PUBLIC ${STATIC_LIB_FLAG})
	target_link_libraries (toadlet_flick_win32joydevice_s ${STATIC_LIB_LINK} toadlet_flick_s)
	install (TARGETS toadlet_flick_win32joydevice_s DESTINATION ${LIB_INSTALL_DIR} COMPONENT flick)
	set (FLICK_INSTALL ${FLICK_INSTALL} toadlet_flick_win32joydevice_s PARENT_SCOPE)
endif (TOADLET_BUILD_STATIC)
