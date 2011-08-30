# Setup Java Compilation
set (CMAKE_Java_COMPILER javac)
enable_language (Java OPTIONAL)

# Java is fussy about pathing, so do all cmake work here instead of in subdirectories.
set (JAVA_SRC_DIR us/toadlet)

#Special setup for java cmake compiling
set (CMAKE_Java_COMPILE_OBJECT
        "<CMAKE_Java_COMPILER> -Xlint:unchecked -classpath ${ANDROID_JAR} <SOURCE>"
)
set (CMAKE_Java_CREATE_STATIC_LIBRARY
        "<CMAKE_Java_ARCHIVE> -cf <TARGET> -C <OBJECT_DIR> ."
)


# Configure
# toadlet_peeper
configure_file (${JAVA_SRC_DIR}/peeper/Version.java.cmake ${CMAKE_CURRENT_BINARY_DIR}/${JAVA_SRC_DIR}/peeper/Version.java)

set (PEEPER_SRC
	${JAVA_SRC_DIR}/peeper/RenderTarget.java
	${JAVA_SRC_DIR}/peeper/GLRenderTarget.java
	${JAVA_SRC_DIR}/peeper/EGLRenderTarget.java
	${JAVA_SRC_DIR}/peeper/EGLWindowRenderTarget.java
)

add_library (jtoadlet_peeper STATIC ${PEEPER_SRC})
set_target_properties (jtoadlet_peeper PROPERTIES COMPILE_DEFINITIONS_DEBUG TOADLET_DEBUG DEBUG_POSTFIX d)
install (TARGETS jtoadlet_peeper DESTINATION lib)


# toadlet_pad
configure_file (${JAVA_SRC_DIR}/pad/Version.java.cmake ${CMAKE_CURRENT_BINARY_DIR}/${JAVA_SRC_DIR}/pad/Version.java)
set (PAD_SRC
	${JAVA_SRC_DIR}/pad/ApplicationActivity.java
	${JAVA_SRC_DIR}/pad/Application.java
	${JAVA_SRC_DIR}/pad/NApplicationActivity.java
	${JAVA_SRC_DIR}/pad/Version.java
)

add_library (jtoadlet_pad STATIC ${PAD_SRC})
set_target_properties (jtoadlet_pad PROPERTIES COMPILE_DEFINITIONS_DEBUG TOADLET_DEBUG DEBUG_POSTFIX d)
add_dependencies (jtoadlet_pad jtoadlet_peeper)
install (TARGETS jtoadlet_pad DESTINATION lib)

