# Setup Versions
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Version.h.in ${CMAKE_CURRENT_BINARY_DIR}/Version.h


# Source files for toadlet_hop
set (HOP_SRC
	Constraint.cpp
	Shape.cpp
	Simulator.cpp
	Solid.cpp
	Version.cpp
)


# Headers
set (HOP_HEADERS
	Collision.h
	CollisionListener.h
	Constraint.h
	Manager.h
	Shape.h
	Simulator.h
	Solid.h
	Types.h
	Version.h
)


# Dynamic library
if (NOT TOADLET_PLATFORM_IPHONE)
	add_library (toadlet_hop SHARED ${HOP_SRC} ${HOP_HEADERS})
	set_target_properties (toadlet_hop PROPERTIES ${DYNAMIC_LIB_PROPS})
	target_link_libraries (toadlet_hop toadlet_egg)
	install (TARGETS toadlet_hop DESTINATION lib)
endif (NOT TOADLET_PLATFORM_IPHONE)


# Static library
add_library (toadlet_hop_s STATIC ${HOP_SRC} ${HOP_HEADERS})
set_target_properties (toadlet_hop_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_hop_s toadlet_egg_s)
install (TARGETS toadlet_hop_s DESTINATION lib)

