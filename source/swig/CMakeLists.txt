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

	# List the main and dependent interface files and the output language-specific files for each library

	# Toadlet Flick
	set (FLICK_SWIG_INT 
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/flick/flick.i 
		PARENT_SCOPE
	)
	set (FLICK_SWIG_INT_DEPS
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/flick/BaseInputDevice.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/flick/InputData.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/flick/InputDevice.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/flick/InputDeviceListener.i
		PARENT_SCOPE
	)
	if (TOADLET_PLATFORM_ANDROID)
		set (FLICK_SWIG_INT_OUTPUTS
			${PROJECT_BINARY_DIR}/source/java/us/toadlet/flick/Version.java
			${PROJECT_BINARY_DIR}/source/java/us/toadlet/flick/BaseInputDevice.java
			${PROJECT_BINARY_DIR}/source/java/us/toadlet/flick/InputData.java
			${PROJECT_BINARY_DIR}/source/java/us/toadlet/flick/InputDevice.java
			${PROJECT_BINARY_DIR}/source/java/us/toadlet/flick/InputDeviceListener.java
			${PROJECT_BINARY_DIR}/source/java/us/toadlet/flick/flick.java
			${PROJECT_BINARY_DIR}/source/java/us/toadlet/flick/flickJNI.java
			PARENT_SCOPE
		)	
	elseif (TOADLET_PLATFORM_IOS)
		set (FLICK_SWIG_INT_OUTPUTS
		)
	endif (TOADLET_PLATFORM_ANDROID)

	# Toadlet Peeper
	set (PEEPER_SWIG_INT
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/peeper/peeper.i 
		PARENT_SCOPE
	)
	set (PEEPER_SWIG_INT_DEPS
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/peeper/RenderDevice.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/peeper/RenderTarget.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/peeper/GLRenderTarget.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/peeper/WindowRenderTargetFormat.i
		PARENT_SCOPE
	)
	if (TOADLET_PLATFORM_ANDROID)
		set (PEEPER_SWIG_INT_OUTPUTS
		)
	elseif (TOADLET_PLATFORM_IOS)
		set (PEEPER_SWIG_INT_OUTPUTS
		)
	endif (TOADLET_PLATFORM_ANDROID)

	# Toadlet Ribbit
	set (RIBBIT_SWIG_INT
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/ribbit/ribbit.i
		PARENT_SCOPE
	)
	set (RIBBIT_SWIG_INT_DEPS
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/ribbit/AudioFormat.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/ribbit/AudioBuffer.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/ribbit/AudioStream.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/ribbit/Audio.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/ribbit/AudioDevice.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/ribbit/BaseAudio.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/ribbit/BaseAudioBuffer.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/ribbit/BaseAudioDevice.i
		PARENT_SCOPE
	)
	if (TOADLET_PLATFORM_ANDROID)
		set (RIBBIT_SWIG_INT_OUTPUTS
		)
	elseif (TOADLET_PLATFORM_IOS)
		set (RIBBIT_SWIG_INT_OUTPUTS
		)
	endif (TOADLET_PLATFORM_ANDROID)

	# Toadlet Tadpole
	set (TADPOLE_SWIG_INT
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/tadpole/tadpole.i
		PARENT_SCOPE
	)
	set (TADPOLE_SWIG_INT_DEPS
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/tadpole/ActionComponent.i
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
	if (TOADLET_PLATFORM_ANDROID)
		set (TADPOLE_SWIG_INT_OUTPUTS
		)
	elseif (TOADLET_PLATFORM_IOS)
		set (TADPOLE_SWIG_INT_OUTPUTS
		)
	endif (TOADLET_PLATFORM_ANDROID)
else (NOT TOADLET_PLATFORM_PYTHON)
	# Python needs a global interface file
	set (TOADLET_SWIG_INT
		toadlet/toadlet.i
	)

	# Python swig setup
	set (CMAKE_SWIG_FLAGS "")
	find_package(PythonLibs)
	include_directories(${PYTHON_INCLUDE_PATH})
	include_directories(${PROJECT_SOURCE_DIR}/source/swig)
	include_directories(${PROJECT_SOURCE_DIR}/source/cpp)
	set_source_files_properties(${TOADLET_SWIG_INT} PROPERTIES CPLUSPLUS ON)
	set_source_files_properties(${TOADLET_SWIG_INT} PROPERTIES SWIG_FLAGS "-includeall")

	# Create the python swig library
	swig_add_module(pytoadlet python ${TOADLET_SWIG_INT})
	swig_link_libraries(pytoadlet ${PYTHON_LIBRARIES} ${TOADLET_LIBRARIES})

	# Install the python library
	install (FILES ${pytoadlet_BINARY_DIR}/pytoadlet.py DESTINATION ${CMAKE_INSTALL_PREFIX}/Lib)
	install (FILES ${pytoadlet_BINARY_DIR}/Release/_pytoadlet.pyd DESTINATION ${CMAKE_INSTALL_PREFIX}/Lib CONFIGURATIONS Release)
	install (FILES ${pytoadlet_BINARY_DIR}/Debug/_pytoadlet.pyd DESTINATION ${CMAKE_INSTALL_PREFIX}/Lib CONFIGURATIONS Debug)
endif (NOT TOADLET_PLATFORM_PYTHON)


