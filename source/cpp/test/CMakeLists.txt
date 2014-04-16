if (NOT TOADLET_PLATFORM_ANDROID AND NOT TOADLET_PLATFORM_IOS)
	set (UNIT_SRC
		ContainerTest.cpp
		MathTest.cpp
		StreamTest.cpp
		NodeTest.cpp
		unitTest.cpp
	)
	
	add_executable (unitTest ${UNIT_SRC})
	
	# Platform specific libraries
	if (TOADLET_PLATFORM_WIN32)
		target_link_libraries (unitTest toadlet_egg toadlet_peeper toadlet_ribbit toadlet_tadpole)

		# On windows we use fixup_bundle to collect the required dlls for build directory execution
		set (FIXUP_APP ${CMAKE_CURRENT_BINARY_DIR}/\${CONFIGURATION}/unitTest.exe)
		file(TO_CMAKE_PATH ${CMAKE_PREFIX_PATH}/lib PREFIX_LIB)
		file(TO_CMAKE_PATH ${CMAKE_PREFIX_PATH}/bin PREFIX_BIN)
		set (FIXUP_DIRS
			${CMAKE_CURRENT_BINARY_DIR}/../toadlet/egg/\${CONFIGURATION}
			${CMAKE_CURRENT_BINARY_DIR}/../toadlet/hop/\${CONFIGURATION}
			${CMAKE_CURRENT_BINARY_DIR}/../toadlet/peeper/\${CONFIGURATION}
			${CMAKE_CURRENT_BINARY_DIR}/../toadlet/ribbit/\${CONFIGURATION}
			${CMAKE_CURRENT_BINARY_DIR}/../toadlet/tadpole/\${CONFIGURATION}
			${PREFIX_LIB}
			${PREFIX_BIN}
		)
		configure_file (${PROJECT_SOURCE_DIR}/cmake/Modules/Fixup.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/Fixup.cmake @ONLY)
		add_custom_command (TARGET unitTest POST_BUILD COMMAND ${CMAKE_COMMAND} -DCONFIGURATION=$<CONFIGURATION> -P ${CMAKE_CURRENT_BINARY_DIR}/Fixup.cmake)
	elseif (TOADLET_PLATFORM_OSX)
		target_link_libraries (unitTest toadlet_egg toadlet_peeper toadlet_ribbit toadlet_tadpole  "-framework Foundation")
	elseif (TOADLET_PLATFORM_EMSCRIPTEN)
		target_link_libraries (unitTest toadlet_egg_s toadlet_peeper_s toadlet_ribbit_s toadlet_tadpole_s)
		set_target_properties( unitTest PROPERTIES OUTPUT_NAME unitTest.html )
	elseif (TOADLET_PLATFORM_POSIX)
		target_link_libraries (unitTest toadlet_egg toadlet_peeper toadlet_ribbit toadlet_tadpole -lpthread)
	endif (TOADLET_PLATFORM_WIN32)	

	if (TOADLET_RUN_MEMCHECK)
		# Valgrind if available
		find_program (MEMCHECK_EXE valgrind)

		if (MEMCHECK_EXE)
			if (${CMAKE_GENERATOR} STREQUAL "Xcode")
				set (UNITTEST_EXE Debug/unitTest)
			elseif (${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")
				set (UNITTEST_EXE ./unitTest)
			endif (${CMAKE_GENERATOR} STREQUAL "Xcode")

			if (APPLE)
				add_test (NAME memcheck_toadletUnitTests COMMAND ${MEMCHECK_EXE} "--leak-check=yes" "--dsymutil=yes" "--error-exitcode=1" "--suppressions=${PROJECT_SOURCE_DIR}/valgrind/osx-suppressions.txt" ${UNITTEST_EXE})
			else (APPLE)
				add_test (NAME memcheck_toadletUnitTests COMMAND ${MEMCHECK_EXE} "--leak-check=yes" "--error-exitcode=1" ${UNITTEST_EXE})
			endif(APPLE)
		else (MEMCHECK_EXE)
			message (FATAL_ERROR "No program found for MEMCHECK_EXE, memcheck cannot be run")
		endif (MEMCHECK_EXE)
	else (TOADLET_RUN_MEMCHECK)
		# Otherwise run the unit test
		add_test (NAME toadletUnitTests COMMAND unitTest)
	endif (TOADLET_RUN_MEMCHECK)
endif (NOT TOADLET_PLATFORM_ANDROID AND NOT TOADLET_PLATFORM_IOS)

