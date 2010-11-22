# Source files for toadlet_ribbit_mmplayer
set (RIBBIT_MMPLAYER_SRC
	MMAudioBuffer.cpp
	MMAudio.cpp
	MMPlayer.cpp
)


# Headers
set (RIBBIT_MMPLAYER_HEADERS
	MMAudioBuffer.h
	MMAudio.h
	MMPlayer.h
)


source_group ("Source Files\\\\plugins\\\\mmplayer" FILES ${RIBBIT_MMPLAYER_SRC})
source_group ("Header Files\\\\plugins\\\\mmplayer" FILES ${RIBBIT_MMPLAYER_HEADERS})
set (RIBBIT_MMPLAYER_SRC ${RIBBIT_MMPLAYER_SRC})
set (RIBBIT_MMPLAYER_HEADERS ${RIBBIT_MMPLAYER_HEADERS})


# Dynamic library
add_library (toadlet_ribbit_mmplayer SHARED ${RIBBIT_MMPLAYER_SRC} ${RIBBIT_MMPLAYER_HEADERS})
set_target_properties (toadlet_ribbit_mmplayer PROPERTIES ${DYNAMIC_LIB_PROPS})
target_link_libraries (toadlet_ribbit_mmplayer toadlet_ribbit)
install (TARGETS toadlet_ribbit_mmplayer DESTINATION lib)


# Static library
add_library (toadlet_ribbit_mmplayer_s STATIC ${RIBBIT_MMPLAYER_SRC} ${RIBBIT_MMPLAYER_HEADERS})
set_target_properties (toadlet_ribbit_mmplayer_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_ribbit_mmplayer_s toadlet_ribbit_s)
install (TARGETS toadlet_ribbit_mmplayer_s DESTINATION lib)

