# On Android NDK API levels < 9, toadlet uses java shim libraries for peeper, ribbit, and pad.


# Shortcut variables
set (JAVA_SRC_DIR us/toadlet)
set (JAVA_OBJ obj)


# Setup for Android Java Compilation
set (CMAKE_Java_COMPILER javac)
set (CMAKE_Java_ARCHIVE jar)
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
	${JAVA_SRC_DIR}/ribbit/ATAudioRegister.java
	${JAVA_SRC_DIR}/ribbit/ATAudio.java
	${JAVA_SRC_DIR}/ribbit/ATAudioDevice.java
	${JAVA_SRC_DIR}/ribbit/NAudioStream.java
	${CMAKE_CURRENT_BINARY_DIR}/${JAVA_SRC_DIR}/ribbit/Version.java
)

add_library (jtoadlet_ribbit STATIC ${RIBBIT_SRC})
set_target_properties (jtoadlet_ribbit PROPERTIES COMPILE_FLAGS "-d \"${RIBBIT_OBJ_DIR}\"")
set_target_properties (jtoadlet_ribbit PROPERTIES COMPILE_DEFINITIONS_DEBUG TOADLET_DEBUG DEBUG_POSTFIX _d)
install (TARGETS jtoadlet_ribbit DESTINATION lib)


# toadlet_pad
set (PAD_OBJ_DIR ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/jtoadlet_pad.dir/${JAVA_OBJ})
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${PAD_OBJ_DIR})
include_directories (${PAD_OBJ_DIR})
configure_file (${JAVA_SRC_DIR}/pad/Version.java.cmake ${CMAKE_CURRENT_BINARY_DIR}/${JAVA_SRC_DIR}/pad/Version.java)

set (PAD_SRC
	${JAVA_SRC_DIR}/pad/RenderDevice.java
	${JAVA_SRC_DIR}/pad/Engine.java
	${JAVA_SRC_DIR}/pad/Applet.java
	${JAVA_SRC_DIR}/pad/AndroidApplication.java
	${JAVA_SRC_DIR}/pad/NApplet.java
	${CMAKE_CURRENT_BINARY_DIR}/${JAVA_SRC_DIR}/pad/Version.java
)

add_library (jtoadlet_pad STATIC ${PAD_SRC})
set_target_properties (jtoadlet_pad PROPERTIES COMPILE_FLAGS "-d \"${PAD_OBJ_DIR}\"")
set_target_properties (jtoadlet_pad PROPERTIES COMPILE_DEFINITIONS_DEBUG TOADLET_DEBUG DEBUG_POSTFIX _d)
add_dependencies (jtoadlet_pad jtoadlet_peeper jtoadlet_ribbit)
install (TARGETS jtoadlet_pad DESTINATION lib)

