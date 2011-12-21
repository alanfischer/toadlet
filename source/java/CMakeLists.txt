# On Android NDK API levels < 9, toadlet uses java shim libraries for flick, peeper, ribbit, and pad.


# Shortcut variables
set (JAVA_SRC_DIR us/toadlet)
set (JAVA_OBJ obj)


# Used by the cpp classes to location the javah generated headers
set (TOADLET_JAVA_BASEDIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)


# Setup for Android Java Compilation - uses the find_host_program macro from the Android toolchain
find_host_program (CMAKE_Java_COMPILER javac)
find_host_program (CMAKE_Java_GENERATOR javah)
find_host_program (CMAKE_Java_ARCHIVE jar)
if (NOT EXISTS ${CMAKE_Java_COMPILER})
	message (FATAL_ERROR "Java compiler 'javac' could not be found. Ensure it is installed and in your PATH.")
endif (NOT EXISTS ${CMAKE_Java_COMPILER})
if (NOT EXISTS ${CMAKE_Java_GENERATOR})
	message (FATAL_ERROR "Java generator 'javah' could not be found. Ensure it is installed and in your PATH.")
endif (NOT EXISTS ${CMAKE_Java_GENERATOR})
if (NOT EXISTS ${CMAKE_Java_ARCHIVE})
	message (FATAL_ERROR "Java archiver 'jar' could not be found. Ensure it is installed and in your PATH.")
endif (NOT EXISTS ${CMAKE_Java_ARCHIVE})
enable_language (Java OPTIONAL)
include_directories (${ANDROID_JAR})


# Cmake wants to use always use posix path separators due to the android ndk toolchain; force the proper ones here
if (${NDKSYSTEM} STREQUAL "windows")
	set (CMAKE_INCLUDE_FLAG_SEP_Java ";")
else (${NDKSYSTEM} STREQUAL "windows")
	set (CMAKE_INCLUDE_FLAG_SEP_Java ":")
endif (${NDKSYSTEM} STREQUAL "windows")


# Override the standard cmake java compile and link settings
set (CMAKE_Java_COMPILE_OBJECT
	"<CMAKE_Java_COMPILER> -Xlint:unchecked <FLAGS> <SOURCE>"
)
set (CMAKE_Java_CREATE_STATIC_LIBRARY
	"<CMAKE_Java_ARCHIVE> -cf <TARGET> -C <OBJECT_DIR>/${JAVA_OBJ} ."
)


# Set the jar postfix used by the javah generator
string (TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
if (${BUILD_TYPE} STREQUAL "DEBUG")
	set (JARLOC LOCATION_DEBUG)
else (${BUILD_TYPE} STREQUAL "DEBUG")
	set (JARLOC LOCATION)
endif (${BUILD_TYPE} STREQUAL "DEBUG")


# Include the current directory for java class files
include_directories (.)


# toadlet_flick
set (FLICK_OBJ_DIR ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/jtoadlet_flick.dir/${JAVA_OBJ})
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${FLICK_OBJ_DIR})
include_directories (${FLICK_OBJ_DIR})
configure_file (${JAVA_SRC_DIR}/flick/Version.java.cmake ${CMAKE_CURRENT_BINARY_DIR}/${JAVA_SRC_DIR}/flick/Version.java)

set (FLICK_SRC
	${JAVA_SRC_DIR}/flick/InputDevice.java
	${JAVA_SRC_DIR}/flick/InputDeviceListener.java
	${JAVA_SRC_DIR}/flick/NInputDeviceListener.java
	${JAVA_SRC_DIR}/flick/AndroidSensorDevice.java
	${CMAKE_CURRENT_BINARY_DIR}/${JAVA_SRC_DIR}/flick/Version.java
)

set (FLICK_H
	us.toadlet.flick.NInputDeviceListener
)

add_library (jtoadlet_flick STATIC ${FLICK_SRC})
set_target_properties (jtoadlet_flick PROPERTIES COMPILE_FLAGS "-d \"${FLICK_OBJ_DIR}\"")
set_target_properties (jtoadlet_flick PROPERTIES COMPILE_DEFINITIONS_DEBUG TOADLET_DEBUG DEBUG_POSTFIX _d)
install (TARGETS jtoadlet_flick DESTINATION lib)

# Generate headers
get_target_property (JARFILE jtoadlet_flick ${JARLOC})
add_custom_command (TARGET jtoadlet_flick POST_BUILD
	COMMAND ${CMAKE_Java_GENERATOR} -classpath ${JARFILE} ${FLICK_H}
)


# toadlet_peeper
set (PEEPER_OBJ_DIR ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/jtoadlet_peeper.dir/${JAVA_OBJ})
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${PEEPER_OBJ_DIR})
include_directories (${PEEPER_OBJ_DIR})
configure_file (${JAVA_SRC_DIR}/peeper/Version.java.cmake ${CMAKE_CURRENT_BINARY_DIR}/${JAVA_SRC_DIR}/peeper/Version.java)

set (PEEPER_SRC
	${JAVA_SRC_DIR}/peeper/TextureFormat.java
	${JAVA_SRC_DIR}/peeper/WindowRenderTargetFormat.java
	${JAVA_SRC_DIR}/peeper/RenderTarget.java
	${JAVA_SRC_DIR}/peeper/GLRenderTarget.java
	${JAVA_SRC_DIR}/peeper/EGLRenderTarget.java
	${JAVA_SRC_DIR}/peeper/EGLWindowRenderTarget.java
	${CMAKE_CURRENT_BINARY_DIR}/${JAVA_SRC_DIR}/peeper/Version.java
)

add_library (jtoadlet_peeper STATIC ${PEEPER_SRC})
set_target_properties (jtoadlet_peeper PROPERTIES COMPILE_FLAGS "-d \"${PEEPER_OBJ_DIR}\"")
set_target_properties (jtoadlet_peeper PROPERTIES COMPILE_DEFINITIONS_DEBUG TOADLET_DEBUG DEBUG_POSTFIX _d)
install (TARGETS jtoadlet_peeper DESTINATION lib)


# toadlet_ribbit
set (RIBBIT_OBJ_DIR ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/jtoadlet_ribbit.dir/${JAVA_OBJ})
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${RIBBIT_OBJ_DIR})
include_directories (${RIBBIT_OBJ_DIR})
configure_file (${JAVA_SRC_DIR}/ribbit/Version.java.cmake ${CMAKE_CURRENT_BINARY_DIR}/${JAVA_SRC_DIR}/ribbit/Version.java)

set (RIBBIT_SRC
	${JAVA_SRC_DIR}/ribbit/AudioFormat.java
	${JAVA_SRC_DIR}/ribbit/AudioStream.java
	${JAVA_SRC_DIR}/ribbit/AudioBuffer.java
	${JAVA_SRC_DIR}/ribbit/Audio.java
	${JAVA_SRC_DIR}/ribbit/AudioCaps.java
	${JAVA_SRC_DIR}/ribbit/AudioDevice.java
	${JAVA_SRC_DIR}/ribbit/ATAudioBuffer.java
	${JAVA_SRC_DIR}/ribbit/ATAudio.java
	${JAVA_SRC_DIR}/ribbit/ATAudioDevice.java
	${JAVA_SRC_DIR}/ribbit/NAudioStream.java
	${CMAKE_CURRENT_BINARY_DIR}/${JAVA_SRC_DIR}/ribbit/Version.java
)

set (RIBBIT_H
	us.toadlet.ribbit.NAudioStream
)

add_library (jtoadlet_ribbit STATIC ${RIBBIT_SRC})
set_target_properties (jtoadlet_ribbit PROPERTIES COMPILE_FLAGS "-d \"${RIBBIT_OBJ_DIR}\"")
set_target_properties (jtoadlet_ribbit PROPERTIES COMPILE_DEFINITIONS_DEBUG TOADLET_DEBUG DEBUG_POSTFIX _d)
install (TARGETS jtoadlet_ribbit DESTINATION lib)

# Generate headers
get_target_property (JARFILE jtoadlet_ribbit ${JARLOC})
add_custom_command (TARGET jtoadlet_ribbit POST_BUILD
	COMMAND ${CMAKE_Java_GENERATOR} -classpath ${JARFILE} ${RIBBIT_H}
)


# toadlet_pad
set (PAD_OBJ_DIR ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/jtoadlet_pad.dir/${JAVA_OBJ})
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${PAD_OBJ_DIR})
include_directories (${PAD_OBJ_DIR})
configure_file (${JAVA_SRC_DIR}/pad/Version.java.cmake ${CMAKE_CURRENT_BINARY_DIR}/${JAVA_SRC_DIR}/pad/Version.java)

set (PAD_SRC
	${JAVA_SRC_DIR}/pad/Applet.java
	${JAVA_SRC_DIR}/pad/AndroidApplication.java
	${JAVA_SRC_DIR}/pad/Engine.java
	${JAVA_SRC_DIR}/pad/RenderDevice.java
	${JAVA_SRC_DIR}/pad/NApplet.java
	${CMAKE_CURRENT_BINARY_DIR}/${JAVA_SRC_DIR}/pad/Version.java
)

set (PAD_H
	us.toadlet.pad.AndroidApplication
	us.toadlet.pad.Engine
	us.toadlet.pad.RenderDevice
	us.toadlet.pad.NApplet
)

add_library (jtoadlet_pad STATIC ${PAD_SRC})
set_target_properties (jtoadlet_pad PROPERTIES COMPILE_FLAGS "-d \"${PAD_OBJ_DIR}\"")
set_target_properties (jtoadlet_pad PROPERTIES COMPILE_DEFINITIONS_DEBUG TOADLET_DEBUG DEBUG_POSTFIX _d)
add_dependencies (jtoadlet_pad jtoadlet_peeper jtoadlet_ribbit jtoadlet_flick)
install (TARGETS jtoadlet_pad DESTINATION lib)

# Generate headers
get_target_property (JARFILE jtoadlet_pad ${JARLOC})
add_custom_command (TARGET jtoadlet_pad POST_BUILD
	COMMAND ${CMAKE_Java_GENERATOR} -classpath ${JARFILE} ${PAD_H}
)
