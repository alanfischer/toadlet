# All toadlet libraries require the same includes
include_directories (${toadlet_SOURCE_DIR}/source/cpp ${toadlet_BINARY_DIR}/source/cpp)


# Set engine wide library properties
set (DYNAMIC_LIB_PROPS COMPILE_DEFINITIONS TOADLET_BUILD_DYNAMIC COMPILE_DEFINITIONS_DEBUG TOADLET_DEBUG DEBUG_POSTFIX _d)
set (STATIC_LIB_PROPS COMPILE_DEFINITIONS TOADLET_BUILD_STATIC COMPILE_DEFINITIONS_DEBUG TOADLET_DEBUG DEBUG_POSTFIX d)
set (LIB_INSTALL_DIR lib)


# Grab the scripts for dealing with purely static libraries
include (LocateStaticLibraries)
include (MergeStaticLibraries)


# By default we always build static and dynamic libraries
option (TOADLET_BUILD_DYNAMIC "Build dynamic libraries" ON)
option (TOADLET_BUILD_STATIC "Build static libraries" ON)


# Setup all platform specific options
if (TOADLET_PLATFORM_ANDROID)
	# Android libraries install to an architecture specific subdirectory
	set (LIB_INSTALL_DIR libs/${NDK_NAME_ARCH})

	# Android does not use static libraries
	set (TOADLET_BUILD_STATIC OFF CACHE BOOL "Build static libraries" FORCE)

	# Set defaults 
	option (TOADLET_RTTI "Enable Run Time Type Information" OFF)
	option (TOADLET_EXCEPTIONS "Enable Exception handling" OFF)

	# Enable/Disable RTTI	
	if (TOADLET_RTTI)
		message (STATUS "RTTI enabled")
	else (TOADLET_RTTI)
		set (DYNAMIC_LIB_FLAGS "${DYNAMIC_LIB_FLAGS} -fno-rtti")
		set (STATIC_LIB_FLAGS "${STATIC_LIB_FLAGS} -fno-rtti")
		message (STATUS "RTTI disabled")
	endif (TOADLET_RTTI)

	# Enable/Disable Exceptions
	if (TOADLET_EXCEPTIONS)
		set (DYNAMIC_LIB_FLAGS "${DYNAMIC_LIB_FLAGS} -fexceptions")
		set (STATIC_LIB_FLAGS "${STATIC_LIB_FLAGS} -fexceptions")
		message (STATUS "Exceptions enabled")
	else (TOADLET_EXCEPTIONS)
		set (DYNAMIC_LIB_FLAGS "${DYNAMIC_LIB_FLAGS} -fno-exceptions")
		set (STATIC_LIB_FLAGS "${STATIC_LIB_FLAGS} -fno-exceptions")
		message (STATUS "Exceptions disabled")
	endif (TOADLET_EXCEPTIONS)

	# Append the compile flags to the properties
	if (DYNAMIC_LIB_FLAGS)
		set (DYNAMIC_LIB_PROPS ${DYNAMIC_LIB_PROPS} COMPILE_FLAGS ${DYNAMIC_LIB_FLAGS})
	endif (DYNAMIC_LIB_FLAGS)
	if (STATIC_LIB_FLAGS)
		set (STATIC_LIB_PROPS ${STATIC_LIB_PROPS} COMPILE_FLAGS ${STATIC_LIB_FLAGS})
	endif (STATIC_LIB_FLAGS)
elseif (TOADLET_PLATFORM_IOS)
	# iOS does not permit dynamic libraries
	set (TOADLET_BUILD_DYNAMIC OFF CACHE BOOL "Build dynamic libraries" FORCE)

	if (STATIC_LIB_FLAGS)
		set (STATIC_LIB_PROPS ${STATIC_LIB_PROPS} COMPILE_FLAGS ${STATIC_LIB_FLAGS})
	endif (STATIC_LIB_FLAGS)
elseif (TOADLET_PLATFORM_POSIX)
	# Set defaults 
	option (TOADLET_RTTI "Enable Run Time Type Information" ON)
	option (TOADLET_EXCEPTIONS "Enable Exception handling" ON)

	# Enable/Disable RTTI	
	if (TOADLET_RTTI)
		message (STATUS "RTTI enabled")
	else (TOADLET_RTTI)
		set (DYNAMIC_LIB_FLAGS "${DYNAMIC_LIB_FLAGS} -fno-rtti")
		set (STATIC_LIB_FLAGS "${STATIC_LIB_FLAGS} -fno-rtti")
		message (STATUS "RTTI disabled")
	endif (TOADLET_RTTI)

	# Enable/Disable Exceptions
	if (TOADLET_EXCEPTIONS)
		set (DYNAMIC_LIB_FLAGS "${DYNAMIC_LIB_FLAGS} -fexceptions")
		set (STATIC_LIB_FLAGS "${STATIC_LIB_FLAGS} -fexceptions")
		message (STATUS "Exceptions enabled")
	else (TOADLET_EXCEPTIONS)
		set (DYNAMIC_LIB_FLAGS "${DYNAMIC_LIB_FLAGS} -fno-exceptions")
		set (STATIC_LIB_FLAGS "${STATIC_LIB_FLAGS} -fno-exceptions")
		message (STATUS "Exceptions disabled")
	endif (TOADLET_EXCEPTIONS)

	# Check for SSE2 support
	include (CheckCXXCompilerFlag)
	CHECK_CXX_COMPILER_FLAG ("-msse2" TOADLET_HAS_SSE)
	if (TOADLET_HAS_SSE)
		set (DYNAMIC_LIB_FLAGS "${DYNAMIC_LIB_FLAGS} -msse2")
		set (STATIC_LIB_FLAGS "${STATIC_LIB_FLAGS} -msse2")
		message (STATUS "SSE2 support enabled")
	endif (TOADLET_HAS_SSE)

	# Append the compile flags to the properties
	if (DYNAMIC_LIB_FLAGS)
		set (DYNAMIC_LIB_PROPS ${DYNAMIC_LIB_PROPS} COMPILE_FLAGS ${DYNAMIC_LIB_FLAGS})
	endif (DYNAMIC_LIB_FLAGS)
	if (STATIC_LIB_FLAGS)
		set (STATIC_LIB_PROPS ${STATIC_LIB_PROPS} COMPILE_FLAGS ${STATIC_LIB_FLAGS})
	endif (STATIC_LIB_FLAGS)
elseif (TOADLET_PLATFORM_WIN32)
	# Set defaults
	if (TOADLET_PLATFORM_WINCE)
		option (TOADLET_USE_BLUETOOTH "Enable Bluetooth support" ON)
		option (TOADLET_DYNAMIC_MULTITHREAD_DYNAMIC "Use the /MD (Not /MT) compile switch with dynamic libraries" OFF)
		option (TOADLET_STATIC_MULTITHREAD_DYNAMIC "Use the /MD (Not /MT) compile switch with static libraries" OFF)
		option (TOADLET_RTTI "Enable Run Time Type Information" OFF)
		option (TOADLET_EXCEPTIONS "Enable Exception handling" OFF)
	else (TOADLET_PLATFORM_WINCE)
		option (TOADLET_USE_BLUETOOTH "Enable Bluetooth support" OFF)
		option (TOADLET_DYNAMIC_MULTITHREAD_DYNAMIC "Use the /MD (Not /MT) compile switch with dynamic libraries" ON)
		option (TOADLET_STATIC_MULTITHREAD_DYNAMIC "Use the /MD (Not /MT) compile switch with static libraries" OFF)
		option (TOADLET_RTTI "Enable Run Time Type Information" ON)
		option (TOADLET_EXCEPTIONS "Enable Exception handling" ON)
	endif (TOADLET_PLATFORM_WINCE)
	
	# Deal with multithread static/dynamic libraries
	# TODO: Once cmake supports _<CONFIGURATION> compile flags, make the /MDd /MD /MTd /MT all configuration specific!
	# Monitor issue 0006493 - http://public.kitware.com/Bug/view.php?id=6493
	# Ultimate Result should be:
	#   if(/MDd) {nodefaultlib=libcmt,libcmtd,msvcrt}
	#   if(/MD) {nodefaultlib=libcmt,libcmtd,msvcrtd}
	#   if(/MTd) {nodefaultlib=libcmt,msvcrt,msvcrtd}
	#   if(/MT) {nodefaultlib=libcmtd,msvcrt,msvcrtd}
	if (TOADLET_DYNAMIC_MULTITHREAD_DYNAMIC)
		set (DYNAMIC_LIB_LINK_DEBUG "/NODEFAULTLIB:LIBCMT.lib,LIBCMTd.lib,MSVCRT.lib")
		set (DYNAMIC_LIB_LINK_RELEASE "/NODEFAULTLIB:LIBCMT.lib,LIBCMTd.lib,MSVCRTd.lib")
	else (TOADLET_DYNAMIC_MULTITHREAD_DYNAMIC)
		message (STATUS "WARNING: Building dynamic libraries with /MT switch. Debug builds of toadlet dlls will fail!")
		set (DYNAMIC_LIB_FLAGS "/MT")
		set (DYNAMIC_LIB_LINK_DEBUG "/NODEFAULTLIB:LIBCMT.lib,MSVCRT.lib,MSVCRTd.lib")
		set (DYNAMIC_LIB_LINK_RELEASE "/NODEFAULTLIB:LIBCMTd.lib,MSCVRT.lib,MSVCRTd.lib")
	endif (TOADLET_DYNAMIC_MULTITHREAD_DYNAMIC)
	if (TOADLET_STATIC_MULTITHREAD_DYNAMIC)
		set (STATIC_LIB_LINK_DEBUG "/NODEFAULTLIB:LIBCMT.lib,LIBCMTd.lib,MSVCRT.lib")
		set (STATIC_LIB_LINK_RELEASE "/NODEFAULTLIB:LIBCMT.lib,LIBCMTd.lib,MSVCRTd.lib")
	else (TOADLET_STATIC_MULTITHREAD_DYNAMIC)
		set (STATIC_LIB_FLAGS "/MT")
		set (STATIC_LIB_LINK_DEBUG "/NODEFAULTLIB:LIBCMT.lib,MSVCRT.lib,MSVCRTd.lib")
		set (STATIC_LIB_LINK_RELEASE "/NODEFAULTLIB:LIBCMTd.lib,MSVCRT.lib,MSVCRTd.lib")
	endif (TOADLET_STATIC_MULTITHREAD_DYNAMIC)

	# Enable/Disable RTTI	
	if (TOADLET_RTTI)
		set (DYNAMIC_LIB_FLAGS "${DYNAMIC_LIB_FLAGS} /GR")
		set (STATIC_LIB_FLAGS "${STATIC_LIB_FLAGS} /GR")
		if (TOADLET_PLATFORM_WINCE)
			set (CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} ccrtrtti.lib")
		endif (TOADLET_PLATFORM_WINCE)
		message (STATUS "RTTI enabled")
	else (TOADLET_RTTI)
		set (DYNAMIC_LIB_FLAGS "${DYNAMIC_LIB_FLAGS} /GR-")
		set (STATIC_LIB_FLAGS "${STATIC_LIB_FLAGS} /GR-")
		message (STATUS "RTTI disabled")
	endif (TOADLET_RTTI)

	# Enable/Disable Exceptions
	if (TOADLET_EXCEPTIONS)
		set (DYNAMIC_LIB_FLAGS "${DYNAMIC_LIB_FLAGS} /EHsc")
		set (STATIC_LIB_FLAGS "${STATIC_LIB_FLAGS} /EHsc")
		message (STATUS "Exceptions enabled")
	else (TOADLET_EXCEPTIONS)
		set (DYNAMIC_LIB_FLAGS "${DYNAMIC_LIB_FLAGS} /EHs-c-")
		set (STATIC_LIB_FLAGS "${STATIC_LIB_FLAGS} /EHs-c-")
		message (STATUS "Exceptions disabled")
	endif (TOADLET_EXCEPTIONS)

	# Check for SSE2 support
	include (CheckCXXCompilerFlag)
	CHECK_CXX_COMPILER_FLAG ("/arch:SSE2" TOADLET_HAS_SSE)
	if (TOADLET_HAS_SSE)
		set (DYNAMIC_LIB_FLAGS "${DYNAMIC_LIB_FLAGS} /arch:SSE2")
		set (STATIC_LIB_FLAGS "${STATIC_LIB_FLAGS} /arch:SSE2")
		message (STATUS "SSE2 support enabled")
	endif (TOADLET_HAS_SSE)

	# Append the compile flags to the properties
	# TODO: Also clean this up once issue 0006493 is resolved
	if (DYNAMIC_LIB_FLAGS)
		set (DYNAMIC_LIB_PROPS ${DYNAMIC_LIB_PROPS} COMPILE_FLAGS ${DYNAMIC_LIB_FLAGS})
	endif (DYNAMIC_LIB_FLAGS)
	if (STATIC_LIB_FLAGS)
		set (STATIC_LIB_PROPS ${STATIC_LIB_PROPS} COMPILE_FLAGS ${STATIC_LIB_FLAGS})
	endif (STATIC_LIB_FLAGS)
	
	# Append the link flags to the properties
	set (DYNAMIC_LIB_PROPS ${DYNAMIC_LIB_PROPS} 
		LINK_FLAGS_DEBUG ${DYNAMIC_LIB_LINK_DEBUG} 
		LINK_FLAGS_RELEASE ${DYNAMIC_LIB_LINK_RELEASE}
		LINK_FLAGS_MINSIZEREL ${DYNAMIC_LIB_LINK_RELEASE}
		LINK_FLAGS_RELWITHDEBINFO ${DYNAMIC_LIB_LINK_RELEASE}
	)
	set (STATIC_LIB_PROPS ${STATIC_LIB_PROPS} 
		LINK_FLAGS_DEBUG ${STATIC_LIB_LINK_DEBUG} 
		LINK_FLAGS_RELEASE ${STATIC_LIB_LINK_RELEASE}
		LINK_FLAGS_MINSIZEREL ${STATIC_LIB_LINK_RELEASE}
		LINK_FLAGS_RELWITHDEBINFO ${STATIC_LIB_LINK_RELEASE}
	)
else (TOADLET_PLATFORM_EMSCRIPTEN)
	# emscripten does not handle dynamic libraries
	set (TOADLET_BUILD_DYNAMIC OFF CACHE BOOL "Build dynamic libraries" FORCE)

	if (STATIC_LIB_FLAGS)
		set (STATIC_LIB_PROPS ${STATIC_LIB_PROPS} COMPILE_FLAGS ${STATIC_LIB_FLAGS})
	endif (STATIC_LIB_FLAGS)
else (TOADLET_PLATFORM_ANDROID)
	message (FATAL_ERROR "Toadlet platform undefined. Toadlet will not be built")
endif (TOADLET_PLATFORM_ANDROID)


# add the toadlet source code subdirectory
add_subdirectory (toadlet)

