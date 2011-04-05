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
#include <toadlet/peeper/BlendState.h>
#include <toadlet/peeper/DepthState.h>
#include <toadlet/peeper/MaterialState.h>
#include <toadlet/peeper/TextureStage.h>
#include <toadlet/peeper/RasterizerState.h>
#include <toadlet/peeper/CapabilityState.h>
#include <toadlet/peeper/IndexData.h>

namespace toadlet{
namespace peeper{

class D3D9Renderer:public Renderer{
public:
	D3D9Renderer();
	virtual ~D3D9Renderer();

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
	void clear(int clearFlags,const Vector4 &clearcolor);
	void swap();
	void beginScene();
	void endScene();
	void renderPrimitive(const VertexData::ptr &vertexData,const IndexData::ptr &indexData);
	bool copyFrameBufferToPixelBuffer(PixelBuffer *dst);
	bool copyPixelBuffer(PixelBuffer *dst,PixelBuffer *src);
	bool copySurface(IDirect3DSurface9 *dst,IDirect3DSurface9 *src);
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
	int getCloseTextureFormat(int format,int usage);
	int getCloseTextureFormat(int format);
	const CapabilityState &getCapabilityState(){return mCapabilityState;}

	void setBlendState(const BlendState &state);
	void setDepthState(const DepthState &state);
	void setRasterizerState(const RasterizerState &state);
	void setFogState(const FogState &state);
	void setPointState(const PointState &state);
	void setMaterialState(const MaterialState &state);

	inline IDirect3DDevice9 *getDirect3DDevice9(){return mD3DDevice;}
	inline const D3DCAPS9 &getD3DCAPS9() const{return mD3DCaps;}
	bool isD3DFORMATValid(D3DFORMAT textureFormat,DWORD usage);

	Matrix4x4 cacheMatrix4x4;
	D3DMATRIX cacheD3DMatrix;

	static DWORD getD3DTOP(TextureBlend::Operation operation,TextureBlend::Source alphaSource);
	static D3DFORMAT getD3DFORMAT(int textureFormat);
	static DWORD getD3DUSAGE(int textureFormat,int usage);
	static D3DPOOL getD3DPOOL(int usage);
	static DWORD getD3DTADDRESS(TextureStage::AddressMode addressMode);
	static DWORD getD3DTEXF(TextureStage::Filter filter);
	static DWORD getD3DTA(TextureBlend::Source blend);
	static D3DBLEND getD3DBLEND(BlendState::Operation operation);
	static D3DCULL getD3DCULL(RasterizerState::CullType type);
	static D3DFILLMODE getD3DFILLMODE(RasterizerState::FillType type);
	static D3DSHADEMODE getD3DSHADEMODE(MaterialState::ShadeType type);
	static DWORD getFVF(VertexFormat *vertexFormat);

protected:
	static void setCapabilityStateFromCaps(CapabilityState &caps,const D3DCAPS9 &d3dcaps,bool renderToTexture,bool renderToDepthTexture);
	static void getPrimitiveTypeAndCount(D3DPRIMITIVETYPE &d3dpt,int &count,IndexData::Primitive prim,int numIndexes);

	IDirect3D9 *mD3D;
	IDirect3DDevice9 *mD3DDevice;
	UINT mD3DAdapter;
	D3DDEVTYPE mD3DDevType;
	D3DFORMAT mD3DAdapterFormat;
	D3DCAPS9 mD3DCaps;
	RenderTarget *mPrimaryRenderTarget;
	D3D9RenderTarget *mD3DPrimaryRenderTarget;
	RenderTarget *mRenderTarget;
	D3D9RenderTarget *mD3DRenderTarget;

	CapabilityState mCapabilityState;

	Matrix4x4 cache_setTextureStage_transform;
};

}
}

#endif
