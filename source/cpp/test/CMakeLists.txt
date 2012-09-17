if (NOT TOADLET_PLATFORM_ANDROID AND NOT TOADLET_PLATFORM_IOS)
	set (UNIT_SRC
		MathTest.cpp
		unitTest.cpp
	)
	
	add_executable (unitTest ${UNIT_SRC})
	
	# Platform specific libraries
	if (TOADLET_PLATFORM_WIN32)
		target_link_libraries (unitTest toadlet_egg_s)
	elseif (TOADLET_PLATFORM_OSX)
		target_link_libraries (unitTest toadlet_egg_s "-framework Foundation")
	elseif (TOADLET_PLATFORM_POSIX)
		target_link_libraries (unitTest toadlet_egg_s -lpthread)
	endif (TOADLET_PLATFORM_WIN32)	

	add_test (NAME toadletUnitTests COMMAND unitTest)
	
	# Valgrind if available
	if (TOADLET_RUN_MEMCHECK)
		find_program (MEMCHECK_EXE valgrind)

		if (MEMCHECK_EXE)
			if (APPLE)
				add_test (NAME memcheck_toadletUnitTests COMMAND ${MEMCHECK_EXE} "--leak-check=yes" Debug/unitTest)
			elseif (UNIX)
				add_test (NAME memcheck_toadletUnitTests COMMAND ${MEMCHECK_EXE} "--leak-check=yes" ./unitTest)
			endif (APPLE)
		else (MEMCHECK_EXE)
			message (FATAL_ERROR "No program found for MEMCHECK_EXE, memcheck cannot be run")
		endif (MEMCHECK_EXE)
	endif (TOADLET_RUN_MEMCHECK)
endif (NOT TOADLET_PLATFORM_ANDROID AND NOT TOADLET_PLATFORM_IOS)

