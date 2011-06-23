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

namespace toadlet{
namespace peeper{

class GLBuffer;
class GLSLShader;
class GLSLShaderState;
class GLTexture;
class GLVertexFormat;

class TOADLET_API GLRenderDevice:public RenderDevice{
public:
	int gl_version;

	// Options
	const static int Option_USE_PBUFFERS=1;
	const static int Option_USE_FBOS=2;
	const static int Option_USE_HARDWARE_BUFFERS=3;

	GLRenderDevice();
	virtual ~GLRenderDevice();

	// Startup/Shutdown
	bool create(RenderTarget *target,int *options);
	void destroy();
	DeviceStatus getStatus(){return DeviceStatus_OK;}
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
	void setBuffer(int i,VariableBuffer *buffer);
	void setTexture(int i,Texture *texture);

	// Old fixed states
	void setModelMatrix(const Matrix4x4 &matrix);
	void setViewMatrix(const Matrix4x4 &matrix);
	void setProjectionMatrix(const Matrix4x4 &matrix);
	void setNormalize(const Normalize &normalize);
	void setLightEnabled(int i,bool enable);
	void setLightState(int i,const LightState &state);
	void setAmbientColor(const Vector4 &ambient);

	// Misc operations
	int getCloseTextureFormat(int textureFormat,int usage);
	bool getShaderProfileSupported(const egg::String &profile);
	void getShadowBiasMatrix(const Texture *shadowTexture,Matrix4x4 &result);
	bool getRenderCaps(RenderCaps &caps){caps.set(mCaps);return true;}

	/// @todo: Move these all to pointers once we have things working
	void setBlendState(const BlendState &state);
	void setDepthState(const DepthState &state);
	void setRasterizerState(const RasterizerState &state);
	void setFogState(const FogState &state);
	void setPointState(const PointState &state);
	void setMaterialState(const MaterialState &state);
	void setSamplerState(int i,SamplerState *state);
	void setTextureState(int i,TextureState *state);
	void setSamplerStatePostTexture(int i,SamplerState *state);
	void setTextureStatePostTexture(int i,TextureState *state);

	bool hardwareBuffersSupported(GLBuffer *buffer) const;
	bool hardwareMappingSupported(GLBuffer *buffer) const;

	inline egg::mathfixed::fixed *colorArray(egg::mathfixed::fixed result[],const Vector4 &src){result[0]=egg::MathConversion::scalarToFixed(src.x);result[1]=egg::MathConversion::scalarToFixed(src.y);result[2]=egg::MathConversion::scalarToFixed(src.z);result[3]=egg::MathConversion::scalarToFixed(src.w);return result;}
	inline float *colorArray(float result[],const Vector4 &src){result[0]=egg::MathConversion::scalarToFloat(src.x);result[1]=egg::MathConversion::scalarToFloat(src.y);result[2]=egg::MathConversion::scalarToFloat(src.z);result[3]=egg::MathConversion::scalarToFloat(src.w);return result;}
	inline egg::mathfixed::fixed *lightDirArray(egg::mathfixed::fixed result[],const Vector3 &src){result[0]=egg::MathConversion::scalarToFixed(-src.x);result[1]=egg::MathConversion::scalarToFixed(-src.y);result[2]=egg::MathConversion::scalarToFixed(-src.z);result[3]=0;return result;}
	inline float *lightDirArray(float result[],const Vector3 &src){result[0]=egg::MathConversion::scalarToFloat(-src.x);result[1]=egg::MathConversion::scalarToFloat(-src.y);result[2]=egg::MathConversion::scalarToFloat(-src.z);result[3]=0;return result;}
	inline egg::mathfixed::fixed *lightPosArray(egg::mathfixed::fixed result[],const Vector3 &src){result[0]=egg::MathConversion::scalarToFixed(src.x);result[1]=egg::MathConversion::scalarToFixed(src.y);result[2]=egg::MathConversion::scalarToFixed(src.z);result[3]=egg::MathConversion::scalarToFixed(Math::ONE);return result;}
	inline float *lightPosArray(float result[],const Vector3 &src){result[0]=egg::MathConversion::scalarToFloat(src.x);result[1]=egg::MathConversion::scalarToFloat(src.y);result[2]=egg::MathConversion::scalarToFloat(src.z);result[3]=egg::MathConversion::scalarToFloat(Math::ONE);return result;}

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
	static GLuint getGLWrap(SamplerState::AddressType address,bool hasClampToEdge=true);
	static GLuint getGLMinFilter(SamplerState::FilterType minFilter,SamplerState::FilterType mipFilter);
	static GLuint getGLMagFilter(SamplerState::FilterType magFilter);
	static GLuint getGLTextureBlendSource(TextureState::Source source);
	static GLuint getGLTextureBlendOperation(TextureState::Operation operation);
	static float getGLTextureBlendScale(TextureState::Operation operation);
	static GLuint getGLDepthTextureMode(TextureState::ShadowResult shadow);
	static int getFixedAttribFromSemantic(int semantic,int index);
	static GLuint getClientStateFromSemantic(int semantic,int index);
	static int getVariableFormat(GLuint type);
	static GLuint GLCubeFaces[6];

protected:
	void setVertexData(const VertexData *vertexData);
	void setFixedVertexData(const VertexData *vertexData);
	void setShaderVertexData(const VertexData *vertexData);

	void vertexFormatCreated(GLVertexFormat *format);
	void vertexFormatDestroyed(GLVertexFormat *format);
	void shaderStateCreated(GLSLShaderState *state);
	void shaderStateDestroyed(GLSLShaderState *state);

	int mMatrixMode;

	bool mPBuffers,mFBOs;
	bool mVBOs,mIBOs,mPBOs,mUBOs;
	bool mNPOT,mNPOTR;

	Viewport mViewport;
	DepthState mDepthState;
	PointState mPointState;
	egg::Collection<GLTexture*> mLastTextures;
	egg::Collection<SamplerState*> mLastSamplerStates;
	egg::Collection<TextureState*> mLastTextureStates;
	short mMaxTexCoordIndex;
	egg::Collection<short> mTexCoordIndexes;
	short mLastMaxTexCoordIndex;
	egg::Collection<GLenum> mLastTexTargets;
	egg::Collection<VertexBuffer*> mLastVertexBuffers;
	int mLastFixedSemanticBits,mLastFixedTexCoordSemanticBits;
	egg::Collection<short> mLastTexCoordIndexes;
	int mLastShaderSemanticBits;
	GLSLShaderState *mLastShaderState;
	egg::Collection<GLSLShaderState*> mShaderStates;
	egg::Collection<GLVertexFormat*> mVertexFormats;

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

	RenderTarget *mPrimaryRenderTarget;
	GLRenderTarget *mGLPrimaryRenderTarget;
	RenderTarget *mRenderTarget;
	GLRenderTarget *mGLRenderTarget;

	friend class GLBuffer;
	friend class GLTexture;
	friend class GLVertexFormat;
	friend class GLSLShader;
	friend class GLSLShaderState;
};

}
}

#endif
