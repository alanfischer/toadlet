# Setup Java Compilation
set (CMAKE_Java_COMPILER javac)
enable_language (Java OPTIONAL)

# Java is fussy about pathing, so do all cmake work here instead of in subdirectories.
set (JAVA_SRC_DIR us/toadlet)

# This causes cmake to bring in the android.jar file 
include_directories(${ANDROID_JAR})


# toadlet_peeper
set (PEEPER_SRC
	${JAVA_SRC_DIR}/peeper/EGLRenderTarget.java
	${JAVA_SRC_DIR}/peeper/EGLWindowRenderTarget.java
	${JAVA_SRC_DIR}/peeper/GLRenderTarget.java
	${JAVA_SRC_DIR}/peeper/RenderTarget.java
)

add_library (jtoadlet_peeper STATIC ${PEEPER_SRC})
set_target_properties (jtoadlet_peeper PROPERTIES COMPILE_DEFINITIONS_DEBUG TOADLET_DEBUG DEBUG_POSTFIX d)
install (TARGETS jtoadlet_peeper DESTINATION lib)


# toadlet_pad
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

