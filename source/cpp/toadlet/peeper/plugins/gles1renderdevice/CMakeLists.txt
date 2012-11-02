# Make all platform specific decisions for gles1renderdevice
include (../glrenderdevice/CommonGLRenderSources.cmake)

# Source files
set (PEEPER_GLES1RENDERDEVICE_SRC
	${PEEPER_GLRENDERDEVICE_SRC}
	../glrenderdevice/GLBuffer.cpp
	../glrenderdevice/GLRenderDevice.cpp
	../glrenderdevice/GLRenderState.cpp
	../glrenderdevice/GLTexture.cpp
	../glrenderdevice/GLTextureMipPixelBuffer.cpp
	../glrenderdevice/GLVertexFormat.cpp
)


# Headers
set (PEEPER_GLES1RENDERDEVICE_HDR
	${PEEPER_GLRENDERDEVICE_HDR}
	../glrenderdevice/GLBuffer.h
	../glrenderdevice/GLIncludes.h
	../glrenderdevice/GLPixelBuffer.h
	../glrenderdevice/GLRenderDevice.h
	../glrenderdevice/GLRenderState.h
	../glrenderdevice/GLTexture.h
	../glrenderdevice/GLTextureMipPixelBuffer.h
	../glrenderdevice/GLVertexFormat.h
)


# Definitions
set (PEEPER_GLES1RENDERDEVICE_DEFS "TOADLET_HAS_GLES" "TOADLET_HAS_GL_11") 
if (EGL)
	set (PEEPER_GLES1RENDERDEVICE_DEFS ${PEEPER_GLES1RENDERDEVICE_DEFS} "TOADLET_HAS_EGL" "TOADLET_HAS_EGL_11") 
endif (EGL)


# Dependencies
if (TOADLET_PLATFORM_ANDROID)
	set (PEEPER_GLES1RENDERDEVICE_LIBS ${GLESV1_LIBS})
elseif (TOADLET_PLATFORM_IOS)
	set (PEEPER_GLES1RENDERDEVICE_H ${PEEPER_GLES1RENDERDEVICE_H}
		../glrenderdevice/GLFBORenderTarget.h
		../glrenderdevice/GLFBOPixelBuffer.h
	)
	set (PEEPER_GLES1RENDERDEVICE_SRC ${PEEPER_GLES1RENDERDEVICE_SRC}
		../glrenderdevice/GLFBORenderTarget.cpp
		../glrenderdevice/GLFBOPixelBuffer.cpp
	)
	find_library (UIKIT_LIBS UIKit)
	set (PEEPER_GLES1RENDERDEVICE_LIBS objc ${UIKIT_LIBS})
endif (TOADLET_PLATFORM_ANDROID)


# Dynamic library
if (TOADLET_BUILD_DYNAMIC)
	add_library (toadlet_peeper_gles1renderdevice SHARED ${PEEPER_GLES1RENDERDEVICE_SRC} ${PEEPER_GLES1RENDERDEVICE_HDR})
	set_target_properties (toadlet_peeper_gles1renderdevice PROPERTIES ${DYNAMIC_LIB_PROPS})
	set_property (TARGET toadlet_peeper_gles1renderdevice APPEND PROPERTY COMPILE_DEFINITIONS ${PEEPER_GLES1RENDERDEVICE_DEFS})
	target_link_libraries (toadlet_peeper_gles1renderdevice toadlet_peeper ${PEEPER_GLES1RENDERDEVICE_LIBS})
	install (TARGETS toadlet_peeper_gles1renderdevice DESTINATION ${LIB_INSTALL_DIR} COMPONENT peeper)
	set (PEEPER_INSTALL ${PEEPER_INSTALL} toadlet_peeper_gles1renderdevice PARENT_SCOPE)
endif (TOADLET_BUILD_DYNAMIC)


# Static library
if (TOADLET_BUILD_STATIC)
	add_library (toadlet_peeper_gles1renderdevice_s STATIC ${PEEPER_GLES1RENDERDEVICE_SRC} ${PEEPER_GLES1RENDERDEVICE_HDR})
	set_target_properties (toadlet_peeper_gles1renderdevice_s PROPERTIES ${STATIC_LIB_PROPS})
	set_property (TARGET toadlet_peeper_gles1renderdevice_s APPEND PROPERTY COMPILE_DEFINITIONS ${PEEPER_GLES1RENDERDEVICE_DEFS})
	target_link_libraries (toadlet_peeper_gles1renderdevice_s toadlet_peeper_s ${PEEPER_GLES1RENDERDEVICE_LIBS})
	MERGE_STATIC_LIBRARIES (toadlet_peeper_gles1renderdevice_s "${PEEPER_GLES1RENDERDEVICE_LIBS_S}")
	install (TARGETS toadlet_peeper_gles1renderdevice_s DESTINATION ${LIB_INSTALL_DIR} COMPONENT peeper)
	set (PEEPER_INSTALL ${PEEPER_INSTALL} toadlet_peeper_gles1renderdevice_s PARENT_SCOPE)

	# HACK: install the library on iOS. This will disappear once cmake issue:0012506 is resolved. 
	if (TOADLET_PLATFORM_IOS)
	    install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/\$ENV{CONFIGURATION}\$ENV{EFFECTIVE_PLATFORM_NAME}/ DESTINATION ${LIB_INSTALL_DIR} COMPONENT peeper FILES_MATCHING PATTERN "*.a")
	endif (TOADLET_PLATFORM_IOS)
endif (TOADLET_BUILD_STATIC)

