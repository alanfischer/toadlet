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

#ifndef TOADLET_PEEPER_D3D9RENDERER_H
#define TOADLET_PEEPER_D3D9RENDERER_H

#include "D3D9Includes.h"
#include "D3D9RenderTarget.h"
#include <toadlet/peeper/Renderer.h>
#include <toadlet/peeper/Blend.h>
#include <toadlet/peeper/LightEffect.h>
#include <toadlet/peeper/TextureStage.h>
#include <toadlet/peeper/StatisticsSet.h>
#include <toadlet/peeper/CapabilitySet.h>
#include <toadlet/peeper/IndexData.h>
#include <toadlet/peeper/VertexElement.h>

namespace toadlet{
namespace peeper{

class D3D9Renderer:public Renderer{
public:
	D3D9Renderer();
	virtual ~D3D9Renderer();

	// Startup/Shutdown
	bool startup(RenderTarget *target,int *options);
	bool shutdown();
	RendererStatus getStatus();
	bool reset();

	// Resource operations
	Texture *createTexture();
	SurfaceRenderTarget *createSurfaceRenderTarget();
	BufferPeer *createBufferPeer(Buffer *buffer);
	ProgramPeer *createProgramPeer(Program *program);
	ShaderPeer *createShaderPeer(Shader *shader);

	// Matrix operations
	void setModelMatrix(const Matrix4x4 &matrix);
	void setViewMatrix(const Matrix4x4 &matrix);
	void setProjectionMatrix(const Matrix4x4 &matrix);

	// Rendering operations
	RenderTarget *getPrimaryRenderTarget(){return mPrimaryRenderTarget;}
	bool setRenderTarget(RenderTarget *target);
	RenderTarget *getRenderTarget(){return mRenderTarget;}
	void setViewport(const Viewport &viewport);
	void clear(int clearFlags,const Color &clearcolor);
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
	void setTexCoordGen(int stage,const TexCoordGen &texCoordGen,const Matrix4x4 &matrix);
	void getShadowBiasMatrix(const Texture *shadowTexture,Matrix4x4 &result);
	void setShadowComparisonMethod(bool enabled);

	// Light operations
	void setLight(int i,Light *light);
	void setLightEnabled(int i,bool enable);
	void setAmbientColor(const Color &ambient);

	// Misc operations
	void setMirrorY(bool mirrorY);
	void copyFrameBufferToTexture(Texture *texture);

	const StatisticsSet &getStatisticsSet();
	const CapabilitySet &getCapabilitySet();

	inline IDirect3DDevice9 *getDirect3DDevice9(){return mD3DDevice;}
	inline const D3DCAPS9 &getD3DCAPS9() const{return mD3DCaps;}

	Matrix4x4 cacheMatrix4x4;
	D3DMATRIX cacheD3DMatrix;

protected:
	static void setCapabilitySetFromCaps(CapabilitySet &capabilitySet,const D3DCAPS9 &caps,bool renderToTexture,bool renderToDepthTexture);
	static DWORD getD3DTextureBlendSource(TextureBlend::Source blend);
	static D3DBLEND getD3DBlendOperation(Blend::Operation blend);
	static void getPrimitiveTypeAndCount(D3DPRIMITIVETYPE &d3dpt,int &count,IndexData::Primitive prim,int numIndexes);

	IDirect3D9 *mD3D;
	IDirect3DDevice9 *mD3DDevice;
	D3DCAPS9 mD3DCaps;
	RenderTarget *mPrimaryRenderTarget;
	D3D9RenderTarget *mD3DPrimaryRenderTarget;
	RenderTarget *mRenderTarget;
	D3D9RenderTarget *mD3DRenderTarget;
	bool mShutdown;

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

	bool mMirrorY;

	StatisticsSet mStatisticsSet;
	CapabilitySet mCapabilitySet;
};

}
}

#endif
