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

#include "GLRenderer.h"
#include "GLTexture.h"
#include "GLBuffer.h"
#include "GLRenderTarget.h"
#if defined(TOADLET_HAS_GLFBOS)
	#include "GLFBOSurfaceRenderTarget.h"
#endif
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/peeper/IndexData.h>
#include <toadlet/peeper/IndexBuffer.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/peeper/VertexBuffer.h>
#include <toadlet/peeper/Viewport.h>
#include <toadlet/peeper/Program.h>
#include <toadlet/peeper/Shader.h>
#include <toadlet/peeper/Light.h>

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
	extern bool GLPBufferSurfaceRenderTarget_available(GLRenderer *renderer);
	extern SurfaceRenderTarget *new_GLPBufferSurfaceRenderTarget(GLRenderer *renderer);
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

	mDepthWrite(false),
	mFaceCulling(FaceCulling_NONE),
	mInTexGen(false),
	mMirrorY(false),
	mMaxTexCoordIndex(0),
	//mTexCoordIndexes,
	//mLastTexTargets,
	//mLastVertexData,
	mLastTypeBits(0),
	//mLastTexCoordIndexes,

	//mModelMatrix,
	//mViewMatrix,

	//mStatisticsSet,
	//mCapabilitySet,

	mPrimaryRenderTarget(NULL),
	mRenderTarget(NULL)

	#if defined(TOADLET_DEBUG)
		,mBeginEndCounter(0)
	#endif
{}

GLRenderer::~GLRenderer(){
	destroy();
}

bool GLRenderer::create(RenderTarget *target,int *options){
	Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ALERT,
		"creating GLRenderer");

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
					Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ALERT,
						String("Setting PBuffer usage to:")+usePBuffers);
				break;
				case Option_USE_FBOS:
					useFBOs=options[i++]>0;
					Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ALERT,
						String("Setting FBO usage to:")+useFBOs);
				break;
				case Option_USE_HARDWARE_BUFFERS:
					useHardwareBuffers=options[i++]>0;
					Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ALERT,
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
		Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ALERT,
			String("glesem_getAccelerated result:")+glesemAcceleratedResult);

		gl_version=glesem_gl_majorVersion*10+glesem_gl_minorVersion;
	#elif defined(TOADLET_HAS_GLEW)
		glewExperimental=true;
		GLenum glewInitResult=glewInit();
		if(glewInitResult!=GLEW_OK){
			Error::unknown(Categories::TOADLET_PEEPER,
				"glewInit failed");
			return false;
		}

		gl_version=GLEW_VERSION_2_1?21:(GLEW_VERSION_2_0?20:(GLEW_VERSION_1_5?15:(GLEW_VERSION_1_4?14:(GLEW_VERSION_1_3?13:(GLEW_VERSION_1_2?12:(GLEW_VERSION_1_1?11:00))))));
	#endif
	Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ALERT,
		String("CALCULATED GL VERSION:")+(gl_version/10)+"."+(gl_version%10));

	GLRenderTarget *gltarget=(GLRenderTarget*)target->getRootRenderTarget();

	mPrimaryRenderTarget=target;
	mGLPrimaryRenderTarget=gltarget;
	mRenderTarget=target;
	mGLRenderTarget=gltarget;

	Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ALERT,
		String("GL_VENDOR:") + glGetString(GL_VENDOR));
	Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ALERT,
		String("GL_RENDERER:") + glGetString(GL_RENDERER));
	Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ALERT,
		String("GL_VERSION:") + glGetString(GL_VERSION));
	Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ALERT,
		String("GL_EXTENSIONS:") + glGetString(GL_EXTENSIONS));

	#if defined(TOADLET_HAS_GLESEM)
		mCapabilitySet.hardwareTextures=glesemAcceleratedResult>0;
	#else
		mCapabilitySet.hardwareTextures=true;
	#endif

	GLint maxTextureStages=0;
	glGetIntegerv(GL_MAX_TEXTURE_UNITS,&maxTextureStages);
	if(maxTextureStages<=0){
		maxTextureStages=1;
	}
	mCapabilitySet.maxTextureStages=maxTextureStages;

	mLastTextures.resize(mCapabilitySet.maxTextureStages,NULL);
	mLastTexTargets.resize(mCapabilitySet.maxTextureStages,0);
	mTexCoordIndexes.resize(mCapabilitySet.maxTextureStages,-1);
	mLastTexCoordIndexes.resize(mCapabilitySet.maxTextureStages,-1);

	GLint maxTextureSize=0;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE,&maxTextureSize);
	if(maxTextureSize<=0){
		maxTextureSize=256;
	}
	mCapabilitySet.maxTextureSize=maxTextureSize;

	GLint maxLights=0;
	glGetIntegerv(GL_MAX_LIGHTS,&maxLights);
	if(maxLights<=0){
		maxLights=8;
	}
	mCapabilitySet.maxLights=maxLights;

	#if defined(TOADLET_HAS_GLPBUFFERS)
		mPBuffersAvailable=usePBuffers && GLPBufferSurfaceRenderTarget_available(this);
	#else
		mPBuffersAvailable=false;
	#endif
	#if defined(TOADLET_HAS_GLFBOS)
		mFBOsAvailable=useFBOs && GLFBOSurfaceRenderTarget::available(this);
	#else
		mFBOsAvailable=false;
	#endif

	mCapabilitySet.renderToTexture=mPBuffersAvailable || mFBOsAvailable;

	mCapabilitySet.renderToDepthTexture=mFBOsAvailable;

	#if defined(TOADLET_HAS_GLEW)
		mCapabilitySet.textureDot3=(GLEW_ARB_texture_env_dot3!=0);
	#elif defined(TOADLET_HAS_GLES)
		mCapabilitySet.textureDot3=(gl_version>=11);
	#endif

	#if defined(TOADLET_HAS_GLEW)
		// Usefully, GL_TEXTURE_RECTANGLE_ARB == GL_TEXTURE_RECTANGLE_EXT == GL_TEXTURE_RECTANGLE_NV
		mCapabilitySet.textureNonPowerOf2Restricted=(GLEW_ARB_texture_rectangle!=0) || (GLEW_EXT_texture_rectangle!=0) || (GLEW_NV_texture_rectangle!=0);
		mCapabilitySet.renderToTextureNonPowerOf2Restricted=mFBOsAvailable && mCapabilitySet.textureNonPowerOf2Restricted;
		mCapabilitySet.textureNonPowerOf2=(GLEW_ARB_texture_non_power_of_two!=0);
	#endif

	#if defined(TOADLET_HAS_GLEW)
		mCapabilitySet.hardwareIndexBuffers=(mCapabilitySet.hardwareVertexBuffers=(useHardwareBuffers && GLEW_ARB_vertex_buffer_object));
	#elif defined(TOADLET_HAS_GLES)
		mCapabilitySet.hardwareIndexBuffers=(mCapabilitySet.hardwareVertexBuffers=(useHardwareBuffers && gl_version>=11));
	#endif

	#if defined(TOADLET_HAS_GLEW)
		mCapabilitySet.pointSprites=(GLEW_ARB_point_parameters!=0);
	#elif defined(TOADLET_HAS_GLES)
		mCapabilitySet.pointSprites=(gl_version>=11);
	#endif

	#if defined(TOADLET_HAS_GLEW)
		mCapabilitySet.vertexShaders=(GLEW_ARB_vertex_program>0);
		if(mCapabilitySet.vertexShaders) glGetProgramivARB(GL_VERTEX_PROGRAM_ARB,GL_MAX_PROGRAM_ENV_PARAMETERS_ARB,(GLint*)&mCapabilitySet.maxVertexShaderLocalParameters);
	#endif

	#if defined(TOADLET_HAS_GLEW)
		mCapabilitySet.fragmentShaders=(GLEW_ARB_fragment_program>0);
		if(mCapabilitySet.fragmentShaders) glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_MAX_PROGRAM_ENV_PARAMETERS_ARB,(GLint*)&mCapabilitySet.maxFragmentShaderLocalParameters);
	#endif

	#if defined(TOADLET_HAS_GLES) && defined(TOADLET_FIXED_POINT)
		mCapabilitySet.idealVertexFormatBit=VertexElement::Format_BIT_FIXED_32;
	#else
		mCapabilitySet.idealVertexFormatBit=VertexElement::Format_BIT_FLOAT_32;
	#endif

	setDefaultStates();

	TOADLET_CHECK_GLERROR("create");

	Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ALERT,
		"created GLRenderer");

	return true;
}

bool GLRenderer::destroy(){
	return true;
}

bool GLRenderer::reset(){
	// No device reset necessary

	setDefaultStates();

	return true;
}

// Resource operations
Texture *GLRenderer::createTexture(){
	return new GLTexture(this);
}

SurfaceRenderTarget *GLRenderer::createSurfaceRenderTarget(){
	#if defined(TOADLET_HAS_GLFBOS)
		if(mFBOsAvailable){
			return new GLFBOSurfaceRenderTarget(this);
		}
	#endif
	#if defined(TOADLET_HAS_GLPBUFFERS)
		if(mPBuffersAvailable){
			return new_GLPBufferSurfaceRenderTarget(this);
		}
	#endif
	return NULL;
}

VertexBuffer *GLRenderer::createVertexBuffer(){
	return new GLBuffer(this);
}

IndexBuffer *GLRenderer::createIndexBuffer(){
	return new GLBuffer(this);
}

Program *GLRenderer::createProgram(){
	return NULL;
}

Shader *GLRenderer::createShader(){
	return NULL;
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

	if(mMirrorY){
		#if defined(TOADLET_FIXED_POINT)
			#if defined(TOADLET_HAS_GLES)
				cacheMatrix4x4.set(matrix);
				cacheMatrix4x4.setAt(1,0,-cacheMatrix4x4.at(1,0));
				cacheMatrix4x4.setAt(1,1,-cacheMatrix4x4.at(1,1));
				cacheMatrix4x4.setAt(1,2,-cacheMatrix4x4.at(1,2));
				cacheMatrix4x4.setAt(1,3,-cacheMatrix4x4.at(1,3));

				glLoadMatrixx(cacheMatrix4x4.data);
			#else
				MathConversion::scalarToFloat(cacheMatrix4x4,matrix);
				cacheMatrix4x4.setAt(1,0,-cacheMatrix4x4.at(1,0));
				cacheMatrix4x4.setAt(1,1,-cacheMatrix4x4.at(1,1));
				cacheMatrix4x4.setAt(1,2,-cacheMatrix4x4.at(1,2));
				cacheMatrix4x4.setAt(1,3,-cacheMatrix4x4.at(1,3));

				glLoadMatrixf(cacheMatrix4x4.data);
			#endif
		#else
			cacheMatrix4x4.set(matrix);
			cacheMatrix4x4.setAt(1,0,-cacheMatrix4x4.at(1,0));
			cacheMatrix4x4.setAt(1,1,-cacheMatrix4x4.at(1,1));
			cacheMatrix4x4.setAt(1,2,-cacheMatrix4x4.at(1,2));
			cacheMatrix4x4.setAt(1,3,-cacheMatrix4x4.at(1,3));

			glLoadMatrixf(cacheMatrix4x4.data);
		#endif
	}
	else{
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
	}

	TOADLET_CHECK_GLERROR("setProjectionMatrix");
}

bool GLRenderer::setRenderTarget(RenderTarget *target){
	TOADLET_CHECK_GLERROR("before setRenderTarget");

	if(target==NULL){
		Error::nullPointer(Categories::TOADLET_PEEPER,
			"RenderTarget is NULL");
		return false;
	}

	GLRenderTarget *gltarget=(GLRenderTarget*)target->getRootRenderTarget();
	if(gltarget==NULL){
		Error::nullPointer(Categories::TOADLET_PEEPER,
			"RenderTarget is not a GLRenderTarget");
		return false;
	}

	mRenderTarget=target;
	mGLRenderTarget=gltarget;

	mGLRenderTarget->makeCurrent();

	TOADLET_CHECK_GLERROR("setRenderTarget");

	return true;
}

void GLRenderer::setViewport(const Viewport &viewport){
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

	TOADLET_CHECK_GLERROR("setViewport");
}

void GLRenderer::clear(int clearFlags,const Color &clearColor){
	int bufferBits=0;

	if((clearFlags & ClearFlag_COLOR)>0){
		bufferBits|=GL_COLOR_BUFFER_BIT;
		glClearColor(clearColor.r,clearColor.g,clearColor.b,clearColor.a);
	}
	if((clearFlags & ClearFlag_DEPTH)>0){
		bufferBits|=GL_DEPTH_BUFFER_BIT;
	}
	if((clearFlags & ClearFlag_STENCIL)>0){
		bufferBits|=GL_STENCIL_BUFFER_BIT;
	}

	if(mDepthWrite==false){
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

	mStatisticsSet.reset();

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
	if(mCapabilitySet.hardwareVertexBuffers){
		glBindBuffer(GL_ARRAY_BUFFER,0);
	}
	if(mCapabilitySet.hardwareIndexBuffers){
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	}

	int i;
	for(i=0;i<mCapabilitySet.maxTextureStages;++i){
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
			mStatisticsSet.pointCount+=indexData->count;
		break;
		case IndexData::Primitive_LINES:
			type=GL_LINES;
			mStatisticsSet.lineCount+=indexData->count/2;
		break;
		case IndexData::Primitive_LINESTRIP:
			type=GL_LINE_STRIP;
			mStatisticsSet.lineCount+=indexData->count-1;
		break;
		case IndexData::Primitive_TRIS:
			type=GL_TRIANGLES;
			mStatisticsSet.triangleCount+=indexData->count/3;
		break;
		case IndexData::Primitive_TRISTRIP:
			type=GL_TRIANGLE_STRIP;
			mStatisticsSet.triangleCount+=indexData->count-2;
		break;
		case IndexData::Primitive_TRIFAN:
			type=GL_TRIANGLE_FAN;
			mStatisticsSet.triangleCount+=indexData->count-2;
		break;
	}

	bool rebind=false;
	if(mLastVertexData.get()!=vertexData.get()){
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
		mLastTypeBits=setVertexData(vertexData,mLastTypeBits);
		mLastVertexData=vertexData;
	}

	IndexBuffer *indexBuffer=indexData->indexBuffer;
	if(indexBuffer!=NULL){
		GLenum indexType=0;
		switch(indexBuffer->getIndexFormat()){
			case IndexBuffer::IndexFormat_UINT_8:
				indexType=GL_UNSIGNED_BYTE;
			break;
			case IndexBuffer::IndexFormat_UINT_16:
				indexType=GL_UNSIGNED_SHORT;
			break;
			#if !defined(TOADLET_HAS_GLES)
				case IndexBuffer::IndexFormat_UINT_32:
					indexType=GL_UNSIGNED_INT;
				break;
			#endif
			default:
			break;
		}

		GLBuffer *glindexBuffer=(GLBuffer*)indexBuffer->getRootIndexBuffer();
		if(glindexBuffer->mHandle==0){
			if(mCapabilitySet.hardwareIndexBuffers){
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

void GLRenderer::setDefaultStates(){
	// General states
	mInTexGen=false;

	setAlphaTest(AlphaTest_NONE,Math::HALF);
	setBlend(Blend::Combination_DISABLED);
	setDepthWrite(true);
	setDepthTest(DepthTest_LEQUAL);
	setDithering(false);
	setFaceCulling(FaceCulling_BACK);
	setFogParameters(Fog_NONE,0,Math::ONE,Colors::BLACK);
	setLighting(false);
	setShading(Shading_SMOOTH);
	setNormalize(Normalize_RESCALE);
	#if defined(TOADLET_HAS_GLES)
		setTexturePerspective(true);
	#endif

	int i;
	for(i=0;i<mCapabilitySet.maxTextureStages;++i){
		mLastTexTargets[i]=GL_TEXTURE_2D;
		setTextureStage(i,NULL);
	}

	setLightEffect(LightEffect());
	setAmbientColor(Colors::WHITE);
	// We leave the current lights enabled because the SceneManager does not re-set the lights between layers

	// GL specific states
	{
		// Move specular to separate color
		#if !defined(TOADLET_HAS_GLES) && defined(TOADLET_HAS_GL_11)
		if(gl_version>=11){
			glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL,GL_SEPARATE_SPECULAR_COLOR);
		}
		#endif

		// TODO: Change the texture loading to agree with this?
		glPixelStorei(GL_PACK_ALIGNMENT,1);
		glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	}

	TOADLET_CHECK_GLERROR("setDefaultStates");
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

void GLRenderer::setBlend(const Blend &blend){
	if(blend.equals(Blend::Combination_DISABLED)){
		glDisable(GL_BLEND);
	}
	else{
		int src=getGLBlendOperation(blend.source);
		int dest=getGLBlendOperation(blend.dest);

		glBlendFunc(src,dest);
		glEnable(GL_BLEND);
	}

	TOADLET_CHECK_GLERROR("setBlend");
}

void GLRenderer::setDepthTest(const DepthTest &depthTest){
	if(depthTest==DepthTest_NONE){
		glDisable(GL_DEPTH_TEST);
	}
	else{
		glDepthFunc(getGLDepthFunc(depthTest));
		glEnable(GL_DEPTH_TEST);
	}

	TOADLET_CHECK_GLERROR("setDepthTest");
}

void GLRenderer::setDepthWrite(bool depthWrite){
	glDepthMask(depthWrite?GL_TRUE:GL_FALSE);

	mDepthWrite=depthWrite;

	TOADLET_CHECK_GLERROR("setDepthWrite");
}

void GLRenderer::setDithering(bool dithering){
	if(dithering){
		glEnable(GL_DITHER);
	}
	else{
		glDisable(GL_DITHER);
	}

	TOADLET_CHECK_GLERROR("setDithering");
}

void GLRenderer::setFaceCulling(const FaceCulling &faceCulling){
	if(faceCulling==FaceCulling_NONE){
		glDisable(GL_CULL_FACE);
	}
	else{
		switch(faceCulling){
			case FaceCulling_FRONT:
				glCullFace(mMirrorY?GL_BACK:GL_FRONT);
			break;
			case FaceCulling_BACK:
				glCullFace(mMirrorY?GL_FRONT:GL_BACK);
			break;
			default:
			break;
		}

		glEnable(GL_CULL_FACE);
	}

	TOADLET_CHECK_GLERROR("setFaceCulling");
}

void GLRenderer::setFogParameters(const Fog &fog,scalar nearDistance,scalar farDistance,const Color &color){
	if(fog==Fog_NONE){
		glDisable(GL_FOG);
	}
	else{
		glEnable(GL_FOG);
		#if defined(TOADLET_FIXED_POINT)
			#if defined(TOADLET_HAS_GLES)
				glFogx(GL_FOG_START,nearDistance);
				glFogx(GL_FOG_END,farDistance);
				glFogxv(GL_FOG_COLOR,color.getData());
			#else
				glFogf(GL_FOG_START,MathConversion::scalarToFloat(nearDistance));
				glFogf(GL_FOG_END,MathConversion::scalarToFloat(farDistance));
				glFogfv(GL_FOG_COLOR,colorArray(cacheArray,color));
			#endif
		#else
			glFogf(GL_FOG_START,nearDistance);
			glFogf(GL_FOG_END,farDistance);
			glFogfv(GL_FOG_COLOR,color.getData());
		#endif
	}

	TOADLET_CHECK_GLERROR("setFogParameters");
}

void GLRenderer::setLightEffect(const LightEffect &lightEffect){
	// The GL_COLOR_MATERIAL command must come before the actual Material settings
	if(lightEffect.trackColor){
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
			glMaterialxv(GL_FRONT_AND_BACK,GL_AMBIENT,lightEffect.ambient.getData());
			glMaterialxv(GL_FRONT_AND_BACK,GL_DIFFUSE,lightEffect.diffuse.getData());
			glMaterialxv(GL_FRONT_AND_BACK,GL_SPECULAR,lightEffect.specular.getData());
			glMaterialx(GL_FRONT_AND_BACK,GL_SHININESS,lightEffect.shininess);
			glMaterialxv(GL_FRONT_AND_BACK,GL_EMISSION,lightEffect.emissive.getData());
		#else
			glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,colorArray(cacheArray,lightEffect.ambient));
			glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,colorArray(cacheArray,lightEffect.diffuse));
			glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,colorArray(cacheArray,lightEffect.specular));
			glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,MathConversion::scalarToFloat(lightEffect.shininess));
			glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,colorArray(cacheArray,lightEffect.emissive));
		#endif
	#else
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,lightEffect.ambient.getData());
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,lightEffect.diffuse.getData());
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,lightEffect.specular.getData());
		glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,lightEffect.shininess);
		glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,lightEffect.emissive.getData());
	#endif

	TOADLET_CHECK_GLERROR("setLightEffect");
}

void GLRenderer::setFill(const Fill &fill){
	#if defined(TOADLET_HAS_GLES)
		Error::unimplemented(Categories::TOADLET_PEEPER,
			"GLRenderer::setFill: unimplemented");
	#else
		if(fill==Fill_POINT){
			glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
		}
		else if(fill==Fill_LINE){
			glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		}
		else{
			glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		}

		TOADLET_CHECK_GLERROR("setFill");
	#endif
}

void GLRenderer::setLighting(bool lighting){
	if(lighting){
		glEnable(GL_LIGHTING);
	}
	else{
		glDisable(GL_LIGHTING);
	}

	TOADLET_CHECK_GLERROR("setLighting");
}

void GLRenderer::setShading(const Shading &shading){
	switch(shading){
		case Shading_FLAT:
			glShadeModel(GL_FLAT);
		break;
		case Shading_SMOOTH:
			glShadeModel(GL_SMOOTH);
		break;
	}

	TOADLET_CHECK_GLERROR("setShading");
}

void GLRenderer::setColorWrite(bool color){
	if(color){
		glColorMask(true,true,true,true);
	}
	else{
		glColorMask(false,false,false,false);
	}

	TOADLET_CHECK_GLERROR("setColorWrite");
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

void GLRenderer::setDepthBias(scalar constant,scalar slope){
	// TODO: We may need to scale these to be more in tune with the Direct3D ones

	if(constant!=0 && slope!=0){
		#if defined(TOADLET_FIXED_POINT) && defined(TOADLET_HAS_GLES)
			glPolygonOffsetx(slope,constant);
		#else
			glPolygonOffset(MathConversion::scalarToFloat(slope),MathConversion::scalarToFloat(constant));
		#endif
		glEnable(GL_POLYGON_OFFSET_FILL);
	}
	else{
		glDisable(GL_POLYGON_OFFSET_FILL);
	}

	TOADLET_CHECK_GLERROR("setDepthBias");
}

void GLRenderer::setTexturePerspective(bool texturePerspective){
	#if !defined(TOADLET_HAS_GLES)
		Error::unimplemented(Categories::TOADLET_PEEPER,
			"GLRenderer::setTexturePerspective: unimplemented");
	#else
		glHint(GL_PERSPECTIVE_CORRECTION_HINT,texturePerspective?GL_NICEST:GL_FASTEST);

		TOADLET_CHECK_GLERROR("setTexturePerspective");
	#endif
}

void GLRenderer::setPointParameters(bool sprite,scalar size,bool attenuated,scalar constant,scalar linear,scalar quadratic,scalar minSize,scalar maxSize){
	// pointsize = size / sqrt(constant + linear*d + quadratic*d*d)
	// if a&b = 0, then quadratic = 1/(C*C) where C = first component of projMatrix * 1/2 screen width
	if(mCapabilitySet.pointSprites){
		int value;
		if(sprite){
			glEnable(GL_POINT_SPRITE);
			value=1;
		}
		else{
			glDisable(GL_POINT_SPRITE);
			value=0;
		}
		int stage;
		for(stage=0;stage<mCapabilitySet.maxTextureStages;++stage){
			glActiveTexture(GL_TEXTURE0+stage);
			glTexEnvi(GL_POINT_SPRITE,GL_COORD_REPLACE,value);
		}
	}
	
	glPointSize(MathConversion::scalarToFloat(size));
	
	// Attenuation is unimplemented on android
	#if !defined(TOADLET_PLATFORM_ANDROID)
		if(attenuated){
			#if defined(TOADLET_FIXED_POINT)
				#if defined(TOADLET_HAS_GLES)
					cacheArray[0]=constant; cacheArray[1]=linear; cacheArray[2]=quadratic;
					glPointParameterxv(GL_POINT_DISTANCE_ATTENUATION,cacheArray);
				#else
					cacheArray[0]=MathConversion::scalarToFloat(constant); cacheArray[1]=MathConversion::scalarToFloat(linear); cacheArray[2]=MathConversion::scalarToFloat(quadratic);
					glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION,cacheArray);
				#endif
			#else
				cacheArray[0]=constant; cacheArray[1]=linear; cacheArray[2]=quadratic;
				glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION,cacheArray);
			#endif
		}
	#endif

	TOADLET_CHECK_GLERROR("setPointParameters");
}

void GLRenderer::setTextureStage(int stage,TextureStage *textureStage){
	mStatisticsSet.textureChangeCount++;

	glActiveTexture(GL_TEXTURE0+stage);

	if(textureStage!=NULL){
		Texture *texture=textureStage->texture;
		GLuint textureTarget=0;
		if(texture!=NULL){
			GLTexture *gltexture=(GLTexture*)texture->getRootTexture();

			textureTarget=gltexture->mTarget;

			if(mLastTextures[stage]!=texture){
				glBindTexture(textureTarget,gltexture->mHandle);
				mLastTextures[stage]=texture;
			}

			TextureStage::Calculation calculation=textureStage->calculation;
			if(calculation!=TextureStage::Calculation_DISABLED || (gltexture->mUsageFlags&(Texture::UsageFlags_NPOT_RESTRICTED|Texture::UsageFlags_RENDERTARGET))>0){
				if(mMatrixMode!=GL_TEXTURE){
					mMatrixMode=GL_TEXTURE;
					glMatrixMode(mMatrixMode);
				}

				const Matrix4x4 &matrix=textureStage->matrix;

				if(calculation==TextureStage::Calculation_NORMAL){
					#if defined(TOADLET_FIXED_POINT)
						#if defined(TOADLET_HAS_GLES)
							glLoadMatrixx(matrix.data);
						#else
							glLoadMatrixf(MathConversion::scalarToFloat(cacheMatrix4x4,matrix).data);
						#endif
					#else
						glLoadMatrixf(matrix.data);
					#endif
				}
				else{
					glLoadIdentity();
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

				#if !defined(TOADLET_HAS_GLES)
					if(calculation>TextureStage::Calculation_NORMAL){
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

						mInTexGen=true;
					}
				#endif
			}
			else{
				if(mMatrixMode!=GL_TEXTURE){
					mMatrixMode=GL_TEXTURE;
					glMatrixMode(mMatrixMode);
				}
				glLoadIdentity();
			}

			#if !defined(TOADLET_HAS_GLES)
				if(mInTexGen && calculation<=TextureStage::Calculation_NORMAL){
					mInTexGen=false;

					glDisable(GL_TEXTURE_GEN_S);
					glDisable(GL_TEXTURE_GEN_T);
					glDisable(GL_TEXTURE_GEN_R);
					glDisable(GL_TEXTURE_GEN_Q);
				}
			#endif
		}
		else{
			textureTarget=GL_TEXTURE_2D;
			if(mLastTextures[stage]!=texture){
				glBindTexture(textureTarget,0);
				mLastTextures[stage]=texture;
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

		const TextureBlend &blend=textureStage->blend;
		if(blend.operation!=TextureBlend::Operation_UNSPECIFIED){
			int mode=0;
			switch(blend.operation){
				case TextureBlend::Operation_REPLACE:
					mode=GL_REPLACE;
				break;
				case TextureBlend::Operation_MODULATE:
					mode=GL_MODULATE;
				break;
				case TextureBlend::Operation_ADD:
					mode=GL_ADD;
				break;
				case TextureBlend::Operation_DOTPRODUCT:
					mode=GL_DOT3_RGB;
				break;
				default:
				break;
			}
			
			if(blend.source1==TextureBlend::Source_UNSPECIFIED || blend.source2==TextureBlend::Source_UNSPECIFIED){
				glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode);
			}
			else{
				glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
				glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,mode);
				glTexEnvi(GL_TEXTURE_ENV,GL_SRC0_RGB,GLTexture::getGLTextureBlendSource(blend.source1));
				glTexEnvi(GL_TEXTURE_ENV,GL_SRC1_RGB,GLTexture::getGLTextureBlendSource(blend.source2));
				glTexEnvi(GL_TEXTURE_ENV,GL_SRC2_RGB,GL_CONSTANT);
			}
		}
		else{
			glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
		}

		bool hasClampToEdge=
			#if defined(TOADLET_HAS_GLES)
				true;
			#else
				gl_version>=12;
			#endif
		glTexParameteri(textureTarget,GL_TEXTURE_WRAP_S,GLTexture::getGLWrap(textureStage->sAddressMode,hasClampToEdge));
		glTexParameteri(textureTarget,GL_TEXTURE_WRAP_T,GLTexture::getGLWrap(textureStage->tAddressMode,hasClampToEdge));
		#if !defined(TOADLET_HAS_GLES) && defined(TOADLET_HAS_GL_12)
		if(gl_version>=12){
			glTexParameteri(textureTarget,GL_TEXTURE_WRAP_R,GLTexture::getGLWrap(textureStage->rAddressMode,hasClampToEdge));
		}
		#endif

		glTexParameteri(textureTarget,GL_TEXTURE_MIN_FILTER,GLTexture::getGLMinFilter(textureStage->minFilter,textureStage->mipFilter));
		glTexParameteri(textureTarget,GL_TEXTURE_MAG_FILTER,GLTexture::getGLMagFilter(textureStage->magFilter));

		#if !defined(TOADLET_HAS_GLES)
			if(gl_version>=14){
				if(texture!=NULL && (texture->getFormat()&Texture::Format_BIT_DEPTH)>0){
					// Enable shadow comparison
					glTexParameteri(textureTarget,GL_TEXTURE_COMPARE_MODE_ARB,GL_COMPARE_R_TO_TEXTURE);

					// Shadow comparison should be true (ie not in shadow) if r<=texture
					glTexParameteri(textureTarget,GL_TEXTURE_COMPARE_FUNC_ARB,GL_LEQUAL);

					// Shadow comparison should generate an INTENSITY result
					glTexParameteri(textureTarget,GL_DEPTH_TEXTURE_MODE_ARB,GL_INTENSITY);
				}
				else{
					// Only disable this TexParameter, the others are just parameters to this one
					glTexParameteri(textureTarget,GL_TEXTURE_COMPARE_MODE_ARB,GL_NONE);
				}
			}
		#endif

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

void GLRenderer::setProgram(const Program *program){
}

void GLRenderer::setShadowComparisonMethod(bool enabled){
	if(enabled){
		setAlphaTest(AlphaTest_GEQUAL,Math::HALF);
	}
	else{
		setAlphaTest(AlphaTest_NONE,Math::HALF);
	}
}

void GLRenderer::setLight(int i,Light *light){
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

	switch(light->getType()){
		case Light::Type_DIRECTION:{
			#if defined(TOADLET_FIXED_POINT) && defined(TOADLET_HAS_GLES)
				glLightxv(l,GL_POSITION,lightDirArray(cacheArray,light->direction));

				glLightx(l,GL_CONSTANT_ATTENUATION,Math::ONE);
				glLightx(l,GL_LINEAR_ATTENUATION,0);
				glLightx(l,GL_QUADRATIC_ATTENUATION,0);

				glLightx(l,GL_SPOT_CUTOFF,Math::ONE*180);
				glLightx(l,GL_SPOT_EXPONENT,0);

			#else
				glLightfv(l,GL_POSITION,lightDirArray(cacheArray,light->direction));

				glLightf(l,GL_CONSTANT_ATTENUATION,Math::ONE);
				glLightf(l,GL_LINEAR_ATTENUATION,0);
				glLightf(l,GL_QUADRATIC_ATTENUATION,0);

				glLightf(l,GL_SPOT_CUTOFF,MathConversion::scalarToFloat(Math::ONE)*180);
				glLightf(l,GL_SPOT_EXPONENT,0);
			#endif

			break;
		}
		case Light::Type_POSITION:{
			#if defined(TOADLET_FIXED_POINT) && defined(TOADLET_HAS_GLES)
				glLightxv(l,GL_POSITION,lightPosArray(cacheArray,light->position));

				glLightx(l,GL_CONSTANT_ATTENUATION,0);
				glLightx(l,GL_LINEAR_ATTENUATION,light->linearAttenuation);
				glLightx(l,GL_QUADRATIC_ATTENUATION,light->linearAttenuation);

				glLightx(l,GL_SPOT_CUTOFF,Math::ONE*180);
				glLightx(l,GL_SPOT_EXPONENT,0);
			#else
				glLightfv(l,GL_POSITION,lightPosArray(cacheArray,light->position));

				glLightf(l,GL_CONSTANT_ATTENUATION,0);
				glLightf(l,GL_LINEAR_ATTENUATION,MathConversion::scalarToFloat(light->linearAttenuation));
				glLightf(l,GL_QUADRATIC_ATTENUATION,MathConversion::scalarToFloat(light->linearAttenuation));

				glLightf(l,GL_SPOT_CUTOFF,MathConversion::scalarToFloat(Math::ONE)*180);
				glLightf(l,GL_SPOT_EXPONENT,0);
			#endif
			break;
		}
		case Light::Type_SPOT:{
			#if defined(TOADLET_FIXED_POINT) && defined(TOADLET_HAS_GLES)
				glLightxv(l,GL_POSITION,lightPosArray(cacheArray,light->position));

				glLightx(l,GL_CONSTANT_ATTENUATION,0);
				glLightx(l,GL_LINEAR_ATTENUATION,light->linearAttenuation);
				glLightx(l,GL_QUADRATIC_ATTENUATION,light->linearAttenuation);
			#else
				glLightfv(l,GL_POSITION,lightPosArray(cacheArray,light->position));

				glLightf(l,GL_CONSTANT_ATTENUATION,0);
				glLightf(l,GL_LINEAR_ATTENUATION,MathConversion::scalarToFloat(light->linearAttenuation));
				glLightf(l,GL_QUADRATIC_ATTENUATION,MathConversion::scalarToFloat(light->linearAttenuation));
			#endif

			#if defined(TOADLET_FIXED_POINT) && defined(TOADLET_HAS_GLES)
				glLightxv(l,GL_SPOT_DIRECTION,lightPosArray(cacheArray,light->direction));

				glLightx(l,GL_SPOT_CUTOFF,(Math::ONE-light->spotCutoff)*90);
				glLightx(l,GL_SPOT_EXPONENT,Math::ONE*5);
			#else
				glLightfv(l,GL_SPOT_DIRECTION,lightPosArray(cacheArray,light->direction));

				glLightf(l,GL_SPOT_CUTOFF,MathConversion::scalarToFloat(Math::ONE-light->spotCutoff)*90);
				glLightf(l,GL_SPOT_EXPONENT,MathConversion::scalarToFloat(Math::ONE)*5);
			#endif

			break;
		}
	}

	glPopMatrix();

	#if defined(TOADLET_FIXED_POINT)
		#if defined(TOADLET_HAS_GLES)
			glLightxv(l,GL_SPECULAR,light->specularColor.getData());
			glLightxv(l,GL_DIFFUSE,light->diffuseColor.getData());
			// Ambient lighting works through the GL_LIGHT_MODEL_AMBIENT
			glLightxv(l,GL_AMBIENT,Colors::BLACK.getData());
		#else
			glLightfv(l,GL_SPECULAR,colorArray(cacheArray,light->specularColor));
			glLightfv(l,GL_DIFFUSE,colorArray(cacheArray,light->diffuseColor));
			// Ambient lighting works through the GL_LIGHT_MODEL_AMBIENT
			glLightfv(l,GL_AMBIENT,colorArray(cacheArray,Colors::BLACK));
		#endif
	#else
		glLightfv(l,GL_SPECULAR,light->specularColor.getData());
		glLightfv(l,GL_DIFFUSE,light->diffuseColor.getData());
		// Ambient lighting works through the GL_LIGHT_MODEL_AMBIENT
		glLightfv(l,GL_AMBIENT,Colors::BLACK.getData());
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

void GLRenderer::setAmbientColor(const Color &ambient){
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

// Misc operations
void GLRenderer::setMirrorY(bool mirrorY){
	mMirrorY=mirrorY;

	FaceCulling faceCulling=mFaceCulling;
	mFaceCulling=FaceCulling_NONE;
	setFaceCulling(faceCulling);
}

void GLRenderer::getShadowBiasMatrix(const Texture *shadowTexture,Matrix4x4 &result){
	result.set( Math::HALF, 0,          0,          Math::HALF,
				0,          Math::HALF, 0,          Math::HALF,
				0,          0,          Math::HALF, Math::HALF,
				0,          0,          0,          Math::ONE);
}

// Thanks to Ogre3D for this threshold
bool GLRenderer::useMapping(GLBuffer *buffer) const{
	return buffer->mDataSize>(1024 * 32);
}

GLenum GLRenderer::getGLDepthFunc(DepthTest depthTest){
	switch(depthTest){
		case DepthTest_NEVER:
			return GL_NEVER;
		case DepthTest_LESS:
			return GL_LESS;
		case DepthTest_EQUAL:
			return GL_EQUAL;
		case DepthTest_LEQUAL:
			return GL_LEQUAL;
		case DepthTest_GREATER:
			return GL_GREATER;
		case DepthTest_NOTEQUAL:
			return GL_NOTEQUAL;
		case DepthTest_GEQUAL:
			return GL_GEQUAL;
		case DepthTest_ALWAYS:
			return GL_ALWAYS;
		default:
			return 0;
	}
}

GLenum GLRenderer::getGLAlphaFunc(AlphaTest alphaTest){
	switch(alphaTest){
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
			return 0;
	}
}

GLenum GLRenderer::getGLBlendOperation(Blend::Operation blend){
	switch(blend){
		case Blend::Operation_ONE:
			return GL_ONE;
		case Blend::Operation_ZERO:
			return GL_ZERO;
		case Blend::Operation_DEST_COLOR:
			return GL_DST_COLOR;
		case Blend::Operation_SOURCE_COLOR:
			return GL_SRC_COLOR;
		case Blend::Operation_ONE_MINUS_DEST_COLOR:
			return GL_ONE_MINUS_DST_COLOR;
		case Blend::Operation_ONE_MINUS_SOURCE_COLOR:
			return GL_ONE_MINUS_SRC_COLOR;
		case Blend::Operation_DEST_ALPHA:
			return GL_ONE_MINUS_DST_ALPHA;
		case Blend::Operation_SOURCE_ALPHA:
			return GL_SRC_ALPHA;
		case Blend::Operation_ONE_MINUS_DEST_ALPHA:
			return GL_ONE_MINUS_DST_ALPHA;
		case Blend::Operation_ONE_MINUS_SOURCE_ALPHA:
			return GL_ONE_MINUS_SRC_ALPHA;
		default:
			return 0;
	}
}

GLint GLRenderer::getGLElementCount(int format){
	switch(format&~VertexElement::Format_MASK_TYPES){ // Mask out the types
		case VertexElement::Format_BIT_COUNT_1:
			return 1;
		case VertexElement::Format_BIT_COUNT_2:
			return 2;
		case VertexElement::Format_BIT_COUNT_3:
			return 3;
		case VertexElement::Format_BIT_COUNT_4:
		case VertexElement::Format_COLOR_RGBA:
			return 4;
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"getGLElementCount: Invalid element count");
			return 0;
	}
}

GLenum GLRenderer::getGLDataType(int format){
	switch(format&~VertexElement::Format_MASK_COUNTS){ // Mask out the counts
		case VertexElement::Format_BIT_UINT_8:
		case VertexElement::Format_COLOR_RGBA:
			return GL_UNSIGNED_BYTE;
		case VertexElement::Format_BIT_INT_8:
			return GL_BYTE;
		case VertexElement::Format_BIT_INT_16:
			return GL_SHORT;
		case VertexElement::Format_BIT_FLOAT_32:
			return GL_FLOAT;
		#if !defined(TOADLET_HAS_GLES)
			case VertexElement::Format_BIT_INT_32:
				return GL_INT;
			case VertexElement::Format_BIT_DOUBLE_64:
				return GL_DOUBLE;
		#else
			case VertexElement::Format_BIT_FIXED_32:
				return GL_FIXED;
		#endif
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"getGLDataType: Invalid data type");
			return 0;
	}
}

GLuint GLClientStates[6]={
	GL_VERTEX_ARRAY,
	0, // Blend Weights - TODO
	0, // Blend Indices - TODO
	GL_NORMAL_ARRAY,
	GL_COLOR_ARRAY,
	#if defined(TOADLET_HAS_GLEW)
		GL_SECONDARY_COLOR_ARRAY,
	#else
		0,
	#endif
};

int GLRenderer::setVertexData(const VertexData *vertexData,int lastTypeBits){
	int numVertexBuffers=0;
	// Type bits are a bitfield covering all the non-texture element types,
	//  bitwise ORed with which texture stages are enabled.
	int typeBits=0;

	if(vertexData!=NULL){
		numVertexBuffers=vertexData->vertexBuffers.size();
	}

	int i,j,k;
	for(i=0;i<numVertexBuffers;++i){
		GLBuffer *glvertexBuffer=(GLBuffer*)vertexData->vertexBuffers[i]->getRootVertexBuffer();

		VertexFormat *vertexFormat=glvertexBuffer->mVertexFormat;
		GLsizei vertexSize=vertexFormat->vertexSize;
		int numVertexElements=vertexFormat->vertexElements.size();

		if(glvertexBuffer->mHandle==0){
			if(mCapabilitySet.hardwareVertexBuffers){
				glBindBuffer(glvertexBuffer->mTarget,0);
			}
		}
		else{
			glBindBuffer(glvertexBuffer->mTarget,glvertexBuffer->mHandle);
		}

		for(j=0;j<numVertexElements;++j){
			int type=vertexFormat->vertexElements[j].type;
			switch(type){
				case VertexElement::Type_POSITION:
					typeBits|=(1<<type);
					glVertexPointer(
						glvertexBuffer->mElementCounts[j],
						glvertexBuffer->mElementTypes[j],
						vertexSize,
						glvertexBuffer->mElementOffsets[j]);
				break;
				case VertexElement::Type_NORMAL:
					typeBits|=(1<<type);
					glNormalPointer(
						glvertexBuffer->mElementTypes[j],
						vertexSize,
						glvertexBuffer->mElementOffsets[j]);
				break;
				case VertexElement::Type_COLOR_DIFFUSE:
					typeBits|=(1<<type);
					glColorPointer(
						glvertexBuffer->mElementCounts[j],
						glvertexBuffer->mElementTypes[j],
						vertexSize,
						glvertexBuffer->mElementOffsets[j]);
				break;
				case VertexElement::Type_COLOR_SPECULAR:
					#if defined(TOADLET_HAS_GLEW)
						if(GLEW_EXT_secondary_color){
							typeBits|=(1<<type);
	 						glSecondaryColorPointerEXT(
								glvertexBuffer->mElementCounts[j],
								glvertexBuffer->mElementTypes[j],
								vertexSize,
								glvertexBuffer->mElementOffsets[j]);
						}
					#endif
				break;
				default:{
					int texCoordIndex=type-VertexElement::Type_TEX_COORD;
					for(k=0;k<mMaxTexCoordIndex;++k){
						if(mTexCoordIndexes[k]==texCoordIndex){
							typeBits|=(1<<(k+VertexElement::Type_TEX_COORD));
							glClientActiveTexture(GL_TEXTURE0+k);
							glTexCoordPointer(
								glvertexBuffer->mElementCounts[j],
								glvertexBuffer->mElementTypes[j],
								vertexSize,
								glvertexBuffer->mElementOffsets[j]);
							mLastTexCoordIndexes[k]=texCoordIndex;
						}
					}
				}break;
			}
		}
	}

	if(typeBits!=lastTypeBits){
		// Go through all the non-texture VertexElement types, check to see if the enabling state between now and last were different.
		//  If so check to see if the state needs to be enabled or disabled.
		int state=0;
		int tb=(typeBits&VertexElement::Type_MASK_NON_TEX_COORD),ltb=(lastTypeBits&VertexElement::Type_MASK_NON_TEX_COORD);
		while(tb>0 || ltb>0){
			if((tb&1)!=(ltb&1)){
				if((tb&1)>(ltb&1)){
					glEnableClientState(GLClientStates[state]);
				}
				else{
					glDisableClientState(GLClientStates[state]);
				}
			}
			tb>>=1;
			ltb>>=1;
			state++;
		}

		// Go through all used texture stages, and see if the enabling state betwen now and last were different.
		//  If so check to see if the state needs to be enabled or disabled.
		tb=typeBits>>VertexElement::Type_TEX_COORD;
		ltb=lastTypeBits>>VertexElement::Type_TEX_COORD;
		int stci; // Shifted Tex Coord Indexes
		for(i=0;i<mMaxTexCoordIndex;++i){
			stci=(1<<i);
			if((tb&stci)!=(ltb&stci)){
				glClientActiveTexture(GL_TEXTURE0+i);
				if((tb&stci)>(ltb&stci)){
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				}
				else{
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				}
			}
		}
	}

	if((typeBits&(1<<VertexElement::Type_COLOR_DIFFUSE))>0){
		glColor4f(1.0f,1.0f,1.0f,1.0f);
	}
	#if defined(TOADLET_HAS_GLEW)
		if((typeBits&(1<<VertexElement::Type_COLOR_SPECULAR))>0 && GLEW_EXT_secondary_color){
			glSecondaryColor3fEXT(1.0f,1.0f,1.0f);
		}
	#endif

	TOADLET_CHECK_GLERROR("setVertexData");

	return typeBits;
}

}
}
