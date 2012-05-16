# These platforms require swig to generate bindings before compilation
if (TOADLET_PLATFORM_ANDROID OR TOADLET_PLATFORM_IOS)
	# Find swig on our host and include our swig wrapper macro
	find_host_package (SWIG REQUIRED)
	include (SwigWrapFiles)

	# Add the swig package
	add_subdirectory (swig)
endif (TOADLET_PLATFORM_ANDROID OR TOADLET_PLATFORM_IOS)


# Add the c++ source code
add_subdirectory (cpp)


# Python requries the swig bindings be added as a separate module
if (TOADLET_PLATFORM_PYTHON)
	# Find swig and include cmake's swig package
	find_package (SWIG REQUIRED)
	include (UseSwig)
	
	# Add the swig package
	add_subdirectory (swig)
endif (TOADLET_PLATFORM_PYTHON)


# Older versions of android require shell java libraries
if (TOADLET_PLATFORM_ANDROID AND ANDROID_NDK_API_LEVEL LESS 9)
	if (NOT EXISTS ${ANDROID_JAR})
		message (FATAL_ERROR "ANDROID_JAR not found. Toadlet with Android NDK API levels < 9 requires the ANDROID_SDK variable to be set to your install of the Android SDK" )
	endif (NOT EXISTS ${ANDROID_JAR})

	add_subdirectory (java)
endif (TOADLET_PLATFORM_ANDROID AND ANDROID_NDK_API_LEVEL LESS 9)

