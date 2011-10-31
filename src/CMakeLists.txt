# Add the source code tree
add_subdirectory (cpp)

# Older versions of android require shell java libraries
if (TOADLET_PLATFORM_ANDROID AND ANDROID_NDK_API_LEVEL LESS 9)
	if (NOT DEFINED ANDROID_SDK)
		message (FATAL_ERROR "Toadlet with Android NDK API levels < 9 require the ANDROID_SDK variable to be set to your install of the Android SDK" )
	endif (NOT DEFINED ANDROID_SDK)

	add_subdirectory (java)
endif (TOADLET_PLATFORM_ANDROID AND ANDROID_NDK_API_LEVEL LESS 9)

