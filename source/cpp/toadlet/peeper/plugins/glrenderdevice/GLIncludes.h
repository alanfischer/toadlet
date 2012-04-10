/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright 2009, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer, Andrew Fischer
 *
 * This file is part of The Toadlet Engine.
 *
 * The Toadlet Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * The Toadlet Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with The Toadlet Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 ********** Copyright header - do not remove **********/

#ifndef TOADLET_PEEPER_GLINCLUDES_H
#define TOADLET_PEEPER_GLINCLUDES_H

#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/MathConversion.h>
#include <toadlet/peeper/Types.h>

#if defined(TOADLET_PLATFORM_ANDROID)
	#if ANDROID_NDK_API_LEVEL>=9
		#include <EGL/egl.h>
		#define TOADLET_HAS_GLPBUFFERS
		#define TOADLET_HAS_EGL
		#define TOADLET_HAS_EGL_11
	#else
		// PBuffers would be possible to implement, but it wouldn't be that clean since they tie into Textures and PixelBuffers.  Since NDK 8 is a minimum target, we will not support render to texture there
	#endif

	#if defined(TOADLET_HAS_GL_11)
		#include <GLES/gl.h>
	#elif defined(TOADLET_HAS_GL_20)
		#include <GLES2/gl2.h>
	#endif
	typedef char GLchar;
#elif defined(TOADLET_PLATFORM_IOS)
	#if defined(TOADLET_HAS_GL_11)
		#include <OpenGLES/ES1/gl.h>
		#include <OpenGLES/ES1/glext.h>
		#define TOADLET_HAS_EAGL
		#define TOADLET_HAS_EGL
		#define TOADLET_HAS_EGL_11
	#elif defined(TOADLET_HAS_GL_20)
		#include <OpenGLES/ES2/gl.h>
		#include <OpenGLES/ES2/glext.h>
		#define TOADLET_HAS_EAGL
		#define TOADLET_HAS_EGL
		#define TOADLET_HAS_EGL_20
	#endif
#else
	#if defined(TOADLET_PLATFORM_WIN32)
		#define GLEW_STATIC
	#endif
	extern "C" {
		#include <GL/glew.h>
		#if defined(TOADLET_PLATFORM_WIN32)
			#define TOADLET_HAS_WGL
			#include <GL/wglew.h>
			#include <GL/gl.h>
		#elif defined(TOADLET_PLATFORM_OSX)
			#define TOADLET_HAS_NSGL
			#include <OpenGL/OpenGL.h>
			#include <AGL/agl.h>
		#elif defined(TOADLET_PLATFORM_POSIX)
			#define TOADLET_HAS_GLX
			#include <GL/glxew.h>
			#include <GL/gl.h>
		#else
			#error "Unknown platform"
		#endif
	}
	#define TOADLET_HAS_GLEW
	#if defined(GL_VERSION_2_1)
		#define TOADLET_HAS_GL_21
	#endif
	#if defined(GL_VERSION_2_0)
		#define TOADLET_HAS_GL_20
	#endif
	#if defined(GL_VERSION_1_5)
		#define TOADLET_HAS_GL_15
	#endif
	#if defined(GL_VERSION_1_4)
		#define TOADLET_HAS_GL_14
	#endif
	#if defined(GL_VERSION_1_3)
		#define TOADLET_HAS_GL_13
	#endif
	#if defined(GL_VERSION_1_2)
		#define TOADLET_HAS_GL_12
	#endif
	#if defined(GL_VERSION_1_1)
		#define TOADLET_HAS_GL_11
	#endif
#endif

#if defined(TOADLET_HAS_GLES) && defined(TOADLET_HAS_GL_11)
	#define glTexEnvi glTexEnvx
	#define glTexParameteri glTexParameterx
#endif

#if !defined(GL_SRC0_RGB)
	#define GL_SRC0_RGB GL_SOURCE0_RGB 
#endif

#if !defined(GL_SRC1_RGB)
	#define GL_SRC1_RGB GL_SOURCE1_RGB 
#endif

#if !defined(GL_SRC2_RGB)
	#define GL_SRC2_RGB GL_SOURCE2_RGB 
#endif

#if defined(TOADLET_HAS_GLES) && defined(TOADLET_HAS_GL_11)
	#define GL_RENDERBUFFER GL_RENDERBUFFER_OES
	#define GL_COLOR_ATTACHMENT0 GL_COLOR_ATTACHMENT0_OES
	#define GL_DEPTH_ATTACHMENT GL_DEPTH_ATTACHMENT_OES
	#define GL_DEPTH_COMPONENT16 GL_DEPTH_COMPONENT16_OES
	#define GL_DEPTH_COMPONENT24 GL_DEPTH_COMPONENT24_OES
	#define GL_FRAMEBUFFER GL_FRAMEBUFFER_OES
	#define glGenRenderbuffers glGenRenderbuffersOES
	#define glBindRenderbuffer glBindRenderbufferOES
	#define glDeleteRenderbuffers glDeleteRenderbuffersOES
	#define glRenderbufferStorage glRenderbufferStorageOES
	#define glGenFramebuffers glGenFramebuffersOES
	#define glDeleteFramebuffers glDeleteFramebuffersOES
	#define glBindFramebuffer glBindFramebufferOES
	#define glFramebufferTexture2D glFramebufferTexture2DOES
	#define glFramebufferRenderbuffer glFramebufferRenderbufferOES
	#define glCheckFramebufferStatus glCheckFramebufferStatusOES
	#define glGenerateMipmap glGenerateMipmapOES
	#define GL_FRAMEBUFFER_COMPLETE GL_FRAMEBUFFER_COMPLETE_OES
	#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_OES
	#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_OES
	#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_OES
	#define GL_FRAMEBUFFER_INCOMPLETE_FORMATS GL_FRAMEBUFFER_INCOMPLETE_FORMATS_OES
	#define GL_POINT_SPRITE GL_POINT_SPRITE_OES
	#define GL_COORD_REPLACE GL_COORD_REPLACE_OES
#elif !defined(TOADLET_HAS_GLES)
	#ifndef GL_RENDERBUFFER
		#define GL_RENDERBUFFER GL_RENDERBUFFER_EXT
	#endif
	#ifndef GL_COLOR_ATTACHMENT0
		#define GL_COLOR_ATTACHMENT0 GL_COLOR_ATTACHMENT0_EXT
	#endif
	#ifndef GL_COLOR_ATTACHMENT1
		#define GL_COLOR_ATTACHMENT1 GL_COLOR_ATTACHMENT1_EXT
	#endif
	#ifndef GL_COLOR_ATTACHMENT2
		#define GL_COLOR_ATTACHMENT2 GL_COLOR_ATTACHMENT2_EXT
	#endif
	#ifndef GL_COLOR_ATTACHMENT3
		#define GL_COLOR_ATTACHMENT3 GL_COLOR_ATTACHMENT3_EXT
	#endif
	#ifndef GL_DEPTH_ATTACHMENT
		#define GL_DEPTH_ATTACHMENT GL_DEPTH_ATTACHMENT_EXT
	#endif
	#ifndef GL_FRAMEBUFFER
		#define GL_FRAMEBUFFER GL_FRAMEBUFFER_EXT
	#endif
	#ifndef glGenRenderbuffers
		#define glGenRenderbuffers glGenRenderbuffersEXT
	#endif
	#ifndef glBindRenderbuffer
		#define glBindRenderbuffer glBindRenderbufferEXT
	#endif
	#ifndef glDeleteRenderbuffers
		#define glDeleteRenderbuffers glDeleteRenderbuffersEXT
	#endif
	#ifndef glRenderbufferStorage
		#define glRenderbufferStorage glRenderbufferStorageEXT
	#endif
	#ifndef glGenFramebuffers
		#define glGenFramebuffers glGenFramebuffersEXT
	#endif
	#ifndef glDeleteFramebuffers
		#define glDeleteFramebuffers glDeleteFramebuffersEXT
	#endif
	#ifndef glBindFramebuffer
		#define glBindFramebuffer glBindFramebufferEXT
	#endif
	#ifndef glFramebufferTexture2D
		#define glFramebufferTexture2D glFramebufferTexture2DEXT
	#endif
	#ifndef glFramebufferRenderbuffer	
		#define glFramebufferRenderbuffer glFramebufferRenderbufferEXT
	#endif
	#ifndef glCheckFramebufferStatus
		#define glCheckFramebufferStatus glCheckFramebufferStatusEXT
	#endif
	#ifndef glGenerateMipmap
		#define glGenerateMipmap glGenerateMipmapEXT
	#endif
	#ifndef glPointParameterfv
		#define glPointParameterfv glPointParameterfvARB
	#endif
	#ifndef GL_FRAMEBUFFER_COMPLETE
		#define GL_FRAMEBUFFER_COMPLETE GL_FRAMEBUFFER_COMPLETE_EXT
	#endif
	#ifndef GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT
		#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT
	#endif
	#ifndef GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT
		#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT
	#endif
	#ifndef GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS
		#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT
	#endif
	#ifndef GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER
		#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT
	#endif
	#ifndef GL_FRAMEBUFFER_INCOMPLETE_FORMATS
		#define GL_FRAMEBUFFER_INCOMPLETE_FORMATS GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT
	#endif
	#ifndef GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER
		#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT
	#endif
#endif

#if defined(TOADLET_PLATFORM_WIN32) || defined(TOADLET_PLATFORM_WINCE) || \
	(defined(TOADLET_PLATFORM_POSIX) && !defined(TOADLET_PLATFORM_ANDROID) && !defined(TOADLET_PLATFORM_OSX) && !defined(TOADLET_PLATFORM_IOS))
	#define TOADLET_HAS_GLPBUFFERS
#endif
#if defined(TOADLET_HAS_GLEW) || defined(TOADLET_HAS_EAGL) || (defined(TOADLET_HAS_GLES) && defined(TOADLET_HAS_GL_20))
	#define TOADLET_HAS_GLFBOS
#endif
#if defined(TOADLET_HAS_GLEW)
	#define TOADLET_HAS_GLQUERY
	#define TOADLET_HAS_GLSL
#endif

#if defined(GL_ARB_vertex_buffer_object)
	#define TOADLET_HAS_GLVBOS
	#define TOADLET_HAS_GLIBOS
#endif
#if defined(GL_ARB_pixel_buffer_object)
	#define TOADLET_HAS_GLPBOS
#endif
#if defined(GL_ARB_uniform_buffer_object)
	#define TOADLET_HAS_GLUBOS
#endif
#if defined(TOADLET_HAS_GLES)
	#define TOADLET_HAS_GLVBOS
	#define TOADLET_HAS_GLIBOS
#endif

#if defined(TOADLET_HAS_GL_11)
	#define TOADLET_HAS_GLFIXED
#endif
#if defined(TOADLET_HAS_GL_20)
	#define TOADLET_HAS_GLSHADERS
	#define TOADLET_HAS_GLSL
#endif

#if defined(TOADLET_HAS_GLES)
	#if defined(TOADLET_HAS_GL_20)
		#define GLBuffer			GLES2Buffer
		#define GLFBOPixelBuffer	GLES2FBOPixelBuffer
		#define GLFBORenderTarget	GLES2FBORenderTarget
		#define GLPixelBuffer		GLES2PixelBuffer
		#define GLRenderDevice		GLES2RenderDevice
		#define GLRenderState		GLES2RenderState
		#define GLRenderTarget		GLES2RenderTarget
		#define GLSLShader			GLES2SLShader
		#define GLSLShaderState		GLES2SLShaderState
		#define GLSLVertexLayout	GLES2SLVertexLayout
		#define GLTexture			GLES2Texture
		#define GLTextureMipPixelBuffer	GLES2TextureMipPixelBuffer
		#define GLVertexFormat		GLES2VertexFormat

		#define EGLWindowRenderTarget EGL2WindowRenderTarget
		#define EGLRenderTarget		EGL2RenderTarget
		#define EGLPBufferRenderTarget EGL2PBufferRenderTarget

		#define EAGLWindowRenderTarget EAGL2WindowRenderTarget
		#define EAGLRenderTarget	EAGL2RenderTarget
	#else
		#define GLBuffer			GLES1Buffer
		#define GLFBOPixelBuffer	GLES1FBOPixelBuffer
		#define GLFBORenderTarget	GLES1FBORenderTarget
		#define GLPixelBuffer		GLES1PixelBuffer
		#define GLRenderDevice		GLES1RenderDevice
		#define GLRenderState		GLES1RenderState
		#define GLRenderTarget		GLES1RenderTarget
		#define GLTexture			GLES1Texture
		#define GLTextureMipPixelBuffer	GLES1TextureMipPixelBuffer
		#define GLVertexFormat		GLES1VertexFormat
	#endif
#endif

#if defined(TOADLET_DEBUG)
	#define TOADLET_CHECK_GLERROR(function) \
		{ int error=glGetError(); \
		if(error!=GL_NO_ERROR) \
			toadlet::egg::Error::unknown(toadlet::egg::Categories::TOADLET_PEEPER, \
			toadlet::egg::String("GL Error in ") + function + ": error=" + error); }
#else
	#define TOADLET_CHECK_GLERROR(function)
#endif

#if defined(TOADLET_HAS_EGL) && defined(TOADLET_DEBUG)
	#define TOADLET_CHECK_EGLERROR(function) \
		{ int error=eglGetError(); \
		if(error!=EGL_SUCCESS) \
			toadlet::egg::Error::unknown(toadlet::egg::Categories::TOADLET_PEEPER, \
			toadlet::egg::String("EGL Error in ") + function + ": error=" + error); }
#else
	#define TOADLET_CHECK_EGLERROR(function)
#endif

#if defined(TOADLET_HAS_WGL) && defined(TOADLET_DEBUG)
	#define TOADLET_CHECK_WGLERROR(function) \
		{ int error=GetLastError(); \
		if(error!=0) \
			toadlet::egg::Error::unknown(toadlet::egg::Categories::TOADLET_PEEPER, \
			toadlet::egg::String("WGL Error in ") + function + ": error=" + error); }
#else
	#define TOADLET_CHECK_WGLERROR(function)
#endif

#endif

