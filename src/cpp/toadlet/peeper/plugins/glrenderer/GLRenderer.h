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
#include <toadlet/peeper/Blend.h>
#include <toadlet/peeper/LightEffect.h>
#include <toadlet/peeper/TextureStage.h>
#include <toadlet/peeper/StatisticsSet.h>
#include <toadlet/peeper/CapabilitySet.h>
#include <toadlet/peeper/VertexElement.h>

namespace toadlet{
namespace peeper{

class GLBuffer;

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
	bool destroy();
	RendererStatus getStatus(){return RendererStatus_OK;}
	bool reset();

	// Resource operations
	Texture *createTexture();
	SurfaceRenderTarget *createSurfaceRenderTarget();
	VertexBuffer *createVertexBuffer();
	IndexBuffer *createIndexBuffer();
	Program *createProgram();
	Shader *createShader();

	// Matrix operations
	void setModelMatrix(const Matrix4x4 &matrix);
	void setViewMatrix(const Matrix4x4 &matrix);
	void setProjectionMatrix(const Matrix4x4 &matrix);

	// Rendering operations
	RenderTarget *getPrimaryRenderTarget(){return mPrimaryRenderTarget;}
	bool setRenderTarget(RenderTarget *target);
	RenderTarget *getRenderTarget(){return mRenderTarget;}
	void setViewport(const Viewport &viewport);
	void clear(int clearFlags,const Color &clearColor);
	void swap();
	void beginScene();
	void endScene();
	void renderPrimitive(const VertexData::ptr &vertexData,const IndexData::ptr &indexData);

	// Render state operations
	void setDefaultStates();
	void setAlphaTest(const AlphaTest &alphaTest,scalar cutoff);
	void setBlend(const Blend &blend);
	void setFaceCulling(const FaceCulling &faceCulling);
	void setDepthTest(const DepthTest &depthTest);
	void setDepthWrite(bool depthWrite);
	void setDithering(bool dithering);
	void setFogParameters(const Fog &fog,scalar nearDistance,scalar farDistance,const Color &color);
	void setLightEffect(const LightEffect &lightEffect);
	void setFill(const Fill &fill);
	void setLighting(bool lighting);
	void setShading(const Shading &shading);
	void setColorWrite(bool color);
	void setNormalize(const Normalize &normalize);
	void setDepthBias(scalar constant,scalar slope);
	void setTexturePerspective(bool texturePerspective);
	void setPointParameters(bool sprite,scalar size,bool attenuated,scalar constant,scalar linear,scalar quadratic,scalar minSize,scalar maxSize);
	void setTextureStage(int stage,TextureStage *textureStage);
	void setProgram(const Program *program);
	void setShadowComparisonMethod(bool enabled);

	// Light operations
	void setLight(int i,Light *light);
	void setLightEnabled(int i,bool enable);
	void setAmbientColor(const Color &ambient);

	// Misc operations
	void setMirrorY(bool mirrorY);
	void getShadowBiasMatrix(const Texture *shadowTexture,Matrix4x4 &result);

	const StatisticsSet &getStatisticsSet(){return mStatisticsSet;}
	const CapabilitySet &getCapabilitySet(){return mCapabilitySet;}

	bool useMapping(GLBuffer *buffer) const;

	inline egg::mathfixed::fixed *colorArray(egg::mathfixed::fixed result[],const Color &src){result[0]=egg::MathConversion::scalarToFixed(src.r);result[1]=egg::MathConversion::scalarToFixed(src.g);result[2]=egg::MathConversion::scalarToFixed(src.b);result[3]=egg::MathConversion::scalarToFixed(src.a);return result;}
	inline float *colorArray(float result[],const Color &src){result[0]=egg::MathConversion::scalarToFloat(src.r);result[1]=egg::MathConversion::scalarToFloat(src.g);result[2]=egg::MathConversion::scalarToFloat(src.b);result[3]=egg::MathConversion::scalarToFloat(src.a);return result;}
	inline egg::mathfixed::fixed *lightDirArray(egg::mathfixed::fixed result[],const Vector3 &src){result[0]=egg::MathConversion::scalarToFixed(-src.x);result[1]=egg::MathConversion::scalarToFixed(-src.y);result[2]=egg::MathConversion::scalarToFixed(-src.z);result[3]=0;return result;}
	inline float *lightDirArray(float result[],const Vector3 &src){result[0]=egg::MathConversion::scalarToFloat(-src.x);result[1]=egg::MathConversion::scalarToFloat(-src.y);result[2]=egg::MathConversion::scalarToFloat(-src.z);result[3]=0;return result;}
	inline egg::mathfixed::fixed *lightPosArray(egg::mathfixed::fixed result[],const Vector3 &src){result[0]=egg::MathConversion::scalarToFixed(src.x);result[1]=egg::MathConversion::scalarToFixed(-src.y);result[2]=egg::MathConversion::scalarToFixed(-src.z);result[3]=egg::MathConversion::scalarToFixed(Math::ONE);return result;}
	inline float *lightPosArray(float result[],const Vector3 &src){result[0]=egg::MathConversion::scalarToFloat(src.x);result[1]=egg::MathConversion::scalarToFloat(-src.y);result[2]=egg::MathConversion::scalarToFloat(-src.z);result[3]=egg::MathConversion::scalarToFloat(Math::ONE);return result;}

	#if defined(TOADLET_FIXED_POINT) && defined(TOADLET_HAS_GLES)
		egg::mathfixed::Matrix4x4 cacheMatrix4x4;
		egg::mathfixed::fixed cacheArray[4];
	#else
		egg::math::Matrix4x4 cacheMatrix4x4;
		float cacheArray[4];
	#endif

	static GLenum getGLDepthFunc(DepthTest alphaTest);
	static GLenum getGLAlphaFunc(AlphaTest alphaTest);
	static GLenum getGLBlendOperation(Blend::Operation blend);
	static GLint getGLElementCount(int format);
	static GLenum getGLDataType(int format);

protected:
	inline int setVertexData(const VertexData *vertexData,int lastTypeBits);

	bool mShutdown;
	int mMatrixMode;

	bool mPBuffersAvailable;
	bool mFBOsAvailable;

	// Cached render state attributes
	AlphaTest mAlphaTest;
	scalar mAlphaCutoff;
	Blend mBlend;
	DepthTest mDepthTest;
	bool mDepthWrite;
	bool mDithering;
	FaceCulling mFaceCulling;
	Fill mFill;
	bool mLighting;
	Normalize mNormalize;
	Shading mShading;
	bool mTexturePerspective;
	bool mInTexGen;

	egg::Collection<Texture*> mLastTextures;

	unsigned short mMaxTexCoordIndex;
	egg::Collection<short> mTexCoordIndexes;
	egg::Collection<GLenum> mLastTexTargets;
	VertexData::wptr mLastVertexData;
	int mLastTypeBits;
	egg::Collection<short> mLastTexCoordIndexes;

	bool mMirrorY;

	#if defined(TOADLET_FIXED_POINT) && defined(TOADLET_HAS_GLES)
		egg::mathfixed::Matrix4x4 mModelMatrix;
		egg::mathfixed::Matrix4x4 mViewMatrix;
	#else
		egg::math::Matrix4x4 mModelMatrix;
		egg::math::Matrix4x4 mViewMatrix;
	#endif

	StatisticsSet mStatisticsSet;
	CapabilitySet mCapabilitySet;

	RenderTarget *mPrimaryRenderTarget;
	GLRenderTarget *mGLPrimaryRenderTarget;
	RenderTarget *mRenderTarget;
	GLRenderTarget *mGLRenderTarget;

	class ScratchBuffer{
	public:
		uint8 *data;
		int size;
		bool free;
	};
	egg::Collection<ScratchBuffer> mScratchBuffers;

	#if defined(TOADLET_DEBUG)
		int mBeginEndCounter;
	#endif

	friend class GLBuffer;
};

}
}

#endif
