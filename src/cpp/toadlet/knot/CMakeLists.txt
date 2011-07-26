# Setup Versions
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/Version.h)
install (FILES  ${CMAKE_CURRENT_BINARY_DIR}/Version.h DESTINATION include/toadlet/knot)


# Source files for toadlet_knot
set (KNOT_SRC
	DebugListener.cpp
	LANPeerEventConnector.cpp
	SimpleClient.cpp
	SimpleEventConnection.cpp
	SimplePredictedClient.cpp
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
	ConnectionListener.h
	Connector.h
	DebugListener.h
	EventConnection.h
	EventConnectionListener.h
	LANPeerEventConnector.h
	SimpleClient.h
	SimpleEventConnection.h
	SimplePredictedClient.h
	SimpleServer.h
	SynchronizedPeerEventConnection.h
	PeerPacketConnection.h
	TCPConnection.h
	TCPConnector.h
	Version.h
)


# Give the headers parent scope for inclusion into the master knot.h file
set (KNOT_HEADERS ${KNOT_HEADERS} PARENT_SCOPE)


# Dynamic library
if (NOT TOADLET_PLATFORM_IOS)
	add_library (toadlet_knot SHARED ${KNOT_SRC} ${KNOT_HEADERS})
	set_target_properties (toadlet_knot PROPERTIES ${DYNAMIC_LIB_PROPS})
	target_link_libraries (toadlet_knot toadlet_egg)
	install (TARGETS toadlet_knot DESTINATION lib)
endif (NOT TOADLET_PLATFORM_IOS)


# Static library
add_library (toadlet_knot_s STATIC ${KNOT_SRC} ${KNOT_HEADERS})
set_target_properties (toadlet_knot_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_knot_s toadlet_egg_s)
install (TARGETS toadlet_knot_s DESTINATION lib)

