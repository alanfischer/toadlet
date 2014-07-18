cmake_minimum_required (VERSION 2.8.12)


# Project name and compilers
project (toadlet CXX C)


# Version Settings for cpp libraries
set (TOADLET_BUILD_VERSION_EGG_MAJOR 0)
set (TOADLET_BUILD_VERSION_EGG_MINOR 9)
set (TOADLET_BUILD_VERSION_EGG_MICRO 0)
set (TOADLET_BUILD_VERSION_FLICK_MAJOR 0)
set (TOADLET_BUILD_VERSION_FLICK_MINOR 9)
set (TOADLET_BUILD_VERSION_FLICK_MICRO 0)
set (TOADLET_BUILD_VERSION_HOP_MAJOR 0)
set (TOADLET_BUILD_VERSION_HOP_MINOR 9)
set (TOADLET_BUILD_VERSION_HOP_MICRO 0)
set (TOADLET_BUILD_VERSION_KNOT_MAJOR 0)
set (TOADLET_BUILD_VERSION_KNOT_MINOR 9)
set (TOADLET_BUILD_VERSION_KNOT_MICRO 0)
set (TOADLET_BUILD_VERSION_PEEPER_MAJOR 0)
set (TOADLET_BUILD_VERSION_PEEPER_MINOR 9)
set (TOADLET_BUILD_VERSION_PEEPER_MICRO 0)
set (TOADLET_BUILD_VERSION_RIBBIT_MAJOR 0)
set (TOADLET_BUILD_VERSION_RIBBIT_MINOR 9)
set (TOADLET_BUILD_VERSION_RIBBIT_MICRO 0)
set (TOADLET_BUILD_VERSION_TADPOLE_MAJOR 0)
set (TOADLET_BUILD_VERSION_TADPOLE_MINOR 9)
set (TOADLET_BUILD_VERSION_TADPOLE_MICRO 0)
set (TOADLET_BUILD_VERSION_PAD_MAJOR 0)
set (TOADLET_BUILD_VERSION_PAD_MINOR 9)
set (TOADLET_BUILD_VERSION_PAD_MICRO 0)


# Setup CTest
enable_testing()


# Custom cmake scripts
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules/")

# Get the current hg revision
if (ANDROID OR IOS)
	find_host_package (Mercurial)
else (ANDROID OR IOS)
	find_package (Mercurial)
endif (ANDROID OR IOS)
if (MERCURIAL_FOUND)
	MERCURIAL_HG_INFO(${PROJECT_SOURCE_DIR} TOADLET)
endif (MERCURIAL_FOUND)


# Assign the TOADLET_PLATFORM_X value, used throughout toadlet for consistency
if (ANDROID)
	set (TOADLET_PLATFORM_ANDROID ON CACHE BOOL "Toadlet for Android")

	option (INSTALL_ANDROID_APP "Install just the libs for use by an android application" OFF)
endif (ANDROID)
if (WIN32)
	set (TOADLET_PLATFORM_WIN32 ON CACHE BOOL "Toadlet for windows platforms")
	if (WINCE)
		# TODO: Right now WinCE is being blocked due to the uncertain future of Toadlet on Windows Phone Platforms 
		message (FATAL_ERROR "WindowsCE builds are currently disabled given the uncertain future of native code on Windows Phone 7. You may comment out this error to attempt the build anyway. Your mileage may vary.")
		set (TOADLET_PLATFORM_WINCE ON CACHE BOOL "Toadlet for Windows CE")
	endif(WINCE)
elseif (UNIX)
	set (TOADLET_PLATFORM_POSIX ON CACHE BOOL "Toadlet for unix platforms")
	if (APPLE) 
		set (TOADLET_PLATFORM_OSX ON CACHE BOOL "Toadlet for OSX")
		if (IOS)
			set (TOADLET_PLATFORM_IOS ON CACHE BOOL "Toadlet for iOS")
		endif (IOS)
	endif (APPLE)
	if (EMSCRIPTEN)
		set (TOADLET_PLATFORM_EMSCRIPTEN ON CACHE BOOL "Toadlet for EMSCRIPTEN")
	endif (EMSCRIPTEN)
endif (WIN32)


# Toadlet options
option (TOADLET_RUN_MEMCHECK "Run memory check operations on unit tests" OFF)
option (TOADLET_BUILD_DOXYGEN "Build doxygen documentation along with source files" OFF)


# Build doxygen or build toadlet
if (TOADLET_BUILD_DOXYGEN)
	find_package (Doxygen)
	find_package (SWIG)
	if (DOXYGEN_FOUND AND DOXYGEN_DOT_FOUND AND SWIG_FOUND)
		# Generate the swig wrapped files
		include (SwigWrapFiles)
		add_subdirectory (source/swig)

		# Setup doxygen
		add_subdirectory (doxygen/java)
		add_subdirectory (doxygen/cpp)
	else (DOXYGEN_FOUND AND DOXYGEN_DOT_FOUND AND SWIG_FOUND)
		message ("Doxygen, dot and/or swig was not found. Doxygen docs will not be built")
	endif (DOXYGEN_FOUND AND DOXYGEN_DOT_FOUND AND SWIG_FOUND)
else (TOADLET_BUILD_DOXYGEN)
	if (TOADLET_PLATFORM_ANDROID)
		if (NOT EXISTS ${ANDROID_JAR})
			message (FATAL_ERROR "ANDROID_JAR not found")
		endif (NOT EXISTS ${ANDROID_JAR})

		# Android requires swig to generate bindings before compilation
		# Find swig on our host and include our swig wrapper macro
		find_host_package (SWIG REQUIRED)
		include (SwigWrapFiles)

		# Add the swig files
		add_subdirectory (source/swig)

		# Android requires java code
		add_subdirectory (source/java)
	endif (TOADLET_PLATFORM_ANDROID)

	# c++ code
	add_subdirectory (source/cpp)

	if (TOADLET_PLATFORM_PYTHON)
		# Python requries the swig bindings be added as a separate module
		find_package (SWIG REQUIRED)
		include (UseSwig)
		
		# Add the swig files
		add_subdirectory (source/swig)
	endif (TOADLET_PLATFORM_PYTHON)
endif (TOADLET_BUILD_DOXYGEN)

