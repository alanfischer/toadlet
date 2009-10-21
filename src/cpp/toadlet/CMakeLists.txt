
# All toadlet libraries require the same toadlet includes
include_directories (${toadlet_SOURCE_DIR}/src/cpp ${toadlet_BINARY_DIR}/src/cpp)


# Set engine wide library properties
set (DYNAMIC_LIB_PROPS COMPILE_DEFINITIONS TOADLET_BUILD_DYNAMIC COMPILE_DEFINITIONS_DEBUG TOADLET_DEBUG DEBUG_POSTFIX _d)
set (STATIC_LIB_PROPS COMPILE_DEFINITIONS TOADLET_BUILD_STATIC COMPILE_DEFINITIONS_DEBUG TOADLET_DEBUG DEBUG_POSTFIX d)

# Set engine wide compile flags - these need to be appended separately due to some crazy way cmake works


# Set platform specific options
if (TOADLET_PLATFORM_WIN32)
	# Set defaults
	if (TOADLET_PLATFORM_WINCE OR TOADLET_PLATFORM_IPHONE)
		option (TOADLET_USE_BLUETOOTH "Enable Bluetooth support" ON)
		option (TOADLET_DYNAMIC_MULTITHREAD_DYNAMIC "Use the /MD compile switch with dynamic libraries" OFF)
		option (TOADLET_STATIC_MULTITHREAD_DYNAMIC "Use the /MD compile switch with static libraries" OFF)
		option (TOADLET_DEPENDENCY_EXTENSION_NAMES "Use _s, _d and _sd exensions for dependencies" OFF)
		option (TOADLET_RTTI "Enable Run Time Type Information" OFF)
		option (TOADLET_EXCEPTIONS "Enable Exception handling" OFF)
	else (TOADLET_PLATFORM_WINCE OR TOADLET_PLATFORM_IPHONE)
		option (TOADLET_USE_BLUETOOTH "Enable Bluetooth support" OFF)
		option (TOADLET_DYNAMIC_MULTITHREAD_DYNAMIC "Use the /MD compile switch with dynamic libraries" ON)
		option (TOADLET_STATIC_MULTITHREAD_DYNAMIC "Use the /MD compile switch with static libraries" OFF)
		option (TOADLET_DEPENDENCY_EXTENSION_NAMES "Use _s, _d and _sd exensions for dependencies" ON)
		option (TOADLET_RTTI "Enable Run Time Type Information" ON)
		option (TOADLET_EXCEPTIONS "Enable Exception handling" ON)
	endif (TOADLET_PLATFORM_WINCE OR TOADLET_PLATFORM_IPHONE)
	
	# Deal with multithread static/dynamic libraries	
	if (TOADLET_DYNAMIC_MULTITHREAD_DYNAMIC)
		set (DYNAMIC_LIB_FLAGS "/MD")
	else (TOADLET_DYNAMIC_MULTITHREAD_DYNAMIC)
		set (DYNAMIC_LIB_FLAGS "/MT")
	endif (TOADLET_DYNAMIC_MULTITHREAD_DYNAMIC)
	if (TOADLET_STATIC_MULTITHREAD_DYNAMIC)
		set (STATIC_LIB_FLAGS "/MD")
	else (TOADLET_STATIC_MULTITHREAD_DYNAMIC)
		set (STATIC_LIB_FLAGS "/MT")
	endif (TOADLET_STATIC_MULTITHREAD_DYNAMIC)

	# Enable/Disable RTTI	
	if (TOADLET_RTTI)
		set (DYNAMIC_LIB_FLAGS "${DYNAMIC_LIB_FLAGS} /GR")
		set (STATIC_LIB_FLAGS "${STATIC_LIB_FLAGS} /GR")
		if (TOADLET_PLATFORM_WINCE)
			set (CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} ccrtrtti.lib")
		endif (TOADLET_PLATFORM_WINCE)
		message ("RTTI enabled")
	else (TOADLET_RTTI)
		set (DYNAMIC_LIB_FLAGS "${DYNAMIC_LIB_FLAGS} /GR-")
		set (STATIC_LIB_FLAGS "${STATIC_LIB_FLAGS} /GR-")
		message ("RTTI disabled")
	endif (TOADLET_RTTI)

	# Enable/Disable Exceptions
	if (TOADLET_EXCEPTIONS)
		set (DYNAMIC_LIB_FLAGS "${DYNAMIC_LIB_FLAGS} /EHsc")
		set (STATIC_LIB_FLAGS "${STATIC_LIB_FLAGS} /EHsc")
		message ("Exceptions enabled")
	else (TOADLET_EXCEPTIONS)
		set (DYNAMIC_LIB_FLAGS "${DYNAMIC_LIB_FLAGS} /EHs-c-")
		set (STATIC_LIB_FLAGS "${STATIC_LIB_FLAGS} /EHs-c-")
		message ("Exceptions disabled")
	endif (TOADLET_EXCEPTIONS)

	# Append the compile flags to the properties
	set (DYNAMIC_LIB_PROPS ${DYNAMIC_LIB_PROPS} COMPILE_FLAGS ${DYNAMIC_LIB_FLAGS})
	set (STATIC_LIB_PROPS ${STATIC_LIB_PROPS} COMPILE_FLAGS ${STATIC_LIB_FLAGS})
endif (TOADLET_PLATFORM_WIN32)
if (TOADLET_PLATFORM_IPHONE)
	# iPhone builds require hidden visibility in their libraries
	set (STATIC_LIB_PROPS ${STATIC_LIB_PROPS} COMPILE_FLAGS "-fvisibility=hidden")
endif (TOADLET_PLATFORM_IPHONE)


# Add the individual toadlet libraries
add_subdirectory (egg)
add_subdirectory (flick)
add_subdirectory (hop)
add_subdirectory (knot)
add_subdirectory (peeper)
add_subdirectory (ribbit)
add_subdirectory (tadpole)
add_subdirectory (pad)


# Install header files
install (DIRECTORY ${toadlet_SOURCE_DIR}/src/cpp/toadlet DESTINATION include FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)
install (FILES ${toadlet_BINARY_DIR}/src/cpp/toadlet/Config.h DESTINATION include/toadlet)


# Setup toadlet configure file
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Config.h.in ${CMAKE_CURRENT_BINARY_DIR}/Config.h)
