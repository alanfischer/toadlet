# On Android NDK API levels < 9, toadlet uses java shim libraries for flick, peeper, ribbit, and pad.

# Special settings for debug builds
string (TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)

# Setup for Android Java Compilation
# Variables are named to mirror those in the FindJava.cmake script, which will not work under the android toolchain
# This uses the find_host_program macro from the Android toolchain
find_host_program (Java_JAVAC_EXECUTABLE javac)
find_host_program (Java_JAVAH_EXECUTABLE javah)
find_host_program (Java_JAR_EXECUTABLE jar)
if (NOT EXISTS ${Java_JAVAC_EXECUTABLE})
    message (FATAL_ERROR "Java compiler 'javac' could not be found. Ensure it is installed and in your PATH.")
endif (NOT EXISTS ${Java_JAVAC_EXECUTABLE})
if (NOT EXISTS ${Java_JAVAH_EXECUTABLE})
    message (FATAL_ERROR "Java generator 'javah' could not be found. Ensure it is installed and in your PATH.")
endif (NOT EXISTS ${Java_JAVAH_EXECUTABLE})
if (NOT EXISTS ${Java_JAR_EXECUTABLE})
    message (FATAL_ERROR "Java archiver 'jar' could not be found. Ensure it is installed and in your PATH.")
endif (NOT EXISTS ${Java_JAR_EXECUTABLE})
set (Java_FOUND TRUE)

# Cmake wants to use always use posix path separators due to the android ndk toolchain
# This does not work with nmake on windows, so force the proper ones here
if (${NDKSYSTEM} STREQUAL "windows" AND ${CMAKE_GENERATOR} STREQUAL "NMake Makefiles")
    set (WIN32 TRUE)
endif (${NDKSYSTEM} STREQUAL "windows" AND ${CMAKE_GENERATOR} STREQUAL "NMake Makefiles")

# Grab the cmake java script
include (UseJava)

# Add the android jar to the classpath
set (CMAKE_JAVA_INCLUDE_PATH ${ANDROID_JAR})

# Compiler flags
set (CMAKE_JAVA_COMPILE_FLAGS -Xlint:unchecked)

# Used by the cpp classes to locate the javah generated headers
set (TOADLET_JAVA_BASEDIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)


# toadlet_flick
configure_file (us/toadlet/flick/Version.java.cmake ${CMAKE_CURRENT_BINARY_DIR}/us/toadlet/flick/Version.java)

set (FLICK_SRC
	us/toadlet/flick/InputDevice.java
	us/toadlet/flick/InputDeviceListener.java
	us/toadlet/flick/NInputDeviceListener.java
	us/toadlet/flick/AndroidSensorDevice.java
	${CMAKE_CURRENT_BINARY_DIR}/us/toadlet/flick/Version.java
)

set (FLICK_H
	us.toadlet.flick.NInputDeviceListener
)

# Create the jar file with UseJava.cmake functions. See UseJava.cmake for details.
if (${BUILD_TYPE} STREQUAL "DEBUG")
	set (CMAKE_JAVA_TARGET_OUTPUT_NAME jtoadlet_flick_d)
endif (${BUILD_TYPE} STREQUAL "DEBUG")
set (CMAKE_JAR_CLASSES_PREFIX us/toadlet/flick)
add_jar (jtoadlet_flick ${FLICK_SRC})
get_property (JARFILE TARGET jtoadlet_flick PROPERTY JAR_FILE)
install (FILES ${JARFILE} DESTINATION lib)

# Generate headers
add_custom_command (TARGET jtoadlet_flick POST_BUILD
	COMMAND ${Java_JAVAH_EXECUTABLE} -classpath ${JARFILE} ${FLICK_H}
)


# toadlet_peeper
configure_file (us/toadlet/peeper/Version.java.cmake ${CMAKE_CURRENT_BINARY_DIR}/us/toadlet/peeper/Version.java)

set (PEEPER_SRC
	us/toadlet/peeper/TextureFormat.java
	us/toadlet/peeper/WindowRenderTargetFormat.java
	us/toadlet/peeper/RenderTarget.java
	us/toadlet/peeper/RenderDevice.java
	us/toadlet/peeper/GLRenderTarget.java
	us/toadlet/peeper/EGLRenderTarget.java
	us/toadlet/peeper/EGLWindowRenderTarget.java
	us/toadlet/peeper/NGLRenderDevice.java
	${CMAKE_CURRENT_BINARY_DIR}/us/toadlet/peeper/Version.java
)

set (PEEPER_H
	us.toadlet.peeper.NGLRenderDevice
)

# Create the jar file with UseJava.cmake functions. See UseJava.cmake for details.
if (${BUILD_TYPE} STREQUAL "DEBUG")
    set (CMAKE_JAVA_TARGET_OUTPUT_NAME jtoadlet_peeper_d)
endif (${BUILD_TYPE} STREQUAL "DEBUG")
set (CMAKE_JAR_CLASSES_PREFIX us/toadlet/peeper)
add_jar (jtoadlet_peeper ${PEEPER_SRC})
get_property (JARFILE TARGET jtoadlet_peeper PROPERTY JAR_FILE)
install (FILES ${JARFILE} DESTINATION lib)

# Generate headers
add_custom_command (TARGET jtoadlet_peeper POST_BUILD
    COMMAND ${Java_JAVAH_EXECUTABLE} -classpath ${JARFILE} ${PEEPER_H}
)


# toadlet_ribbit
configure_file (us/toadlet/ribbit/Version.java.cmake ${CMAKE_CURRENT_BINARY_DIR}/us/toadlet/ribbit/Version.java)

set (RIBBIT_SRC
	us/toadlet/ribbit/AudioFormat.java
	us/toadlet/ribbit/AudioStream.java
	us/toadlet/ribbit/AudioBuffer.java
	us/toadlet/ribbit/Audio.java
	us/toadlet/ribbit/AudioCaps.java
	us/toadlet/ribbit/AudioDevice.java
	us/toadlet/ribbit/ATAudioBuffer.java
	us/toadlet/ribbit/ATAudio.java
	us/toadlet/ribbit/ATAudioDevice.java
	us/toadlet/ribbit/NAudioStream.java
	${CMAKE_CURRENT_BINARY_DIR}/us/toadlet/ribbit/Version.java
)

set (RIBBIT_H
	us.toadlet.ribbit.NAudioStream
)

# Create the jar file with UseJava.cmake functions. See UseJava.cmake for details.
if (${BUILD_TYPE} STREQUAL "DEBUG")
    set (CMAKE_JAVA_TARGET_OUTPUT_NAME jtoadlet_ribbit_d)
endif (${BUILD_TYPE} STREQUAL "DEBUG")
set (CMAKE_JAR_CLASSES_PREFIX us/toadlet/ribbit)
add_jar (jtoadlet_ribbit ${RIBBIT_SRC})
get_property (JARFILE TARGET jtoadlet_ribbit PROPERTY JAR_FILE)
install (FILES ${JARFILE} DESTINATION lib)

# Generate headers
add_custom_command (TARGET jtoadlet_ribbit POST_BUILD
    COMMAND ${Java_JAVAH_EXECUTABLE} -classpath ${JARFILE} ${RIBBIT_H}
)


# toadlet_tadpole
configure_file (us/toadlet/tadpole/Version.java.cmake ${CMAKE_CURRENT_BINARY_DIR}/us/toadlet/tadpole/Version.java)

set (TADPOLE_SRC
	us/toadlet/tadpole/Engine.java
	us/toadlet/tadpole/Scene.java
	us/toadlet/tadpole/Node.java
	us/toadlet/tadpole/CameraNode.java
	us/toadlet/tadpole/LightNode.java
	us/toadlet/tadpole/MeshNode.java
	${CMAKE_CURRENT_BINARY_DIR}/us/toadlet/tadpole/Version.java
)

set (TADPOLE_H
	us.toadlet.tadpole.Engine
	us.toadlet.tadpole.Scene
	us.toadlet.tadpole.Node
	us.toadlet.tadpole.CameraNode
	us.toadlet.tadpole.LightNode
	us.toadlet.tadpole.MeshNode
)

# Create the jar file with UseJava.cmake functions. See UseJava.cmake for details.
if (${BUILD_TYPE} STREQUAL "DEBUG")
    set (CMAKE_JAVA_TARGET_OUTPUT_NAME jtoadlet_tadpole_d)
endif (${BUILD_TYPE} STREQUAL "DEBUG")
set (CMAKE_JAR_CLASSES_PREFIX us/toadlet/tadpole)
add_jar (jtoadlet_tadpole ${TADPOLE_SRC})
get_property (JARFILE TARGET jtoadlet_tadpole PROPERTY JAR_FILE)
install (FILES ${JARFILE} DESTINATION lib)

# Generate headers
add_custom_command (TARGET jtoadlet_tadpole POST_BUILD
    COMMAND ${Java_JAVAH_EXECUTABLE} -classpath ${JARFILE} ${TADPOLE_H}
)


# toadlet_pad
configure_file (us/toadlet/pad/Version.java.cmake ${CMAKE_CURRENT_BINARY_DIR}/us/toadlet/pad/Version.java)

set (PAD_SRC
	us/toadlet/pad/Applet.java
	us/toadlet/pad/AndroidApplication.java
	us/toadlet/pad/NApplet.java
	${CMAKE_CURRENT_BINARY_DIR}/us/toadlet/pad/Version.java
)

set (PAD_H
	us.toadlet.pad.NApplet
)

# Create the jar file with UseJava.cmake functions. See UseJava.cmake for details.
if (${BUILD_TYPE} STREQUAL "DEBUG")
    set (CMAKE_JAVA_TARGET_OUTPUT_NAME jtoadlet_pad_d)
endif (${BUILD_TYPE} STREQUAL "DEBUG")
set (CMAKE_JAR_CLASSES_PREFIX us/toadlet/pad)
add_jar (jtoadlet_pad ${PAD_SRC})
get_property (JARFILE TARGET jtoadlet_pad PROPERTY JAR_FILE)
install (FILES ${JARFILE} DESTINATION lib)

# Generate headers
add_custom_command (TARGET jtoadlet_pad POST_BUILD
    COMMAND ${Java_JAVAH_EXECUTABLE} -classpath ${JARFILE} ${PAD_H}
)


# Undo the forcing of WIN32
if (${NDKSYSTEM} STREQUAL "windows" AND ${CMAKE_GENERATOR} STREQUAL "NMake Makefiles")
    unset (WIN32)
endif (${NDKSYSTEM} STREQUAL "windows" AND ${CMAKE_GENERATOR} STREQUAL "NMake Makefiles")

