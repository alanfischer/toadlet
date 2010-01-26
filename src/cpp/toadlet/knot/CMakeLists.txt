# Setup Versions
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Version.h.in ${CMAKE_CURRENT_BINARY_DIR}/Version.h)


# Source files for toadlet_knot
set (KNOT_SRC
	DebugListener.cpp
	LANPeerEventConnector.cpp
	SimpleClient.cpp
	SimpleEventConnection.cpp
	SimpleServer.cpp
	SynchronizedPeerEventConnection.cpp
	PeerPacketConnection.cpp
	TCPConnection.cpp
	TCPConnector.cpp
	Version.cpp
)


# Headers
set (KNOT_HEADERS
	Connection.h
	Connector.h
	ConnectorListener.h
	DebugListener.h
	EventConnection.h
	LANPeerEventConnector.h
	SimpleClient.h
	SimpleEventConnection.h
	SimpleServer.h
	SynchronizedPeerEventConnection.h
	PeerPacketConnection.h
	TCPConnection.h
	TCPConnector.h
	Version.h
)


# Dynamic library
if (NOT TOADLET_PLATFORM_IPHONE)
	add_library (toadlet_knot SHARED ${KNOT_SRC} ${KNOT_HEADERS})
	set_target_properties (toadlet_knot PROPERTIES ${DYNAMIC_LIB_PROPS})
	target_link_libraries (toadlet_knot toadlet_egg)
	install (TARGETS toadlet_knot DESTINATION lib)
endif (NOT TOADLET_PLATFORM_IPHONE)


# Static library
add_library (toadlet_knot_s STATIC ${KNOT_SRC} ${KNOT_HEADERS})
set_target_properties (toadlet_knot_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_knot_s toadlet_egg_s)
install (TARGETS toadlet_knot_s DESTINATION lib)

