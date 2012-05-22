# Source files for toadlet_flick_win32joydevice
set (WIN32JOYDEVICE_SRC
	Win32JoyDevice.cpp
)


# Headers
set (WIN32JOYDEVICE_HEADERS
	Win32JoyDevice.h
)


# Dynamic library
if (TOADLET_BUILD_DYNAMIC)
	add_library (toadlet_flick_win32joydevice SHARED ${WIN32JOYDEVICE_SRC} ${WIN32JOYDEVICE_HEADERS})
	set_target_properties (toadlet_flick_win32joydevice PROPERTIES ${DYNAMIC_LIB_PROPS})
	set_property (TARGET toadlet_flick_win32joydevice APPEND PROPERTY COMPILE_DEFINITIONS ${WIN32JOYDEVICE_DEFS})
	target_link_libraries (toadlet_flick_win32joydevice toadlet_flick ${WIN32JOYDEVICE_LIBS})
	install (TARGETS toadlet_flick_win32joydevice DESTINATION ${LIB_INSTALL_DIR})
endif (TOADLET_BUILD_DYNAMIC)

# Static library
if (TOADLET_BUILD_STATIC)
	add_library (toadlet_flick_win32joydevice_s STATIC ${WIN32JOYDEVICE_SRC} ${WIN32JOYDEVICE_HEADERS})
	set_target_properties (toadlet_flick_win32joydevice_s PROPERTIES ${STATIC_LIB_PROPS})
	target_link_libraries (toadlet_flick_win32joydevice_s toadlet_flick_s)
	install (TARGETS toadlet_flick_win32joydevice_s DESTINATION ${LIB_INSTALL_DIR})
endif (TOADLET_BUILD_STATIC)
