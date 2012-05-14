# Swig is required
find_host_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

# Interface files for each library
set (TOADLET_PEEPER_INT
	toadlet/peeper/RenderDevice.i
	toadlet/peeper/RenderTarget.i
)
set (TOADLET_RIBBIT_INT
	toadlet/ribbit/AudioDevice.i
)
set (TOADLET_TADPOLE_INT
	toadlet/tadpole/AnimationActionComponent.i
	toadlet/tadpole/AudioComponent.i
	toadlet/tadpole/CameraNode.i
	toadlet/tadpole/Component.i
	toadlet/tadpole/Engine.i
	toadlet/tadpole/LightNode.i
	toadlet/tadpole/MeshNode.i
	toadlet/tadpole/Node.i
	toadlet/tadpole/Scene.i
	toadlet/tadpole/animation/Animation.i
	toadlet/tadpole/animation/BaseAnimation.i
	toadlet/tadpole/animation/MeshAnimation.i
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
	set_source_files_properties(${INT} PROPERTIES CPLUSPLUS ON)
	set_source_files_properties(${INT} PROPERTIES SWIG_FLAGS "-includeall")

	# Create the python swig library
	swig_add_module(pytoadlet python ${TOADLET_INT})
	swig_link_libraries(pytoadlet ${PYTHON_LIBRARIES} ${TOADLET_LIBRARIES})

	# Install the python library
	install (FILES ${pytoadlet_BINARY_DIR}/pytoadlet.py DESTINATION ${CMAKE_INSTALL_PREFIX}/Lib)
	install (FILES ${pytoadlet_BINARY_DIR}/Release/_pytoadlet.pyd DESTINATION ${CMAKE_INSTALL_PREFIX}/Lib CONFIGURATIONS Release)
	install (FILES ${pytoadlet_BINARY_DIR}/Debug/_pytoadlet.pyd DESTINATION ${CMAKE_INSTALL_PREFIX}/Lib CONFIGURATIONS Debug)
else (TOADLET_PLATFORM_PYTHON)
	# Other languages create the binding object files that toadlet compiles into the standard libraries
	if (TOADLET_PLATFORM_ANDROID)
		set (SWIG_LANG java)
	else (TOADLET_PLATFORM_IOS)
		set (SWIG_LANG objc)
	endif (TOADLET_PLATFORM_ANDROID)

	# Create the wrapper objects
	add_custom_command (OUTPUT toadlet_peeper_wrap.cpp
		COMMAND ${SWIG_EXECUTABLE} -${SWIG_LANGUAGE} -c++ -o toadlet_peeper_wrap.cpp ${TOADLET_PEEPER_INT}
	)
	add_custom_command (OUTPUT toadlet_ribbit_wrap
		COMMAND ${SWIG_EXECUTABLE} -${SWIG_LANGUAGE} -c++ -o toadlet_ribbit_wrap.cpp ${TOADLET_RIBBIT_INT}
	)
	add_custom_command (OUTPUT toadlet_tadpole_wrap
		COMMAND ${SWIG_EXECUTABLE} -${SWIG_LANGUAGE} -c++ -o toadlet_tadpole_wrap.cpp ${TOADLET_TADPOLE_INT}
	)
endif (TOADLET_PLATFORM_PYTHON)

