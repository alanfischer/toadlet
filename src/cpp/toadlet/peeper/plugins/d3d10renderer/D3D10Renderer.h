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

#ifndef TOADLET_PEEPER_D3D10RENDERER_H
#define TOADLET_PEEPER_D3D10RENDERER_H

#include "D3D10Includes.h"
#include "D3D10RenderTarget.h"
#include <toadlet/peeper/Renderer.h>
#include <toadlet/peeper/Blend.h>
#include <toadlet/peeper/LightEffect.h>
#include <toadlet/peeper/TextureStage.h>
#include <toadlet/peeper/StatisticsSet.h>
#include <toadlet/peeper/CapabilitySet.h>
#include <toadlet/peeper/IndexData.h>

namespace toadlet{
namespace peeper{

class D3D10Renderer:public Renderer{
public:
LPD3D10EFFECT effect;
LPD3D10EFFECTTECHNIQUE technique;
ID3D10ShaderResourceView *texture;
D3D10_PASS_DESC passDesc;

	D3D10Renderer();
	virtual ~D3D10Renderer();

	// Startup/Shutdown
	bool create(RenderTarget *target,int *options);
	void destroy();
	RendererStatus getStatus();
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
	bool copyFrameBufferToPixelBuffer(PixelBuffer *dst);
	bool copyPixelBuffer(PixelBuffer *dst,PixelBuffer *src);

	// Render state operations
	void setDefaultStates();
	void setAlphaTest(const AlphaTest &alphaTest,scalar cutoff);
	void setBlend(const Blend &blend);
	void setDepthTest(const DepthTest &depthTest);
	void setDepthWrite(bool depthWrite);
	void setDithering(bool dithering);
	void setFaceCulling(const FaceCulling &faceCulling);
	void setFogState(const FogState &state);
	void setLighting(bool lighting);
	void setLightEffect(const LightEffect &lightEffect);
	void setFill(const Fill &fill);
	void setShading(const Shading &shading);
	void setColorWrite(bool r,bool b,bool g,bool a);
	void setNormalize(const Normalize &normalize);
	void setDepthBias(scalar constant,scalar slope);
	void setTexturePerspective(bool texturePerspective);
	void setPointState(const PointState &state);
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
	int getClosestTextureFormat(int textureFormat);
	void setStrictFormats(bool strict){mStrict=strict;}
	bool getStrictFormats(){return mStrict;}

	const StatisticsSet &getStatisticsSet(){return mStatisticsSet;}
	const CapabilitySet &getCapabilitySet(){return mCapabilitySet;}

	inline ID3D10Device *getD3D10Device(){return mD3DDevice;}

	static D3D10_PRIMITIVE_TOPOLOGY getD3D10_PRIMITIVE_TOPOLOGY(IndexData::Primitive primitive);
	static DXGI_FORMAT getIndexDXGI_FORMAT(IndexBuffer::IndexFormat format);
	static DXGI_FORMAT getVertexDXGI_FORMAT(int format);
	static DXGI_FORMAT getTextureDXGI_FORMAT(int textureFormat);
	static D3D10_MAP getD3D10_MAP(int access,int usage);
	static D3D10_USAGE getD3D10_USAGE(int usage);
	static char *getSemanticName(int semantic);

protected:
	ID3D10Device *mD3DDevice;
	RenderTarget *mPrimaryRenderTarget;
	D3D10RenderTarget *mD3DPrimaryRenderTarget;
	RenderTarget *mRenderTarget;
	D3D10RenderTarget *mD3DRenderTarget;
	bool mStrict;

	Matrix4x4 mModelMatrix;
	Matrix4x4 mViewMatrix;
	Matrix4x4 mProjectionMatrix;

	StatisticsSet mStatisticsSet;
	CapabilitySet mCapabilitySet;

	friend class D3D10Buffer;
};

}
}

#endif
