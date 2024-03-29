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
#include "GLRenderDevice.h"
#include "GLRenderTarget.h"
#include "GLRenderState.h"
#include "GLTexture.h"
#include "GLSLShader.h"
#include "GLSLShaderState.h"
#include "GLSLVertexLayout.h"
#if defined(TOADLET_HAS_GLFBOS)
	#include "GLFBORenderTarget.h"
#endif
#if defined(TOADLET_HAS_GLQUERY)
	#include "GLQuery.h"
#endif
#include <toadlet/peeper/IndexData.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/peeper/Viewport.h>

#if defined(TOADLET_PLATFORM_WIN32) && !defined(TOADLET_PLATFORM_WINCE)
	#pragma comment(lib,"opengl32.lib")
	#if defined(TOADLET_LIBGLEW_NAME)
		#pragma comment(lib,TOADLET_LIBGLEW_NAME)
	#endif
#endif

namespace toadlet{
namespace peeper{

#if defined(TOADLET_HAS_GLPBUFFERS)
	extern bool GLPBufferRenderTarget_available(GLRenderDevice *renderDevice);
	extern PixelBufferRenderTarget *new_GLPBufferRenderTarget(GLRenderDevice *renderDevice);
#endif

#if defined(TOADLET_HAS_GLES)
	#if defined(TOADLET_HAS_GL_20)
		TOADLET_C_API RenderDevice* new_GLES2RenderDevice(){
			return new GLRenderDevice();
		}
	#else
		TOADLET_C_API RenderDevice* new_GLES1RenderDevice(){
			return new GLRenderDevice();
		}
	#endif
#else
	TOADLET_C_API RenderDevice* new_GLRenderDevice(){
		return new GLRenderDevice();
	}
#endif

#if defined(TOADLET_BUILD_DYNAMIC)
	TOADLET_C_API RenderDevice* new_RenderDevice(){
		return new GLRenderDevice();
	}
#endif

GLRenderDevice::GLRenderDevice():
	gl_version(0),
	mPBuffers(false),mFBOs(false),
	mVBOs(false),mIBOs(false),mPBOs(false),mUBOs(false),

	mMaxTexCoordIndex(0),
	//mTexCoordIndexes,
	mLastMaxTexCoordIndex(0),
	//mLastTexTargets,
	//mLastVertexBuffers,
	mLastFixedSemanticBits(0),mLastFixedTexCoordSemanticBits(0),
	//mLastTexCoordIndexes,
	mLastShaderSemanticBits(0),
	mLastShaderState(NULL),
	mLastActiveTexture(0),

	//mShaderStates,
	//mVertexFormats,

	//mModelMatrix,
	//mViewMatrix,

	mMultiTexture(false),
	mHasClampToEdge(false)
{}

bool GLRenderDevice::create(RenderTarget *target,int options){
	Log::alert(Categories::TOADLET_PEEPER,
		"creating "+Categories::TOADLET_PEEPER+".GLRenderDevice");

	if(target==NULL){
		Error::nullPointer(Categories::TOADLET_PEEPER,
			"NULL RenderTarget");
		return false;
	}

	bool usePBuffers=(options&Option_BIT_NOPBUFFERS)==0;
	bool useFBOs=(options&Option_BIT_NOFBOS)==0;
	bool useHardwareBuffers=(options&Option_BIT_NOHARDWAREBUFFERS)==0;
	bool useFixed=(options&Option_BIT_NOFIXED)==0;
	bool useShader=(options&Option_BIT_NOSHADER)==0;

	TOADLET_IGNORE_UNUSED_VARIABLE_WARNING(usePBuffers);
	TOADLET_IGNORE_UNUSED_VARIABLE_WARNING(useFBOs);
	TOADLET_IGNORE_UNUSED_VARIABLE_WARNING(useHardwareBuffers);
	TOADLET_IGNORE_UNUSED_VARIABLE_WARNING(useFixed);
	TOADLET_IGNORE_UNUSED_VARIABLE_WARNING(useShader);
	
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
		Log::alert(Categories::TOADLET_PEEPER,
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

		if(GLEW_VERSION_4_1)	  gl_version=41;
		else if(GLEW_VERSION_4_0) gl_version=40;
		else if(GLEW_VERSION_3_3) gl_version=33;
		else if(GLEW_VERSION_3_2) gl_version=32;
		else if(GLEW_VERSION_3_1) gl_version=31;
		else if(GLEW_VERSION_3_0) gl_version=30;
		else if(GLEW_VERSION_2_1) gl_version=21;
		else if(GLEW_VERSION_2_0) gl_version=20;
		else if(GLEW_VERSION_1_5) gl_version=15;
		else if(GLEW_VERSION_1_4) gl_version=14;
		else if(GLEW_VERSION_1_3) gl_version=13;
		else if(GLEW_VERSION_1_2) gl_version=12;
		else if(GLEW_VERSION_1_1) gl_version=11;
		else					  gl_version=10;
	#else
		const char *versionString=(const char*)glGetString(GL_VERSION);
		if(strstr(versionString,"4.")!=NULL){
			gl_version=40;
		}
		else if(strstr(versionString,"3.")!=NULL){
			gl_version=30;
		}
		else if(strstr(versionString,"2.")!=NULL){
			gl_version=20;
		}
		else if(strstr(versionString,"1.1")!=NULL){
			gl_version=11;
		}
		else{
			#if defined(TOADLET_HAS_GLFIXED)
				gl_version=10;
			#else
				gl_version=20;
			#endif
		}
	#endif

	Log::alert(Categories::TOADLET_PEEPER,
		String("CALCULATED GL VERSION:")+(gl_version/10)+"."+(gl_version%10));

	GLRenderTarget *gltarget=(GLRenderTarget*)target->getRootRenderTarget();

	mPrimaryRenderTarget=target;
	mGLPrimaryRenderTarget=gltarget;
	mRenderTarget=target;
	mGLRenderTarget=gltarget;

	if(mGLRenderTarget!=NULL){
		mGLRenderTarget->activate();
	}

	Log::alert(Categories::TOADLET_PEEPER,
		String("GL_VENDOR:") + glGetString(GL_VENDOR));
	Log::alert(Categories::TOADLET_PEEPER,
		String("GL_RENDERER:") + glGetString(GL_RENDERER));
	Log::alert(Categories::TOADLET_PEEPER,
		String("GL_VERSION:") + glGetString(GL_VERSION));
	#if defined(TOADLET_HAS_GLSL)
		Log::alert(Categories::TOADLET_PEEPER,
			String("GL_SHADING_LANGUAGE_VERSION:") + glGetString(GL_SHADING_LANGUAGE_VERSION));
	#endif
	Log::alert(Categories::TOADLET_PEEPER,
		String("GL_EXTENSIONS:") + glGetString(GL_EXTENSIONS));

	RenderCaps &caps=mCaps;
	{
		GLint maxTextureStages=0;
		if(gl_version>10){ // 1.0 doesn't support multitexturing
			#if defined(GL_MAX_TEXTURE_UNITS)
				glGetIntegerv(GL_MAX_TEXTURE_UNITS,&maxTextureStages);
			#elif defined(GL_MAX_TEXTURE_IMAGE_UNITS)
				glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS,&maxTextureStages);
			#endif
		}
		if(maxTextureStages<=0){
			maxTextureStages=1;
		}
		caps.maxTextureStages=maxTextureStages;
		mMultiTexture=maxTextureStages>1;
		mHasClampToEdge=
			#if defined(TOADLET_HAS_GLES)
				true;
			#else
				gl_version>=12;
			#endif

		mLastTextures.resize(caps.maxTextureStages,NULL);
		mLastSamplerStates.resize(caps.maxTextureStages,NULL);
		mLastTextureStates.resize(caps.maxTextureStages,NULL);
		mLastTexTargets.resize(caps.maxTextureStages,0);
		mLastTextureType.resize(caps.maxTextureStages,(Shader::ShaderType)0);
		mTexCoordIndexes.resize(caps.maxTextureStages,-1);
		mLastTexCoordIndexes.resize(caps.maxTextureStages,-1);

		#if defined(TOADLET_HAS_GLPBUFFERS)
			mPBuffers=usePBuffers && GLPBufferRenderTarget_available(this);
		#else
			mPBuffers=false;
		#endif
		#if defined(TOADLET_HAS_GLFBOS)
			mFBOs=useFBOs && GLFBORenderTarget::available(this);
		#else
			mFBOs=false;
		#endif

		#if defined(GL_ARB_vertex_buffer_object)
			mVBOs=mIBOs=(GLEW_ARB_vertex_buffer_object>0);
		#endif
		#if defined(GL_ARB_pixel_buffer_object)
			mPBOs=(GLEW_ARB_pixel_buffer_object>0);
		#endif
		#if defined(GL_ARB_uniform_buffer_object)
			mUBOs=(GLEW_ARB_uniform_buffer_object>0);
		#endif
		#if defined(TOADLET_HAS_GLES)
			mVBOs=mIBOs=(gl_version>=11);
			mPBOs=false;
			mUBOs=false;
		#endif
		mVBOs&=useHardwareBuffers;
		mIBOs&=useHardwareBuffers;
		mPBOs&=useHardwareBuffers;
		mUBOs&=useHardwareBuffers;
		#if defined(TOADLET_HAS_GLEW)
			mNPOTR=(GLEW_ARB_texture_rectangle!=0) || (GLEW_EXT_texture_rectangle!=0) || (GLEW_NV_texture_rectangle!=0);
			mNPOT=(GLEW_ARB_texture_non_power_of_two!=0);
		#else
			mNPOTR=false;
			mNPOT=false;
		#endif

		caps.textureNonPowerOf2Restricted=mNPOTR;
		caps.renderToTextureNonPowerOf2Restricted=mFBOs && mNPOTR;
		caps.textureNonPowerOf2=mNPOT;

		GLint maxTextureSize=0;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE,&maxTextureSize);
		if(maxTextureSize<=0){
			maxTextureSize=256;
		}
		caps.maxTextureSize=maxTextureSize;

		#if defined(TOADLET_HAS_GLFIXED)
			GLint maxLights=0;
			glGetIntegerv(GL_MAX_LIGHTS,&maxLights);
			if(maxLights<=0){
				maxLights=8;
			}
			caps.maxLights=maxLights;
		#endif

		caps.renderToTexture=mPBuffers || mFBOs;

		caps.renderToDepthTexture=mFBOs;

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
			caps.pointSprites=(GLEW_ARB_point_parameters!=0);
		#elif defined(TOADLET_HAS_GLES)
			caps.pointSprites=(gl_version>=11);
		#endif

		#if defined(TOADLET_HAS_GLFIXED)
			caps.hasFixed[Shader::ShaderType_VERTEX]=useFixed;
			caps.hasFixed[Shader::ShaderType_FRAGMENT]=useFixed;
			caps.hasFixed[Shader::ShaderType_GEOMETRY]=useFixed;
		#endif
		
		#if !defined(TOADLET_FIXED_POINT) && defined(TOADLET_HAS_GLSL)
			caps.hasShader[Shader::ShaderType_VERTEX]=useShader;
			caps.hasShader[Shader::ShaderType_FRAGMENT]=useShader;
			caps.hasShader[Shader::ShaderType_GEOMETRY]=useShader;
		#endif

		#if defined(TOADLET_HAS_GLES) && defined(TOADLET_FIXED_POINT)
			caps.idealVertexFormatType=VertexFormat::Format_TYPE_FIXED_32;
		#else
			caps.idealVertexFormatType=VertexFormat::Format_TYPE_FLOAT_32;
		#endif

		// OSX needs a notification to update the back buffer on a resize
		#if defined(TOADLET_PLATFORM_OSX) && !defined(TOADLET_PLATFORM_EAGL)
			mCaps.resetOnResize=true;
		#endif

		caps.triangleFan=true;
		#if !defined(TOADLET_HAS_GLES)
			caps.cubeMap=true;
		#else
			caps.texturePerspective=true;
			#if TOADLET_HAS_GL_20
				caps.cubeMap=true;
			#else
				caps.cubeMap=false;
			#endif
		#endif
	}

	mDefaultState=new GLRenderState(this);
	mDefaultState->setBlendState(BlendState());
	mDefaultState->setDepthState(DepthState());
	mDefaultState->setFogState(FogState());
	mDefaultState->setGeometryState(GeometryState());
	mDefaultState->setRasterizerState(RasterizerState());
	mDefaultState->setMaterialState(MaterialState());

	setDefaultState();

	TOADLET_CHECK_GLERROR("create");

	Log::alert(Categories::TOADLET_PEEPER,
		"created GLRenderDevice");

	return true;
}

void GLRenderDevice::destroy(){
	if(mPrimaryRenderTarget!=NULL){
		mPrimaryRenderTarget=NULL;
		mGLPrimaryRenderTarget=NULL;
		mRenderTarget=NULL;
		mGLRenderTarget=NULL;

		Log::alert(Categories::TOADLET_PEEPER,
			"destroyed GLRenderDevice");
	}
}

RenderDevice::DeviceStatus GLRenderDevice::activate(){
	if(mGLPrimaryRenderTarget!=NULL){
		mGLPrimaryRenderTarget->activate();
	}
	return DeviceStatus_OK;
}

bool GLRenderDevice::reset(){
	if(mGLPrimaryRenderTarget!=NULL){
		mGLPrimaryRenderTarget->reset();
	}

	setDefaultState();

	return true;
}

bool GLRenderDevice::activateAdditionalContext(){
	if(mGLPrimaryRenderTarget!=NULL){
		return mGLPrimaryRenderTarget->activateAdditionalContext();
	}
	return false;
}

// Resource operations
Texture *GLRenderDevice::createTexture(){
	return new GLTexture(this);
}

PixelBufferRenderTarget *GLRenderDevice::createPixelBufferRenderTarget(){
	PixelBufferRenderTarget *target=NULL;
	#if defined(TOADLET_HAS_GLFBOS)
		if(target==NULL && mFBOs){
			target=new GLFBORenderTarget(this);
		}
	#endif
	#if defined(TOADLET_HAS_GLPBUFFERS)
		if(target==NULL && mPBuffers){
			target=new_GLPBufferRenderTarget(this);
		}
	#endif
	if(target==NULL){
		Error::unimplemented("PixelBufferRenderTargets are unavailable");
	}
	return target;
}

PixelBuffer *GLRenderDevice::createPixelBuffer(){
	return new GLBuffer(this);
}

VertexFormat *GLRenderDevice::createVertexFormat(){
	return new GLVertexFormat(this);
}

VertexBuffer *GLRenderDevice::createVertexBuffer(){
	return new GLBuffer(this);
}

IndexBuffer *GLRenderDevice::createIndexBuffer(){
	return new GLBuffer(this);
}

VariableBuffer *GLRenderDevice::createVariableBuffer(){
	return new GLBuffer(this);
}

Shader *GLRenderDevice::createShader(){
	Shader *shader=NULL;
	#if defined(TOADLET_HAS_GLSL)
		if(shader==NULL && mCaps.hasShader[Shader::ShaderType_VERTEX]){
			shader=new GLSLShader(this);
		}
	#endif
	if(shader==NULL){
 		Error::unimplemented("Shaders are unavailable");
	}
	return shader;
}

Query *GLRenderDevice::createQuery(){
	Query *query=NULL;
	#if defined(TOADLET_HAS_GLQUERY)
		query=new GLQuery(this);
	#endif
	if(query==NULL){
		Error::unimplemented("Queries are unavailable");
	}
	return query;
}

RenderState *GLRenderDevice::createRenderState(){
	return new GLRenderState(this);
}

ShaderState *GLRenderDevice::createShaderState(){
	ShaderState *state=NULL;
	#if defined(TOADLET_HAS_GLSL)
		if(state==NULL && mCaps.hasShader[Shader::ShaderType_VERTEX]){
			state=new GLSLShaderState(this);
		}
	#endif
	if(state==NULL){
		Error::unimplemented("ShaderStates are unavailable");
	}
	return state;
}

void GLRenderDevice::setMatrix(MatrixType type,const Matrix4x4 &matrix){
	#if defined(TOADLET_HAS_GLFIXED)
		if(type==MatrixType_MODEL || type==MatrixType_VIEW){
			if(type==MatrixType_MODEL){
				#if defined(TOADLET_FIXED_POINT) && !defined(TOADLET_HAS_GLES)
					MathConversion::scalarToFloat(mModelMatrix,matrix);
				#else
					mModelMatrix.set(matrix);
				#endif
			}
			else{
				#if defined(TOADLET_FIXED_POINT) && !defined(TOADLET_HAS_GLES)
					MathConversion::scalarToFloat(mViewMatrix,matrix);
				#else
					mViewMatrix.set(matrix);
				#endif
			}

			glMatrixMode(GL_MODELVIEW);

			#if defined(TOADLET_FIXED_POINT) && defined(TOADLET_HAS_GLES)
				glLoadMatrixx(mViewMatrix.data);
				glMultMatrixx(mModelMatrix.data);
			#else
				glLoadMatrixf(mViewMatrix.data);
				glMultMatrixf(mModelMatrix.data);
			#endif
		}
		else{
			glMatrixMode(GL_PROJECTION);

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

		TOADLET_CHECK_GLERROR("setMatrix");
	#endif
}

bool GLRenderDevice::setRenderTarget(RenderTarget *target){
	GLRenderTarget *gltarget=target!=NULL?(GLRenderTarget*)target->getRootRenderTarget():NULL;
	if(gltarget==NULL || mRenderTarget==target){
		return false;
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

	setDefaultState();

	return true;
}

void GLRenderDevice::setViewport(const Viewport &viewport){
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

	// Update GeometryState, since it is dependent upon viewport size
	setGeometryState(&mGeometryState);

	TOADLET_CHECK_GLERROR("setViewport");
}

void GLRenderDevice::clear(int clearFlags,const Vector4 &clearColor){
	int bufferBits=0;

	TOADLET_CHECK_GLERROR("entering clear");

	if((clearFlags & RenderDevice::ClearType_BIT_COLOR)!=0){
		bufferBits|=GL_COLOR_BUFFER_BIT;
		glClearColor(clearColor.x,clearColor.y,clearColor.z,clearColor.w);
	}
	if((clearFlags & RenderDevice::ClearType_BIT_DEPTH)!=0){
		bufferBits|=GL_DEPTH_BUFFER_BIT;
	}
	if((clearFlags & RenderDevice::ClearType_BIT_STENCIL)!=0){
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

void GLRenderDevice::swap(){
	if(mGLRenderTarget!=NULL){
		mGLRenderTarget->swap();
	}

	TOADLET_CHECK_GLERROR("swap");
}

void GLRenderDevice::beginScene(){
	if(mGLRenderTarget!=NULL){
		mGLRenderTarget->activate();
	}

	TOADLET_CHECK_GLERROR("beginScene");
}

void GLRenderDevice::endScene(){
	setVertexData(NULL);
	setShaderState(NULL);
	int i,j;
	for(i=0;i<mCaps.maxTextureStages;++i){
		setSamplerState(i,NULL);
		setTextureState(i,NULL);
		// Reset all shader types, since setTexture remembers the previous type used
		for(j=0;j<Shader::ShaderType_MAX;++j){
			setTexture((Shader::ShaderType)j,i,NULL);
		}
	}

	#if defined(TOADLET_HAS_FIXED)
		#if defined(TOADLET_HAS_GLIBOS)
			if(mIBOs){
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
			}
		#endif
		#if defined(TOADLET_HAS_GLVBOS)
			if(mVBOs){
				glBindBuffer(GL_ARRAY_BUFFER,0);
			}
		#endif
		#if defined(TOADLET_HAS_GLPBOS)
			if(mPBOs){
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);
			}
		#endif
		#if defined(TOADLET_HAS_GLUBOS)
			if(mUBOs){
				glBindBuffer(GL_UNIFORM_BUFFER,0);
			}
		#endif
		#if defined(TOADLET_HAS_GLSL)
			if(mCaps.hasShader[Shader::ShaderType_VERTEX]){
				glUseProgram(0);
			}
		#endif
	#endif

	TOADLET_CHECK_GLERROR("endScene");
}

void GLRenderDevice::renderPrimitive(VertexData *vertexData,IndexData *indexData){
	int i;
	if(vertexData==NULL || indexData==NULL){
		Error::nullPointer(Categories::TOADLET_PEEPER,
			"VertexData or IndexData is NULL");
		return;
	}

	if(indexData->count<=0){
		return;
	}

	IndexData::Primitive primitive=indexData->primitive;
	#if defined(TOADLET_HAS_GLES)
		switch(mRasterizerFill){
			case RasterizerState::FillType_POINT:
				primitive=IndexData::Primitive_POINTS;
			break;
			case RasterizerState::FillType_LINE:
				switch(primitive){
					case IndexData::Primitive_TRIS:
					case IndexData::Primitive_TRISTRIP:
					case IndexData::Primitive_TRIFAN:
						primitive=IndexData::Primitive_LINESTRIP;
					break;
					default: break;
				}
			break;
			default: break;
		}
	#endif

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

	{
		bool rebindTexCoords=false;
		if(mLastVertexBuffers.size()!=vertexData->vertexBuffers.size() || mMaxTexCoordIndex!=mLastMaxTexCoordIndex){
			rebindTexCoords=true;
		}
		if(rebindTexCoords==false){
			// Rebind if the vertexBuffers have changed
			for(i=mLastVertexBuffers.size()-1;i>=0;--i){
				if(mLastVertexBuffers[i]!=vertexData->vertexBuffers[i]){
					rebindTexCoords=true;
					break;
				}
			}
		}
		if(rebindTexCoords==false){
			// Rebind if the texCoordIndex portions of the TextureStages have changed
			for(i=mMaxTexCoordIndex-1;i>=0;--i){
				if(mLastTexCoordIndexes[i]!=mTexCoordIndexes[i]){
					rebindTexCoords=true;
					break;
				}
			}
		}
		if(rebindTexCoords){
			mLastMaxTexCoordIndex=mMaxTexCoordIndex;
			setVertexData(vertexData);
		}
	}

	IndexBuffer *indexBuffer=indexData->indexBuffer;
	if(indexBuffer!=NULL){
		GLenum indexType=getGLIndexType(indexBuffer->getIndexFormat());
		GLBuffer *glindexBuffer=(GLBuffer*)indexBuffer->getRootIndexBuffer();

		if(glindexBuffer->mHandle==0){
			if(mIBOs){
				glBindBuffer(glindexBuffer->mTarget,0);
			}
			glDrawElements(type,indexData->count,indexType,glindexBuffer->mData+indexData->start*glindexBuffer->mIndexFormat);
		}
		else{
			glBindBuffer(glindexBuffer->mTarget,glindexBuffer->mHandle);
			glDrawElements(type,indexData->count,indexType,reinterpret_cast<uint8*>(indexData->start*glindexBuffer->mIndexFormat));
		}

		TOADLET_CHECK_GLERROR("glDrawElements");
	}
	else{
		glDrawArrays(type,indexData->start,indexData->count);

		TOADLET_CHECK_GLERROR("glDrawArrays");
	}
}

bool GLRenderDevice::copyFrameBufferToPixelBuffer(PixelBuffer *dst){
	GLPixelBuffer *glpixelBuffer=(GLPixelBuffer*)dst->getRootPixelBuffer();

	GLTextureMipPixelBuffer *textureBuffer=glpixelBuffer->castToGLTextureMipPixelBuffer();
	GLFBOPixelBuffer *fboBuffer=glpixelBuffer->castToGLFBOPixelBuffer();
	GLBuffer *pixelBuffer=glpixelBuffer->castToGLBuffer();
	RenderTarget *renderTarget=mRenderTarget->getRootRenderTarget();

	if(textureBuffer!=NULL){
		GLTexture *gltexture=textureBuffer->getTexture();

		glBindTexture(gltexture->mTarget,gltexture->mHandle);
		TOADLET_CHECK_GLERROR("glBindTexture");

		glCopyTexSubImage2D(gltexture->mTarget,textureBuffer->getLevel(),0,0,0,renderTarget->getHeight()-gltexture->mFormat->getHeight(),gltexture->mFormat->getWidth(),gltexture->mFormat->getDepth());
		TOADLET_CHECK_GLERROR("glCopyTexSubImage2D");

		Matrix4x4 matrix;
		Math::setMatrix4x4FromScale(matrix,Math::ONE,-Math::ONE,Math::ONE);
		Math::setMatrix4x4FromTranslate(matrix,0,Math::ONE,0);
		gltexture->setMatrix(matrix);
	}
	else if(fboBuffer!=NULL){
		Error::unimplemented(Categories::TOADLET_PEEPER,
			"GLRenderDevice::copyToSurface: unimplemented for fbo");
		return false;
	}
	else if(pixelBuffer!=NULL){
		Error::unimplemented(Categories::TOADLET_PEEPER,
			"GLRenderDevice::copyToSurface: unimplemented for pbo");
		return false;
	}

	return true;
}

bool GLRenderDevice::copyPixelBuffer(PixelBuffer *dst,PixelBuffer *src){
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
			srcTexture->load(srcTextureBuffer->mFormat,NULL);

			glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB,0);
		}
		else if(srcPixelBuffer!=NULL && dstTextureBuffer!=NULL){
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB,srcPixelBuffer->mHandle);

			GLTexture *dstTexture=dstTextureBuffer->mTexture;
			dstTexture->load(dstTextureBuffer->mFormat,NULL);

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

void GLRenderDevice::setDefaultState(){
	setRenderState(mDefaultState);

	setVertexData(NULL);
	setShaderState(NULL);
	int i,j;
	for(i=0;i<mCaps.maxTextureStages;++i){
		setSamplerState(i,NULL);
		setTextureState(i,NULL);
		// Reset all shader types, since setTexture remembers the previous type used
		for(j=0;j<Shader::ShaderType_MAX;++j){
			setTexture((Shader::ShaderType)j,i,NULL);
		}
	}

	setAmbientColor(Math::ONE_VECTOR4);

	// GL specific states
	#if defined(TOADLET_HAS_GLFIXED) && !defined(TOADLET_HAS_GLES) && defined(TOADLET_HAS_GL_11)
	if(gl_version>=11){
		glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL,GL_SEPARATE_SPECULAR_COLOR);
	}
	#endif

	TOADLET_CHECK_GLERROR("setDefaultState");
}

bool GLRenderDevice::setRenderState(RenderState *renderState){
	GLRenderState *glrenderState=renderState!=NULL?(GLRenderState*)renderState->getRootRenderState():NULL;
	if(glrenderState==NULL){
		return false;
	}

	if(glrenderState->mBlendState!=NULL){
		setBlendState(glrenderState->mBlendState);
	}
	if(glrenderState->mDepthState!=NULL){
		setDepthState(glrenderState->mDepthState);
	}
	if(glrenderState->mRasterizerState!=NULL){
		setRasterizerState(glrenderState->mRasterizerState);
	}
	if(glrenderState->mFogState!=NULL){
		setFogState(glrenderState->mFogState);
	}
	if(glrenderState->mGeometryState!=NULL){
		setGeometryState(glrenderState->mGeometryState);
	}
	if(glrenderState->mMaterialState!=NULL){
		setMaterialState(glrenderState->mMaterialState);
	}
	int i;
	for(i=0;i<glrenderState->mSamplerStates.size();++i){
		setSamplerState(i,glrenderState->mSamplerStates[i]);
	}
	for(i=0;i<glrenderState->mTextureStates.size();++i){
		setTextureState(i,glrenderState->mTextureStates[i]);
	}

	return true;
}

bool GLRenderDevice::setShaderState(ShaderState *shaderState){
	#if defined(TOADLET_HAS_GLSL)
		GLSLShaderState *glshaderState=shaderState!=NULL?(GLSLShaderState*)shaderState->getRootShaderState():NULL;
		if(glshaderState==NULL){
			glUseProgram(0);
			mLastShaderState=NULL;
			return false;
		}

		if(glshaderState->activate()){
			mLastShaderState=glshaderState;
		}
		else{
			mLastShaderState=NULL;
		}

		TOADLET_CHECK_GLERROR("setShaderState");

		return true;
	#else
		return false;
	#endif
}

void GLRenderDevice::setBlendState(BlendState *state){
	if(state->equals(BlendState::Combination_DISABLED)){
		glDisable(GL_BLEND);
	}
	else{
		int src=getGLBlendOperation(state->source);
		int dest=getGLBlendOperation(state->dest);

		glBlendFunc(src,dest);
		glEnable(GL_BLEND);
	}

	glColorMask((state->colorWrite&BlendState::ColorWrite_BIT_R)!=0,(state->colorWrite&BlendState::ColorWrite_BIT_G)!=0,(state->colorWrite&BlendState::ColorWrite_BIT_B)!=0,(state->colorWrite&BlendState::ColorWrite_BIT_A)!=0);

	TOADLET_CHECK_GLERROR("setBlendState");
}

void GLRenderDevice::setDepthState(DepthState *state){
	mDepthState.set(*state);

	if(state->test==DepthState::DepthTest_ALWAYS){
		glDisable(GL_DEPTH_TEST);
	}
	else{
		glDepthFunc(getGLDepthFunc(state->test));
		glEnable(GL_DEPTH_TEST);
	}

	glDepthMask(state->write);

	TOADLET_CHECK_GLERROR("setDepthState");
}

void GLRenderDevice::setFogState(FogState *state){
#if defined(TOADLET_HAS_GLFIXED)
	if(state->fog==FogState::FogType_NONE){
		glDisable(GL_FOG);
	}
	else{
		glEnable(GL_FOG);
		glFogf(GL_FOG_MODE,getGLFogType(state->fog));
		#if defined(TOADLET_FIXED_POINT)
			#if defined(TOADLET_HAS_GLES)
				glFogx(GL_FOG_START,state->nearDistance);
				glFogx(GL_FOG_END,state->farDistance);
				glFogx(GL_FOG_DENSITY,state->density);
				glFogxv(GL_FOG_COLOR,state->color.getData());
			#else
				glFogf(GL_FOG_START,MathConversion::scalarToFloat(state->nearDistance));
				glFogf(GL_FOG_END,MathConversion::scalarToFloat(state->farDistance));
				glFogf(GL_FOG_DENSITY,MathConversion::scalarToFloat(state->density));
				glFogfv(GL_FOG_COLOR,colorArray(cacheArray,state->color));
			#endif
		#else
			glFogf(GL_FOG_START,state->nearDistance);
			glFogf(GL_FOG_END,state->farDistance);
			glFogf(GL_FOG_DENSITY,state->density);
			glFogfv(GL_FOG_COLOR,state->color.getData());
		#endif
	}

	TOADLET_CHECK_GLERROR("setFogParameters");
#endif
}

void GLRenderDevice::setMaterialState(MaterialState *state){
#if defined(TOADLET_HAS_GLFIXED)
	if(state->light){
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
	if(state->trackColor){
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
			glMaterialxv(GL_FRONT_AND_BACK,GL_AMBIENT,state->ambient.getData());
			glMaterialxv(GL_FRONT_AND_BACK,GL_DIFFUSE,state->diffuse.getData());
			glMaterialxv(GL_FRONT_AND_BACK,GL_SPECULAR,state->specular.getData());
			glMaterialx(GL_FRONT_AND_BACK,GL_SHININESS,state->shininess);
			glMaterialxv(GL_FRONT_AND_BACK,GL_EMISSION,state->emissive.getData());
		#else
			glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,colorArray(cacheArray,state->ambient));
			glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,colorArray(cacheArray,state->diffuse));
			glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,colorArray(cacheArray,state->specular));
			glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,MathConversion::scalarToFloat(state->shininess));
			glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,colorArray(cacheArray,state->emissive));
		#endif
	#else
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,state->ambient.getData());
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,state->diffuse.getData());
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,state->specular.getData());
		glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,state->shininess);
		glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,state->emissive.getData());
	#endif

	glShadeModel(getGLShadeModel(state->shade));

	switch(state->normalize){
		case MaterialState::NormalizeType_NONE:
			glDisable(GL_NORMALIZE);
			glDisable(GL_RESCALE_NORMAL);
		break;
		case MaterialState::NormalizeType_RESCALE:
			glDisable(GL_NORMALIZE);
			glEnable(GL_RESCALE_NORMAL);
		break;
		case MaterialState::NormalizeType_NORMALIZE:
			glEnable(GL_NORMALIZE);
			glDisable(GL_RESCALE_NORMAL);
		break;
	}

	if(state->alphaTest==MaterialState::AlphaTest_NEVER){
		glDisable(GL_ALPHA_TEST);
	}
	else{
		GLenum func=getGLAlphaFunc(state->alphaTest);
		#if defined(TOADLET_FIXED_POINT) && defined(TOADLET_HAS_GLES)
			glAlphaFuncx(func,state->alphaCutoff);
		#else
			glAlphaFunc(func,MathConversion::scalarToFloat(state->alphaCutoff));
		#endif
		glEnable(GL_ALPHA_TEST);
	}

	TOADLET_CHECK_GLERROR("setMaterialState");
#endif
}

void GLRenderDevice::setSamplerState(int i,SamplerState *state){
	if(i>=mCaps.maxTextureStages){
		return;
	}

	mLastSamplerStates[i]=state;
}

void GLRenderDevice::setTextureState(int i,TextureState *state){
	if(i>=mCaps.maxTextureStages) return;

#if defined(TOADLET_HAS_GLFIXED)
	if(!activeTexture(i)) return;

	if(state!=NULL){
		// Setup texture blending
		bool specifyColor=(state->colorOperation!=TextureState::Operation_UNSPECIFIED);
		bool specifyAlpha=(state->alphaOperation!=TextureState::Operation_UNSPECIFIED);
		bool specifyColorSource=!(state->colorSource1==TextureState::Source_UNSPECIFIED && state->colorSource2==TextureState::Source_UNSPECIFIED);
		bool specifyAlphaSource=!(state->alphaSource1==TextureState::Source_UNSPECIFIED && state->alphaSource2==TextureState::Source_UNSPECIFIED);

		if(!specifyColorSource && !specifyAlphaSource){
			if(specifyColor==false){
				glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
			}
			else{
				glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,getGLTextureBlendOperation(state->colorOperation));
			}
		}
		else{
			glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
			if(state->colorOperation==TextureState::Operation_DOTPRODUCT && state->alphaOperation==TextureState::Operation_DOTPRODUCT){
				glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_DOT3_RGBA);
			}
			else{
				if(specifyColor){
					glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,getGLTextureBlendOperation(state->colorOperation));
				}
				if(specifyAlpha){
					if(state->alphaOperation==TextureState::Operation_DOTPRODUCT){
						Error::unknown(Categories::TOADLET_PEEPER,"can not use Operation_DOTPRODUCT in only alpha channel");
					}
					else{
						glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,getGLTextureBlendOperation(state->alphaOperation));
					}
				}
			}

			#if defined(TOADLET_FIXED_POINT) && defined(TOADLET_HAS_GLES)
				glTexEnvxv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,state->constantColor.getData());
			#else
				glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,colorArray(cacheArray,state->constantColor));
			#endif

			if(specifyColor){
				if(state->colorSource1!=TextureState::Source_UNSPECIFIED){
					glTexEnvi(GL_TEXTURE_ENV,GL_SRC0_RGB,getGLTextureBlendSource(state->colorSource1));
				}
				if(state->colorSource2!=TextureState::Source_UNSPECIFIED){
					glTexEnvi(GL_TEXTURE_ENV,GL_SRC1_RGB,getGLTextureBlendSource(state->colorSource2));
				}
				if(state->colorSource3!=TextureState::Source_UNSPECIFIED){
					glTexEnvi(GL_TEXTURE_ENV,GL_SRC2_RGB,getGLTextureBlendSource(state->colorSource3));
				}
				// No need to specify GL_OPERANDs for ALPHABLEND, since RGB defaults are COLOR,COLOR,ALPHA
			}
			if(specifyAlpha){
				if(state->alphaSource1!=TextureState::Source_UNSPECIFIED){
					glTexEnvi(GL_TEXTURE_ENV,GL_SRC0_ALPHA,getGLTextureBlendSource(state->alphaSource1));
				}
				if(state->alphaSource2!=TextureState::Source_UNSPECIFIED){
					glTexEnvi(GL_TEXTURE_ENV,GL_SRC1_ALPHA,getGLTextureBlendSource(state->alphaSource2));
				}
				if(state->alphaSource3!=TextureState::Source_UNSPECIFIED){
					glTexEnvi(GL_TEXTURE_ENV,GL_SRC2_ALPHA,getGLTextureBlendSource(state->alphaSource3));
				}
				// No need to specify GL_OPERANDs for ALPHABLEND, since ALPHA defaults are ALPHA,ALPHA,ALPHA
			}
		}
		if(specifyColor){
			glTexEnvf(GL_TEXTURE_ENV,GL_RGB_SCALE,getGLTextureBlendScale(state->colorOperation));
		}
		if(specifyAlpha){
			glTexEnvf(GL_TEXTURE_ENV,GL_ALPHA_SCALE,getGLTextureBlendScale(state->alphaOperation));
		}
	}
#endif

	mLastTextureStates[i]=state;
}

void GLRenderDevice::setSamplerStatePostTexture(int i,SamplerState *state){
	if(!activeTexture(i)) return;

	GLTexture *texture=mLastTextures[i];
	GLuint textureTarget=texture!=NULL?texture->mTarget:0;

	if(state!=NULL && textureTarget!=0){
		glTexParameteri(textureTarget,GL_TEXTURE_WRAP_S,getGLWrap(state->uAddress,mHasClampToEdge));
		glTexParameteri(textureTarget,GL_TEXTURE_WRAP_T,getGLWrap(state->vAddress,mHasClampToEdge));
		#if !defined(TOADLET_HAS_GLES) && defined(TOADLET_HAS_GL_12)
			if(gl_version>=12){
				glTexParameteri(textureTarget,GL_TEXTURE_WRAP_R,getGLWrap(state->wAddress,mHasClampToEdge));
			}
		#endif

		SamplerState::FilterType mipFilter=(texture->mFormat->getMipMax()==1?SamplerState::FilterType_NONE:state->mipFilter);
		glTexParameteri(textureTarget,GL_TEXTURE_MIN_FILTER,getGLMinFilter(state->minFilter,mipFilter));
		glTexParameteri(textureTarget,GL_TEXTURE_MAG_FILTER,getGLMagFilter(state->magFilter));

		/// @todo: Determine if this should be used and override the settings in the GLTexture itself?
		/*
		#if !defined(TOADLET_HAS_GLES)
			if(gl_version>=12){
				glTexParameteri(textureTarget,GL_TEXTURE_BASE_LEVEL,state->minLOD);
				glTexParameteri(textureTarget,GL_TEXTURE_MAX_LEVEL,state->maxLOD);
			}
		#endif
		*/

		if(i==0){
			#if defined(TOADLET_HAS_GLES) && defined(TOADLET_HAS_GL_11)
				glHint(GL_PERSPECTIVE_CORRECTION_HINT,state->perspective?GL_NICEST:GL_FASTEST);
			#endif
		}
	}

	TOADLET_CHECK_GLERROR("setSamplerState");
}

void GLRenderDevice::setTextureStatePostTexture(int i,TextureState *state){
#if defined(TOADLET_HAS_GLFIXED)
	if(!activeTexture(i)) return;

	GLTexture *texture=mLastTextures[i];
	GLuint textureTarget=texture!=NULL?texture->mTarget:0;
	int texCoordIndex=0;

	if(state!=NULL){
		// Setup TexCoordIndex
		texCoordIndex=state->texCoordIndex;

		// Setup calculations
		if(texture!=NULL){
			TextureState::CalculationType calculation=state->calculation;
			if(calculation==TextureState::CalculationType_DISABLED && ((texture->mUsage&(Texture::Usage_BIT_NPOT_RESTRICTED|Texture::Usage_BIT_RENDERTARGET))>0)){
				calculation=TextureState::CalculationType_NORMAL;
			}

			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();

			if(calculation!=TextureState::CalculationType_DISABLED){
				const Matrix4x4 &matrix=state->matrix;
				if(calculation==TextureState::CalculationType_NORMAL){
					#if defined(TOADLET_FIXED_POINT)
						#if defined(TOADLET_HAS_GLES)
							glMultMatrixx(texture->getMatrix().data);
						#else
							glMultMatrixf(MathConversion::scalarToFloat(cacheMatrix4x4,texture->getMatrix()).data);
						#endif
					#else
						glMultMatrixf(texture->getMatrix().data);
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
				else if(calculation>TextureState::CalculationType_NORMAL){
					#if !defined(TOADLET_HAS_GLES)
						glMatrixMode(GL_MODELVIEW);

						glPushMatrix();
						glLoadIdentity();

						GLint gltg=0;
						GLint glpl=0;
						switch(calculation){
							case TextureState::CalculationType_OBJECTSPACE:
								gltg=GL_OBJECT_LINEAR;
								glpl=GL_OBJECT_PLANE;
							break;
							case TextureState::CalculationType_CAMERASPACE:
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

			#if !defined(TOADLET_HAS_GLES)
				if(calculation<=TextureState::CalculationType_NORMAL){
					glDisable(GL_TEXTURE_GEN_S);
					glDisable(GL_TEXTURE_GEN_T);
					glDisable(GL_TEXTURE_GEN_R);
					glDisable(GL_TEXTURE_GEN_Q);
				}
			#endif
		}

		if(textureTarget!=0){
			// Setup shadow result
			#if !defined(TOADLET_HAS_GLES)
				if(gl_version>=14){
					if(state->shadowResult!=TextureState::ShadowResult_DISABLED){
						glTexParameteri(textureTarget,GL_DEPTH_TEXTURE_MODE_ARB,getGLDepthTextureMode(state->shadowResult));
						glTexParameteri(textureTarget,GL_TEXTURE_COMPARE_FUNC_ARB,GL_LEQUAL);
						glTexParameteri(textureTarget,GL_TEXTURE_COMPARE_MODE_ARB,GL_COMPARE_R_TO_TEXTURE);
					}
					else{
						glTexParameteri(textureTarget,GL_TEXTURE_COMPARE_MODE_ARB,GL_NONE);
					}
				}
			#endif
		}
	}

	// Setup current TexCoordIndex and compact if necessary
	mTexCoordIndexes[i]=texCoordIndex;
	if(i>=mMaxTexCoordIndex){
		mMaxTexCoordIndex=i+1;
	}
	while(mMaxTexCoordIndex>0 && mLastTexTargets[mMaxTexCoordIndex-1]==0){
		mMaxTexCoordIndex--;
	}

	TOADLET_CHECK_GLERROR("setTextureState");
#endif
}

void GLRenderDevice::setBuffer(Shader::ShaderType shaderType,int i,VariableBuffer *buffer){
#if defined(TOADLET_HAS_GLSHADERS)
	GLBuffer *glbuffer=buffer!=NULL?(GLBuffer*)buffer->getRootVariableBuffer():NULL;
	if(glbuffer==NULL){
		return;
	}

	if(glbuffer->mHandle>0){
		#if defined(TOADLET_HAS_GLUBOS)
			glUniformBlockBinding(mLastShaderState->mHandle,0,0);
			glBindBufferBase(GL_UNIFORM_BUFFER,0,glbuffer->mHandle);
		#endif
	}
	else{
		glbuffer->activateUniforms();
	}

	TOADLET_CHECK_GLERROR("setBuffer");
#endif
}

void GLRenderDevice::setTexture(Shader::ShaderType shaderType,int i,Texture *texture){
	// Skip the setTexture if it's a disable, and aimed at a different shaderType, since GL doesn't differentiate.
	// Otherwise we can have problems where the texture ends up being disabled because it was affecting a different shaderType, and it shouldn't have been.
	if(!activeTexture(i) || (texture==NULL && mLastTextureType[i]!=shaderType)) return;

	GLuint textureTarget=0;
	GLTexture *gltexture=texture!=NULL?(GLTexture*)texture->getRootTexture():NULL;
	if(gltexture!=NULL){
		textureTarget=gltexture->mTarget;

		if(mLastTexTargets[i]!=textureTarget){
			if(mLastTexTargets[i]!=0){
				glBindTexture(mLastTexTargets[i],0);
				glDisable(mLastTexTargets[i]);
			}
			mLastTexTargets[i]=textureTarget;
		}

		if(mLastTextures[i]!=gltexture){
			glBindTexture(textureTarget,gltexture->mHandle);
			mLastTextures[i]=gltexture;
		}

		// Always re-enable the texture, since it can get glDisabled in mipmap generation
		#if defined(TOADLET_HAS_GLFIXED)
			if(textureTarget!=0){
				glEnable(textureTarget);
			}
		#endif
	}
	else{
		if(mLastTextures[i]!=NULL){
			mLastTextures[i]=NULL;
		}

		if(mLastTexTargets[i]!=0){
			#if defined(TOADLET_HAS_GLFIXED)
				glBindTexture(mLastTexTargets[i],0);
				glDisable(mLastTexTargets[i]);
			#endif
			mLastTexTargets[i]=0;
		}
	}

	mLastTextureType[i]=shaderType;

	setSamplerStatePostTexture(i,mLastSamplerStates[i]);
	setTextureStatePostTexture(i,mLastTextureStates[i]);

	TOADLET_CHECK_GLERROR("setTexture");
}

void GLRenderDevice::setGeometryState(GeometryState *state){
	mGeometryState.set(*state);

	// pointsize = size / sqrt(constant + linear*d + quadratic*d*d)
	// if a&b = 0, then quadratic = 1/(C*C) where C = first component of projMatrix * 1/2 screen width
	if(mCaps.pointSprites){
		int value;
		if(state->sprite){
			#if defined(TOADLET_HAS_GLSL) && defined(TOADLET_HAS_GL_32)
				if(gl_version>=32){
					glEnable(GL_PROGRAM_POINT_SIZE);
				}
			#endif
			#if defined(TOADLET_HAS_GLFIXED)
				glEnable(GL_POINT_SPRITE);
				value=1;

				#if defined(TOADLET_FIXED_POINT)
					#if defined(TOADLET_HAS_GLES)
						glPointParameterx(GL_POINT_SIZE_MIN,state->minSize);
						glPointParameterx(GL_POINT_SIZE_MAX,state->maxSize);
					#else
						glPointParameterf(GL_POINT_SIZE_MIN,MathConversion::scalarToFloat(state->minSize));
						glPointParameterf(GL_POINT_SIZE_MAX,MathConversion::scalarToFloat(state->maxSize));
					#endif
				#else
					glPointParameterf(GL_POINT_SIZE_MIN,state->minSize);
					glPointParameterf(GL_POINT_SIZE_MAX,state->maxSize);
				#endif

				if(state->attenuated){
					#if defined(TOADLET_FIXED_POINT)
						#if defined(TOADLET_HAS_GLES)
							cacheArray[0]=state->constant; cacheArray[1]=state->linear; cacheArray[2]=state->quadratic;
							glPointParameterxv(GL_POINT_DISTANCE_ATTENUATION,cacheArray);
						#else
							cacheArray[0]=MathConversion::scalarToFloat(state->constant); cacheArray[1]=MathConversion::scalarToFloat(state->linear); cacheArray[2]=MathConversion::scalarToFloat(state->quadratic);
							glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION,cacheArray);
						#endif
					#else
						cacheArray[0]=state->constant; cacheArray[1]=state->linear; cacheArray[2]=state->quadratic;
						glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION,cacheArray);
					#endif
				}
				else{
					#if defined(TOADLET_FIXED_POINT) && defined(TOADLET_HAS_GLES)
						cacheArray[0]=Math::ONE; cacheArray[1]=0; cacheArray[2]=0;
						glPointParameterxv(GL_POINT_DISTANCE_ATTENUATION,cacheArray);
					#else
						cacheArray[0]=1.0f; cacheArray[1]=0; cacheArray[2]=0;
						glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION,cacheArray);
					#endif
				}
			#endif
		}
		else{
			#if defined(TOADLET_HAS_GLSL) && defined(TOADLET_HAS_GL_32)
				if(gl_version>=32){
					glDisable(GL_PROGRAM_POINT_SIZE);
				}
			#endif
			#if defined(TOADLET_HAS_GLFIXED)
				glDisable(GL_POINT_SPRITE);
			#endif
			value=0;
		}

		#if defined(TOADLET_HAS_GLFIXED)
			if(mMultiTexture){
				int stage;
				for(stage=0;stage<mCaps.maxTextureStages;++stage){
					glActiveTexture(GL_TEXTURE0+stage);
					mLastActiveTexture=stage;
					glTexEnvi(GL_POINT_SPRITE,GL_COORD_REPLACE,value);
				}
			}
			else{
				glTexEnvi(GL_POINT_SPRITE,GL_COORD_REPLACE,value);
			}
		#endif
	}

	#if defined(TOADLET_HAS_GLFIXED)
		if(state->size>0){
			if(state->attenuated){
				glPointSize(MathConversion::scalarToFloat(state->size) * mViewport.height);
			}
			else{
				glPointSize(MathConversion::scalarToFloat(state->size));
			}
		}
	#endif

	TOADLET_CHECK_GLERROR("setGeometryState");
}

void GLRenderDevice::setRasterizerState(RasterizerState *state){
	if(state->cull==RasterizerState::CullType_NONE){
		glDisable(GL_CULL_FACE);
	}
	else{
		glCullFace(getGLCullFace(state->cull));
		glEnable(GL_CULL_FACE);
	}

	#if !defined(TOADLET_HAS_GLES)
		glPolygonMode(GL_FRONT_AND_BACK,getGLPolygonMode(state->fill));
	#else
		mRasterizerFill=state->fill;
	#endif

	if(state->depthBiasConstant==0 && state->depthBiasSlope==0){
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
	else{
		#if defined(TOADLET_FIXED_POINT) && defined(TOADLET_HAS_GLES)
			glPolygonOffsetx(state->depthBiasSlope,state->depthBiasConstant);
		#else
			glPolygonOffset(MathConversion::scalarToFloat(state->depthBiasSlope),MathConversion::scalarToFloat(state->depthBiasConstant));
		#endif
		glEnable(GL_POLYGON_OFFSET_FILL);
	}

	#if defined(TOADLET_HAS_GLFIXED)
		if(state->multisample){
			glEnable(GL_MULTISAMPLE);
		}
		else{
			glDisable(GL_MULTISAMPLE);
		}
	#endif

	if(state->dither){
		glEnable(GL_DITHER);
	}
	else{
		glDisable(GL_DITHER);
	}

	glLineWidth(MathConversion::scalarToFloat(state->lineSize));

	TOADLET_CHECK_GLERROR("setRasterizerState");
}

void GLRenderDevice::setLightState(int i,const LightState &state){
#if defined(TOADLET_HAS_GLFIXED)
	GLenum l=GL_LIGHT0+i;

	glMatrixMode(GL_MODELVIEW);

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
#endif
}

void GLRenderDevice::setLightEnabled(int i,bool enable){
#if defined(TOADLET_HAS_GLFIXED)
	if(enable){
		glEnable(GL_LIGHT0+i);
	}
	else{
		glDisable(GL_LIGHT0+i);
	}

	TOADLET_CHECK_GLERROR("setLightEnabled");
#endif
}

void GLRenderDevice::setAmbientColor(const Vector4 &ambient){
#if defined(TOADLET_HAS_GLFIXED)
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
#endif
}

void GLRenderDevice::getShadowBiasMatrix(Matrix4x4 &result,const Texture *shadowTexture,scalar bias){
	result.set( Math::HALF, 0,          0,          Math::HALF,
				0,          Math::HALF, 0,          Math::HALF,
				0,          0,          Math::HALF, Math::HALF - bias / 4, // Not positive that the bias is valid here, but its a try
				0,          0,          0,          Math::ONE);
}

int GLRenderDevice::getClosePixelFormat(int format,int usage){
	#if !defined(TOADLET_HAS_GLES) && defined(TOADLET_HAS_GL_12)
		if(gl_version<12)
	#endif
	{
		switch(format){
			case TextureFormat::Format_BGR_8:
				return TextureFormat::Format_RGB_8;
			case TextureFormat::Format_BGRA_8:
				return TextureFormat::Format_RGBA_8;
		}
	}

	if((usage&Texture::Usage_BIT_RENDERTARGET)!=0){
		switch(format){
			case TextureFormat::Format_L_8:
				return TextureFormat::Format_RGB_8;
		}
	}

	switch(format){
		case TextureFormat::Format_R_8:
		case TextureFormat::Format_RG_8:
			return TextureFormat::Format_RGB_8;
		default:
			return format;
	}
}

bool GLRenderDevice::getShaderProfileSupported(const String &profile){
	#if defined(TOADLET_HAS_GLSL)
		return profile=="glsl";
	#else
		return false;
	#endif
}

bool GLRenderDevice::hardwareBuffersSupported(GLBuffer *buffer) const{
	if(buffer->mIndexFormat>0){
		return mIBOs;
	}
	else if(buffer->mVertexFormat!=NULL){
		return mVBOs;
	}
	else if(buffer->mTextureFormat!=NULL){
		return mPBOs;
	}
	else if(buffer->mVariableFormat!=NULL && buffer->mVariableFormat->getPrimary()==false){
		return mUBOs;
	}
	else{
		return false;
	}
}

bool GLRenderDevice::hardwareMappingSupported(GLBuffer *buffer) const{
	#if defined(TOADLET_HAS_GLES)
		return false;
	#endif

	// Thanks to Ogre3D for this threshold
	if(buffer->mVertexFormat!=NULL){
		return buffer->mDataSize>(1024 * 32) && mVBOs;
	}
	else if(buffer->mIndexFormat>0){
		return buffer->mDataSize>(1024 * 32) && mIBOs;
	}
	else{
		return false;
	}
}

void GLRenderDevice::setVertexData(const VertexData *vertexData){
	#if defined(TOADLET_HAS_GLSHADERS) && defined(TOADLET_HAS_GLFIXED)
		if(mLastShaderState==NULL){
			if(mLastShaderSemanticBits!=0){
				setShaderVertexData(NULL);
			}
			setFixedVertexData(vertexData);
		}
		else{
			if(mLastFixedSemanticBits!=0){
				setFixedVertexData(NULL);
			}
			setShaderVertexData(vertexData);
		}
	#elif defined(TOADLET_HAS_GLSHADERS)
		setShaderVertexData(vertexData);
	#elif defined(TOADLET_HAS_GLFIXED)
		setFixedVertexData(vertexData);
	#endif

	if(vertexData!=NULL){
		mLastVertexBuffers.resize(vertexData->vertexBuffers.size());
		int i;
		for(i=0;i<mLastVertexBuffers.size();++i){
			mLastVertexBuffers[i]=vertexData->vertexBuffers[i];
		}
	}
	else{
		mLastVertexBuffers.clear();
	}
}

#if defined(TOADLET_HAS_GLFIXED)
void GLRenderDevice::setFixedVertexData(const VertexData *vertexData){
	int numVertexBuffers=0;
	int semanticBits=0,lastSemanticBits=mLastFixedSemanticBits,texCoordSemanticBits=0,lastTexCoordSemanticBits=mLastFixedTexCoordSemanticBits;

	if(vertexData!=NULL){
		numVertexBuffers=vertexData->vertexBuffers.size();
	}

	int i,j,k;
	for(i=0;i<numVertexBuffers;++i){
		GLBuffer *glbuffer=(GLBuffer*)vertexData->vertexBuffers[i]->getRootVertexBuffer();

		GLVertexFormat *glvertexFormat=(GLVertexFormat*)(glbuffer->mVertexFormat->getRootVertexFormat());
		GLsizei vertexSize=glvertexFormat->mVertexSize;
		int numElements=glvertexFormat->mSemantics.size();
		semanticBits|=(glvertexFormat->mSemanticBits&~(1<<VertexFormat::Semantic_TEXCOORD));

		if(glbuffer->mHandle==0){
			if(mVBOs){
				glBindBuffer(glbuffer->mTarget,0);
			}
		}
		else{
			glBindBuffer(glbuffer->mTarget,glbuffer->mHandle);
		}

		for(j=0;j<numElements;++j){
			int semantic=glvertexFormat->mSemantics[j];
			int index=glvertexFormat->mIndexes[j];

			switch(semantic){
				case VertexFormat::Semantic_POSITION:
					glVertexPointer(
						glvertexFormat->mGLElementCounts[j],
						glvertexFormat->mGLDataTypes[j],
						vertexSize,
						glbuffer->mElementOffsets[j]);
				break;
				case VertexFormat::Semantic_NORMAL:
					glNormalPointer(
						glvertexFormat->mGLDataTypes[j],
						vertexSize,
						glbuffer->mElementOffsets[j]);
				break;
				case VertexFormat::Semantic_COLOR:
					if(index==0){
						glColorPointer(
							glvertexFormat->mGLElementCounts[j],
							glvertexFormat->mGLDataTypes[j],
							vertexSize,
							glbuffer->mElementOffsets[j]);
					}
				break;
				case VertexFormat::Semantic_TEXCOORD:
					for(k=0;k<mMaxTexCoordIndex;++k){
						if(mTexCoordIndexes[k]==index){
							texCoordSemanticBits|=(1<<k);
							if(mMultiTexture){
								glClientActiveTexture(GL_TEXTURE0+k);
							}
							glTexCoordPointer(
								glvertexFormat->mGLElementCounts[j],
								glvertexFormat->mGLDataTypes[j],
								vertexSize,
								glbuffer->mElementOffsets[j]);
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
		int sb=semanticBits,lsb=lastSemanticBits;
		while(sb>0 || lsb>0){
			if((sb&1)!=(lsb&1)){
				if((sb&1)>(lsb&1)){
					glEnableClientState(getClientStateFromSemantic(state,0));
				}
				else{
					glDisableClientState(getClientStateFromSemantic(state,0));
				}
			}
			sb>>=1;
			lsb>>=1;
			state++;
		}
	}

	if(texCoordSemanticBits!=lastTexCoordSemanticBits){
		// Go through all used texture stages, and see if the enabling state betwen now and last were different.
		//  If so check to see if the state needs to be enabled or disabled.
		int sb=texCoordSemanticBits,lsb=lastTexCoordSemanticBits;
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

	mLastFixedSemanticBits=semanticBits;mLastFixedTexCoordSemanticBits=texCoordSemanticBits;

	TOADLET_CHECK_GLERROR("setFixedVertexData");
}
#endif

#if defined(TOADLET_HAS_GLSHADERS)
void GLRenderDevice::setShaderVertexData(const VertexData *vertexData){
	if(mLastShaderState==NULL){
		return;
	}

	int numVertexBuffers=0;
	int semanticBits=0,lastSemanticBits=mLastShaderSemanticBits;

	if(vertexData!=NULL){
		numVertexBuffers=vertexData->vertexBuffers.size();
	}

	int i,j;
	for(i=0;i<numVertexBuffers;++i){
		GLBuffer *glbuffer=(GLBuffer*)vertexData->vertexBuffers[i]->getRootVertexBuffer();

		GLVertexFormat *glvertexFormat=(GLVertexFormat*)(glbuffer->mVertexFormat->getRootVertexFormat());
		GLsizei vertexSize=glvertexFormat->mVertexSize;
		int numElements=glvertexFormat->mSemantics.size();

		GLSLVertexLayout *gllayout=mLastShaderState->findVertexLayout(glvertexFormat);

		if(glbuffer->mHandle==0){
			if(mVBOs){
				glBindBuffer(glbuffer->mTarget,0);
			}
		}
		else{
			glBindBuffer(glbuffer->mTarget,glbuffer->mHandle);
		}

		for(j=0;j<numElements;++j){
			int semantic=glvertexFormat->mSemantics[j];
			int semanticIndex=gllayout->mSemanticIndexes[j];

			semanticBits|=(1<<semanticIndex);
			bool normalized=(semantic==VertexFormat::Semantic_COLOR);
			glVertexAttribPointer(
				semanticIndex,
				glvertexFormat->mGLElementCounts[j],
				glvertexFormat->mGLDataTypes[j],
				normalized,
				vertexSize,
				glbuffer->mElementOffsets[j]);
		}
	}

	if(semanticBits!=lastSemanticBits){
		// Go through all the VertexFormat types, check to see if the enabling state between now and last were different.
		//  If so check to see if the state needs to be enabled or disabled.
		int semanticIndex=0;
		int sb=semanticBits,lsb=lastSemanticBits;
		while(sb>0 || lsb>0){
			if((sb&1)!=(lsb&1)){
				if((sb&1)>(lsb&1)){
					glEnableVertexAttribArray(semanticIndex);
				}
				else{
					glDisableVertexAttribArray(semanticIndex);
				}
			}
			sb>>=1;
			lsb>>=1;
			semanticIndex++;
		}
	}

	mLastShaderSemanticBits=semanticBits;

	TOADLET_CHECK_GLERROR("setShaderVertexData");
}
#endif

GLenum GLRenderDevice::getGLDepthFunc(DepthState::DepthTest test){
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

GLenum GLRenderDevice::getGLAlphaFunc(MaterialState::AlphaTest test){
	switch(test){
		case MaterialState::AlphaTest_NEVER:
			return GL_NEVER;
		case MaterialState::AlphaTest_LESS:
			return GL_LESS;
		case MaterialState::AlphaTest_EQUAL:
			return GL_EQUAL;
		case MaterialState::AlphaTest_LEQUAL:
			return GL_LEQUAL;
		case MaterialState::AlphaTest_GREATER:
			return GL_GREATER;
		case MaterialState::AlphaTest_NOTEQUAL:
			return GL_NOTEQUAL;
		case MaterialState::AlphaTest_GEQUAL:
			return GL_GEQUAL;
		case MaterialState::AlphaTest_ALWAYS:
			return GL_ALWAYS;
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"getGLAlphaFunc: Invalid alpha test");
			return 0;
	}
}

GLenum GLRenderDevice::getGLBlendOperation(BlendState::Operation operation){
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

GLenum GLRenderDevice::getGLCullFace(RasterizerState::CullType type){
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

GLenum GLRenderDevice::getGLPolygonMode(RasterizerState::FillType type){
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

#if defined(TOADLET_HAS_GLFIXED)
GLenum GLRenderDevice::getGLFogType(FogState::FogType type){
	switch(type){
		case FogState::FogType_LINEAR:
			return GL_LINEAR;
		case FogState::FogType_EXP:
			return GL_EXP;
		case FogState::FogType_EXP2:
			return GL_EXP2;
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"getGLFogType: Invalid fog type");
			return 0;
	}
}
#endif

#if defined(TOADLET_HAS_GLFIXED)
GLenum GLRenderDevice::getGLShadeModel(MaterialState::ShadeType type){
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
#endif

GLint GLRenderDevice::getGLElementCount(int format){
	if(format==VertexFormat::Format_TYPE_COLOR_RGBA){
		return 4;
	}
	else{
		return (format&VertexFormat::Format_MASK_COUNTS)>>VertexFormat::Format_SHIFT_COUNTS;
	}
}

GLenum GLRenderDevice::getGLDataType(int format){
	if(format==VertexFormat::Format_TYPE_COLOR_RGBA){
		return GL_UNSIGNED_BYTE;
	}

	switch(format&VertexFormat::Format_MASK_TYPES){ // Mask out the counts
		case VertexFormat::Format_TYPE_UINT_8:
			return GL_UNSIGNED_BYTE;
		case VertexFormat::Format_TYPE_INT_8:
			return GL_BYTE;
		case VertexFormat::Format_TYPE_INT_16:
			return GL_SHORT;
		case VertexFormat::Format_TYPE_FLOAT_32:
			return GL_FLOAT;
		#if !defined(TOADLET_HAS_GLES)
			case VertexFormat::Format_TYPE_INT_32:
				return GL_INT;
			case VertexFormat::Format_TYPE_DOUBLE_64:
				return GL_DOUBLE;
		#else
			case VertexFormat::Format_TYPE_FIXED_32:
				return GL_FIXED;
		#endif
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"getGLDataType: Invalid data type");
			return 0;
	}
}

GLuint GLRenderDevice::getGLFormat(int textureFormat,bool internal){
	int formatSemantic=(textureFormat&TextureFormat::Format_MASK_SEMANTICS);
	int formatType=(textureFormat&TextureFormat::Format_MASK_TYPES);
	switch(formatSemantic){
		case TextureFormat::Format_SEMANTIC_L:
			return GL_LUMINANCE;
		case TextureFormat::Format_SEMANTIC_A:
			return GL_ALPHA;
		case TextureFormat::Format_SEMANTIC_LA:
			return GL_LUMINANCE_ALPHA;
		#if !defined(TOADLET_HAS_GLES) && defined(TOADLET_HAS_GL_12)
			case TextureFormat::Format_SEMANTIC_BGR:
				if(internal==false){
					return GL_BGR;
				}
				else{
					return GL_RGB;
				}
			case TextureFormat::Format_SEMANTIC_BGRA:
				if(internal==false){
					return GL_BGRA;
				}
				else{
					return GL_RGBA;
				}
		#endif
		case TextureFormat::Format_SEMANTIC_RGB:
			#if defined(TOADLET_HAS_GLEW)
				if(formatType==TextureFormat::Format_TYPE_DXT1){
					return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
				}
			#endif
			return GL_RGB;
		case TextureFormat::Format_SEMANTIC_RGBA:
			#if defined(TOADLET_HAS_GLEW)
				if(formatType==TextureFormat::Format_TYPE_DXT2 || formatType==TextureFormat::Format_TYPE_DXT3){
					return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				}
				else if(formatType==TextureFormat::Format_TYPE_DXT4 || formatType==TextureFormat::Format_TYPE_DXT5){
					return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				}
			#endif
			return GL_RGBA;
		#if defined(TOADLET_HAS_GLFBOS)
			case TextureFormat::Format_SEMANTIC_DEPTH:
				if(internal && formatType==TextureFormat::Format_TYPE_UINT_16){
					return GL_DEPTH_COMPONENT16;
				}
				#if !defined(TOADLET_HAS_GLES)
					else if(internal && formatType==TextureFormat::Format_TYPE_UINT_24){
						return GL_DEPTH_COMPONENT24;
					}
					else if(internal && formatType==TextureFormat::Format_TYPE_UINT_32){
						return GL_DEPTH_COMPONENT32;
					}
				#endif
				else{
					#if defined(GL_DEPTH_COMPONENT)
						return GL_DEPTH_COMPONENT;
					#else
						return GL_DEPTH_COMPONENT16;
					#endif
				}
		#endif
	}
	return 0;
}

GLuint GLRenderDevice::getGLType(int textureFormat){
	int formatType=(textureFormat&TextureFormat::Format_MASK_TYPES);
	switch(formatType){
		case TextureFormat::Format_TYPE_UINT_8:
		case TextureFormat::Format_TYPE_UINT_16:
		case TextureFormat::Format_TYPE_UINT_24:
		case TextureFormat::Format_TYPE_UINT_32:
			return GL_UNSIGNED_BYTE;
		case TextureFormat::Format_TYPE_FLOAT_32:
			return GL_FLOAT;
		case TextureFormat::Format_TYPE_UINT_5_6_5:
			return GL_UNSIGNED_SHORT_5_6_5;
		case TextureFormat::Format_TYPE_UINT_5_5_5_1:
			return GL_UNSIGNED_SHORT_5_5_5_1;
		case TextureFormat::Format_TYPE_UINT_4_4_4_4:
			return GL_UNSIGNED_SHORT_4_4_4_4;
	}
	return 0;
}

GLuint GLRenderDevice::getGLIndexType(int indexFormat){
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

GLuint GLRenderDevice::getGLTextureCompressed(int format){
	switch(format){
		case TextureFormat::Format_RGB_DXT1:
		case TextureFormat::Format_RGBA_DXT2:
		case TextureFormat::Format_RGBA_DXT3:
		case TextureFormat::Format_RGBA_DXT4:
		case TextureFormat::Format_RGBA_DXT5:
			return true;
		default:
			return false;
	}
}

GLuint GLRenderDevice::getGLWrap(SamplerState::AddressType address,bool hasClampToEdge){
	switch(address){
		case SamplerState::AddressType_REPEAT:
			return GL_REPEAT;
		#if !defined(TOADLET_HAS_GLES)
			case SamplerState::AddressType_CLAMP_TO_BORDER:
				return GL_CLAMP_TO_BORDER;
			case SamplerState::AddressType_MIRRORED_REPEAT:
				return GL_MIRRORED_REPEAT;
			case SamplerState::AddressType_CLAMP_TO_EDGE:
				return hasClampToEdge?GL_CLAMP_TO_EDGE:GL_CLAMP;
		#else
			case SamplerState::AddressType_CLAMP_TO_EDGE:
				return GL_CLAMP_TO_EDGE;
		#endif
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"getGLWrap: Invalid address");
			return 0;
	}
}

GLuint GLRenderDevice::getGLMinFilter(SamplerState::FilterType minFilter,SamplerState::FilterType mipFilter){
	switch(mipFilter){
		case SamplerState::FilterType_NONE:
			switch(minFilter){
				case SamplerState::FilterType_NEAREST:
					return GL_NEAREST;
				case SamplerState::FilterType_LINEAR:
					return GL_LINEAR;
				default:
				break;
			}
		break;
		case SamplerState::FilterType_NEAREST:
			switch(minFilter){
				case SamplerState::FilterType_NEAREST:
					return GL_NEAREST_MIPMAP_NEAREST;
				case SamplerState::FilterType_LINEAR:
					return GL_NEAREST_MIPMAP_LINEAR;
				default:
				break;
			}
		break;
		case SamplerState::FilterType_LINEAR:
			switch(minFilter){
				case SamplerState::FilterType_NEAREST:
					return GL_LINEAR_MIPMAP_NEAREST;
				case SamplerState::FilterType_LINEAR:
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

GLuint GLRenderDevice::getGLMagFilter(SamplerState::FilterType magFilter){
	switch(magFilter){
		case SamplerState::FilterType_NEAREST:
			return GL_NEAREST;
		case SamplerState::FilterType_LINEAR:
			return GL_LINEAR;
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"getGLMagFilter: Invalid filter");
			return 0;
	}
}

#if defined(TOADLET_HAS_GLFIXED)
GLuint GLRenderDevice::getGLTextureBlendSource(TextureState::Source source){
	switch(source){
		case TextureState::Source_PREVIOUS:
			return GL_PREVIOUS;
		case TextureState::Source_TEXTURE:
			return GL_TEXTURE;
		case TextureState::Source_PRIMARY_COLOR:
			return GL_PRIMARY_COLOR;
		case TextureState::Source_CONSTANT_COLOR:
			return GL_CONSTANT;
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"getGLTextureBlendSource: Invalid source");
			return 0;
	}
}
#endif

#if defined(TOADLET_HAS_GLFIXED)
GLuint GLRenderDevice::getGLTextureBlendOperation(TextureState::Operation operation){
	switch(operation){
		case TextureState::Operation_REPLACE:
			return GL_REPLACE;
		case TextureState::Operation_MODULATE:
		case TextureState::Operation_MODULATE_2X:
		case TextureState::Operation_MODULATE_4X:
			return GL_MODULATE;
		case TextureState::Operation_ADD:
			return GL_ADD;
		case TextureState::Operation_DOTPRODUCT:
			return GL_DOT3_RGB;
		case TextureState::Operation_ALPHABLEND:
			return GL_INTERPOLATE;
		default:
			return 0;
	}
}
#endif

#if defined(TOADLET_HAS_GLFIXED)
float GLRenderDevice::getGLTextureBlendScale(TextureState::Operation operation){
	switch(operation){
		case TextureState::Operation_MODULATE:
			return 1.0f;
		case TextureState::Operation_MODULATE_2X:
			return 2.0f;
		case TextureState::Operation_MODULATE_4X:
			return 4.0f;
		default:
			return 1.0f;
	}
}
#endif

GLuint GLRenderDevice::getGLDepthTextureMode(TextureState::ShadowResult shadow){
	switch(shadow){
		case TextureState::ShadowResult_L:
			return GL_LUMINANCE;
		#if !defined(TOADLET_HAS_GLES)
			case TextureState::ShadowResult_A:
				return GL_INTENSITY;
		#endif
		default:
			return 0;
	}
}

int GLRenderDevice::getFixedAttribFromSemantic(int semantic,int index){
	switch(semantic){
		case VertexFormat::Semantic_POSITION:
			return 0;
		case VertexFormat::Semantic_NORMAL:
			return 2;
		case VertexFormat::Semantic_COLOR:
			switch(index){
				case 0:
					return 3;
				case 1:
					return 4;
				default:
					return -1;
			}
		case VertexFormat::Semantic_TEXCOORD:
			return 5+index;
		default:
			return -1;
	}
}

#if defined(TOADLET_HAS_GLFIXED)
GLuint GLRenderDevice::getClientStateFromSemantic(int semantic,int index){
	switch(semantic){
		case VertexFormat::Semantic_POSITION:
			return GL_VERTEX_ARRAY;
		case VertexFormat::Semantic_NORMAL:
			return GL_NORMAL_ARRAY;
		case VertexFormat::Semantic_COLOR:
			switch(index){
				case 0:
					return GL_COLOR_ARRAY;
				#if !defined(TOADLET_HAS_GLES)
					case 1:
						return GL_SECONDARY_COLOR_ARRAY;
				#endif
				default:
					return 0;
			}
		default:
			return 0;
	}
};
#endif

#if defined(TOADLET_HAS_GLSHADERS)
int GLRenderDevice::getVariableFormat(GLuint type){
	switch(type){
		case GL_FLOAT:
			return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_1;
		case GL_FLOAT_VEC2:
			return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_2;
		case GL_FLOAT_VEC3:
			return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_3;
		case GL_FLOAT_VEC4:
			return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4;
		case GL_INT:
			return VariableBufferFormat::Format_TYPE_INT_32|VariableBufferFormat::Format_COUNT_1;
		case GL_INT_VEC2:
			return VariableBufferFormat::Format_TYPE_INT_32|VariableBufferFormat::Format_COUNT_2;
		case GL_INT_VEC3:
			return VariableBufferFormat::Format_TYPE_INT_32|VariableBufferFormat::Format_COUNT_3;
		case GL_INT_VEC4:
			return VariableBufferFormat::Format_TYPE_INT_32|VariableBufferFormat::Format_COUNT_4;
		case GL_BOOL:
			return VariableBufferFormat::Format_TYPE_UINT_8|VariableBufferFormat::Format_COUNT_1;
		case GL_BOOL_VEC2:
			return VariableBufferFormat::Format_TYPE_UINT_8|VariableBufferFormat::Format_COUNT_2;
		case GL_BOOL_VEC3:
			return VariableBufferFormat::Format_TYPE_UINT_8|VariableBufferFormat::Format_COUNT_3;
		case GL_BOOL_VEC4:
			return VariableBufferFormat::Format_TYPE_UINT_8|VariableBufferFormat::Format_COUNT_4;
		case GL_FLOAT_MAT2:
			return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_2X2;
		case GL_FLOAT_MAT3:
			return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_3X3;
		case GL_FLOAT_MAT4:
			return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4X4;
		#if !defined(TOADLET_HAS_GLES)
			case GL_FLOAT_MAT2x3:
				return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_2X3;
			case GL_FLOAT_MAT2x4:
				return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_2X4;
			case GL_FLOAT_MAT3x2:
				return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_3X2;
			case GL_FLOAT_MAT3x4:
				return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_3X4;
			case GL_FLOAT_MAT4x2:
				return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4X2;
			case GL_FLOAT_MAT4x3:
				return VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4X3;
		#endif
		case GL_SAMPLER_2D:
		case GL_SAMPLER_CUBE:
		#if !defined(TOADLET_HAS_GLES)
			case GL_SAMPLER_1D:
			case GL_SAMPLER_3D:
			case GL_SAMPLER_1D_SHADOW:
			case GL_SAMPLER_2D_SHADOW:
		#endif
			return VariableBufferFormat::Format_TYPE_RESOURCE;
		default:
			return 0;
	}
}
#endif

#if !defined(TOADLET_HAS_GLES) || defined(TOADLET_HAS_GL_20)
	GLuint GLRenderDevice::GLCubeFaces[6]={
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};
#endif

void GLRenderDevice::vertexFormatCreated(GLVertexFormat *format){
	#if defined(TOADLET_THREADSAFE)
		mMutex.lock();
	#endif

	int handle=mVertexFormats.size();
	format->mRenderHandle=handle;
	mVertexFormats.resize(handle+1);
	mVertexFormats[handle]=format;

	#if defined(TOADLET_THREADSAFE)
		mMutex.unlock();
	#endif
}

void GLRenderDevice::vertexFormatDestroyed(GLVertexFormat *format){
	#if defined(TOADLET_THREADSAFE)
		mMutex.lock();
	#endif

	int handle=format->mRenderHandle;
	if(handle==-1 || mVertexFormats[handle]!=format){
		#if defined(TOADLET_THREADSAFE)
			mMutex.unlock();
		#endif

		return;
	}

	int i;
	#if defined(TOADLET_HAS_GLSL)
		for(i=mShaderStates.size()-1;i>=0;--i){
			GLSLShaderState *state=mShaderStates[i];
			if(handle<state->mLayouts.size()){
				GLSLVertexLayout *layout=state->mLayouts[handle];
				if(layout!=NULL){
					layout->destroy();
				}
				state->mLayouts.erase(state->mLayouts.begin()+handle);
			}
		}
	#endif
	for(i=handle;i<mVertexFormats.size();++i){
		mVertexFormats[i]->mRenderHandle--;
	}
	mVertexFormats.erase(mVertexFormats.begin()+handle);

	#if defined(TOADLET_THREADSAFE)
		mMutex.unlock();
	#endif
}

void GLRenderDevice::shaderStateCreated(GLSLShaderState *state){
	#if defined(TOADLET_THREADSAFE)
		mMutex.lock();
	#endif

	mShaderStates.push_back(state);

	#if defined(TOADLET_THREADSAFE)
		mMutex.unlock();
	#endif
}

void GLRenderDevice::shaderStateDestroyed(GLSLShaderState *state){
	#if defined(TOADLET_THREADSAFE)
		mMutex.lock();
	#endif

	mShaderStates.erase(stlit::remove(mShaderStates.begin(),mShaderStates.end(),state),mShaderStates.end());

	#if defined(TOADLET_THREADSAFE)
		mMutex.unlock();
	#endif
}

}
}
