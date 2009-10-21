# Source files for toadlet_ribbit_audiereplayer
set (RIBBIT_AUDIEREPLAYER_SRC
	AudiereAudioPeer.cpp
	AudierePlayer.cpp
)


# Headers
set (RIBBIT_AUDIEREPLAYER_HEADERS
	AudiereAudioBufferPeer.h
	AudiereAudioPeer.h
	AudierePlayer.h
)


# Dynamic library
add_library (toadlet_ribbit_audiereplayer SHARED ${RIBBIT_AUDIEREPLAYER_SRC} ${RIBBIT_AUDIEREPLAYER_HEADERS})
set_target_properties (toadlet_ribbit_audiereplayer PROPERTIES ${DYNAMIC_LIB_PROPS})
target_link_libraries (toadlet_ribbit_audiereplayer toadlet_ribbit)
install (TARGETS toadlet_ribbit_audiereplayer DESTINATION lib)


# Static library
add_library (toadlet_ribbit_audiereplayer_s STATIC ${RIBBIT_AUDIEREPLAYER_SRC} ${RIBBIT_AUDIEREPLAYER_HEADERS})
set_target_properties (toadlet_ribbit_audiereplayer_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_ribbit_audiereplayer_s toadlet_ribbit_s)
install (TARGETS toadlet_ribbit_audiereplayer_s DESTINATION lib)

