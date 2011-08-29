# Add the source code tree
add_subdirectory (cpp)
if (TOADLET_PLATFORM_ANDROID AND ANDROID_API_LEVEL LESS 9)
	add_subdirectory (java)
endif (TOADLET_PLATFORM_ANDROID AND ANDROID_API_LEVEL LESS 9)

