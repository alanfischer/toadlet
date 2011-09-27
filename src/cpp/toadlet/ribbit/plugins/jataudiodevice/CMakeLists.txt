# Source files for toadlet_ribbit_jataudiodevice
set (RIBBIT_JATAUDIODEVICE_SRC
	JATAudioBuffer.cpp
	JATAudio.cpp
	JATAudioDevice.cpp
)


# Headers
set (RIBBIT_JATAUDIODEVICE_HEADERS
	JATAudioBuffer.h
	JATAudio.h
	JATAudioDevice.h
)


source_group ("Source Files\\\\plugins\\\\jataudiodevice" FILES ${RIBBIT_JATAUDIODEVICE_SRC})
source_group ("Header Files\\\\plugins\\\\jataudiodevice" FILES ${RIBBIT_JATAUDIODEVICE_HEADERS})
set (RIBBIT_JATAUDIODEVICE_SRC ${RIBBIT_JATAUDIODEVICE_SRC})
set (RIBBIT_JATAUDIODEVICE_HEADERS ${RIBBIT_JATAUDIODEVICE_HEADERS})


# Dynamic library
add_library (toadlet_ribbit_jataudiodevice SHARED ${RIBBIT_JATAUDIODEVICE_SRC} ${RIBBIT_JATAUDIODEVICE_HEADERS})
set_target_properties (toadlet_ribbit_jataudiodevice PROPERTIES ${DYNAMIC_LIB_PROPS})
target_link_libraries (toadlet_ribbit_jataudiodevice toadlet_ribbit)
install (TARGETS toadlet_ribbit_jataudiodevice DESTINATION ${LIB_INSTALL_DIR})


# Static library
add_library (toadlet_ribbit_jataudiodevice_s STATIC ${RIBBIT_JATAUDIODEVICE_SRC} ${RIBBIT_JATAUDIODEVICE_HEADERS})
set_target_properties (toadlet_ribbit_jataudiodevice_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_ribbit_jataudiodevice_s toadlet_ribbit_s)
install (TARGETS toadlet_ribbit_jataudiodevice_s DESTINATION ${LIB_INSTALL_DIR})

