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

#ifndef TOADLET_PEEPER_GLRENDERDEVICE_H
#define TOADLET_PEEPER_GLRENDERDEVICE_H

#include "GLIncludes.h"
#include "GLRenderTarget.h"
#include <toadlet/peeper/RenderDevice.h>
#include <toadlet/peeper/BlendState.h>
#include <toadlet/peeper/DepthState.h>
#include <toadlet/peeper/FogState.h>
#include <toadlet/peeper/MaterialState.h>
#include <toadlet/peeper/LightState.h>
#include <toadlet/peeper/PointState.h>
#include <toadlet/peeper/RasterizerState.h>
#include <toadlet/peeper/SamplerState.h>
#include <toadlet/peeper/TextureState.h>
#include <toadlet/peeper/Viewport.h>
#include <toadlet/peeper/RenderCaps.h>
#include <toadlet/peeper/RenderState.h>

namespace toadlet{
namespace peeper{

class GLBuffer;
class GLSLShader;
class GLSLShaderState;
class GLTexture;
class GLVertexFormat;

class TOADLET_API GLRenderDevice:public Object,public RenderDevice{
public:
	TOADLET_OBJECT(GLRenderDevice);

	int gl_version;

	enum{
		Option_BIT_NOPBUFFERS=			1<<0,
		Option_BIT_NOFBOS=				1<<1,
		Option_BIT_NOHARDWAREBUFFERS=	1<<2,
		Option_BIT_NOFIXED=				1<<3,
		Option_BIT_NOSHADER=			1<<4,
	};

	GLRenderDevice();

	// Startup/Shutdown
	bool create(RenderTarget *target,int options);
	void destroy();
	DeviceStatus activate();
	bool reset();
	bool activateAdditionalContext();

	// Resource operations
	Texture *createTexture();
	PixelBufferRenderTarget *createPixelBufferRenderTarget();
	PixelBuffer *createPixelBuffer();
	VertexFormat *createVertexFormat();
	VertexBuffer *createVertexBuffer();
	IndexBuffer *createIndexBuffer();
	VariableBuffer *createVariableBuffer();
	Shader *createShader();
	Query *createQuery();
	RenderState *createRenderState();
	ShaderState *createShaderState();

	// Rendering operations
	RenderTarget *getPrimaryRenderTarget(){return mPrimaryRenderTarget;}
	bool setRenderTarget(RenderTarget *target);
	RenderTarget *getRenderTarget(){return mRenderTarget;}
	void setViewport(const Viewport &viewport);
	void clear(int clearFlags,const Vector4 &clearColor);
	void swap();
	void beginScene();
	void endScene();
	void renderPrimitive(VertexData *vertexData,IndexData *indexData);
	bool copyFrameBufferToPixelBuffer(PixelBuffer *dst);
	bool copyPixelBuffer(PixelBuffer *dst,PixelBuffer *src);
	void setDefaultState();
	bool setRenderState(RenderState *renderState);
	bool setShaderState(ShaderState *shaderState);
	void setBuffer(Shader::ShaderType shaderType,int i,VariableBuffer *buffer);
	void setTexture(Shader::ShaderType shaderType,int i,Texture *texture);

	// Old fixed states
	void setMatrix(MatrixType type,const Matrix4x4 &matrix);
	void setLightEnabled(int i,bool enable);
	void setLightState(int i,const LightState &state);
	void setAmbientColor(const Vector4 &ambient);

	// Misc operations
	int getClosePixelFormat(int pixelFormat,int usage);
	bool getShaderProfileSupported(const String &profile);
	void getShadowBiasMatrix(const Texture *shadowTexture,Matrix4x4 &result);
	bool getRenderCaps(RenderCaps &caps){caps.set(mCaps);return true;}

	void setBlendState(BlendState *state);
	void setDepthState(DepthState *state);
	void setRasterizerState(RasterizerState *state);
	void setFogState(FogState *state);
	void setPointState(PointState *state);
	void setMaterialState(MaterialState *state);
	void setSamplerState(int i,SamplerState *state);
	void setTextureState(int i,TextureState *state);
	void setSamplerStatePostTexture(int i,SamplerState *state);
	void setTextureStatePostTexture(int i,TextureState *state);

	bool hardwareBuffersSupported(GLBuffer *buffer) const;
	bool hardwareMappingSupported(GLBuffer *buffer) const;

	inline egg::mathfixed::fixed *colorArray(egg::mathfixed::fixed result[],const Vector4 &src){result[0]=MathConversion::scalarToFixed(src.x);result[1]=MathConversion::scalarToFixed(src.y);result[2]=MathConversion::scalarToFixed(src.z);result[3]=MathConversion::scalarToFixed(src.w);return result;}
	inline float *colorArray(float result[],const Vector4 &src){result[0]=MathConversion::scalarToFloat(src.x);result[1]=MathConversion::scalarToFloat(src.y);result[2]=MathConversion::scalarToFloat(src.z);result[3]=MathConversion::scalarToFloat(src.w);return result;}
	inline egg::mathfixed::fixed *lightDirArray(egg::mathfixed::fixed result[],const Vector3 &src){result[0]=MathConversion::scalarToFixed(-src.x);result[1]=MathConversion::scalarToFixed(-src.y);result[2]=MathConversion::scalarToFixed(-src.z);result[3]=0;return result;}
	inline float *lightDirArray(float result[],const Vector3 &src){result[0]=MathConversion::scalarToFloat(-src.x);result[1]=MathConversion::scalarToFloat(-src.y);result[2]=MathConversion::scalarToFloat(-src.z);result[3]=0;return result;}
	inline egg::mathfixed::fixed *lightPosArray(egg::mathfixed::fixed result[],const Vector3 &src){result[0]=MathConversion::scalarToFixed(src.x);result[1]=MathConversion::scalarToFixed(src.y);result[2]=MathConversion::scalarToFixed(src.z);result[3]=MathConversion::scalarToFixed(Math::ONE);return result;}
	inline float *lightPosArray(float result[],const Vector3 &src){result[0]=MathConversion::scalarToFloat(src.x);result[1]=MathConversion::scalarToFloat(src.y);result[2]=MathConversion::scalarToFloat(src.z);result[3]=MathConversion::scalarToFloat(Math::ONE);return result;}

	inline bool activeTexture(int i){
		if(i>=mCaps.maxTextureStages){
			return false;
		}
		else if(mMultiTexture && i!=mLastActiveTexture){
			glActiveTexture(GL_TEXTURE0+i);
			mLastActiveTexture=i;
		}
		return true;
	}

	#if defined(TOADLET_FIXED_POINT) && defined(TOADLET_HAS_GLES)
		egg::mathfixed::Matrix4x4 cacheMatrix4x4;
		egg::mathfixed::fixed cacheArray[4];
	#else
		egg::math::Matrix4x4 cacheMatrix4x4;
		float cacheArray[4];
	#endif

	static GLenum getGLDepthFunc(DepthState::DepthTest test);
	static GLenum getGLAlphaFunc(MaterialState::AlphaTest test);
	static GLenum getGLBlendOperation(BlendState::Operation operation);
	static GLenum getGLCullFace(RasterizerState::CullType type);
	static GLenum getGLPolygonMode(RasterizerState::FillType type);
	static GLenum getGLFogType(FogState::FogType type);
	static GLenum getGLShadeModel(MaterialState::ShadeType type);
	static GLint getGLElementCount(int format);
	static GLenum getGLDataType(int format);
	static GLuint getGLFormat(int textureFormat,bool internal);
	static GLuint getGLType(int textureFormat);
	static GLuint getGLIndexType(int indexFormat);
	static GLuint getGLTextureCompressed(int textureFormat);
	static GLuint getGLWrap(SamplerState::AddressType address,bool hasClampToEdge=true);
	static GLuint getGLMinFilter(SamplerState::FilterType minFilter,SamplerState::FilterType mipFilter);
	static GLuint getGLMagFilter(SamplerState::FilterType magFilter);
	static GLuint getGLTextureBlendSource(TextureState::Source source);
	static GLuint getGLTextureBlendOperation(TextureState::Operation operation);
	static float getGLTextureBlendScale(TextureState::Operation operation);
	static GLuint getGLDepthTextureMode(TextureState::ShadowResult shadow);
	static int getFixedAttribFromSemantic(int semantic,int index);
	static GLuint getClientStateFromSemantic(int semantic,int index);
	#if defined(TOADLET_HAS_GLSHADERS)
		static int getVariableFormat(GLuint type);
	#endif
	static GLuint GLCubeFaces[6];

protected:
	void setVertexData(const VertexData *vertexData);
	#if defined(TOADLET_HAS_GLFIXED)
		void setFixedVertexData(const VertexData *vertexData);
	#endif
	#if defined(TOADLET_HAS_GLSHADERS)
		void setShaderVertexData(const VertexData *vertexData);
	#endif

	void vertexFormatCreated(GLVertexFormat *format);
	void vertexFormatDestroyed(GLVertexFormat *format);
	void shaderStateCreated(GLSLShaderState *state);
	void shaderStateDestroyed(GLSLShaderState *state);

	bool mPBuffers,mFBOs;
	bool mVBOs,mIBOs,mPBOs,mUBOs;
	bool mNPOT,mNPOTR;

	Viewport mViewport;
	DepthState mDepthState;
	PointState mPointState;
	Collection<GLTexture*> mLastTextures;
	Collection<SamplerState*> mLastSamplerStates;
	Collection<TextureState*> mLastTextureStates;
	Collection<Shader::ShaderType> mLastTextureType;
	short mMaxTexCoordIndex;
	Collection<short> mTexCoordIndexes;
	short mLastMaxTexCoordIndex;
	Collection<GLenum> mLastTexTargets;
	Collection<VertexBuffer*> mLastVertexBuffers;
	int mLastFixedSemanticBits,mLastFixedTexCoordSemanticBits;
	Collection<short> mLastTexCoordIndexes;
	int mLastShaderSemanticBits;
	GLSLShaderState *mLastShaderState;
	int mLastActiveTexture;

	Collection<GLSLShaderState*> mShaderStates;
	Collection<GLVertexFormat*> mVertexFormats;
	#if defined(TOADLET_HAS_GLES)
		RasterizerState::FillType mRasterizerFill;
	#endif

	#if defined(TOADLET_FIXED_POINT) && defined(TOADLET_HAS_GLES)
		egg::mathfixed::Matrix4x4 mModelMatrix;
		egg::mathfixed::Matrix4x4 mViewMatrix;
	#else
		egg::math::Matrix4x4 mModelMatrix;
		egg::math::Matrix4x4 mViewMatrix;
	#endif

	RenderCaps mCaps;
	bool mMultiTexture;
	bool mHasClampToEdge;

	RenderState::ptr mDefaultState;

	RenderTarget::ptr mPrimaryRenderTarget;
	GLRenderTarget::ptr mGLPrimaryRenderTarget;
	RenderTarget::ptr mRenderTarget;
	GLRenderTarget::ptr mGLRenderTarget;

	friend class GLBuffer;
	friend class GLTexture;
	friend class GLVertexFormat;
	friend class GLSLShader;
	friend class GLSLShaderState;
};

}
}

#endif
