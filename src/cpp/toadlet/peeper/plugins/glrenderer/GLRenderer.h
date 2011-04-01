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

#ifndef TOADLET_PEEPER_GLRENDERER_H
#define TOADLET_PEEPER_GLRENDERER_H

#include "GLIncludes.h"
#include "GLRenderTarget.h"
#include <toadlet/peeper/Renderer.h>
#include <toadlet/peeper/BlendState.h>
#include <toadlet/peeper/DepthState.h>
#include <toadlet/peeper/FogState.h>
#include <toadlet/peeper/MaterialState.h>
#include <toadlet/peeper/LightState.h>
#include <toadlet/peeper/PointState.h>
#include <toadlet/peeper/RasterizerState.h>
#include <toadlet/peeper/TextureStage.h>
#include <toadlet/peeper/Viewport.h>
#include <toadlet/peeper/CapabilityState.h>

namespace toadlet{
namespace peeper{

class GLBuffer;
class GLTexture;

class TOADLET_API GLRenderer:public Renderer{
public:
	int gl_version;

	// Options
	const static int Option_USE_PBUFFERS=1;
	const static int Option_USE_FBOS=2;
	const static int Option_USE_HARDWARE_BUFFERS=3;

	GLRenderer();
	virtual ~GLRenderer();

	// Startup/Shutdown
	bool create(RenderTarget *target,int *options);
	void destroy();
	RendererStatus getStatus(){return RendererStatus_OK;}
	bool reset();
	bool activateAdditionalContext();

	// Resource operations
	Texture *createTexture();
	PixelBufferRenderTarget *createPixelBufferRenderTarget();
	PixelBuffer *createPixelBuffer();
	VertexFormat *createVertexFormat();
	VertexBuffer *createVertexBuffer();
	IndexBuffer *createIndexBuffer();
	Program *createProgram();
	Shader *createShader();
	Query *createQuery();
	RenderStateSet *createRenderStateSet();

	// Matrix operations
	void setModelMatrix(const Matrix4x4 &matrix);
	void setViewMatrix(const Matrix4x4 &matrix);
	void setProjectionMatrix(const Matrix4x4 &matrix);

	// Rendering operations
	RenderTarget *getPrimaryRenderTarget(){return mPrimaryRenderTarget;}
	bool setRenderTarget(RenderTarget *target);
	RenderTarget *getRenderTarget(){return mRenderTarget;}
	void setViewport(const Viewport &viewport);
	void clear(int clearFlags,const Vector4 &clearColor);
	void swap();
	void beginScene();
	void endScene();
	void renderPrimitive(const VertexData::ptr &vertexData,const IndexData::ptr &indexData);
	bool copyFrameBufferToPixelBuffer(PixelBuffer *dst);
	bool copyPixelBuffer(PixelBuffer *dst,PixelBuffer *src);
	void setDefaultStates();
	bool setRenderStateSet(RenderStateSet *set);

	// Old fixed states
	void setTextureStage(int stage,TextureStage *textureStage);
	void setAlphaTest(const AlphaTest &alphaTest,scalar cutoff);
	void setNormalize(const Normalize &normalize);
	void setLightEnabled(int i,bool enable);
	void setLightState(int i,const LightState &state);
	void setAmbientColor(const Vector4 &ambient);

	// Misc operations
	void getShadowBiasMatrix(const Texture *shadowTexture,Matrix4x4 &result);
	int getClosestTextureFormat(int textureFormat);
	void setStrictFormats(bool strict){mStrict=strict;}
	bool getStrictFormats(){return mStrict;}
	const CapabilityState &getCapabilityState(){return mCapabilityState;}

	void setBlendState(const BlendState &state);
	void setDepthState(const DepthState &state);
	void setRasterizerState(const RasterizerState &state);
	void setFogState(const FogState &state);
	void setPointState(const PointState &state);
	void setMaterialState(const MaterialState &state);

	bool useMapping(GLBuffer *buffer) const;

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
	static GLenum getGLAlphaFunc(AlphaTest test);
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
	static GLuint getGLWrap(TextureStage::AddressMode addressMode,bool hasClampToEdge=true);
	static GLuint getGLMinFilter(TextureStage::Filter minFilter,TextureStage::Filter mipFilter);
	static GLuint getGLMagFilter(TextureStage::Filter magFilter);
	static GLuint getGLTextureBlendSource(TextureBlend::Source source);
	static GLuint getGLTextureBlendOperation(TextureBlend::Operation operation);
	static GLuint getGLDepthTextureMode(TextureStage::ShadowComparison comparison);
	static float getGLTextureBlendScale(TextureBlend::Operation operation);
	static GLuint GLClientStates[6];
	static GLuint GLCubeFaces[6];

protected:
	inline int setVertexData(const VertexData *vertexData,int lastTypeBits);

	int mMatrixMode;

	bool mPBuffersAvailable;
	bool mFBOsAvailable;

	Viewport mViewport;
	DepthState mDepthState;
	PointState mPointState;
	egg::Collection<GLTexture*> mLastTextures;
	short mMaxTexCoordIndex;
	egg::Collection<short> mTexCoordIndexes;
	short mLastMaxTexCoordIndex;
	egg::Collection<GLenum> mLastTexTargets;
	VertexData::wptr mLastVertexData;
	int mLastTypeBits;
	egg::Collection<short> mLastTexCoordIndexes;

	#if defined(TOADLET_FIXED_POINT) && defined(TOADLET_HAS_GLES)
		egg::mathfixed::Matrix4x4 mModelMatrix;
		egg::mathfixed::Matrix4x4 mViewMatrix;
	#else
		egg::math::Matrix4x4 mModelMatrix;
		egg::math::Matrix4x4 mViewMatrix;
	#endif

	CapabilityState mCapabilityState;
	bool mMultiTexture;

	RenderTarget *mPrimaryRenderTarget;
	GLRenderTarget *mGLPrimaryRenderTarget;
	RenderTarget *mRenderTarget;
	GLRenderTarget *mGLRenderTarget;
	bool mStrict;

	#if defined(TOADLET_DEBUG)
		int mBeginEndCounter;
	#endif

	Matrix4x4 cache_setTextureStage_transform;

	friend class GLBuffer;
};

}
}

#endif
