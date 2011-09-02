# This file is based off of the Platform/Darwin.cmake and Platform/UnixPaths.cmake
# files which are included with CMake 2.8.4
# It has been altered for iOS development
# Standard settings
set (CMAKE_SYSTEM_NAME Darwin)
set (CMAKE_SYSTEM_VERSION 1 )
set (UNIX True)
set (APPLE True)
set (IOS True)

# Force the compilers to gcc for iOS
include (CMakeForceCompiler)
CMAKE_FORCE_C_COMPILER (gcc gcc)
CMAKE_FORCE_CXX_COMPILER (g++ g++)

# Skip the platform compiler checks for cross compiling
set (CMAKE_CXX_COMPILER_WORKS TRUE)
set (CMAKE_C_COMPILER_WORKS TRUE)

# All iOS/Darwin specific settings - some may be redundant
set (CMAKE_SHARED_LIBRARY_PREFIX "lib")
set (CMAKE_SHARED_LIBRARY_SUFFIX ".dylib")
set (CMAKE_SHARED_MODULE_PREFIX "lib")
set (CMAKE_SHARED_MODULE_SUFFIX ".so")
set (CMAKE_MODULE_EXISTS 1)
set (CMAKE_DL_LIBS "")

set (CMAKE_C_OSX_COMPATIBILITY_VERSION_FLAG "-compatibility_version ")
set (CMAKE_C_OSX_CURRENT_VERSION_FLAG "-current_version ")
set (CMAKE_CXX_OSX_COMPATIBILITY_VERSION_FLAG "${CMAKE_C_OSX_COMPATIBILITY_VERSION_FLAG}")
set (CMAKE_CXX_OSX_CURRENT_VERSION_FLAG "${CMAKE_C_OSX_CURRENT_VERSION_FLAG}")

# Hidden visibilty is required for cxx on iOS 
set (CMAKE_C_FLAGS "")
set (CMAKE_CXX_FLAGS "-headerpad_max_install_names -fvisibility=hidden -fvisibility-inlines-hidden")

set (CMAKE_C_LINK_FLAGS "-Wl,-search_paths_first ${CMAKE_C_LINK_FLAGS}")
set (CMAKE_CXX_LINK_FLAGS "-Wl,-search_paths_first ${CMAKE_CXX_LINK_FLAGS}")

set (CMAKE_PLATFORM_HAS_INSTALLNAME 1)
set (CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS "-dynamiclib -headerpad_max_install_names")
set (CMAKE_SHARED_MODULE_CREATE_C_FLAGS "-bundle -headerpad_max_install_names")
set (CMAKE_SHARED_MODULE_LOADER_C_FLAG "-Wl,-bundle_loader,")
set (CMAKE_SHARED_MODULE_LOADER_CXX_FLAG "-Wl,-bundle_loader,")
set (CMAKE_FIND_LIBRARY_SUFFIXES ".dylib" ".so" ".a")

# hack: if a new cmake (which uses CMAKE_INSTALL_NAME_TOOL) runs on an old build tree
# (where install_name_tool was hardcoded) and where CMAKE_INSTALL_NAME_TOOL isn't in the cache
# and still cmake didn't fail in CMakeFindBinUtils.cmake (because it isn't rerun)
# hardcode CMAKE_INSTALL_NAME_TOOL here to install_name_tool, so it behaves as it did before, Alex
if (NOT DEFINED CMAKE_INSTALL_NAME_TOOL)
	find_program(CMAKE_INSTALL_NAME_TOOL install_name_tool)
endif (NOT DEFINED CMAKE_INSTALL_NAME_TOOL)

# Setup iOS developer location
if (NOT DEFINED CMAKE_IOS_DEVELOPER_ROOT)
	set (CMAKE_IOS_DEVELOPER_ROOT "/Developer/Platforms/iPhoneOS.platform/Developer")
endif (NOT DEFINED CMAKE_IOS_DEVELOPER_ROOT)
set (CMAKE_IOS_DEVELOPER_ROOT ${CMAKE_IOS_DEVELOPER_ROOT} CACHE PATH "Location of iOS SDKs")

# Find installed iOS SDKs
file (GLOB _CMAKE_IOS_SDKS "${CMAKE_IOS_DEVELOPER_ROOT}/SDKs/*")

# Find and use the most recent iOS sdk 
if (_CMAKE_IOS_SDKS) 
	list (SORT _CMAKE_IOS_SDKS)
	list (REVERSE _CMAKE_IOS_SDKS)
	list (GET _CMAKE_IOS_SDKS 0 _CMAKE_IOS_SDK_ROOT)

	# Set the sysroot default to the most recent SDK
	set (CMAKE_OSX_SYSROOT ${_CMAKE_IOS_SDK_ROOT} CACHE PATH "Sysroot used for iOS support")

	# set the architecture for iOS - using ARCHS_STANDARD_32_BIT sets armv6,armv7 and appears to be XCode's standard. 
	# The other value that works is ARCHS_UNIVERSAL_IPHONE_OS but that sets armv7 only
	set (CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD_32_BIT)" CACHE string  "Build architecture for iOS")

	# Set the default based on this file and not the environment variable
	set (CMAKE_FIND_ROOT_PATH ${CMAKE_IOS_DEVELOPER_ROOT} ${_CMAKE_IOS_SDK_ROOT} CACHE string  "iOS library search path root")

	# default to searching for frameworks first
	set (CMAKE_FIND_FRAMEWORK FIRST)

	# set up the default search directories for frameworks
	set (CMAKE_SYSTEM_FRAMEWORK_PATH
		${_CMAKE_IOS_SDK_ROOT}/System/Library/Frameworks
		${_CMAKE_IOS_SDK_ROOT}/System/Library/PrivateFrameworks
		${_CMAKE_IOS_SDK_ROOT}/Developer/Library/Frameworks
	)

	# only search the iOS sdks, not the remainder of the host filesystem
	set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY)
	set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
	set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
else (_CMAKE_IOS_SDKS)
	message (FATAL_ERROR "No iOS SDK's found in default seach path ${CMAKE_IOS_DEVELOPER_ROOT}. Set their location or install the iOS SDK.")
endif (_CMAKE_IOS_SDKS)

if (False)

if ("${CMAKE_BACKWARDS_COMPATIBILITY}" MATCHES "^1\\.[0-6]$")
	set (CMAKE_SHARED_MODULE_CREATE_C_FLAGS "${CMAKE_SHARED_MODULE_CREATE_C_FLAGS} -flat_namespace -undefined suppress")
endif ("${CMAKE_BACKWARDS_COMPATIBILITY}" MATCHES "^1\\.[0-6]$")

if (NOT XCODE)
	  # Enable shared library versioning.  This flag is not actually referenced
	  # but the fact that the setting exists will cause the generators to support
	  # soname computation.
	  set (CMAKE_SHARED_LIBRARY_SONAME_C_FLAG "-install_name")
endif (NOT XCODE)

# Xcode does not support -isystem yet.
if (XCODE)
	set (CMAKE_INCLUDE_SYSTEM_FLAG_C)
	set (CMAKE_INCLUDE_SYSTEM_FLAG_CXX)
endif (XCODE)

# Need to list dependent shared libraries on link line.  When building
# with -isysroot (for universal binaries), the linker always looks for
# dependent libraries under the sysroot.  Listing them on the link
# line works around the problem.
set (CMAKE_LINK_DEPENDENT_LIBRARY_FILES 1)

set (CMAKE_C_CREATE_SHARED_LIBRARY_FORBIDDEN_FLAGS -w)
set (CMAKE_CXX_CREATE_SHARED_LIBRARY_FORBIDDEN_FLAGS -w)

set (CMAKE_C_CREATE_SHARED_LIBRARY
	"<CMAKE_C_COMPILER> <LANGUAGE_COMPILE_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS> <LINK_FLAGS> -o <TARGET> -install_name <TARGET_INSTALLNAME_DIR><TARGET_SONAME> <OBJECTS> <LINK_LIBRARIES>")
set (CMAKE_CXX_CREATE_SHARED_LIBRARY
	"<CMAKE_CXX_COMPILER> <LANGUAGE_COMPILE_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS> <LINK_FLAGS> -o <TARGET> -install_name <TARGET_INSTALLNAME_DIR><TARGET_SONAME> <OBJECTS> <LINK_LIBRARIES>")

set (CMAKE_CXX_CREATE_SHARED_MODULE
	"<CMAKE_CXX_COMPILER> <LANGUAGE_COMPILE_FLAGS> <CMAKE_SHARED_MODULE_CREATE_CXX_FLAGS> <LINK_FLAGS> -o <TARGET> <OBJECTS> <LINK_LIBRARIES>")

set (CMAKE_C_CREATE_SHARED_MODULE
	"<CMAKE_C_COMPILER>  <LANGUAGE_COMPILE_FLAGS> <CMAKE_SHARED_MODULE_CREATE_C_FLAGS> <LINK_FLAGS> -o <TARGET> <OBJECTS> <LINK_LIBRARIES>")

set (CMAKE_C_CREATE_MACOSX_FRAMEWORK
	"<CMAKE_C_COMPILER> <LANGUAGE_COMPILE_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS> <LINK_FLAGS> -o <TARGET> -install_name <TARGET_INSTALLNAME_DIR><TARGET_SONAME> <OBJECTS> <LINK_LIBRARIES>")
set (CMAKE_CXX_CREATE_MACOSX_FRAMEWORK
	"<CMAKE_CXX_COMPILER> <LANGUAGE_COMPILE_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS> <LINK_FLAGS> -o <TARGET> -install_name <TARGET_INSTALLNAME_DIR><TARGET_SONAME> <OBJECTS> <LINK_LIBRARIES>")

# Add the install directory of the running cmake to the search directories
# CMAKE_ROOT is CMAKE_INSTALL_PREFIX/share/cmake, so we need to go two levels up
get_filename_component (_CMAKE_INSTALL_DIR "${CMAKE_ROOT}" PATH)
get_filename_component (_CMAKE_INSTALL_DIR "${_CMAKE_INSTALL_DIR}" PATH)
 
# List common installation prefixes.  These will be used for all search types
list (APPEND CMAKE_SYSTEM_PREFIX_PATH
	# Standard
	${CMAKE_IOS_DEVELOPER_ROOT}/usr
	${_CMAKE_IOS_SDK_ROOT}/usr

	# CMake install location
	"${_CMAKE_INSTALL_DIR}"

	# Project install destination.
	"${CMAKE_INSTALL_PREFIX}"
)
message (FATAL_ERROR "F yuou!")
endif (False)
