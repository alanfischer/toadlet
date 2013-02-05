# Swig is used very differently for python and non-python platforms
# This is because python creates a separate shared library after compilation and links to the existing toadlet build,
# otherwise the swig wrapper files are generated first and compiled directly into the toadlet libraries.
if (NOT TOADLET_PLATFORM_PYTHON)
	# Setup the swig wrap macro and include the header output folder
	include_directories (${PROJECT_BINARY_DIR}/source/java)
	swig_wrap_setup (java us.toadlet ${PROJECT_BINARY_DIR}/source/java ${CMAKE_CURRENT_SOURCE_DIR})

	# List the main and dependent interface files and the output language-specific files for each library

	# Toadlet Flick
	set (FLICK_SWIG_INT 
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/flick/flick.i 
		PARENT_SCOPE
	)
	set (FLICK_SWIG_INT_DEPS
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/flick/BaseInputDevice.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/flick/BaseInputDeviceListener.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/flick/InputData.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/flick/InputDevice.i
		PARENT_SCOPE
	)
	set (FLICK_SWIG_INT_OUTPUTS
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/flick/BaseInputDevice.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/flick/BaseInputDeviceListener.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/flick/InputData.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/flick/InputDevice.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/flick/flick.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/flick/flickJNI.java
		PARENT_SCOPE
	)	

	# Toadlet Peeper
	set (PEEPER_SWIG_INT
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/peeper/peeper.i 
		PARENT_SCOPE
	)
	set (PEEPER_SWIG_INT_DEPS
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/peeper/RenderDevice.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/peeper/RenderState.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/peeper/RenderTarget.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/peeper/GLRenderTarget.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/peeper/WindowRenderTargetFormat.i
		PARENT_SCOPE
	)
	set (PEEPER_SWIG_INT_OUTPUTS
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/peeper/RenderDevice.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/peeper/RenderState.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/peeper/RenderTarget.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/peeper/GLRenderTarget.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/peeper/WindowRenderTargetFormat.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/peeper/peeper.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/peeper/peeperJNI.java
		PARENT_SCOPE
	)

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
	set (RIBBIT_SWIG_INT_OUTPUTS
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/ribbit/AudioFormat.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/ribbit/AudioBuffer.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/ribbit/AudioStream.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/ribbit/Audio.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/ribbit/AudioDevice.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/ribbit/BaseAudio.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/ribbit/BaseAudioBuffer.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/ribbit/BaseAudioDevice.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/ribbit/ribbit.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/ribbit/ribbitJNI.java
		PARENT_SCOPE
	)

	# Toadlet Tadpole
	set (TADPOLE_SWIG_INT
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/tadpole/tadpole.i
		PARENT_SCOPE
	)
	set (TADPOLE_SWIG_INT_DEPS
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/tadpole/Engine.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/tadpole/Scene.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/tadpole/Component.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/tadpole/Track.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/tadpole/Sequence.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/tadpole/Action.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/tadpole/Visible.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/tadpole/AnimationAction.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/tadpole/ActionComponent.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/tadpole/AudioComponent.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/tadpole/Camera.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/tadpole/CameraComponent.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/tadpole/MeshComponent.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/tadpole/LightComponent.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/tadpole/Node.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/tadpole/animation/Animation.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/tadpole/animation/BaseAnimation.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/tadpole/animation/MeshAnimation.i
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/tadpole/animation/MaterialAnimation.i
		PARENT_SCOPE
	)
	set (TADPOLE_SWIG_INT_OUTPUTS
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/tadpole/Engine.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/tadpole/Scene.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/tadpole/Component.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/tadpole/Track.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/tadpole/Sequence.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/tadpole/Action.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/tadpole/Visible.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/tadpole/AnimationAction.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/tadpole/ActionComponent.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/tadpole/AudioComponent.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/tadpole/Camera.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/tadpole/CameraComponent.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/tadpole/MeshComponent.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/tadpole/LightComponent.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/tadpole/Node.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/tadpole/tadpole.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/tadpole/tadpoleJNI.java
		PARENT_SCOPE
	)
	
	# Toadlet Pad
	set (PAD_SWIG_INT
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/pad/pad.i
		PARENT_SCOPE
	)
	set (PAD_SWIG_INT_DEPS
		${CMAKE_CURRENT_SOURCE_DIR}/toadlet/pad/Applet.i
		PARENT_SCOPE
	)
	set (PAD_SWIG_INT_OUTPUTS
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/pad/Applet.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/pad/pad.java
		${PROJECT_BINARY_DIR}/source/java/us/toadlet/pad/padJNI.java
		PARENT_SCOPE
	)
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


