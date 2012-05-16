# Swig is used very differently for python and non-python platforms
# This is because python creates a separate shared library after compilation and links to the existing toadlet build,
# otherwise the swig wrapper files are generated first and compiled directly into the toadlet libraries.
if (NOT TOADLET_PLATFORM_PYTHON)
	# Setup the swig wrap macro and include the header output folder
	if (TOADLET_PLATFORM_ANDROID)
		include_directories (${PROJECT_BINARY_DIR}/source/java)
		swig_wrap_setup (java us.toadlet ${PROJECT_BINARY_DIR}/source/java ${CMAKE_CURRENT_SOURCE_DIR})
	else (TOADLET_PLATFORM_IOS)
		include_directories (${PROJECT_BINARY_DIR}/source/objc)
		swig_wrap_setup (objc NO ${PROJECT_BINARY_DIR}/source/objc ${CMAKE_CURRENT_SOURCE_DIR})
	endif (TOADLET_PLATFORM_ANDROID)

	# List the interface files for each library
	set (TOADLET_FLICK_INT
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/flick/flick.i
		PARENT_SCOPE
	)

	set (TOADLET_PEEPER_INT
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/peeper/peeper.i
		PARENT_SCOPE
	)

	set (TOADLET_RIBBIT_INT
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/ribbit/ribbit.i
		PARENT_SCOPE
	)

	set (TOADLET_TADPOLE_INT
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/tadpole/tadpole.i
		PARENT_SCOPE
	)
else (NOT TOADLET_PLATFORM_PYTHON)
	# Python needs a global interface file
	set (TOADLET_INT
		toadlet/toadlet.i
	)

	# Python swig setup
	set (CMAKE_SWIG_FLAGS "")
	find_package(PythonLibs)
	include_directories(${PYTHON_INCLUDE_PATH})
	include_directories(${PROJECT_SOURCE_DIR}/source/swig)
	include_directories(${PROJECT_SOURCE_DIR}/source/cpp)
	set_source_files_properties(${TOADLET_INT} PROPERTIES CPLUSPLUS ON)
	set_source_files_properties(${TOADLET_INT} PROPERTIES SWIG_FLAGS "-includeall")

	# Create the python swig library
	swig_add_module(pytoadlet python ${TOADLET_INT})
	swig_link_libraries(pytoadlet ${PYTHON_LIBRARIES} ${TOADLET_LIBRARIES})

	# Install the python library
	install (FILES ${pytoadlet_BINARY_DIR}/pytoadlet.py DESTINATION ${CMAKE_INSTALL_PREFIX}/Lib)
	install (FILES ${pytoadlet_BINARY_DIR}/Release/_pytoadlet.pyd DESTINATION ${CMAKE_INSTALL_PREFIX}/Lib CONFIGURATIONS Release)
	install (FILES ${pytoadlet_BINARY_DIR}/Debug/_pytoadlet.pyd DESTINATION ${CMAKE_INSTALL_PREFIX}/Lib CONFIGURATIONS Debug)
endif (NOT TOADLET_PLATFORM_PYTHON)

