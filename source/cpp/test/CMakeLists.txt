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

	if (TOADLET_RUN_MEMCHECK)
		# Valgrind if available
		find_program (MEMCHECK_EXE valgrind)

		if (MEMCHECK_EXE)
			if (APPLE)
				add_test (NAME memcheck_toadletUnitTests COMMAND ${MEMCHECK_EXE} "--leak-check=yes" "--error-exitcode=1"  Debug/unitTest)
			elseif (UNIX)
				add_test (NAME memcheck_toadletUnitTests COMMAND ${MEMCHECK_EXE} "--leak-check=yes" "--error-exitcode=1" ./unitTest)
			endif (APPLE)
		else (MEMCHECK_EXE)
			message (FATAL_ERROR "No program found for MEMCHECK_EXE, memcheck cannot be run")
		endif (MEMCHECK_EXE)
	else (TOADLET_RUN_MEMCHECK)
		# Otherwise run the unit test
		add_test (NAME toadletUnitTests COMMAND unitTest)
	endif (TOADLET_RUN_MEMCHECK)
endif (NOT TOADLET_PLATFORM_ANDROID AND NOT TOADLET_PLATFORM_IOS)

