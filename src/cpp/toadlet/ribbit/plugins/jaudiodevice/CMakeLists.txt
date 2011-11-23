# Source files for toadlet_ribbit_jaudiodevice
set (RIBBIT_JAUDIODEVICE_SRC
	JAudioBuffer.cpp
	JAudio.cpp
	JAudioDevice.cpp
	us_toadlet_ribbit.cpp
)


# Headers
set (RIBBIT_JAUDIODEVICE_HEADERS
	JAudioBuffer.h
	JAudio.h
	JAudioDevice.h
)


source_group ("Source Files\\\\plugins\\\\jaudiodevice" FILES ${RIBBIT_JAUDIODEVICE_SRC})
source_group ("Header Files\\\\plugins\\\\jaudiodevice" FILES ${RIBBIT_JAUDIODEVICE_HEADERS})
set (RIBBIT_JAUDIODEVICE_SRC ${RIBBIT_JAUDIODEVICE_SRC})
set (RIBBIT_JAUDIODEVICE_HEADERS ${RIBBIT_JAUDIODEVICE_HEADERS})


# Dynamic library
add_library (toadlet_ribbit_jaudiodevice SHARED ${RIBBIT_JAUDIODEVICE_SRC} ${RIBBIT_JAUDIODEVICE_HEADERS})
set_target_properties (toadlet_ribbit_jaudiodevice PROPERTIES ${DYNAMIC_LIB_PROPS})
target_link_libraries (toadlet_ribbit_jaudiodevice toadlet_ribbit)
install (TARGETS toadlet_ribbit_jaudiodevice DESTINATION ${LIB_INSTALL_DIR})


# Static library
add_library (toadlet_ribbit_jaudiodevice_s STATIC ${RIBBIT_JAUDIODEVICE_SRC} ${RIBBIT_JAUDIODEVICE_HEADERS})
set_target_properties (toadlet_ribbit_jaudiodevice_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_ribbit_jaudiodevice_s toadlet_ribbit_s)
install (TARGETS toadlet_ribbit_jaudiodevice_s DESTINATION ${LIB_INSTALL_DIR})

