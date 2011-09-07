# Setup Versions
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/Version.h)
install (FILES  ${CMAKE_CURRENT_BINARY_DIR}/Version.h DESTINATION include/toadlet/hop)


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
	TraceCallback.h
	Types.h
	Version.h
)


# Give the headers and namespaces parent scope for inclusion into the master hop.h file
set (HOP_HEADERS ${HOP_HEADERS} PARENT_SCOPE)
set (HOP_NAMESPACES ${HOP_NAMESPACES} PARENT_SCOPE)


# Dynamic library
if (NOT TOADLET_PLATFORM_IOS)
	add_library (toadlet_hop SHARED ${HOP_SRC} ${HOP_HEADERS})
	set_target_properties (toadlet_hop PROPERTIES ${DYNAMIC_LIB_PROPS})
	target_link_libraries (toadlet_hop toadlet_egg)
	install (TARGETS toadlet_hop DESTINATION ${LIB_INSTALL_DIR})
endif (NOT TOADLET_PLATFORM_IOS)


# Static library
add_library (toadlet_hop_s STATIC ${HOP_SRC} ${HOP_HEADERS})
set_target_properties (toadlet_hop_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_hop_s toadlet_egg_s)
install (TARGETS toadlet_hop_s DESTINATION ${LIB_INSTALL_DIR})

