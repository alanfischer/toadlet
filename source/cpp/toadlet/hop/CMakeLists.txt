# Setup Versions
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/Version.h)


# Source files for toadlet_hop
set (HOP_SRC
	Constraint.cpp
	Shape.cpp
	Simulator.cpp
	Solid.cpp
	Version.cpp
)


# Headers
set (HOP_HDR
	Collision.h
	CollisionListener.h
	Constraint.h
	Manager.h
	Shape.h
	Simulator.h
	Solid.h
	Traceable.h
	Types.h
	Version.h
)


# Give the headers and namespaces parent scope for inclusion into the master hop.h file
set (HOP_HDR ${HOP_HDR} PARENT_SCOPE)
set (HOP_NAMESPACES ${HOP_NAMESPACES} PARENT_SCOPE)


# If we are using swig create the wrapper file from the interface files
if (SWIG_FOUND AND HOP_SWIG_INT)
	swig_wrap_files (hop_wrap.cpp hop ${HOP_SWIG_INT} "${HOP_SWIG_INT_DEPS}" "${HOP_SWIG_INT_OUTPUTS}")	
	set (HOP_SRC ${HOP_SRC} hop_wrap.cpp)
endif (SWIG_FOUND AND HOP_SWIG_INT)


# For individual component installation 
set (HOP_INSTALL)


# Dynamic library
if (TOADLET_BUILD_DYNAMIC)
	add_library (toadlet_hop SHARED ${HOP_SRC} ${HOP_HDR})
	set_target_properties (toadlet_hop PROPERTIES ${DYNAMIC_LIB_PROPS})
	target_link_libraries (toadlet_hop toadlet_egg)
	install (TARGETS toadlet_hop DESTINATION ${LIB_INSTALL_DIR} COMPONENT hop)
	set (HOP_INSTALL ${HOP_INSTALL} toadlet_hop)
endif (TOADLET_BUILD_DYNAMIC)


# Static library
if (TOADLET_BUILD_STATIC)
	add_library (toadlet_hop_s STATIC ${HOP_SRC} ${HOP_HDR})
	set_target_properties (toadlet_hop_s PROPERTIES ${STATIC_LIB_PROPS})
	target_link_libraries (toadlet_hop_s toadlet_egg_s)
	install (TARGETS toadlet_hop_s DESTINATION ${LIB_INSTALL_DIR} COMPONENT hop)
	set (HOP_INSTALL ${HOP_INSTALL} toadlet_hop_s)

	# HACK: install the library on iOS. This will disappear once cmake issue:0012506 is resolved. 
	if (TOADLET_PLATFORM_IOS)
	    install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/\$ENV{CONFIGURATION}\$ENV{EFFECTIVE_PLATFORM_NAME}/ DESTINATION ${LIB_INSTALL_DIR} COMPONENT hop FILES_MATCHING PATTERN "*.a")
	endif (TOADLET_PLATFORM_IOS)
endif (TOADLET_BUILD_STATIC)


# Rule to install only hop
add_custom_target (hop_install
	COMMAND ${CMAKE_COMMAND} -DBUILD_TYPE=$<CONFIGURATION> -DCOMPONENT=required -P ${PROJECT_BINARY_DIR}/cmake_install.cmake
	COMMAND ${CMAKE_COMMAND} -DBUILD_TYPE=$<CONFIGURATION> -DCOMPONENT=egg -P ${PROJECT_BINARY_DIR}/cmake_install.cmake
	COMMAND ${CMAKE_COMMAND} -DBUILD_TYPE=$<CONFIGURATION> -DCOMPONENT=hop -P ${PROJECT_BINARY_DIR}/cmake_install.cmake
	DEPENDS ${HOP_INSTALL}
)
