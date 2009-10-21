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

#ifndef TOADLET_PEEPER_D3DMRENDERER_H
#define TOADLET_PEEPER_D3DMRENDERER_H

#include "D3DMIncludes.h"
#include "D3DMRenderContextPeer.h"
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

class TOADLET_API D3DMRenderer:public Renderer{
public:
	D3DMRenderer();
	virtual ~D3DMRenderer();

	// Startup/Shutdown
	bool startup(RenderContext *renderContext,int *options);
	bool shutdown();
	RendererStatus getStatus();
	bool reset();

	// Resource operations
	TexturePeer *createTexturePeer(Texture *texture);
	BufferPeer *createBufferPeer(Buffer *buffer);
	ProgramPeer *createProgramPeer(Program *program);
	ShaderPeer *createShaderPeer(Shader *shader);

	// Matrix operations 
	void setModelMatrix(const Matrix4x4 &matrix);
	void setViewMatrix(const Matrix4x4 &matrix);
	void setProjectionMatrix(const Matrix4x4 &matrix);

	// Rendering operations
	RenderContext *getRenderContext();
	bool setRenderTarget(RenderTarget *target);
	RenderTarget *getRenderTarget();
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
	void setFogParameters(const Fog &fogType,scalar nearDistance,scalar farDistance,const Color &color);
	void setLightEffect(const LightEffect &lightEffect);
	void setFill(const Fill &fill);
	void setLighting(bool lighting);
	void setShading(const Shading &shading);
	void setColorWrite(bool color);
	void setNormalize(const Normalize &normalize);
	void setDepthBias(scalar constant,scalar slope);
	void setTexturePerspective(bool texturePerspective);
	void setPointParameters(bool sprite,scalar size,bool attenuated,scalar constant,scalar linear,scalar quadratic,scalar minSize,scalar maxSize){}
	void setTextureStage(int stage,TextureStage *textureStage);
	void setProgram(const Program *program);
	void setTexCoordGen(int stage,const TexCoordGen &tcg,const Matrix4x4 &matrix);
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

	inline IDirect3DMobileDevice *getDirect3DMobileDevice() const{return mD3DDevice;}
	inline const D3DMCAPS &getD3DMCAPS() const{return mD3DCaps;}

	D3DMMATRIX cacheD3DMatrix;

protected:
	static void setCapabilitySetFromCaps(CapabilitySet &capabilitySet,const D3DMCAPS &caps,bool renderToTexture,bool renderToDepthTexture);
	static DWORD getD3DTextureBlendSource(TextureBlend::Source blend);
	static D3DMBLEND getD3DBlendOperation(Blend::Operation blend);
	static void getPrimitiveTypeAndCount(D3DMPRIMITIVETYPE &d3dpt,int &count,IndexData::Primitive prim,int numIndexes);

	IDirect3DMobileDevice *mD3DDevice;
	D3DMCAPS mD3DCaps;
	D3DMRenderContextPeer *mD3DRenderContextPeer;
	RenderContext *mRenderContext;
	RenderTarget *mRenderTarget;
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
