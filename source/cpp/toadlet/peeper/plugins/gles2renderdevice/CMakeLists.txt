# Make all platform specific decisions for gles2renderdevice
include (../glrenderdevice/CMakeListsPlatform.txt)

# Source files
set (PEEPER_GLRENDERDEVICE_SRC
	${PEEPER_GLRENDERDEVICE_SRC}
	../glrenderdevice/GLBuffer.cpp
	../glrenderdevice/GLFBOPixelBuffer.cpp
	../glrenderdevice/GLFBORenderTarget.cpp
	../glrenderdevice/GLRenderDevice.cpp
	../glrenderdevice/GLRenderState.cpp
	../glrenderdevice/GLSLShader.cpp
	../glrenderdevice/GLSLShaderState.cpp
	../glrenderdevice/GLSLVertexLayout.cpp
	../glrenderdevice/GLTexture.cpp
	../glrenderdevice/GLTextureMipPixelBuffer.cpp
	../glrenderdevice/GLVertexFormat.cpp
)


# Headers
set (PEEPER_GLRENDERDEVICE_HEADERS
	${PEEPER_GLRENDERDEVICE_HEADERS}
	../glrenderdevice/GLBuffer.h
	../glrenderdevice/GLFBOPixelBuffer.h
	../glrenderdevice/GLFBORenderTarget.h
	../glrenderdevice/GLIncludes.h
	../glrenderdevice/GLPixelBuffer.h
	../glrenderdevice/GLRenderDevice.h
	../glrenderdevice/GLRenderState.h
	../glrenderdevice/GLSLShader.h
	../glrenderdevice/GLSLShaderState.h
	../glrenderdevice/GLSLVertexLayout.h
	../glrenderdevice/GLTexture.h
	../glrenderdevice/GLTextureMipPixelBuffer.h
	../glrenderdevice/GLVertexFormat.h
)


# Definitions
set (PEEPER_GLRENDERDEVICE_DEFS "TOADLET_HAS_GLES" "TOADLET_HAS_GL_20") 


# Dependencies
if (TOADLET_PLATFORM_ANDROID)
	if (ANDROID_NDK_API_LEVEL LESS 9)
		set (PEEPER_GLRENDERDEVICE_SRC ${PEEPER_GLRENDERDEVICE_SRC}
			../glrenderdevice/platform/jgl/us_toadlet_peeper_NGLES2RenderDevice.cpp
		)
	endif (ANDROID_NDK_API_LEVEL LESS 9)
	set (PEEPER_GLRENDERDEVICE_LIBS ${GLESV2_LIBS})
elseif (TOADLET_PLATFORM_IOS)
	find_library (UIKIT_LIBS UIKit)
	set (PEEPER_GLRENDERDEVICE_LIBS objc ${UIKIT_LIBS})
endif (TOADLET_PLATFORM_ANDROID)


# Dynamic library
if (NOT TOADLET_PLATFORM_IOS)
	add_library (toadlet_peeper_gles2renderdevice SHARED ${PEEPER_GLRENDERDEVICE_SRC} ${PEEPER_GLRENDERDEVICE_HEADERS})
	set_target_properties (toadlet_peeper_gles2renderdevice PROPERTIES ${DYNAMIC_LIB_PROPS})
	set_property (TARGET toadlet_peeper_gles2renderdevice APPEND PROPERTY COMPILE_DEFINITIONS ${PEEPER_GLRENDERDEVICE_DEFS})
	target_link_libraries (toadlet_peeper_gles2renderdevice toadlet_peeper ${PEEPER_GLRENDERDEVICE_LIBS})
	install (TARGETS toadlet_peeper_gles2renderdevice DESTINATION ${LIB_INSTALL_DIR})
endif (NOT TOADLET_PLATFORM_IOS)


# Static library
add_library (toadlet_peeper_gles2renderdevice_s STATIC ${PEEPER_GLRENDERDEVICE_SRC} ${PEEPER_GLRENDERDEVICE_HEADERS})
set_target_properties (toadlet_peeper_gles2renderdevice_s PROPERTIES ${STATIC_LIB_PROPS})
set_property (TARGET toadlet_peeper_gles2renderdevice_s APPEND PROPERTY COMPILE_DEFINITIONS ${PEEPER_GLRENDERDEVICE_DEFS})
target_link_libraries (toadlet_peeper_gles2renderdevice_s toadlet_peeper_s ${PEEPER_GLRENDERDEVICE_LIBS})
MERGE_STATIC_LIBRARIES (toadlet_peeper_gles2renderdevice_s "${PEEPER_GLRENDERDEVICE_LIBS_S}")
install (TARGETS toadlet_peeper_gles2renderdevice_s DESTINATION ${LIB_INSTALL_DIR})

# HACK: install the library on iOS. This will disappear once cmake issue:0012506 is resolved. 
if (TOADLET_PLATFORM_IOS)
    install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/\$ENV{CONFIGURATION}\$ENV{EFFECTIVE_PLATFORM_NAME}/ DESTINATION ${LIB_INSTALL_DIR} FILES_MATCHING PATTERN "*.a")
endif (TOADLET_PLATFORM_IOS)