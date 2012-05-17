# Setup Versions
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/Version.h)


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


# If we are using swig create the wrapper file from the interface files
if (SWIG_FOUND AND TOADLET_KNOT_INT)
	swig_wrap_files (knot_wrap.cpp knot ${TOADLET_KNOT_INT} "${TOADLET_KNOT_INT_DEPS}")
	set (KNOT_SRC ${KNOT_SRC} knot_wrap.cpp)
endif (SWIG_FOUND AND TOADLET_KNOT_INT)


# Dynamic library
if (NOT TOADLET_PLATFORM_IOS)
	add_library (toadlet_knot SHARED ${KNOT_SRC} ${KNOT_HEADERS})
	set_target_properties (toadlet_knot PROPERTIES ${DYNAMIC_LIB_PROPS})
	target_link_libraries (toadlet_knot toadlet_egg)
	install (TARGETS toadlet_knot DESTINATION ${LIB_INSTALL_DIR})
endif (NOT TOADLET_PLATFORM_IOS)


# Static library
add_library (toadlet_knot_s STATIC ${KNOT_SRC} ${KNOT_HEADERS})
set_target_properties (toadlet_knot_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_knot_s toadlet_egg_s)
install (TARGETS toadlet_knot_s DESTINATION ${LIB_INSTALL_DIR})

# HACK: install the library on iOS. This will disappear once cmake issue:0012506 is resolved. 
if (TOADLET_PLATFORM_IOS)
    install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/\$ENV{CONFIGURATION}\$ENV{EFFECTIVE_PLATFORM_NAME}/ DESTINATION ${LIB_INSTALL_DIR} FILES_MATCHING PATTERN "*.a")
endif (TOADLET_PLATFORM_IOS)

