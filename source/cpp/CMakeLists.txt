# All toadlet libraries require the same includes
include_directories (${toadlet_SOURCE_DIR}/source/cpp ${toadlet_BINARY_DIR}/source/cpp)


# Set engine wide library properties
set (DYNAMIC_LIB_PROP DEBUG_POSTFIX _d)
set (DYNAMIC_LIB_DEFS TOADLET_BUILD_DYNAMIC $<$<CONFIG:Debug>:TOADLET_DEBUG>)
set (DYNAMIC_LIB_FLAG "")
set (DYNAMIC_LIB_LINK "")
set (STATIC_LIB_PROP DEBUG_POSTFIX d)
set (STATIC_LIB_DEFS TOADLET_BUILD_STATIC $<$<CONFIG:Debug>:TOADLET_DEBUG>)
set (STATIC_LIB_FLAG "")
set (STATIC_LIB_LINK "")
set (LIB_INSTALL_DIR lib)


# Grab the scripts for dealing with purely static libraries
include (LocateStaticLibraries)
include (MergeStaticLibraries)


# Library options
option (TOADLET_BUILD_DYNAMIC "Build dynamic libraries" ON)
option (TOADLET_BUILD_STATIC "Build static libraries" ON)
if (NOT TOADLET_PLATFORM_WIN32)
	option (TOADLET_PROFILE "Build with profiling information" OFF)
endif (NOT TOADLET_PLATFORM_WIN32)
if (TOADLET_PLATFORM_OSX)
	option (TOADLET_OSX_STDCPP "Build against legacy libstdc++" ON)
endif (TOADLET_PLATFORM_OSX)


# Setup link flags according to profiling
if (TOADLET_PROFILE) 
	set (DYNAMIC_LIB_FLAG ${DYNAMIC_LIB_FLAG} -pg)
	set (STATIC_LIB_FLAG ${STATIC_LIB_FLAG} -pg)
	set (DYNAMIC_LIB_LINK ${DYNAMIC_LIB_LINK} -pg)
	set (STATIC_LIB_LINK ${STATIC_LIB_LINK} -pg)
	message (STATUS "Profiling enabled")
endif (TOADLET_PROFILE) 


# Setup all platform specific options
if (TOADLET_PLATFORM_ANDROID)
	if (INSTALL_ANDROID_APP)
		# Android applications expect the libs subdir
		set (LIB_INSTALL_DIR libs/${NDK_NAME_ARCH})
	else (INSTALL_ANDROID_APP)
		# Android libraries still need an architecture specific subdirectory
		set (LIB_INSTALL_DIR lib/${NDK_NAME_ARCH})
	endif (INSTALL_ANDROID_APP)

	# Android does not use static libraries
	set (TOADLET_BUILD_STATIC OFF CACHE BOOL "Build static libraries" FORCE)

	# Set defaults
	option (TOADLET_RTTI "Enable Run Time Type Information" OFF)
	option (TOADLET_EXCEPTIONS "Enable Exception handling" OFF)

	# Enable/Disable RTTI
	if (TOADLET_RTTI)
		message (STATUS "RTTI enabled")
	else (TOADLET_RTTI)
		set (DYNAMIC_LIB_FLAG ${DYNAMIC_LIB_FLAG} -fno-rtti)
		set (STATIC_LIB_FLAG ${STATIC_LIB_FLAG} -fno-rtti)
		message (STATUS "RTTI disabled")
	endif (TOADLET_RTTI)

	# Enable/Disable Exceptions
	if (TOADLET_EXCEPTIONS)
		set (DYNAMIC_LIB_FLAG ${DYNAMIC_LIB_FLAG} -fexceptions)
		set (STATIC_LIB_FLAG ${STATIC_LIB_FLAG} -fexceptions)
		message (STATUS "Exceptions enabled")
	else (TOADLET_EXCEPTIONS)
		set (DYNAMIC_LIB_FLAG ${DYNAMIC_LIB_FLAG} -fno-exceptions)
		set (STATIC_LIB_FLAG ${STATIC_LIB_FLAG} -fno-exceptions)
		message (STATUS "Exceptions disabled")
	endif (TOADLET_EXCEPTIONS)
elseif (TOADLET_PLATFORM_IOS)
	# iOS does not permit dynamic libraries
	set (TOADLET_BUILD_DYNAMIC OFF CACHE BOOL "Build dynamic libraries" FORCE)

	# Set defaults
	option (TOADLET_RTTI "Enable Run Time Type Information" OFF)
	option (TOADLET_EXCEPTIONS "Enable Exception handling" OFF)

	# Enable/Disable RTTI
	if (TOADLET_RTTI)
		message (STATUS "RTTI enabled")
	else (TOADLET_RTTI)
		set (DYNAMIC_LIB_FLAG ${DYNAMIC_LIB_FLAG} -fno-rtti)
		set (STATIC_LIB_FLAG ${STATIC_LIB_FLAG} -fno-rtti)
		message (STATUS "RTTI disabled")
	endif (TOADLET_RTTI)

	# Enable/Disable Exceptions
	if (TOADLET_EXCEPTIONS)
		set (DYNAMIC_LIB_FLAG ${DYNAMIC_LIB_FLAG} -fexceptions)
		set (STATIC_LIB_FLAG ${STATIC_LIB_FLAG} -fexceptions)
		message (STATUS "Exceptions enabled")
	else (TOADLET_EXCEPTIONS)
		set (DYNAMIC_LIB_FLAG ${DYNAMIC_LIB_FLAG} -fno-exceptions)
		set (STATIC_LIB_FLAG ${STATIC_LIB_FLAG} -fno-exceptions)
		message (STATUS "Exceptions disabled")
	endif (TOADLET_EXCEPTIONS)
elseif (TOADLET_PLATFORM_EMSCRIPTEN)
	# emscripten does not handle dynamic libraries
	set (TOADLET_BUILD_DYNAMIC OFF CACHE BOOL "Build dynamic libraries" FORCE)

	if (STATIC_LIB_FLAG)
		set (STATIC_LIB_PROPS ${STATIC_LIB_PROPS} COMPILE_FLAG ${STATIC_LIB_FLAG})
	endif (STATIC_LIB_FLAG)

	set (CMAKE_C_FLAG "-Wno-warn-absolute-paths")
	set (CMAKE_CXX_FLAG "-Wno-warn-absolute-paths")
	set (CMAKE_CXX_FLAG_RELEASE "-O2 --closure 1")
	set (CMAKE_EXE_LINKER_FLAG_RELEASE "-O2 --closure 1")
elseif (TOADLET_PLATFORM_POSIX)
	# Set defaults
	option (TOADLET_RTTI "Enable Run Time Type Information" ON)
	option (TOADLET_EXCEPTIONS "Enable Exception handling" ON)

	# Enable/Disable RTTI
	if (TOADLET_RTTI)
		message (STATUS "RTTI enabled")
	else (TOADLET_RTTI)
		set (DYNAMIC_LIB_FLAG ${DYNAMIC_LIB_FLAG} -fno-rtti)
		set (STATIC_LIB_FLAG ${STATIC_LIB_FLAG} -fno-rtti)
		message (STATUS "RTTI disabled")
	endif (TOADLET_RTTI)

	# Enable/Disable Exceptions
	if (TOADLET_EXCEPTIONS)
		set (DYNAMIC_LIB_FLAG ${DYNAMIC_LIB_FLAG} -fexceptions)
		set (STATIC_LIB_FLAG ${STATIC_LIB_FLAG} -fexceptions)
		message (STATUS "Exceptions enabled")
	else (TOADLET_EXCEPTIONS)
		set (DYNAMIC_LIB_FLAG ${DYNAMIC_LIB_FLAG} -fno-exceptions)
		set (STATIC_LIB_FLAG ${STATIC_LIB_FLAG} -fno-exceptions)
		message (STATUS "Exceptions disabled")
	endif (TOADLET_EXCEPTIONS)

	# Check for SSE2 support
	include (CheckCXXCompilerFlag)
	CHECK_CXX_COMPILER_FLAG ("-msse2" TOADLET_HAS_SSE)
	if (TOADLET_HAS_SSE)
		set (DYNAMIC_LIB_FLAG ${DYNAMIC_LIB_FLAG} -msse2)
		set (STATIC_LIB_FLAG ${STATIC_LIB_FLAG} -msse2)
		message (STATUS "SSE2 support enabled")
	endif (TOADLET_HAS_SSE)

	# Always use position independent code
	set (DYNAMIC_LIB_FLAG ${DYNAMIC_LIB_FLAG} -fPIC)
	set (STATIC_LIB_FLAG ${STATIC_LIB_FLAG} -fPIC)

	# Force the gnu stl
	if (TOADLET_OSX_STDCPP)
		set (DYNAMIC_LIB_FLAG ${DYNAMIC_LIB_FLAG} -mmacosx-version-min=10.5 -stdlib=libstdc++)
		set (STATIC_LIB_FLAG ${STATIC_LIB_FLAG} -mmacosx-version-min=10.5 -stdlib=libstdc++)
		set (DYNAMIC_LIB_LINK ${DYNAMIC_LIB_LINK} -mmacosx-version-min=10.5 -stdlib=libstdc++)
		set (STATIC_LIB_LINK ${STATIC_LIB_LINK} -mmacosx-version-min=10.5 -stdlib=libstdc++)
	endif (TOADLET_OSX_STDCPP)

	# OSX specifically adds the install_name parameter to dynamic libraries
	if (TOADLET_PLATFORM_OSX)
		set (DYNAMIC_LIB_PROPS ${DYNAMIC_LIB_PROPS} INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}")
	endif (TOADLET_PLATFORM_OSX)
elseif (TOADLET_PLATFORM_WIN32)
	# Set defaults
	if (TOADLET_PLATFORM_WINCE)
		option (TOADLET_DYNAMIC_MULTITHREAD_DYNAMIC "Use the /MD (Not /MT) compile switch with dynamic libraries" OFF)
		option (TOADLET_STATIC_MULTITHREAD_DYNAMIC "Use the /MD (Not /MT) compile switch with static libraries" OFF)
		option (TOADLET_RTTI "Enable Run Time Type Information" OFF)
		option (TOADLET_EXCEPTIONS "Enable Exception handling" OFF)
		option (TOADLET_MSVC_CHECK "Enable MSVC check" OFF)
		option (TOADLET_ITERATOR_CHECK "Enable Iterator check" OFF)
		option (TOADLET_RUNTIME_CHECK "Enable Runtime check" OFF)
	else (TOADLET_PLATFORM_WINCE)
		option (TOADLET_DYNAMIC_MULTITHREAD_DYNAMIC "Use the /MD (Not /MT) compile switch with dynamic libraries" ON)
		option (TOADLET_STATIC_MULTITHREAD_DYNAMIC "Use the /MD (Not /MT) compile switch with static libraries" OFF)
		option (TOADLET_RTTI "Enable Run Time Type Information" ON)
		option (TOADLET_EXCEPTIONS "Enable Exception handling" ON)
		option (TOADLET_MSVC_CHECK "Enable MSVC check" OFF)
		option (TOADLET_ITERATOR_CHECK "Enable Iterator check" OFF)
		option (TOADLET_RUNTIME_CHECK "Enable Runtime check" OFF)
	endif (TOADLET_PLATFORM_WINCE)
	
	# Deal with multithread static/dynamic libraries
	#   if(/MDd) {nodefaultlib=libcmt, libcmtd, libcpmt, libcpmtd, msvcrt, msvcprt}
	#   if(/MD)  {nodefaultlib=libcmt, libcmtd, libcpmt, libcpmtd, msvcrtd, msvcprtd}
	#   if(/MTd) {nodefaultlib=libcmt, libcpmt, msvcrt, msvcprt, msvcrtd, msvcprtd}
	#   if(/MT)  {nodefaultlib=libcmtd, libcpmtd, msvcrt, msvcprt, msvcrtd, msvcprtd}
	set (MD_LINK_DEBUG
		"/NODEFAULTLIB:LIBCMT.lib /NODEFAULTLIB:LIBCMTd.lib /NODEFAULTLIB:LIBCPMT.lib /NODEFAULTLIB:LIBCPMTd.lib /NODEFAULTLIB:MSVCRT.lib /NODEFAULTLIB:MSVCPRT.lib"
	)
	set (MD_LINK_RELEASE
		"/NODEFAULTLIB:LIBCMT.lib /NODEFAULTLIB:LIBCMTd.lib /NODEFAULTLIB:LIBCPMT.lib /NODEFAULTLIB:LIBCPMTd.lib /NODEFAULTLIB:MSVCRTd.lib /NODEFAULTLIB:MSVCPRTd.lib"
	)
	set (MT_LINK_DEBUG
		"/NODEFAULTLIB:LIBCMT.lib /NODEFAULTLIB:LIBCPMT.lib /NODEFAULTLIB:MSVCRT.lib /NODEFAULTLIB:MSVCPRT.lib /NODEFAULTLIB:MSVCRTd.lib /NODEFAULTLIB:MSVCPRTd.lib"
	)
	set (MT_LINK_RELEASE
		"/NODEFAULTLIB:LIBCMTd.lib /NODEFAULTLIB:LIBCPMTd.lib /NODEFAULTLIB:MSVCRT.lib /NODEFAULTLIB:MSVCPRT.lib /NODEFAULTLIB:MSVCRTd.lib /NODEFAULTLIB:MSVCPRTd.lib"
	)
	if (TOADLET_DYNAMIC_MULTITHREAD_DYNAMIC)
		set (DYNAMIC_LIB_FLAG ${DYNAMIC_LIB_FLAG} /MD$<$<CONFIG:Debug>:d>)
		set (DYNAMIC_LIB_PROP ${DYNAMIC_LIB_PROP} LINK_FLAGS_DEBUG ${MD_LINK_DEBUG} LINK_FLAGS_RELEASE ${MD_LINK_RELEASE})
	else (TOADLET_DYNAMIC_MULTITHREAD_DYNAMIC)
		message (STATUS "WARNING: Building dynamic libraries with /MT switch. Debug builds of toadlet dlls will fail!")
		set (DYNAMIC_LIB_FLAG ${DYNAMIC_LIB_FLAG} /MT$<$<CONFIG:Debug>:d>)
		set (DYNAMIC_LIB_PROP ${DYNAMIC_LIB_PROP} LINK_FLAGS_DEBUG ${MT_LINK_DEBUG} LINK_FLAGS_RELEASE ${MT_LINK_RELEASE})
	else (TOADLET_DYNAMIC_MULTITHREAD_DYNAMIC)
	endif (TOADLET_DYNAMIC_MULTITHREAD_DYNAMIC)
	if (TOADLET_STATIC_MULTITHREAD_DYNAMIC)
		set (STATIC_LIB_FLAG ${STATIC_LIB_FLAG} /MD$<$<CONFIG:Debug>:d>)
		set (STATIC_LIB_PROP ${DYNAMIC_LIB_PROP} STATIC_LIBRARY_FLAGS_DEBUG ${MD_LINK_DEBUG} STATIC_LIBRARY_FLAGS_RELEASE ${MD_LINK_RELEASE})
	else (TOADLET_STATIC_MULTITHREAD_DYNAMIC)
		set (STATIC_LIB_FLAG ${STATIC_LIB_FLAG} /MT$<$<CONFIG:Debug>:d>)
		set (STATIC_LIB_PROP ${DYNAMIC_LIB_PROP} STATIC_LIBRARY_FLAGS_DEBUG ${MT_LINK_DEBUG} STATIC_LIBRARY_FLAGS_RELEASE ${MT_LINK_RELEASE})
	endif (TOADLET_STATIC_MULTITHREAD_DYNAMIC)

	# Enable/Disable RTTI
	if (TOADLET_RTTI)
		set (DYNAMIC_LIB_FLAG ${DYNAMIC_LIB_FLAG} /GR)
		set (STATIC_LIB_FLAG ${STATIC_LIB_FLAG} /GR)
		if (TOADLET_PLATFORM_WINCE)
			set (CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} ccrtrtti.lib")
		endif (TOADLET_PLATFORM_WINCE)
		message (STATUS "RTTI enabled")
	else (TOADLET_RTTI)
		set (DYNAMIC_LIB_FLAG ${DYNAMIC_LIB_FLAG} /GR-)
		set (STATIC_LIB_FLAG ${STATIC_LIB_FLAG} /GR-)
		message (STATUS "RTTI disabled")
	endif (TOADLET_RTTI)

	# Enable/Disable Exceptions
	if (TOADLET_EXCEPTIONS)
		set (DYNAMIC_LIB_FLAG ${DYNAMIC_LIB_FLAG} /EHsc)
		set (STATIC_LIB_FLAG ${STATIC_LIB_FLAG} /EHsc)
		message (STATUS "Exceptions enabled")
	else (TOADLET_EXCEPTIONS)
		set (DYNAMIC_LIB_FLAG ${DYNAMIC_LIB_FLAG} /EHs-c-)
		set (STATIC_LIB_FLAG ${STATIC_LIB_FLAG} /EHs-c-)
		message (STATUS "Exceptions disabled")
	endif (TOADLET_EXCEPTIONS)

	# Check for SSE2 support
	include (CheckCXXCompilerFlag)
	CHECK_CXX_COMPILER_FLAG ("/arch:SSE2" TOADLET_HAS_SSE)
	if (TOADLET_HAS_SSE)
		set (DYNAMIC_LIB_FLAG ${DYNAMIC_LIB_FLAG} /arch:SSE2)
		set (STATIC_LIB_FLAG ${STATIC_LIB_FLAG} /arch:SSE2)
		message (STATUS "SSE2 support enabled")
	endif (TOADLET_HAS_SSE)
	
	# Disable msvc mismatch
	if (NOT TOADLET_MSVC_CHECK)
		set (DYNAMIC_LIB_DEFS ${DYNAMIC_LIB_DEFS} _ALLOW_MSC_VER_MISMATCH)
		set (STATIC_LIB_DEFS ${STATIC_LIB_DEFS} _ALLOW_MSC_VER_MISMATCH)
		message (STATUS "MSVC mismatch disabled")
	endif (NOT TOADLET_MSVC_CHECK)
	
	# Disable iterator mismatch
	if (NOT TOADLET_ITERATOR_CHECK)
		set (DYNAMIC_LIB_DEFS ${DYNAMIC_LIB_DEFS} _ITERATOR_DEBUG_LEVEL=0 _ALLOW_ITERATOR_DEBUG_LEVEL_MISMATCH)
		set (STATIC_LIB_DEFS ${STATIC_LIB_DEFS} _ITERATOR_DEBUG_LEVEL=0 _ALLOW_ITERATOR_DEBUG_LEVEL_MISMATCH)
		message (STATUS "Iterator mismatch disabled")
	endif (NOT TOADLET_ITERATOR_CHECK)

	# Disable runtime mismatch
	if (NOT TOADLET_RUNTIME_CHECK)
		set (DYNAMIC_LIB_DEFS ${DYNAMIC_LIB_DEFS} _ALLOW_RUNTIME_LIBRARY_MISMATCH)
		set (STATIC_LIB_DEFS ${STATIC_LIB_DEFS} _ALLOW_RUNTIME_LIBRARY_MISMATCH)
		message (STATUS "Runtime mismatch disabled")
	endif (NOT TOADLET_RUNTIME_CHECK)
else (TOADLET_PLATFORM_ANDROID)
	message (FATAL_ERROR "Toadlet platform undefined. Toadlet will not be built")
endif (TOADLET_PLATFORM_ANDROID)


# add the toadlet source code subdirectory
add_subdirectory (toadlet)


# add the unit tests
add_subdirectory (test)
