# Add the source code for the appropriate build
if (TOADLET_PLATFORM_ANDROID)
	add_subdirectory (java)
else (TOADLET_PLATFORM_ANDROID)
	add_subdirectory (cpp)
endif (TOADLET_PLATFORM_ANDROID)

