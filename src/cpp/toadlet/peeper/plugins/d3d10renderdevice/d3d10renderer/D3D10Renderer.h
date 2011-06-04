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
#include <toadlet/peeper/BlendState.h>
#include <toadlet/peeper/DepthState.h>
#include <toadlet/peeper/RasterizerState.h>
#include <toadlet/peeper/SamplerState.h>
#include <toadlet/peeper/RendererCaps.h>
#include <toadlet/peeper/RenderState.h>
#include <toadlet/peeper/IndexData.h>

namespace toadlet{
namespace peeper{

class D3D10Renderer:public Renderer{
public:
LPD3D10EFFECT effect;
LPD3D10EFFECTTECHNIQUE technique;
D3D10_PASS_DESC passDesc;
ID3D10SamplerState *samp;

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
	RenderState *createRenderState();

	// Matrix operations
	void setModelMatrix(const Matrix4x4 &matrix);
	void setViewMatrix(const Matrix4x4 &matrix);
	void setProjectionMatrix(const Matrix4x4 &matrix);

	// Rendering operations
	RenderTarget *getPrimaryRenderTarget(){return mPrimaryRenderTarget;}
	bool setRenderTarget(RenderTarget *target);
	RenderTarget *getRenderTarget(){return mRenderTarget;}
	void setViewport(const Viewport &viewport);
	void clear(int clearFlags,const Vector4 &clearcolor);
	void swap();
	void beginScene();
	void endScene();
	void renderPrimitive(const VertexData::ptr &vertexData,const IndexData::ptr &indexData);
	bool copyFrameBufferToPixelBuffer(PixelBuffer *dst);
	bool copyPixelBuffer(PixelBuffer *dst,PixelBuffer *src);
	void setDefaultState();
	bool setRenderState(RenderState *renderState);
	void setTexture(int stage,Texture *texture);

	// Old fixed states
	void setNormalize(const Normalize &normalize){}
	void setAmbientColor(const Vector4 &ambient){}
	void setLightState(int i,const LightState &light){}
	void setLightEnabled(int i,bool enable){}

	// Misc operations
	int getCloseTextureFormat(int textureFormat,int usage);
	void getShadowBiasMatrix(const Texture *shadowTexture,Matrix4x4 &result);
	bool getRendererCaps(RendererCaps &caps){caps.set(mCaps);return true;}

	inline ID3D10Device *getD3D10Device(){return mD3DDevice;}

	static D3D10_PRIMITIVE_TOPOLOGY getD3D10_PRIMITIVE_TOPOLOGY(IndexData::Primitive primitive);
	static DXGI_FORMAT getIndexDXGI_FORMAT(IndexBuffer::IndexFormat format);
	static DXGI_FORMAT getVertexDXGI_FORMAT(int format);
	static DXGI_FORMAT getTextureDXGI_FORMAT(int textureFormat);
	static D3D10_MAP getD3D10_MAP(int access,int usage);
	static D3D10_USAGE getD3D10_USAGE(int usage);
	static D3D10_BLEND getD3D10_BLEND(BlendState::Operation operation,bool alpha);
	static D3D10_COMPARISON_FUNC getD3D10_COMPARISON_FUNC(DepthState::DepthTest test);
	static D3D10_FILL_MODE getD3D10_FILL_MODE(RasterizerState::FillType type);
	static D3D10_CULL_MODE getD3D10_CULL_MODE(RasterizerState::CullType type);
	static D3D10_FILTER getD3D10_FILTER(SamplerState::FilterType minFilter,SamplerState::FilterType magFilter,SamplerState::FilterType mipFilter);
	static D3D10_TEXTURE_ADDRESS_MODE getD3D10_TEXTURE_ADDRESS_MODE(SamplerState::AddressType type);
	static void getD3D10_BLEND_DESC(D3D10_BLEND_DESC &desc,const BlendState &state);
	static void getD3D10_DEPTH_STENCIL_DESC(D3D10_DEPTH_STENCIL_DESC &desc,const DepthState &state);
	static void getD3D10_RASTERIZER_DESC(D3D10_RASTERIZER_DESC &desc,const RasterizerState &state);
	static void getD3D10_SAMPLER_DESC(D3D10_SAMPLER_DESC &desc,const SamplerState &state);
	static char *getSemanticName(int semantic);

protected:
	ID3D10Device *mD3DDevice;
	RenderTarget *mPrimaryRenderTarget;
	D3D10RenderTarget *mD3DPrimaryRenderTarget;
	RenderTarget *mRenderTarget;
	D3D10RenderTarget *mD3DRenderTarget;

	Matrix4x4 mModelMatrix;
	Matrix4x4 mViewMatrix;
	Matrix4x4 mProjectionMatrix;

	RendererCaps mCaps;

	RenderState::ptr mDefaultState;

	friend class D3D10Buffer;
};

}
}

#endif
