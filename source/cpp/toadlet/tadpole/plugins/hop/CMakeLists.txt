# Source files for toadlet_tadpole_hop
set (TADPOLE_HOP_SRC
	HopEntity.cpp
	HopParticleSimulator.cpp
	HopScene.cpp
)


# Headers
set (TADPOLE_HOP_HEADERS
	HopEntity.h
	HopParticleSimulator.h
	HopScene.h
)


# Dynamic library
if (TOADLET_BUILD_DYNAMIC)
	add_library (toadlet_tadpole_hop SHARED ${TADPOLE_HOP_SRC} ${TADPOLE_HOP_HEADERS})
	set_target_properties (toadlet_tadpole_hop PROPERTIES ${DYNAMIC_LIB_PROPS})
	target_link_libraries (toadlet_tadpole_hop toadlet_hop toadlet_knot toadlet_tadpole)
	install (TARGETS toadlet_tadpole_hop DESTINATION ${LIB_INSTALL_DIR})
endif (TOADLET_BUILD_DYNAMIC)


# Static library
if (TOADLET_BUILD_STATIC)
	add_library (toadlet_tadpole_hop_s STATIC ${TADPOLE_HOP_SRC} ${TADPOLE_HOP_HEADERS})
	set_target_properties (toadlet_tadpole_hop_s PROPERTIES ${STATIC_LIB_PROPS})
	target_link_libraries (toadlet_tadpole_hop_s toadlet_hop_s toadlet_knot_s toadlet_tadpole_s)
	install (TARGETS toadlet_tadpole_hop_s DESTINATION ${LIB_INSTALL_DIR})

	# HACK: install the library on iOS. This will disappear once cmake issue:0012506 is resolved. 
	if (TOADLET_PLATFORM_IOS)
	    install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/\$ENV{CONFIGURATION}\$ENV{EFFECTIVE_PLATFORM_NAME}/ DESTINATION ${LIB_INSTALL_DIR} FILES_MATCHING PATTERN "*.a")
	endif (TOADLET_PLATFORM_IOS)
endif (TOADLET_BUILD_STATIC)

