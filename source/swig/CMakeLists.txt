# List the interface files for each library
set (TOADLET_PEEPER_INT
	${CMAKE_CURRENT_SOURCE_DIR}/toadlet/peeper/RenderDevice.i
	${CMAKE_CURRENT_SOURCE_DIR}/toadlet/peeper/RenderTarget.i
	PARENT_SCOPE
)

set (TOADLET_RIBBIT_INT
	${CMAKE_CURRENT_SOURCE_DIR}/toadlet/ribbit/AudioDevice.i
	PARENT_SCOPE
)

set (TOADLET_TADPOLE_INT
	${CMAKE_CURRENT_SOURCE_DIR}/toadlet/tadpole/AnimationActionComponent.i
	${CMAKE_CURRENT_SOURCE_DIR}/toadlet/tadpole/AudioComponent.i
	${CMAKE_CURRENT_SOURCE_DIR}/toadlet/tadpole/CameraNode.i
	${CMAKE_CURRENT_SOURCE_DIR}/toadlet/tadpole/Component.i
	${CMAKE_CURRENT_SOURCE_DIR}/toadlet/tadpole/Engine.i
	${CMAKE_CURRENT_SOURCE_DIR}/toadlet/tadpole/LightNode.i
	${CMAKE_CURRENT_SOURCE_DIR}/toadlet/tadpole/MeshNode.i
	${CMAKE_CURRENT_SOURCE_DIR}/toadlet/tadpole/Node.i
	${CMAKE_CURRENT_SOURCE_DIR}/toadlet/tadpole/Scene.i
	${CMAKE_CURRENT_SOURCE_DIR}/toadlet/tadpole/animation/Animation.i
	${CMAKE_CURRENT_SOURCE_DIR}/toadlet/tadpole/animation/BaseAnimation.i
	${CMAKE_CURRENT_SOURCE_DIR}/toadlet/tadpole/animation/MeshAnimation.i
	PARENT_SCOPE
)


# Platform specific settings
if (TOADLET_PLATFORM_PYTHON)
	# Python needs an extra interface file
	set (TOADLET_INT
		toadlet/toadlet.i
		${TOADLET_PEEPER_INT}
		${TOADLET_RIBBIT_INT}
		${TOADLET_TADPOLE_INT}
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

