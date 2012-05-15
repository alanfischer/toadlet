# Add this directory to the swing include path
set (SWIG_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)


# List the interface files for each library
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


# Platform specific settings
if (TOADLET_PLATFORM_PYTHON)
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
endif (TOADLET_PLATFORM_PYTHON)

