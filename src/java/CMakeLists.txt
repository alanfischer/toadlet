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
include_directories (${PEEPER_OBJ_DIR})
configure_file (${JAVA_SRC_DIR}/peeper/Version.java.cmake ${PEEPER_OBJ_DIR}/${JAVA_SRC_DIR}/peeper/Version.java)

set (PEEPER_SRC
	${JAVA_SRC_DIR}/peeper/TextureFormat.java
	${JAVA_SRC_DIR}/peeper/WindowRenderTargetFormat.java
	${JAVA_SRC_DIR}/peeper/RenderTarget.java
	${JAVA_SRC_DIR}/peeper/GLRenderTarget.java
	${JAVA_SRC_DIR}/peeper/EGLRenderTarget.java
	${JAVA_SRC_DIR}/peeper/EGLWindowRenderTarget.java
	${PEEPER_OBJ_DIR}/${JAVA_SRC_DIR}/peeper/Version.java
)

add_library (jtoadlet_peeper STATIC ${PEEPER_SRC})
set_target_properties (jtoadlet_peeper PROPERTIES COMPILE_FLAGS "-d \"${PEEPER_OBJ_DIR}\"")
set_target_properties (jtoadlet_peeper PROPERTIES COMPILE_DEFINITIONS_DEBUG TOADLET_DEBUG DEBUG_POSTFIX d)
install (TARGETS jtoadlet_peeper DESTINATION lib)


# toadlet_pad
set (PAD_OBJ_DIR ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/jtoadlet_pad.dir/${JAVA_OBJ})
include_directories (${PAD_OBJ_DIR})
configure_file (${JAVA_SRC_DIR}/pad/Version.java.cmake ${PAD_OBJ_DIR}/${JAVA_SRC_DIR}/pad/Version.java)

set (PAD_SRC
	${JAVA_SRC_DIR}/pad/RenderDevice.java
	${JAVA_SRC_DIR}/pad/Engine.java
	${JAVA_SRC_DIR}/pad/ApplicationActivity.java
	${JAVA_SRC_DIR}/pad/Application.java
	${JAVA_SRC_DIR}/pad/NApplicationActivity.java
	${PAD_OBJ_DIR}/${JAVA_SRC_DIR}/pad/Version.java
)

add_library (jtoadlet_pad STATIC ${PAD_SRC})
set_target_properties (jtoadlet_pad PROPERTIES COMPILE_FLAGS "-d \"${PAD_OBJ_DIR}\"")
set_target_properties (jtoadlet_pad PROPERTIES COMPILE_DEFINITIONS_DEBUG TOADLET_DEBUG DEBUG_POSTFIX d)
add_dependencies (jtoadlet_pad jtoadlet_peeper)
install (TARGETS jtoadlet_pad DESTINATION lib)

