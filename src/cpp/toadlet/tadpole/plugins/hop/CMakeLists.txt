# Source files for toadlet_tadpole_hop
set (TADPOLE_HOP_SRC
	HopEntity.cpp
	HopParticleSimulator.cpp
	HopScene.cpp
)


# Headers
set (TADPOLE_HOP_HEADERS
	HopEntity.h
	HopEntityFactory.h
	HopParticleSimulator.h
	HopScene.h
)


# Dynamic library
if (NOT TOADLET_PLATFORM_IPHONE)
	add_library (toadlet_tadpole_hop SHARED ${TADPOLE_HOP_SRC} ${TADPOLE_HOP_HEADERS})
	set_target_properties (toadlet_tadpole_hop PROPERTIES ${DYNAMIC_LIB_PROPS})
	target_link_libraries (toadlet_tadpole_hop toadlet_hop toadlet_knot toadlet_tadpole)
	install (TARGETS toadlet_tadpole_hop DESTINATION lib)
endif (NOT TOADLET_PLATFORM_IPHONE)


# Static library
add_library (toadlet_tadpole_hop_s STATIC ${TADPOLE_HOP_SRC} ${TADPOLE_HOP_HEADERS})
set_target_properties (toadlet_tadpole_hop_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_tadpole_hop_s toadlet_hop_s toadlet_knot_s toadlet_tadpole_s)
install (TARGETS toadlet_tadpole_hop_s DESTINATION lib)

