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

#include "GLBuffer.h"
#include "GLVertexFormat.h"
#include "GLRenderer.h"
#include "GLRenderTarget.h"
#include "GLRenderStateSet.h"
#include "GLTexture.h"
#if defined(TOADLET_HAS_GLFBOS)
	#include "GLFBORenderTarget.h"
#endif
#if defined(TOADLET_HAS_GLQUERY)
	#include "GLQuery.h"
#endif
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/peeper/IndexData.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/peeper/Viewport.h>
#include <toadlet/peeper/Program.h>
#include <toadlet/peeper/Shader.h>

using namespace toadlet::egg;
using namespace toadlet::egg::image;

#if defined(TOADLET_PLATFORM_WIN32) && !defined(TOADLET_PLATFORM_WINCE)
	#pragma comment(lib,"opengl32.lib")
	#if defined(TOADLET_LIBGLEW_NAME)
		#pragma comment(lib,TOADLET_LIBGLEW_NAME)
	#endif
#endif

namespace toadlet{
namespace peeper{

#if defined(TOADLET_HAS_GLPBUFFERS)
	extern bool GLPBufferRenderTarget_available(GLRenderer *renderer);
	extern PixelBufferRenderTarget *new_GLPBufferRenderTarget(GLRenderer *renderer);
#endif

TOADLET_C_API Renderer* new_GLRenderer(){
	return new GLRenderer();
}

#if defined(TOADLET_BUILD_DYNAMIC)
	TOADLET_C_API Renderer* new_Renderer(){
		return new GLRenderer();
	}
#endif

GLRenderer::GLRenderer():
	mMatrixMode(-1),

	mPBuffersAvailable(false),
	mFBOsAvailable(false),

	mMaxTexCoordIndex(0),
	//mTexCoordIndexes,
	mLastMaxTexCoordIndex(0),
	//mLastTexTargets,
	//mLastVertexData,
	mLastTypeBits(0),
	//mLastTexCoordIndexes,

	//mModelMatrix,
	//mViewMatrix,

	//mStatisticsSet,
	//mCapabilitySet,
	mMultiTexture(false),

	mPrimaryRenderTarget(NULL),
	mGLPrimaryRenderTarget(NULL),
	mRenderTarget(NULL),
	mGLRenderTarget(NULL)

	#if defined(TOADLET_DEBUG)
		,mBeginEndCounter(0)
	#endif
{}

GLRenderer::~GLRenderer(){
	destroy();
}

bool GLRenderer::create(RenderTarget *target,int *options){
	Logger::alert(Categories::TOADLET_PEEPER,
		"creating "+Categories::TOADLET_PEEPER+".GLRenderer");

	if(target==NULL){
		Error::nullPointer(Categories::TOADLET_PEEPER,
			"NULL RenderTarget");
		return false;
	}

	bool usePBuffers=true;
	bool useFBOs=true;
	bool useHardwareBuffers=true;
	if(options!=NULL){
		int i=0;
		while(options[i]!=0){
			switch(options[i++]){
				case Option_USE_PBUFFERS:
					usePBuffers=options[i++]>0;
					Logger::alert(Categories::TOADLET_PEEPER,
						String("Setting PBuffer usage to:")+usePBuffers);
				break;
				case Option_USE_FBOS:
					useFBOs=options[i++]>0;
					Logger::alert(Categories::TOADLET_PEEPER,
						String("Setting FBO usage to:")+useFBOs);
				break;
				case Option_USE_HARDWARE_BUFFERS:
					useHardwareBuffers=options[i++]>0;
					Logger::alert(Categories::TOADLET_PEEPER,
						String("Setting Hardware Buffer usage to:")+useHardwareBuffers);
				break;
			}
		}
	}

	#if defined(TOADLET_HAS_GLESEM)
		if(glesem_getInitialized()==false){
			int glesemInitResult=glesem_init(GLESEM_ACCELERATED_IF_AVAILABLE);
			if(glesemInitResult<=0){
				Error::unknown(Categories::TOADLET_PEEPER,
					"glesem_init failed");
				return false;
			}
		}

		int glesemAcceleratedResult=glesem_getAccelerated();
		Logger::alert(Categories::TOADLET_PEEPER,
			String("glesem_getAccelerated result:")+glesemAcceleratedResult);

		gl_version=glesem_gl_majorVersion*10+glesem_gl_minorVersion;
	#elif defined(TOADLET_HAS_GLEW)
		glewExperimental=true;
		GLenum glewInitResult=glewInit();
	
		if(glewInitResult!=GLEW_OK){
			Error::unknown(Categories::TOADLET_PEEPER,
			   String("glewInit failed:")+glewGetErrorString(glewInitResult));
			return false;
		}

		gl_version=GLEW_VERSION_2_1?21:(GLEW_VERSION_2_0?20:(GLEW_VERSION_1_5?15:(GLEW_VERSION_1_4?14:(GLEW_VERSION_1_3?13:(GLEW_VERSION_1_2?12:(GLEW_VERSION_1_1?11:00))))));
	#elif defined(TOADLET_PLATFORM_IPHONE)
		gl_version=11;
	#else
		gl_version=10;
	#endif

	Logger::alert(Categories::TOADLET_PEEPER,
		String("CALCULATED GL VERSION:")+(gl_version/10)+"."+(gl_version%10));

	GLRenderTarget *gltarget=(GLRenderTarget*)target->getRootRenderTarget();

	mPrimaryRenderTarget=target;
	mGLPrimaryRenderTarget=gltarget;
	mRenderTarget=target;
	mGLRenderTarget=gltarget;

	Logger::alert(Categories::TOADLET_PEEPER,
		String("GL_VENDOR:") + glGetString(GL_VENDOR));
	Logger::alert(Categories::TOADLET_PEEPER,
		String("GL_RENDERER:") + glGetString(GL_RENDERER));
	Logger::alert(Categories::TOADLET_PEEPER,
		String("GL_VERSION:") + glGetString(GL_VERSION));
	Logger::alert(Categories::TOADLET_PEEPER,
		String("GL_EXTENSIONS:") + glGetString(GL_EXTENSIONS));

	CapabilityState &caps=mCapabilityState;

	#if defined(TOADLET_HAS_GLESEM)
		caps.hardwareTextures=glesemAcceleratedResult>0;
	#else
		caps.hardwareTextures=true;
	#endif

	GLint maxTextureStages=0;
	if(gl_version>10){ // 1.0 doesn't support multitexturing
		glGetIntegerv(GL_MAX_TEXTURE_UNITS,&maxTextureStages);
	}
	if(maxTextureStages<=0){
		maxTextureStages=1;
	}
	caps.maxTextureStages=maxTextureStages;
	mMultiTexture=maxTextureStages>1;

	mLastTextures.resize(caps.maxTextureStages,NULL);
	mLastTexTargets.resize(caps.maxTextureStages,0);
	mTexCoordIndexes.resize(caps.maxTextureStages,-1);
	mLastTexCoordIndexes.resize(caps.maxTextureStages,-1);

	GLint maxTextureSize=0;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE,&maxTextureSize);
	if(maxTextureSize<=0){
		maxTextureSize=256;
	}
	caps.maxTextureSize=maxTextureSize;

	GLint maxLights=0;
	glGetIntegerv(GL_MAX_LIGHTS,&maxLights);
	if(maxLights<=0){
		maxLights=8;
	}
	caps.maxLights=maxLights;

	#if defined(TOADLET_HAS_GLPBUFFERS)
		mPBuffersAvailable=usePBuffers && GLPBufferRenderTarget_available(this);
	#else
		mPBuffersAvailable=false;
	#endif
	#if defined(TOADLET_HAS_GLFBOS)
		mFBOsAvailable=useFBOs && GLFBORenderTarget::available(this);
	#else
		mFBOsAvailable=false;
	#endif

	caps.renderToTexture=mPBuffersAvailable || mFBOsAvailable;

	caps.renderToDepthTexture=mFBOsAvailable;

	#if defined(TOADLET_HAS_GLEW)
		caps.textureDot3=(GLEW_ARB_texture_env_dot3!=0);
	#elif defined(TOADLET_HAS_GLES)
		caps.textureDot3=(gl_version>=11);
	#endif

	#if defined(TOADLET_HAS_GLES)
		caps.textureAutogenMipMaps|=(gl_version>=11);
	#elif defined(TOADLET_HAS_GLEW) && defined(GL_EXT_framebuffer_object)
		caps.textureAutogenMipMaps|=(GLEW_EXT_framebuffer_object!=0);
	#else
		caps.textureAutogenMipMaps|=(gl_version>=14);
	#endif

	#if defined(TOADLET_HAS_GLEW)
		// Usefully, GL_TEXTURE_RECTANGLE_ARB == GL_TEXTURE_RECTANGLE_EXT == GL_TEXTURE_RECTANGLE_NV
		caps.textureNonPowerOf2Restricted=(GLEW_ARB_texture_rectangle!=0) || (GLEW_EXT_texture_rectangle!=0) || (GLEW_NV_texture_rectangle!=0);
		caps.renderToTextureNonPowerOf2Restricted=mFBOsAvailable && caps.textureNonPowerOf2Restricted;
		caps.textureNonPowerOf2=(GLEW_ARB_texture_non_power_of_two!=0);
	#endif

	#if defined(TOADLET_HAS_GLEW)
		caps.hardwareIndexBuffers=(caps.hardwareVertexBuffers=(useHardwareBuffers && GLEW_ARB_vertex_buffer_object));
	#elif defined(TOADLET_HAS_GLES)
		caps.hardwareIndexBuffers=(caps.hardwareVertexBuffers=(useHardwareBuffers && gl_version>=11));
	#endif

	#if defined(TOADLET_HAS_GLEW)
		caps.pointSprites=(GLEW_ARB_point_parameters!=0);
	#elif defined(TOADLET_HAS_GLES)
		caps.pointSprites=(gl_version>=11);
	#endif

	#if defined(TOADLET_HAS_GLEW)
		caps.vertexShaders=(GLEW_ARB_vertex_program>0);
		if(caps.vertexShaders) glGetProgramivARB(GL_VERTEX_PROGRAM_ARB,GL_MAX_PROGRAM_ENV_PARAMETERS_ARB,(GLint*)&caps.maxVertexShaderLocalParameters);
	#endif

	#if defined(TOADLET_HAS_GLEW)
		caps.fragmentShaders=(GLEW_ARB_fragment_program>0);
		if(caps.fragmentShaders) glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_MAX_PROGRAM_ENV_PARAMETERS_ARB,(GLint*)&caps.maxFragmentShaderLocalParameters);
	#endif

	#if defined(TOADLET_HAS_GLES) && defined(TOADLET_FIXED_POINT)
		caps.idealVertexFormatBit=VertexFormat::Format_BIT_FIXED_32;
	#else
		caps.idealVertexFormatBit=VertexFormat::Format_BIT_FLOAT_32;
	#endif

	// OSX needs a notification to update the back buffer on a resize
	#if defined(TOADLET_PLATFORM_OSX) && !defined(TOADLET_PLATFORM_EAGL)
		mCapabilitySet.resetOnResize=true;
	#endif

	caps.triangleFan=true;
	#if !defined(TOADLET_HAS_GLES)
		caps.fill=true;
		caps.cubeMap=true;
	#else
		mCapabilitySet.texturePerspective=true;
		#if TOADLET_HAS_GL_20
			caps.cubeMap=true;
		#else
			caps.cubeMap=false;
		#endif
	#endif

	setDefaultStates();

	TOADLET_CHECK_GLERROR("create");

	Logger::alert(Categories::TOADLET_PEEPER,
		"created GLRenderer");

	return true;
}

void GLRenderer::destroy(){
	if(mPrimaryRenderTarget!=NULL){
		mPrimaryRenderTarget=NULL;
		mGLPrimaryRenderTarget=NULL;
		mRenderTarget=NULL;
		mGLRenderTarget=NULL;

		TOADLET_CHECK_GLERROR("destroy");

		Logger::alert(Categories::TOADLET_PEEPER,
			"destroyed GLRenderer");
	}
}

bool GLRenderer::reset(){
	if(mGLPrimaryRenderTarget!=NULL){
		mGLPrimaryRenderTarget->reset();
	}

	setDefaultStates();

	return true;
}

bool GLRenderer::activateAdditionalContext(){
	if(mGLPrimaryRenderTarget!=NULL){
		return mGLPrimaryRenderTarget->activateAdditionalContext();
	}
	return false;
}

// Resource operations
Texture *GLRenderer::createTexture(){
	return new GLTexture(this);
}

PixelBufferRenderTarget *GLRenderer::createPixelBufferRenderTarget(){
	#if defined(TOADLET_HAS_GLFBOS)
		if(mFBOsAvailable){
			return new GLFBORenderTarget(this);
		}
	#endif
	#if defined(TOADLET_HAS_GLPBUFFERS)
		if(mPBuffersAvailable){
			return new_GLPBufferRenderTarget(this);
		}
	#endif

	Error::unimplemented("GLRenderer::createPixelBufferRenderTarget is unavailable");
	return NULL;
}

PixelBuffer *GLRenderer::createPixelBuffer(){
	return new GLBuffer(this);
}

VertexFormat *GLRenderer::createVertexFormat(){
	return new GLVertexFormat(this);
}

VertexBuffer *GLRenderer::createVertexBuffer(){
	return new GLBuffer(this);
}

IndexBuffer *GLRenderer::createIndexBuffer(){
	return new GLBuffer(this);
}

Program *GLRenderer::createProgram(){
	Error::unimplemented("GLRenderer::createProgram is unavailable");
	return NULL;
}

Shader *GLRenderer::createShader(){
	Error::unimplemented("GLRenderer::createShader is unavailable");
	return NULL;
}

Query *GLRenderer::createQuery(){
	#if defined(TOADLET_HAS_GLQUERY)
		return new GLQuery(this);
	#else
		Error::unimplemented("GLRenderer::createQuery is unavailable");
		return NULL;
	#endif
}

RenderStateSet *GLRenderer::createRenderStateSet(){
	return new GLRenderStateSet(this);
}

// Matrix operations
void GLRenderer::setModelMatrix(const Matrix4x4 &matrix){
	if(mMatrixMode!=GL_MODELVIEW){
		mMatrixMode=GL_MODELVIEW;
		glMatrixMode(mMatrixMode);
	}

	#if defined(TOADLET_FIXED_POINT)
		#if defined(TOADLET_HAS_GLES)
			mModelMatrix.set(matrix);

			glLoadMatrixx(mViewMatrix.data);
			glMultMatrixx(mModelMatrix.data);
		#else
			MathConversion::scalarToFloat(mModelMatrix,matrix);

			glLoadMatrixf(mViewMatrix.data);
			glMultMatrixf(mModelMatrix.data);
		#endif
	#else
		mModelMatrix.set(matrix);

		glLoadMatrixf(mViewMatrix.data);
		glMultMatrixf(mModelMatrix.data);
	#endif

	TOADLET_CHECK_GLERROR("setModelMatrix");
}

void GLRenderer::setViewMatrix(const Matrix4x4 &matrix){
	if(mMatrixMode!=GL_MODELVIEW){
		mMatrixMode=GL_MODELVIEW;
		glMatrixMode(mMatrixMode);
	}

	#if defined(TOADLET_FIXED_POINT)
		#if defined(TOADLET_HAS_GLES)
			mViewMatrix.set(matrix);

			glLoadMatrixx(mViewMatrix.data);
			glMultMatrixx(mModelMatrix.data);
		#else
			MathConversion::scalarToFloat(mViewMatrix,matrix);

			glLoadMatrixf(mViewMatrix.data);
			glMultMatrixf(mModelMatrix.data);
		#endif
	#else
		mViewMatrix.set(matrix);

		glLoadMatrixf(mViewMatrix.data);
		glMultMatrixf(mModelMatrix.data);
	#endif

	TOADLET_CHECK_GLERROR("setViewMatrix");
}

void GLRenderer::setProjectionMatrix(const Matrix4x4 &matrix){
	if(mMatrixMode!=GL_PROJECTION){
		mMatrixMode=GL_PROJECTION;
		glMatrixMode(mMatrixMode);
	}

	#if defined(TOADLET_FIXED_POINT)
		#if defined(TOADLET_HAS_GLES)
			glLoadMatrixx(matrix.data);
		#else
			MathConversion::scalarToFloat(cacheMatrix4x4,matrix);
			glLoadMatrixf(cacheMatrix4x4.data);
		#endif
	#else
		glLoadMatrixf(matrix.data);
	#endif

	TOADLET_CHECK_GLERROR("setProjectionMatrix");
}

bool GLRenderer::setRenderTarget(RenderTarget *target){
	GLRenderTarget *gltarget=NULL;
	if(target!=NULL){
		gltarget=(GLRenderTarget*)target->getRootRenderTarget();
		if(gltarget==NULL){
			Error::nullPointer(Categories::TOADLET_PEEPER,
				"RenderTarget is not a GLRenderTarget");
			return false;
		}
	}

	// We only want to deactivate if it's not our PrimaryRenderTarget for OpenGL, otherwise we would lose our context to issue further GL commands
	if(mGLRenderTarget!=NULL && mGLRenderTarget!=mGLPrimaryRenderTarget){
		mGLRenderTarget->deactivate();
	}

	mRenderTarget=target;
	mGLRenderTarget=gltarget;

	if(mGLRenderTarget!=NULL){
		mGLRenderTarget->activate();
	}

	TOADLET_CHECK_GLERROR("setRenderTarget");

	return true;
}

void GLRenderer::setViewport(const Viewport &viewport){
	mViewport.set(viewport);

	if(mRenderTarget!=NULL){
		int rtwidth=mRenderTarget->getWidth();
		int rtheight=mRenderTarget->getHeight();

		glViewport(viewport.x,rtheight-(viewport.y+viewport.height),viewport.width,viewport.height);
		if(viewport.x==0 && viewport.y==0 && viewport.width==rtwidth && viewport.height==rtheight){
			glDisable(GL_SCISSOR_TEST);
		}
		else{
			glScissor(viewport.x,rtheight-(viewport.y+viewport.height),viewport.width,viewport.height);
			glEnable(GL_SCISSOR_TEST);
		}
	}
	else{
		glViewport(0,0,viewport.width,viewport.height);
		glDisable(GL_SCISSOR_TEST);

		Error::unknown(Categories::TOADLET_PEEPER,
			"setViewport called without a valid render target");
	}

	// Update PointState, since it is dependent upon viewport size
	setPointState(mPointState);

	TOADLET_CHECK_GLERROR("setViewport");
}

void GLRenderer::clear(int clearFlags,const Vector4 &clearColor){
	int bufferBits=0;

	TOADLET_CHECK_GLERROR("entering clear");

	if((clearFlags & ClearFlag_COLOR)>0){
		bufferBits|=GL_COLOR_BUFFER_BIT;
		glClearColor(clearColor.x,clearColor.y,clearColor.z,clearColor.w);
	}
	if((clearFlags & ClearFlag_DEPTH)>0){
		bufferBits|=GL_DEPTH_BUFFER_BIT;
	}
	if((clearFlags & ClearFlag_STENCIL)>0){
		bufferBits|=GL_STENCIL_BUFFER_BIT;
	}

	if(mDepthState.write==false){
		glDepthMask(GL_TRUE);
		glClear(bufferBits);
		glDepthMask(GL_FALSE);
	}
	else{
		glClear(bufferBits);
	}

	TOADLET_CHECK_GLERROR("clear");
}

void GLRenderer::swap(){
	if(mGLRenderTarget!=NULL){
		mGLRenderTarget->swap();
	}

	TOADLET_CHECK_GLERROR("swap");
}

void GLRenderer::beginScene(){
	TOADLET_CHECK_GLERROR("before beginScene");

	#if defined(TOADLET_DEBUG)
		mBeginEndCounter++;
		if(mBeginEndCounter!=1){
			mBeginEndCounter--;
			Error::unknown(Categories::TOADLET_PEEPER,
				"GLRenderer::beginScene: Unmached beginScene/endScene");
		}
	#endif

	TOADLET_CHECK_GLERROR("beginScene");
}

void GLRenderer::endScene(){
	#if defined(TOADLET_DEBUG)
		mBeginEndCounter--;
		if(mBeginEndCounter!=0){
			mBeginEndCounter++;
			Error::unknown(Categories::TOADLET_PEEPER,
				"GLRenderer::endScene: Unmached beginScene/endScene");
		}
	#endif

	if(mLastVertexData!=NULL){
		mLastTypeBits=setVertexData(NULL,mLastTypeBits);
		mLastVertexData=VertexData::wptr();
	}
	if(mCapabilityState.hardwareVertexBuffers){
		glBindBuffer(GL_ARRAY_BUFFER,0);
	}
	if(mCapabilityState.hardwareIndexBuffers){
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	}

	int i;
	for(i=0;i<mCapabilityState.maxTextureStages;++i){
		setTextureStage(i,NULL);
	}

	TOADLET_CHECK_GLERROR("endScene");
}

void GLRenderer::renderPrimitive(const VertexData::ptr &vertexData,const IndexData::ptr &indexData){
	#if defined(TOADLET_DEBUG)
		if(mBeginEndCounter!=1){
			Error::unknown(Categories::TOADLET_PEEPER,
				"GLRenderer::renderPrimitive called outside of begin/end scene");
		}
	#endif

	if(vertexData==NULL || indexData==NULL){
		Error::nullPointer(Categories::TOADLET_PEEPER,
			"VertexData or IndexData is NULL");
		return;
	}

	if(indexData->count<=0){
		return;
	}

	IndexData::Primitive primitive=indexData->primitive;

	GLenum type=0;
	switch(primitive){
		case IndexData::Primitive_POINTS:
			type=GL_POINTS;
		break;
		case IndexData::Primitive_LINES:
			type=GL_LINES;
		break;
		case IndexData::Primitive_LINESTRIP:
			type=GL_LINE_STRIP;
		break;
		case IndexData::Primitive_TRIS:
			type=GL_TRIANGLES;
		break;
		case IndexData::Primitive_TRISTRIP:
			type=GL_TRIANGLE_STRIP;
		break;
		case IndexData::Primitive_TRIFAN:
			type=GL_TRIANGLE_FAN;
		break;
	}

	bool rebind=false;
	if(mLastVertexData.get()!=vertexData.get() || mMaxTexCoordIndex!=mLastMaxTexCoordIndex){
		rebind=true;
	}
	else{
		// See if we need to rebind it due to the texCoordIndex portions of the TextureStages changing
		int i;
		for(i=0;i<mMaxTexCoordIndex;++i){
			if(mLastTexCoordIndexes[i]!=mTexCoordIndexes[i]){
				rebind=true;
				break;
			}
		}
	}
	if(rebind){
		mLastMaxTexCoordIndex=mMaxTexCoordIndex;
		mLastTypeBits=setVertexData(vertexData,mLastTypeBits);
		mLastVertexData=vertexData;
	}

	IndexBuffer *indexBuffer=indexData->indexBuffer;
	if(indexBuffer!=NULL){
		GLenum indexType=getGLIndexType(indexBuffer->getIndexFormat());
		GLBuffer *glindexBuffer=(GLBuffer*)indexBuffer->getRootIndexBuffer();
		if(glindexBuffer->mHandle==0){
			if(mCapabilityState.hardwareIndexBuffers){
				glBindBuffer(glindexBuffer->mTarget,0);
			}
			glDrawElements(type,indexData->count,indexType,glindexBuffer->mData+indexData->start*glindexBuffer->mIndexFormat);
		}
		else{
			glBindBuffer(glindexBuffer->mTarget,glindexBuffer->mHandle);
			glDrawElements(type,indexData->count,indexType,(uint8*)(indexData->start*glindexBuffer->mIndexFormat));
		}

		TOADLET_CHECK_GLERROR("glDrawElements");
	}
	else{
		glDrawArrays(type,indexData->start,indexData->count);

		TOADLET_CHECK_GLERROR("glDrawArrays");
	}
}

bool GLRenderer::copyFrameBufferToPixelBuffer(PixelBuffer *dst){
	GLPixelBuffer *glpixelBuffer=(GLPixelBuffer*)dst->getRootPixelBuffer();

	GLTextureMipPixelBuffer *textureBuffer=glpixelBuffer->castToGLTextureMipPixelBuffer();
	GLFBOPixelBuffer *fboBuffer=glpixelBuffer->castToGLFBOPixelBuffer();
	GLBuffer *pixelBuffer=glpixelBuffer->castToGLBuffer();
	RenderTarget *renderTarget=mRenderTarget->getRootRenderTarget();

	if(textureBuffer!=NULL){
		GLTexture *gltexture=textureBuffer->getTexture();

		glBindTexture(gltexture->mTarget,gltexture->mHandle);
		TOADLET_CHECK_GLERROR("glBindTexture");

		glCopyTexSubImage2D(gltexture->mTarget,textureBuffer->getLevel(),0,0,0,renderTarget->getHeight()-gltexture->mHeight,gltexture->mWidth,gltexture->mHeight);
		TOADLET_CHECK_GLERROR("glCopyTexSubImage2D");

		Matrix4x4 matrix;
		Math::setMatrix4x4FromScale(matrix,Math::ONE,-Math::ONE,Math::ONE);
		Math::setMatrix4x4FromTranslate(matrix,0,Math::ONE,0);
		gltexture->setMatrix(matrix);
	}
	else if(fboBuffer!=NULL){
		Error::unimplemented(Categories::TOADLET_PEEPER,
			"GLRenderer::copyToSurface: unimplemented for fbo");
		return false;
	}
	else if(pixelBuffer!=NULL){
		Error::unimplemented(Categories::TOADLET_PEEPER,
			"GLRenderer::copyToSurface: unimplemented for pbo");
		return false;
	}

	return true;
}

bool GLRenderer::copyPixelBuffer(PixelBuffer *dst,PixelBuffer *src){
	#if defined(TOADLET_HAS_GLEW)
		GLPixelBuffer *gldst=(GLPixelBuffer*)dst->getRootPixelBuffer();
		GLPixelBuffer *glsrc=(GLPixelBuffer*)src->getRootPixelBuffer();

		GLTextureMipPixelBuffer *srcTextureBuffer=glsrc->castToGLTextureMipPixelBuffer();
		GLBuffer *srcPixelBuffer=glsrc->castToGLBuffer();
		GLTextureMipPixelBuffer *dstTextureBuffer=gldst->castToGLTextureMipPixelBuffer();
		GLBuffer *dstPixelBuffer=gldst->castToGLBuffer();

		if(srcTextureBuffer!=NULL && dstPixelBuffer!=NULL){
			glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB,dstPixelBuffer->mHandle);

			GLTexture *srcTexture=srcTextureBuffer->mTexture;
			srcTexture->load(srcTexture->mWidth,srcTexture->mHeight,srcTexture->mDepth,srcTextureBuffer->mLevel,NULL);

			glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB,0);
		}
		else if(srcPixelBuffer!=NULL && dstTextureBuffer!=NULL){
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB,srcPixelBuffer->mHandle);

			GLTexture *dstTexture=dstTextureBuffer->mTexture;
			dstTexture->load(dstTexture->mWidth,dstTexture->mHeight,dstTexture->mDepth,dstTextureBuffer->mLevel,NULL);

			glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB,0);
		}
		else
	#endif
	{
		Error::unimplemented("copyPixelBuffer not implemented for these pixel buffer types");
		return false;
	}

	return true;
}

void GLRenderer::setDefaultStates(){
	// General states
	setAlphaTest(AlphaTest_NONE,Math::HALF);
	setBlendState(BlendState());
	setDepthState(DepthState());
	setFogState(FogState());
	setNormalize(Normalize_RESCALE);
	setPointState(PointState());
	setRasterizerState(RasterizerState());
	setMaterialState(MaterialState());
	setAmbientColor(Math::ONE_VECTOR4);

	int i;
	for(i=0;i<mCapabilityState.maxTextureStages;++i){
		mLastTexTargets[i]=GL_TEXTURE_2D;
		setTextureStage(i,NULL);
	}

	// GL specific states
	#if !defined(TOADLET_HAS_GLES) && defined(TOADLET_HAS_GL_11)
	if(gl_version>=11){
		glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL,GL_SEPARATE_SPECULAR_COLOR);
	}
	#endif

	TOADLET_CHECK_GLERROR("setDefaultStates");
}

bool GLRenderer::setRenderStateSet(RenderStateSet *set){
	GLRenderStateSet *glset=NULL;
	if(set!=NULL){
		glset=(GLRenderStateSet*)set->getRootRenderStateSet();
		if(glset==NULL){
			Error::nullPointer(Categories::TOADLET_PEEPER,
				"RenderStateSet is not a GLRenderStateSet");
			return false;
		}
	}

	if(glset->mBlendState!=NULL){
		setBlendState(*glset->mBlendState);
	}
	if(glset->mDepthState!=NULL){
		setDepthState(*glset->mDepthState);
	}
	if(glset->mRasterizerState!=NULL){
		setRasterizerState(*glset->mRasterizerState);
	}
	if(glset->mFogState!=NULL){
		setFogState(*glset->mFogState);
	}
	if(glset->mPointState!=NULL){
		setPointState(*glset->mPointState);
	}
	if(glset->mMaterialState!=NULL){
		setMaterialState(*glset->mMaterialState);
	}

	return true;
}

void GLRenderer::setAlphaTest(const AlphaTest &alphaTest,scalar cutoff){
	if(alphaTest==AlphaTest_NONE){
		glDisable(GL_ALPHA_TEST);
	}
	else{
		GLenum func=getGLAlphaFunc(alphaTest);

		#if defined(TOADLET_FIXED_POINT) && defined(TOADLET_HAS_GLES)
			glAlphaFuncx(func,cutoff);
		#else
			glAlphaFunc(func,MathConversion::scalarToFloat(cutoff));
		#endif

		glEnable(GL_ALPHA_TEST);
	}

	TOADLET_CHECK_GLERROR("setAlphaTest");
}

void GLRenderer::setBlendState(const BlendState &state){
	if(state.equals(BlendState::Combination_DISABLED)){
		glDisable(GL_BLEND);
	}
	else{
		int src=getGLBlendOperation(state.source);
		int dest=getGLBlendOperation(state.dest);

		glBlendFunc(src,dest);
		glEnable(GL_BLEND);
	}

	glColorMask((state.colorWrite&BlendState::ColorWrite_BIT_R)>0,(state.colorWrite&BlendState::ColorWrite_BIT_G)>0,(state.colorWrite&BlendState::ColorWrite_BIT_B)>0,(state.colorWrite&BlendState::ColorWrite_BIT_A)>0);

	TOADLET_CHECK_GLERROR("setBlendState");
}

void GLRenderer::setDepthState(const DepthState &state){
	mDepthState.set(state);

	if(state.test==DepthState::DepthTest_NONE){
		glDisable(GL_DEPTH_TEST);
	}
	else{
		glDepthFunc(getGLDepthFunc(state.test));
		glEnable(GL_DEPTH_TEST);
	}

	glDepthMask(state.write?GL_TRUE:GL_FALSE);

	TOADLET_CHECK_GLERROR("setDepthState");
}

void GLRenderer::setFogState(const FogState &state){
	if(state.type==FogState::FogType_NONE){
		glDisable(GL_FOG);
	}
	else{
		glEnable(GL_FOG);
		glFogf(GL_FOG_MODE,getGLFogType(state.type));
		#if defined(TOADLET_FIXED_POINT)
			#if defined(TOADLET_HAS_GLES)
				glFogx(GL_FOG_START,state.nearDistance);
				glFogx(GL_FOG_END,state.farDistance);
				glFogxv(GL_FOG_COLOR,state.color.getData());
			#else
				glFogf(GL_FOG_START,MathConversion::scalarToFloat(state.nearDistance));
				glFogf(GL_FOG_END,MathConversion::scalarToFloat(state.farDistance));
				glFogfv(GL_FOG_COLOR,colorArray(cacheArray,state.color));
			#endif
		#else
			glFogf(GL_FOG_START,state.nearDistance);
			glFogf(GL_FOG_END,state.farDistance);
			glFogfv(GL_FOG_COLOR,state.color.getData());
		#endif
	}

	TOADLET_CHECK_GLERROR("setFogParameters");
}

void GLRenderer::setMaterialState(const MaterialState &state){
	if(state.lighting){
		// 12/19/10
		// There appears to be a bug in at least:
		// GL_VENDOR:NVIDIA Corporation
		// GL_RENDERER:GeForce 9300M GS/PCI/SSE2
		// GL_VERSION:2.1.2
		// Where in some cases lighting is not properly enabled unless we disable it first
		// Found it in WizardWars when rendering multiple labels and then using the camera gamma
		glDisable(GL_LIGHTING);
		glEnable(GL_LIGHTING);
	}
	else{
		glDisable(GL_LIGHTING);
	}

	// The GL_COLOR_MATERIAL command must come before the actual Material settings
	if(state.trackColor){
		#if !defined(TOADLET_HAS_GLES) && defined(TOADLET_HAS_GL_11)
		if(gl_version>=11){
			glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
		}
		#endif
		glEnable(GL_COLOR_MATERIAL);
	}
	else{
		glDisable(GL_COLOR_MATERIAL);
	}

	#if defined(TOADLET_FIXED_POINT)
		#if defined(TOADLET_HAS_GLES)
			glMaterialxv(GL_FRONT_AND_BACK,GL_AMBIENT,state.ambient.getData());
			glMaterialxv(GL_FRONT_AND_BACK,GL_DIFFUSE,state.diffuse.getData());
			glMaterialxv(GL_FRONT_AND_BACK,GL_SPECULAR,state.specular.getData());
			glMaterialx(GL_FRONT_AND_BACK,GL_SHININESS,state.shininess);
			glMaterialxv(GL_FRONT_AND_BACK,GL_EMISSION,state.emissive.getData());
		#else
			glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,colorArray(cacheArray,state.ambient));
			glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,colorArray(cacheArray,state.diffuse));
			glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,colorArray(cacheArray,state.specular));
			glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,MathConversion::scalarToFloat(state.shininess));
			glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,colorArray(cacheArray,state.emissive));
		#endif
	#else
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,state.ambient.getData());
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,state.diffuse.getData());
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,state.specular.getData());
		glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,state.shininess);
		glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,state.emissive.getData());
	#endif

	glShadeModel(getGLShadeModel(state.shade));

	TOADLET_CHECK_GLERROR("setMaterialState");
}

void GLRenderer::setNormalize(const Normalize &normalize){
	switch(normalize){
		case Normalize_NONE:
			glDisable(GL_NORMALIZE);
			glDisable(GL_RESCALE_NORMAL);
		break;
		case Normalize_RESCALE:
			glDisable(GL_NORMALIZE);
			glEnable(GL_RESCALE_NORMAL);
		break;
		case Normalize_NORMALIZE:
			glEnable(GL_NORMALIZE);
			glDisable(GL_RESCALE_NORMAL);
		break;
	}

	TOADLET_CHECK_GLERROR("setNormalize");
}

void GLRenderer::setPointState(const PointState &state){
	mPointState.set(state);

	// pointsize = size / sqrt(constant + linear*d + quadratic*d*d)
	// if a&b = 0, then quadratic = 1/(C*C) where C = first component of projMatrix * 1/2 screen width
	if(mCapabilityState.pointSprites){
		int value;
		if(state.sprite){
			glEnable(GL_POINT_SPRITE);
			value=1;
		}
		else{
			glDisable(GL_POINT_SPRITE);
			value=0;
		}

		if(mMultiTexture){
			int stage;
			for(stage=0;stage<mCapabilityState.maxTextureStages;++stage){
				glActiveTexture(GL_TEXTURE0+stage);
				glTexEnvi(GL_POINT_SPRITE,GL_COORD_REPLACE,value);
			}
		}
		else{
			glTexEnvi(GL_POINT_SPRITE,GL_COORD_REPLACE,value);
		}

	    glPointParameterf(GL_POINT_SIZE_MIN,state.minSize);
		glPointParameterf(GL_POINT_SIZE_MAX,state.maxSize);

		if(state.attenuated){
			#if defined(TOADLET_FIXED_POINT)
				#if defined(TOADLET_HAS_GLES)
					cacheArray[0]=state.constant; cacheArray[1]=state.linear; cacheArray[2]=state.quadratic;
					glPointParameterxv(GL_POINT_DISTANCE_ATTENUATION,cacheArray);
				#else
					cacheArray[0]=MathConversion::scalarToFloat(state.constant); cacheArray[1]=MathConversion::scalarToFloat(state.linear); cacheArray[2]=MathConversion::scalarToFloat(state.quadratic);
					glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION,cacheArray);
				#endif
			#else
				cacheArray[0]=state.constant; cacheArray[1]=state.linear; cacheArray[2]=state.quadratic;
				glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION,cacheArray);
			#endif
		}
		else{
			cacheArray[0]=Math::ONE; cacheArray[1]=0; cacheArray[2]=0;
			#if defined(TOADLET_HAS_GLES)
				glPointParameterxv(GL_POINT_DISTANCE_ATTENUATION,cacheArray);
			#else
				glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION,cacheArray);
			#endif
		}
	}

	if(state.size>0){
		if(state.attenuated){
			glPointSize(MathConversion::scalarToFloat(state.size) * mViewport.height);
		}
		else{
			glPointSize(MathConversion::scalarToFloat(state.size));
		}
	}

	TOADLET_CHECK_GLERROR("setPointState");
}

void GLRenderer::setRasterizerState(const RasterizerState &state){
	if(state.cull==RasterizerState::CullType_NONE){
		glDisable(GL_CULL_FACE);
	}
	else{
		glCullFace(getGLCullFace(state.cull));
		glEnable(GL_CULL_FACE);
	}

	#if !defined(TOADLET_HAS_GLES)
		glPolygonMode(GL_FRONT_AND_BACK,getGLPolygonMode(state.fill));
	#endif

	if(state.depthBiasConstant==0 && state.depthBiasSlope==0){
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
	else{
		#if defined(TOADLET_FIXED_POINT) && defined(TOADLET_HAS_GLES)
			glPolygonOffsetx(state.depthBiasSlope,state.depthBiasConstant);
		#else
			glPolygonOffset(MathConversion::scalarToFloat(state.depthBiasSlope),MathConversion::scalarToFloat(state.depthBiasConstant));
		#endif
		glEnable(GL_POLYGON_OFFSET_FILL);
	}

	if(state.multisample){
		glEnable(GL_MULTISAMPLE);
	}
	else{
		glDisable(GL_MULTISAMPLE);
	}

	if(state.dither){
		glEnable(GL_DITHER);
	}
	else{
		glDisable(GL_DITHER);
	}

	TOADLET_CHECK_GLERROR("setRasterizerState");
}

void GLRenderer::setTextureStage(int stage,TextureStage *textureStage){
	if(stage>=mCapabilityState.maxTextureStages){
		return;
	}

	if(mMultiTexture){
		glActiveTexture(GL_TEXTURE0+stage);
	}
	else if(stage>0){
		return;
	}

	if(textureStage!=NULL){
		Texture *texture=textureStage->texture;
		GLuint textureTarget=0;
		GLTexture *gltexture=texture!=NULL?(GLTexture*)texture->getRootTexture(textureStage->textureTime):NULL;
		if(gltexture!=NULL){
			textureTarget=gltexture->mTarget;

			if(mLastTextures[stage]!=gltexture){
				glBindTexture(textureTarget,gltexture->mHandle);
				mLastTextures[stage]=gltexture;
			}

			TextureStage::Calculation calculation=textureStage->calculation;
			Matrix4x4 &transform=cache_setTextureStage_transform;
			bool identityTransform=texture->getRootTransform(textureStage->textureTime,transform);
			if(calculation==TextureStage::Calculation_DISABLED && ((gltexture->mUsage&(Texture::Usage_BIT_NPOT_RESTRICTED|Texture::Usage_BIT_RENDERTARGET))>0 || identityTransform==false)){
				calculation=TextureStage::Calculation_NORMAL;
			}

			if(calculation!=TextureStage::Calculation_DISABLED){
				if(mMatrixMode!=GL_TEXTURE){
					mMatrixMode=GL_TEXTURE;
					glMatrixMode(mMatrixMode);
				}

				glLoadIdentity();

				const Matrix4x4 &matrix=textureStage->matrix;
				if(calculation==TextureStage::Calculation_NORMAL){
					if(identityTransform==false){
						#if defined(TOADLET_FIXED_POINT)
							#if defined(TOADLET_HAS_GLES)
								glMultMatrixx(transform.data);
							#else
								glMultMatrixf(MathConversion::scalarToFloat(cacheMatrix4x4,transform).data);
							#endif
						#else
							glMultMatrixf(transform.data);
						#endif
					}

					#if defined(TOADLET_FIXED_POINT)
						#if defined(TOADLET_HAS_GLES)
							glMultMatrixx(gltexture->mMatrix.data);
						#else
							glMultMatrixf(MathConversion::scalarToFloat(cacheMatrix4x4,gltexture->mMatrix).data);
						#endif
					#else
						glMultMatrixf(gltexture->mMatrix.data);
					#endif

					#if defined(TOADLET_FIXED_POINT)
						#if defined(TOADLET_HAS_GLES)
							glMultMatrixx(matrix.data);
						#else
							glMultMatrixf(MathConversion::scalarToFloat(cacheMatrix4x4,matrix).data);
						#endif
					#else
						glMultMatrixf(matrix.data);
					#endif
				}
				else if(calculation>TextureStage::Calculation_NORMAL){
					#if !defined(TOADLET_HAS_GLES)
						if(mMatrixMode!=GL_MODELVIEW){
							mMatrixMode=GL_MODELVIEW;
							glMatrixMode(mMatrixMode);
						}

						glPushMatrix();
						glLoadIdentity();

						GLint gltg=0;
						GLint glpl=0;
						switch(calculation){
							case TextureStage::Calculation_OBJECTSPACE:
								gltg=GL_OBJECT_LINEAR;
								glpl=GL_OBJECT_PLANE;
							break;
							case TextureStage::Calculation_CAMERASPACE:
								gltg=GL_EYE_LINEAR;
								glpl=GL_EYE_PLANE;
							break;
							default:
							break;
						}

						// Set up texgen
						glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,gltg);
						cacheArray[0]=matrix.at(0,0);cacheArray[1]=matrix.at(0,1);cacheArray[2]=matrix.at(0,2);cacheArray[3]=matrix.at(0,3);
						glTexGenfv(GL_S,glpl,cacheArray);
						glEnable(GL_TEXTURE_GEN_S);

						glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,gltg);
						cacheArray[0]=matrix.at(1,0);cacheArray[1]=matrix.at(1,1);cacheArray[2]=matrix.at(1,2);cacheArray[3]=matrix.at(1,3);
						glTexGenfv(GL_T,glpl,cacheArray);
						glEnable(GL_TEXTURE_GEN_T);

						glTexGeni(GL_R,GL_TEXTURE_GEN_MODE,gltg);
						cacheArray[0]=matrix.at(2,0);cacheArray[1]=matrix.at(2,1);cacheArray[2]=matrix.at(2,2);cacheArray[3]=matrix.at(2,3);
						glTexGenfv(GL_R,glpl,cacheArray);
						glEnable(GL_TEXTURE_GEN_R);

						glTexGeni(GL_Q,GL_TEXTURE_GEN_MODE,gltg);
						cacheArray[0]=matrix.at(3,0);cacheArray[1]=matrix.at(3,1);cacheArray[2]=matrix.at(3,2);cacheArray[3]=matrix.at(3,3);
						glTexGenfv(GL_Q,glpl,cacheArray);
						glEnable(GL_TEXTURE_GEN_Q);

						glPopMatrix();
					#endif
				}
			}
			else{
				if(mMatrixMode!=GL_TEXTURE){
					mMatrixMode=GL_TEXTURE;
					glMatrixMode(mMatrixMode);
				}
				glLoadIdentity();
			}

			#if !defined(TOADLET_HAS_GLES)
				if(calculation<=TextureStage::Calculation_NORMAL){
					glDisable(GL_TEXTURE_GEN_S);
					glDisable(GL_TEXTURE_GEN_T);
					glDisable(GL_TEXTURE_GEN_R);
					glDisable(GL_TEXTURE_GEN_Q);
				}
			#endif
		}
		else{
			textureTarget=GL_TEXTURE_2D;
			if(mLastTextures[stage]!=gltexture){
				glBindTexture(textureTarget,0);
				mLastTextures[stage]=gltexture;
			}
		}

		if(mLastTexTargets[stage]!=textureTarget){
			if(mLastTexTargets[stage]!=0){
				glDisable(mLastTexTargets[stage]);
			}
			if(textureTarget!=0){
				glEnable(textureTarget);
			}
			mLastTexTargets[stage]=textureTarget;
		}

		// Setup combiners
		const TextureBlend &blend=textureStage->blend;
		bool specifyColor=(blend.colorOperation!=TextureBlend::Operation_UNSPECIFIED);
		bool specifyAlpha=(blend.alphaOperation!=TextureBlend::Operation_UNSPECIFIED);
		bool specifyColorSource=!(blend.colorSource1==TextureBlend::Source_UNSPECIFIED && blend.colorSource2==TextureBlend::Source_UNSPECIFIED);
		bool specifyAlphaSource=!(blend.alphaSource1==TextureBlend::Source_UNSPECIFIED && blend.alphaSource2==TextureBlend::Source_UNSPECIFIED);

		if(!specifyColorSource && !specifyAlphaSource){
			if(specifyColor==false){
				glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
			}
			else{
				glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,getGLTextureBlendOperation(blend.colorOperation));
			}
		}
		else{
			glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
			if(blend.colorOperation==TextureBlend::Operation_DOTPRODUCT && blend.alphaOperation==TextureBlend::Operation_DOTPRODUCT){
				glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_DOT3_RGBA);
			}
			else{
				if(specifyColor){
					glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,getGLTextureBlendOperation(blend.colorOperation));
				}
				if(specifyAlpha){
					if(blend.alphaOperation==TextureBlend::Operation_DOTPRODUCT){
						Error::unknown(Categories::TOADLET_PEEPER,"can not use Operation_DOTPRODUCT in only alpha channel");
					}
					else{
						glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,getGLTextureBlendOperation(blend.alphaOperation));
					}
				}
			}

			#if defined(TOADLET_FIXED_POINT) && defined(TOADLET_HAS_GLES)
				glTexEnvxv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,textureStage->constantColor.getData());
			#else
				glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,colorArray(cacheArray,textureStage->constantColor));
			#endif

			if(specifyColor){
				if(blend.colorSource1!=TextureBlend::Source_UNSPECIFIED){
					glTexEnvi(GL_TEXTURE_ENV,GL_SRC0_RGB,getGLTextureBlendSource(blend.colorSource1));
				}
				if(blend.colorSource2!=TextureBlend::Source_UNSPECIFIED){
					glTexEnvi(GL_TEXTURE_ENV,GL_SRC1_RGB,getGLTextureBlendSource(blend.colorSource2));
				}
				if(blend.colorSource3!=TextureBlend::Source_UNSPECIFIED){
					glTexEnvi(GL_TEXTURE_ENV,GL_SRC2_RGB,getGLTextureBlendSource(blend.colorSource3));
				}
				// No need to specify GL_OPERANDs for ALPHABLEND, since RGB defaults are COLOR,COLOR,ALPHA
			}
			if(specifyAlpha){
				if(blend.alphaSource1!=TextureBlend::Source_UNSPECIFIED){
					glTexEnvi(GL_TEXTURE_ENV,GL_SRC0_ALPHA,getGLTextureBlendSource(blend.alphaSource1));
				}
				if(blend.alphaSource2!=TextureBlend::Source_UNSPECIFIED){
					glTexEnvi(GL_TEXTURE_ENV,GL_SRC1_ALPHA,getGLTextureBlendSource(blend.alphaSource2));
				}
				if(blend.alphaSource3!=TextureBlend::Source_UNSPECIFIED){
					glTexEnvi(GL_TEXTURE_ENV,GL_SRC2_ALPHA,getGLTextureBlendSource(blend.alphaSource3));
				}
				// No need to specify GL_OPERANDs for ALPHABLEND, since ALPHA defaults are ALPHA,ALPHA,ALPHA
			}
		}
		if(specifyColor){
			glTexEnvf(GL_TEXTURE_ENV,GL_RGB_SCALE,getGLTextureBlendScale(blend.colorOperation));
		}
		if(specifyAlpha){
			glTexEnvf(GL_TEXTURE_ENV,GL_ALPHA_SCALE,getGLTextureBlendScale(blend.alphaOperation));
		}

		bool hasClampToEdge=
			#if defined(TOADLET_HAS_GLES)
				true;
			#else
				gl_version>=12;
			#endif
		glTexParameteri(textureTarget,GL_TEXTURE_WRAP_S,getGLWrap(textureStage->uAddressMode,hasClampToEdge));
		glTexParameteri(textureTarget,GL_TEXTURE_WRAP_T,getGLWrap(textureStage->vAddressMode,hasClampToEdge));
		#if !defined(TOADLET_HAS_GLES) && defined(TOADLET_HAS_GL_12)
		if(gl_version>=12){
			glTexParameteri(textureTarget,GL_TEXTURE_WRAP_R,getGLWrap(textureStage->wAddressMode,hasClampToEdge));
		}
		#endif

		glTexParameteri(textureTarget,GL_TEXTURE_MIN_FILTER,getGLMinFilter(textureStage->minFilter,textureStage->mipFilter));
		glTexParameteri(textureTarget,GL_TEXTURE_MAG_FILTER,getGLMagFilter(textureStage->magFilter));

		#if !defined(TOADLET_HAS_GLES)
			if(gl_version>=14){
				if(textureStage->shadowComparison!=TextureStage::ShadowComparison_DISABLED){
					glTexParameteri(textureTarget,GL_DEPTH_TEXTURE_MODE_ARB,getGLDepthTextureMode(textureStage->shadowComparison));
					glTexParameteri(textureTarget,GL_TEXTURE_COMPARE_FUNC_ARB,GL_LEQUAL);
					glTexParameteri(textureTarget,GL_TEXTURE_COMPARE_MODE_ARB,GL_COMPARE_R_TO_TEXTURE);
				}
				else{
					glTexParameteri(textureTarget,GL_TEXTURE_COMPARE_MODE_ARB,GL_NONE);
				}
			}
		#endif

		if(stage==0){
			#if defined(TOADLET_HAS_GLES)
				glHint(GL_PERSPECTIVE_CORRECTION_HINT,textureStage->perspective?GL_NICEST:GL_FASTEST);
			#endif
		}

		mTexCoordIndexes[stage]=textureStage->texCoordIndex;

		if(stage>=mMaxTexCoordIndex){
			mMaxTexCoordIndex=stage+1;
		}
	}
	else{
		if(mLastTextures[stage]!=NULL){
			mLastTextures[stage]=NULL;
		}

		if(mLastTexTargets[stage]!=0){
			glDisable(mLastTexTargets[stage]);
			mLastTexTargets[stage]=0;
		}

		while(mMaxTexCoordIndex>0 && mLastTexTargets[mMaxTexCoordIndex-1]==0){
			mMaxTexCoordIndex--;
		}
	}

	TOADLET_CHECK_GLERROR("setTextureStage");
}

void GLRenderer::setLightState(int i,const LightState &state){
	GLenum l=GL_LIGHT0+i;

	if(mMatrixMode!=GL_MODELVIEW){
		mMatrixMode=GL_MODELVIEW;
		glMatrixMode(mMatrixMode);
	}

	glPushMatrix();
	#if	defined(TOADLET_FIXED_POINT) && defined(TOADLET_HAS_GLES)
		glLoadMatrixx(mViewMatrix.data);
	#else
		glLoadMatrixf(mViewMatrix.data);
	#endif

	switch(state.type){
		case LightState::Type_DIRECTION:{
			#if defined(TOADLET_FIXED_POINT) && defined(TOADLET_HAS_GLES)
				glLightxv(l,GL_POSITION,lightDirArray(cacheArray,state.direction));
				glLightx(l,GL_SPOT_CUTOFF,Math::ONE*180);
				glLightx(l,GL_SPOT_EXPONENT,0);

			#else
				glLightfv(l,GL_POSITION,lightDirArray(cacheArray,state.direction));
				glLightf(l,GL_SPOT_CUTOFF,180);
				glLightf(l,GL_SPOT_EXPONENT,0);
			#endif
			break;
		}
		case LightState::Type_POINT:{
			#if defined(TOADLET_FIXED_POINT) && defined(TOADLET_HAS_GLES)
				glLightxv(l,GL_POSITION,lightPosArray(cacheArray,state.position));
				glLightx(l,GL_SPOT_CUTOFF,Math::ONE*180);
				glLightx(l,GL_SPOT_EXPONENT,0);
			#else
				glLightfv(l,GL_POSITION,lightPosArray(cacheArray,state.position));
				glLightf(l,GL_SPOT_CUTOFF,180);
				glLightf(l,GL_SPOT_EXPONENT,0);
			#endif
			break;
		}
		case LightState::Type_SPOT:{
			#if defined(TOADLET_FIXED_POINT) && defined(TOADLET_HAS_GLES)
				glLightxv(l,GL_POSITION,lightPosArray(cacheArray,state.position));
				glLightxv(l,GL_SPOT_DIRECTION,lightPosArray(cacheArray,state.direction));
				glLightx(l,GL_SPOT_CUTOFF,Math::radToDeg(state.spotOuterRadius)/2));
				glLightx(l,GL_SPOT_EXPONENT,state.spotExponent);
			#else
				glLightfv(l,GL_POSITION,lightPosArray(cacheArray,state.position));
				glLightfv(l,GL_SPOT_DIRECTION,lightPosArray(cacheArray,state.direction));
				glLightf(l,GL_SPOT_CUTOFF,MathConversion::scalarToFloat(Math::radToDeg(state.spotOuterRadius)/2.0));
				glLightf(l,GL_SPOT_EXPONENT,MathConversion::scalarToFloat(state.spotFalloff));
			#endif
			break;
		}
	}

	glPopMatrix();

	#if defined(TOADLET_FIXED_POINT)
		#if defined(TOADLET_HAS_GLES)
			glLightx(l,GL_CONSTANT_ATTENUATION,light.constantAttenuation);
			glLightx(l,GL_LINEAR_ATTENUATION,light.linearAttenuation);
			glLightx(l,GL_QUADRATIC_ATTENUATION,light.quadraticAttenuation);

			glLightxv(l,GL_SPECULAR,light.specularColor.getData());
			glLightxv(l,GL_DIFFUSE,light.diffuseColor.getData());
			// Ambient lighting works through the GL_LIGHT_MODEL_AMBIENT
			glLightxv(l,GL_AMBIENT,Math::ZERO_VECTOR4.getData());
		#else
			glLightf(l,GL_CONSTANT_ATTENUATION,MathConversion::scalarToFloat(state.constantAttenuation));
			glLightf(l,GL_LINEAR_ATTENUATION,MathConversion::scalarToFloat(state.linearAttenuation));
			glLightf(l,GL_QUADRATIC_ATTENUATION,MathConversion::scalarToFloat(state.quadraticAttenuation));

			glLightfv(l,GL_SPECULAR,colorArray(cacheArray,state.specularColor));
			glLightfv(l,GL_DIFFUSE,colorArray(cacheArray,state.diffuseColor));
			// Ambient lighting works through the GL_LIGHT_MODEL_AMBIENT
			glLightfv(l,GL_AMBIENT,colorArray(cacheArray,Math::ZERO_VECTOR4));
		#endif
	#else
		glLightf(l,GL_CONSTANT_ATTENUATION,state.constantAttenuation);
		glLightf(l,GL_LINEAR_ATTENUATION,state.linearAttenuation);
		glLightf(l,GL_QUADRATIC_ATTENUATION,state.quadraticAttenuation);

		glLightfv(l,GL_SPECULAR,state.specularColor.getData());
		glLightfv(l,GL_DIFFUSE,state.diffuseColor.getData());
		// Ambient lighting works through the GL_LIGHT_MODEL_AMBIENT
		glLightfv(l,GL_AMBIENT,Math::ZERO_VECTOR4.getData());
	#endif

	TOADLET_CHECK_GLERROR("setLight");
}

void GLRenderer::setLightEnabled(int i,bool enable){
	if(enable){
		glEnable(GL_LIGHT0+i);
	}
	else{
		glDisable(GL_LIGHT0+i);
	}

	TOADLET_CHECK_GLERROR("setLightEnabled");
}

void GLRenderer::setAmbientColor(const Vector4 &ambient){
	#if defined(TOADLET_FIXED_POINT)
		#if defined(TOADLET_HAS_GLES)
			glLightModelxv(GL_LIGHT_MODEL_AMBIENT,ambient.getData());
		#else
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT,colorArray(cacheArray,ambient));
		#endif
	#else
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambient.getData());
	#endif

	TOADLET_CHECK_GLERROR("setAmbientColor");
}

void GLRenderer::getShadowBiasMatrix(const Texture *shadowTexture,Matrix4x4 &result){
	result.set( Math::HALF, 0,          0,          Math::HALF,
				0,          Math::HALF, 0,          Math::HALF,
				0,          0,          Math::HALF, Math::HALF,
				0,          0,          0,          Math::ONE);
}

int GLRenderer::getCloseTextureFormat(int textureFormat,int usage){
	if(gl_version<12){
		switch(textureFormat){
			case Texture::Format_BGR_8:
				return Texture::Format_RGB_8;
			case Texture::Format_BGRA_8:
				return Texture::Format_RGBA_8;
		}
	}

	switch(textureFormat){
		case Texture::Format_R_8:
		case Texture::Format_RG_8:
			return Texture::Format_RGB_8;
		default:
			return textureFormat;
	}
}

// Thanks to Ogre3D for this threshold
bool GLRenderer::useMapping(GLBuffer *buffer) const{
	return buffer->mDataSize>(1024 * 32);
}

int GLRenderer::setVertexData(const VertexData *vertexData,int lastSemanticBits){
	int numVertexBuffers=0;
	/// @todo: Move all the semantic bit calculation to GLVertexFormat
	// Semantic bits are a bitfield covering all the non-texture element semantics,
	//  bitwise ORed with which texture stages are enabled.
	int semanticBits=0;

	if(vertexData!=NULL){
		numVertexBuffers=vertexData->vertexBuffers.size();
	}

	int i,j,k;
	for(i=0;i<numVertexBuffers;++i){
		GLBuffer *glvertexBuffer=(GLBuffer*)vertexData->vertexBuffers[i]->getRootVertexBuffer();

		GLVertexFormat *glvertexFormat=(GLVertexFormat*)(glvertexBuffer->mVertexFormat->getRootVertexFormat());
		GLsizei vertexSize=glvertexFormat->mVertexSize;
		int numElements=glvertexFormat->mSemantics.size();

		if(glvertexBuffer->mHandle==0){
			if(mCapabilityState.hardwareVertexBuffers){
				glBindBuffer(glvertexBuffer->mTarget,0);
			}
		}
		else{
			glBindBuffer(glvertexBuffer->mTarget,glvertexBuffer->mHandle);
		}

		for(j=0;j<numElements;++j){
			int semantic=glvertexFormat->mSemantics[j];
			int index=glvertexFormat->mIndexes[j];

			/// @todo: Only use the semanticBits like this if we're not in a shader.
			/// Otherwise we will be using the attribute indexes

			switch(semantic){
				case VertexFormat::Semantic_POSITION:
					semanticBits|=(1<<semantic);
					glVertexPointer(
						glvertexFormat->mGLElementCounts[j],
						glvertexFormat->mGLDataTypes[j],
						vertexSize,
						glvertexBuffer->mElementOffsets[j]);
				break;
				case VertexFormat::Semantic_NORMAL:
					semanticBits|=(1<<semantic);
					glNormalPointer(
						glvertexFormat->mGLDataTypes[j],
						vertexSize,
						glvertexBuffer->mElementOffsets[j]);
				break;
				case VertexFormat::Semantic_COLOR:
					if(index==0){
						semanticBits|=(1<<semantic);
						glColorPointer(
							glvertexFormat->mGLElementCounts[j],
							glvertexFormat->mGLDataTypes[j],
							vertexSize,
							glvertexBuffer->mElementOffsets[j]);
					}
					// This is no longer implemented, we would have to rework the semanticBits
					//#if defined(TOADLET_HAS_GLEW)
					//	else if(GLEW_EXT_secondary_color){
	 				//		glSecondaryColorPointerEXT(
					//			glvertexFormat->mGLElementCounts[j],
					//			glvertexFormat->mGLDataTypes[j],
					//			vertexSize,
					//			glvertexBuffer->mElementOffsets[j]);
					//	}
					//#endif
				break;
				case VertexFormat::Semantic_TEX_COORD:
					for(k=0;k<mMaxTexCoordIndex;++k){
						if(mTexCoordIndexes[k]==index){
							semanticBits|=(1<<(k+VertexFormat::Semantic_TEX_COORD));
							if(mMultiTexture){
								glClientActiveTexture(GL_TEXTURE0+k);
							}
							glTexCoordPointer(
								glvertexFormat->mGLElementCounts[j],
								glvertexFormat->mGLDataTypes[j],
								vertexSize,
								glvertexBuffer->mElementOffsets[j]);
							mLastTexCoordIndexes[k]=index;
						}
					}
				break;
			}
		}
	}

	if(semanticBits!=lastSemanticBits){
		// Go through all the non-texture VertexFormat types, check to see if the enabling state between now and last were different.
		//  If so check to see if the state needs to be enabled or disabled.
		int state=0;
		int sb=(semanticBits&VertexFormat::Semantic_MASK_NON_TEX_COORD),lsb=(lastSemanticBits&VertexFormat::Semantic_MASK_NON_TEX_COORD);
		while(sb>0 || lsb>0){
			if((sb&1)!=(lsb&1)){
				if((sb&1)>(lsb&1)){
					glEnableClientState(GLClientStates[state]);
				}
				else{
					glDisableClientState(GLClientStates[state]);
				}
			}
			sb>>=1;
			lsb>>=1;
			state++;
		}

		// Go through all used texture stages, and see if the enabling state betwen now and last were different.
		//  If so check to see if the state needs to be enabled or disabled.
		sb=semanticBits>>VertexFormat::Semantic_TEX_COORD;
		lsb=lastSemanticBits>>VertexFormat::Semantic_TEX_COORD;
		int stci; // Shifted Tex Coord Indexes
		for(i=0;((sb|lsb)>>i)>0;++i){
			stci=(1<<i);
			if((sb&stci)!=(lsb&stci)){
				if(mMultiTexture){
					glClientActiveTexture(GL_TEXTURE0+i);
				}
				if((sb&stci)>(lsb&stci)){
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				}
				else{
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				}
			}
		}
	}

	if((semanticBits&(1<<VertexFormat::Semantic_NORMAL))==0){
		glNormal3f(0,0,0);
	}
	if((semanticBits&(1<<VertexFormat::Semantic_COLOR))==0){
		glColor4f(1.0f,1.0f,1.0f,1.0f);
	}
	// No longer implemented, see the SPECULAR stuff above
	//#if defined(TOADLET_HAS_GLEW)
	//	if((semanticBits&(1<<VertexFormat::Semantic_COLOR_SPECULAR))==0 && GLEW_EXT_secondary_color){
	//		glSecondaryColor3fEXT(1.0f,1.0f,1.0f);
	//	}
	//#endif

	TOADLET_CHECK_GLERROR("setVertexData");

	return semanticBits;
}

GLenum GLRenderer::getGLDepthFunc(DepthState::DepthTest test){
	switch(test){
		case DepthState::DepthTest_NEVER:
			return GL_NEVER;
		case DepthState::DepthTest_LESS:
			return GL_LESS;
		case DepthState::DepthTest_EQUAL:
			return GL_EQUAL;
		case DepthState::DepthTest_LEQUAL:
			return GL_LEQUAL;
		case DepthState::DepthTest_GREATER:
			return GL_GREATER;
		case DepthState::DepthTest_NOTEQUAL:
			return GL_NOTEQUAL;
		case DepthState::DepthTest_GEQUAL:
			return GL_GEQUAL;
		case DepthState::DepthTest_ALWAYS:
			return GL_ALWAYS;
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"getGLDepthFunc: Invalid depth test");
			return 0;
	}
}

GLenum GLRenderer::getGLAlphaFunc(AlphaTest test){
	switch(test){
		case AlphaTest_LESS:
			return GL_LESS;
		case AlphaTest_EQUAL:
			return GL_EQUAL;
		case AlphaTest_LEQUAL:
			return GL_LEQUAL;
		case AlphaTest_GREATER:
			return GL_GREATER;
		case AlphaTest_NOTEQUAL:
			return GL_NOTEQUAL;
		case AlphaTest_GEQUAL:
			return GL_GEQUAL;
		case AlphaTest_ALWAYS:
			return GL_ALWAYS;
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"getGLAlphaFunc: Invalid alpha test");
			return 0;
	}
}

GLenum GLRenderer::getGLBlendOperation(BlendState::Operation operation){
	switch(operation){
		case BlendState::Operation_ONE:
			return GL_ONE;
		case BlendState::Operation_ZERO:
			return GL_ZERO;
		case BlendState::Operation_DEST_COLOR:
			return GL_DST_COLOR;
		case BlendState::Operation_SOURCE_COLOR:
			return GL_SRC_COLOR;
		case BlendState::Operation_ONE_MINUS_DEST_COLOR:
			return GL_ONE_MINUS_DST_COLOR;
		case BlendState::Operation_ONE_MINUS_SOURCE_COLOR:
			return GL_ONE_MINUS_SRC_COLOR;
		case BlendState::Operation_DEST_ALPHA:
			return GL_ONE_MINUS_DST_ALPHA;
		case BlendState::Operation_SOURCE_ALPHA:
			return GL_SRC_ALPHA;
		case BlendState::Operation_ONE_MINUS_DEST_ALPHA:
			return GL_ONE_MINUS_DST_ALPHA;
		case BlendState::Operation_ONE_MINUS_SOURCE_ALPHA:
			return GL_ONE_MINUS_SRC_ALPHA;
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"getGLBlendOperation: Invalid blend operation");
			return 0;
	}
}

GLenum GLRenderer::getGLCullFace(RasterizerState::CullType type){
	switch(type){
		case RasterizerState::CullType_FRONT:
			return GL_FRONT;
		break;
		case RasterizerState::CullType_BACK:
			return GL_BACK;
		break;
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"getGLCullFace: Invalid cull type");
			return 0;
	}
}

GLenum GLRenderer::getGLPolygonMode(RasterizerState::FillType type){
	#if !defined(TOADLET_HAS_GLES)
		switch(type){
			case RasterizerState::FillType_POINT:
				return GL_POINT;
			case RasterizerState::FillType_LINE:
				return GL_LINE;
			case RasterizerState::FillType_SOLID:
				return GL_FILL;
			default:
				Error::unknown(Categories::TOADLET_PEEPER,
					"getGLCullFace: Invalid fill type");
				return 0;
		}
	#else
		return 0;
	#endif
}

GLenum GLRenderer::getGLFogType(FogState::FogType type){
	switch(type){
		case FogState::FogType_LINEAR:
			return GL_LINEAR;
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"getGLFogType: Invalid fog type");
			return 0;
	}
}

GLenum GLRenderer::getGLShadeModel(MaterialState::ShadeType type){
	switch(type){
		case MaterialState::ShadeType_FLAT:
			return GL_FLAT;
		case MaterialState::ShadeType_GOURAUD:
		case MaterialState::ShadeType_PHONG:
			return GL_SMOOTH;
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"getGLShadeModel: Invalid shade type");
			return 0;
	}
}

GLint GLRenderer::getGLElementCount(int format){
	switch(format&~VertexFormat::Format_MASK_TYPES){ // Mask out the types
		case VertexFormat::Format_BIT_COUNT_1:
			return 1;
		case VertexFormat::Format_BIT_COUNT_2:
			return 2;
		case VertexFormat::Format_BIT_COUNT_3:
			return 3;
		case VertexFormat::Format_BIT_COUNT_4:
		case VertexFormat::Format_COLOR_RGBA:
			return 4;
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"getGLElementCount: Invalid element count");
			return 0;
	}
}

GLenum GLRenderer::getGLDataType(int format){
	switch(format&~VertexFormat::Format_MASK_COUNTS){ // Mask out the counts
		case VertexFormat::Format_BIT_UINT_8:
		case VertexFormat::Format_COLOR_RGBA:
			return GL_UNSIGNED_BYTE;
		case VertexFormat::Format_BIT_INT_8:
			return GL_BYTE;
		case VertexFormat::Format_BIT_INT_16:
			return GL_SHORT;
		case VertexFormat::Format_BIT_FLOAT_32:
			return GL_FLOAT;
		#if !defined(TOADLET_HAS_GLES)
			case VertexFormat::Format_BIT_INT_32:
				return GL_INT;
			case VertexFormat::Format_BIT_DOUBLE_64:
				return GL_DOUBLE;
		#else
			case VertexFormat::Format_BIT_FIXED_32:
				return GL_FIXED;
		#endif
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"getGLDataType: Invalid data type");
			return 0;
	}
}

GLuint GLRenderer::getGLFormat(int textureFormat,bool internal){
	if((textureFormat&Texture::Format_BIT_L)>0){
		return GL_LUMINANCE;
	}
	else if((textureFormat&Texture::Format_BIT_A)>0){
		return GL_ALPHA;
	}
	else if((textureFormat&Texture::Format_BIT_LA)>0){
		return GL_LUMINANCE_ALPHA;
	}
	else if((textureFormat&Texture::Format_BIT_BGR)>0){
		return GL_BGR;
	}
	else if((textureFormat&Texture::Format_BIT_BGRA)>0){
		if(internal==false){
			return GL_BGRA;
		}
		else{
			return GL_RGBA;
		}
	}
	else if((textureFormat&Texture::Format_BIT_RGB)>0){
		if(internal==false){
			return GL_BGR;
		}
		else{
			return GL_RGB;
		}
	}
	else if((textureFormat&Texture::Format_BIT_RGBA)>0){
		return GL_RGBA;
	}
	#if defined(TOADLET_HAS_GLEW)
		else if((textureFormat&Texture::Format_BIT_DXT1)>0){
			return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		}
		else if((textureFormat&Texture::Format_BIT_DXT2)>0 ||
				(textureFormat&Texture::Format_BIT_DXT3)>0){
			return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		}
		else if((textureFormat&Texture::Format_BIT_DXT4)>0 ||
				(textureFormat&Texture::Format_BIT_DXT5)>0){
			return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		}
	#endif

	#if !defined(TOADLET_HAS_GLES) || defined(TOADLET_HAS_EAGL)
		else if((textureFormat&Texture::Format_BIT_DEPTH)>0){
			if(internal && (textureFormat&Texture::Format_BIT_UINT_16)>0){
				return GL_DEPTH_COMPONENT16;
			}
			else if(internal && (textureFormat&Texture::Format_BIT_UINT_24)>0){
				return GL_DEPTH_COMPONENT24;
			}
			#if !defined(TOADLET_HAS_EAGL)
				else if(internal && (textureFormat&Texture::Format_BIT_UINT_32)>0){
					return GL_DEPTH_COMPONENT32;
				}
				else{
					return GL_DEPTH_COMPONENT;
				}
			#endif
		}
	#endif

	return 0;
}

GLuint GLRenderer::getGLType(int textureFormat){
	if((textureFormat&Texture::Format_BIT_UINT_8)>0 || (textureFormat&Texture::Format_BIT_UINT_16)>0 || (textureFormat&Texture::Format_BIT_UINT_24)>0 || (textureFormat&Texture::Format_BIT_UINT_32)>0){
		return GL_UNSIGNED_BYTE;
	}
	else if((textureFormat&Texture::Format_BIT_FLOAT_32)>0){
		return GL_FLOAT;
	}
	else if((textureFormat&Texture::Format_BIT_UINT_5_6_5)>0){
		return GL_UNSIGNED_SHORT_5_6_5;
	}
	else if((textureFormat&Texture::Format_BIT_UINT_5_5_5_1)>0){
		return GL_UNSIGNED_SHORT_5_5_5_1;
	}
	else if((textureFormat&Texture::Format_BIT_UINT_4_4_4_4)>0){
		return GL_UNSIGNED_SHORT_4_4_4_4;
	}
	else{
		return 0;
	}
}

GLuint GLRenderer::getGLIndexType(int indexFormat){
	switch(indexFormat){
		case IndexBuffer::IndexFormat_UINT8:
			return GL_UNSIGNED_BYTE;
		break;
		case IndexBuffer::IndexFormat_UINT16:
			return GL_UNSIGNED_SHORT;
		break;
		#if !defined(TOADLET_HAS_GLES)
			case IndexBuffer::IndexFormat_UINT32:
				return GL_UNSIGNED_INT;
			break;
		#endif
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"getGLIndexType: Invalid type");
			return 0;
		break;
	}
}

GLuint GLRenderer::getGLWrap(TextureStage::AddressMode addressMode,bool hasClampToEdge){
	switch(addressMode){
		case TextureStage::AddressMode_REPEAT:
			return GL_REPEAT;
		#if !defined(TOADLET_HAS_GLES)
			case TextureStage::AddressMode_CLAMP_TO_BORDER:
				return GL_CLAMP_TO_BORDER;
			case TextureStage::AddressMode_MIRRORED_REPEAT:
				return GL_MIRRORED_REPEAT;
			case TextureStage::AddressMode_CLAMP_TO_EDGE:
				return hasClampToEdge?GL_CLAMP_TO_EDGE:GL_CLAMP;
		#else
			case TextureStage::AddressMode_CLAMP_TO_EDGE:
				return GL_CLAMP_TO_EDGE;
		#endif
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"getGLWrap: Invalid address mode");
			return 0;
	}
}

GLuint GLRenderer::getGLMinFilter(TextureStage::Filter minFilter,TextureStage::Filter mipFilter){
	switch(mipFilter){
		case TextureStage::Filter_NONE:
			switch(minFilter){
				case TextureStage::Filter_NEAREST:
					return GL_NEAREST;
				case TextureStage::Filter_LINEAR:
					return GL_LINEAR;
				default:
				break;
			}
		break;
		case TextureStage::Filter_NEAREST:
			switch(minFilter){
				case TextureStage::Filter_NEAREST:
					return GL_NEAREST_MIPMAP_NEAREST;
				case TextureStage::Filter_LINEAR:
					return GL_NEAREST_MIPMAP_LINEAR;
				default:
				break;
			}
		break;
		case TextureStage::Filter_LINEAR:
			switch(minFilter){
				case TextureStage::Filter_NEAREST:
					return GL_LINEAR_MIPMAP_NEAREST;
				case TextureStage::Filter_LINEAR:
					return GL_LINEAR_MIPMAP_LINEAR;
				default:
				break;
			}
		break;
	}

	Error::unknown(Categories::TOADLET_PEEPER,
		"getGLMinFilter: Invalid filter");
	return 0;
}

GLuint GLRenderer::getGLMagFilter(TextureStage::Filter magFilter){
	switch(magFilter){
		case TextureStage::Filter_NEAREST:
			return GL_NEAREST;
		case TextureStage::Filter_LINEAR:
			return GL_LINEAR;
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"getGLMagFilter: Invalid filter");
			return 0;
	}
}

GLuint GLRenderer::getGLTextureBlendSource(TextureBlend::Source source){
	switch(source){
		case TextureBlend::Source_PREVIOUS:
			return GL_PREVIOUS;
		case TextureBlend::Source_TEXTURE:
			return GL_TEXTURE;
		case TextureBlend::Source_PRIMARY_COLOR:
			return GL_PRIMARY_COLOR;
		case TextureBlend::Source_CONSTANT_COLOR:
			return GL_CONSTANT;
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"getGLTextureBlendSource: Invalid source");
			return 0;
	}
}

GLuint GLRenderer::getGLTextureBlendOperation(TextureBlend::Operation operation){
	switch(operation){
		case TextureBlend::Operation_REPLACE:
			return GL_REPLACE;
		case TextureBlend::Operation_MODULATE:
		case TextureBlend::Operation_MODULATE_2X:
		case TextureBlend::Operation_MODULATE_4X:
			return GL_MODULATE;
		case TextureBlend::Operation_ADD:
			return GL_ADD;
		case TextureBlend::Operation_DOTPRODUCT:
			return GL_DOT3_RGB;
		case TextureBlend::Operation_ALPHABLEND:
			return GL_INTERPOLATE;
		default:
			return 0;
	}
}

GLuint GLRenderer::getGLDepthTextureMode(TextureStage::ShadowComparison comparison){
	switch(comparison){
		case TextureStage::ShadowComparison_L:
			return GL_LUMINANCE;
		case TextureStage::ShadowComparison_A:
			return GL_INTENSITY;
		default:
			return 0;
	}
}

float GLRenderer::getGLTextureBlendScale(TextureBlend::Operation operation){
	switch(operation){
		case TextureBlend::Operation_MODULATE:
			return 1.0f;
		case TextureBlend::Operation_MODULATE_2X:
			return 2.0f;
		case TextureBlend::Operation_MODULATE_4X:
			return 4.0f;
		default:
			return 1.0f;
	}
}

GLuint GLRenderer::GLClientStates[6]={
	GL_VERTEX_ARRAY,
	0, /// @todo: Blend Weights
	0, /// @todo: Blend Indices
	GL_NORMAL_ARRAY,
	GL_COLOR_ARRAY,
	#if defined(TOADLET_HAS_GLEW)
		GL_SECONDARY_COLOR_ARRAY,
	#else
		0,
	#endif
};

#if !defined(TOADLET_HAS_GLES)
	GLuint GLRenderer::GLCubeFaces[6]={
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};
#endif

}
}
