if (NOT TOADLET_PLATFORM_ANDROID AND NOT TOADLET_PLATFORM_IOS)
	set (UNIT_SRC
		MathTest.cpp
		unitTest.cpp
	)
	
	add_executable (unitTest ${UNIT_SRC})
	
	#Platform specific libraries
	if (TOADLET_PLATFORM_WIN32)
		target_link_libraries (unitTest toadlet_egg_s)
	elseif (TOADLET_PLATFORM_OSX)
		target_link_libraries (unitTest toadlet_egg_s "-framework Foundation")
	elseif (TOADLET_PLATFORM_POSIX)
		target_link_libraries (unitTest toadlet_egg_s -lpthread)
	endif (TOADLET_PLATFORM_WIN32)	
	
	add_test (NAME toadletUnitTests COMMAND unitTest)
endif (NOT TOADLET_PLATFORM_ANDROID AND NOT TOADLET_PLATFORM_IOS)