# Add the source code tree for the appropriate build
if (TOADLET_PLATFORM_BLACKBERRY)
	add_subdirectory (java)
else (TOADLET_PLATFORM_BLACKBERRY)
	add_subdirectory (cpp)
endif (TOADLET_PLATFORM_BLACKBERRY)

