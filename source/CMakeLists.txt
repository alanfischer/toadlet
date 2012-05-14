# Add the swig bindings first
if (TOADLET_PLATFORM_ANDROID OR TOADLET_PLATFORM_IOS)
	add_subdirectory (swig)
endif (TOADLET_PLATFORM_ANDROID OR TOADLET_PLATFORM_IOS)

# Add the c++ source code
add_subdirectory (cpp)

# Python requries the swig bindings be added later
if (TOADLET_PLATFORM_PYTHON)
	add_subdirectory (swig)
endif (TOADLET_PLATFORM_PYTHON)

# Older versions of android require shell java libraries
if (TOADLET_PLATFORM_ANDROID AND ANDROID_NDK_API_LEVEL LESS 9)
	if (NOT EXISTS ${ANDROID_JAR})
		message (FATAL_ERROR "ANDROID_JAR not found. Toadlet with Android NDK API levels < 9 requires the ANDROID_SDK variable to be set to your install of the Android SDK" )
	endif (NOT EXISTS ${ANDROID_JAR})

	add_subdirectory (java)
endif (TOADLET_PLATFORM_ANDROID AND ANDROID_NDK_API_LEVEL LESS 9)

