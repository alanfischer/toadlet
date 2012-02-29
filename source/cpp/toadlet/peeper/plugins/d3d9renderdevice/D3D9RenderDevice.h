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

#ifndef TOADLET_PEEPER_D3D9RENDERDEVICE_H
#define TOADLET_PEEPER_D3D9RENDERDEVICE_H

#include "D3D9Includes.h"
#include "D3D9RenderTarget.h"
#include <toadlet/peeper/RenderDevice.h>
#include <toadlet/peeper/BlendState.h>
#include <toadlet/peeper/DepthState.h>
#include <toadlet/peeper/FogState.h>
#include <toadlet/peeper/MaterialState.h>
#include <toadlet/peeper/SamplerState.h>
#include <toadlet/peeper/TextureState.h>
#include <toadlet/peeper/RasterizerState.h>
#include <toadlet/peeper/RenderCaps.h>
#include <toadlet/peeper/IndexData.h>

namespace toadlet{
namespace peeper{

class D3D9Texture;

class D3D9RenderDevice:public Object,public RenderDevice{
public:
	TOADLET_OBJECT(D3D9RenderDevice);

	D3D9RenderDevice();
	virtual ~D3D9RenderDevice();

	// Startup/Shutdown
	bool create(RenderTarget *target,int *options);
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
	void clear(int clearFlags,const Vector4 &clearcolor);
	void swap();
	void beginScene();
	void endScene();
	void renderPrimitive(VertexData *vertexData,IndexData *indexData);
	bool copyFrameBufferToPixelBuffer(PixelBuffer *dst);
	bool copyPixelBuffer(PixelBuffer *dst,PixelBuffer *src);
	bool copySurface(IDirect3DSurface9 *dst,IDirect3DSurface9 *src);
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
	void getShadowBiasMatrix(const Texture *shadowTexture,Matrix4x4 &result);
	int getClosePixelFormat(int format,int usage);
	int getClosePixelFormat(int format);
	bool getShaderProfileSupported(const String &profile);
	bool getRenderCaps(RenderCaps &caps){caps.set(mCaps);return true;}
	const RenderCaps &getCaps() const{return mCaps;}

	/// @todo: Move these all to pointers once we have things working
	void setBlendState(const BlendState &state);
	void setDepthState(const DepthState &state);
	void setRasterizerState(const RasterizerState &state);
	void setFogState(const FogState &state);
	void setPointState(const PointState &state);
	void setMaterialState(const MaterialState &state);
	void setSamplerState(int i,SamplerState *state);
	void setSamplerStatePostTexture(int i,SamplerState *state);
	void setTextureState(int i,TextureState *state);

	inline IDirect3DDevice9 *getDirect3DDevice9(){return mD3DDevice;}
	inline const D3DCAPS9 &getD3DCAPS9() const{return mD3DCaps;}
	bool isD3DFORMATValid(D3DFORMAT textureFormat,DWORD usage);
	void updateProjectionMatrix(Matrix4x4 &matrix);

	Matrix4x4 cacheMatrix4x4;
	D3DMATRIX cacheD3DMatrix;

	static DWORD getD3DTOP(TextureState::Operation operation,TextureState::Source alphaSource);
	static D3DFORMAT getD3DFORMAT(int textureFormat);
	static DWORD getD3DUSAGE(int textureFormat,int usage);
	static D3DPOOL getD3DPOOL(int usage);
	static DWORD getD3DTADDRESS(SamplerState::AddressType addressMode);
	static DWORD getD3DTEXF(SamplerState::FilterType filter);
	static DWORD getD3DTA(TextureState::Source blend);
	static D3DBLEND getD3DBLEND(BlendState::Operation operation);
	static D3DCULL getD3DCULL(RasterizerState::CullType type);
	static D3DFILLMODE getD3DFILLMODE(RasterizerState::FillType type);
	static D3DSHADEMODE getD3DSHADEMODE(MaterialState::ShadeType type);
	static D3DFOGMODE getD3DFOGMODE(FogState::FogType type);
	static D3DCMPFUNC getD3DCMPFUNC(MaterialState::AlphaTest type);
	static D3DCMPFUNC getD3DCMPFUNC(DepthState::DepthTest type);
	static DWORD getFVF(VertexFormat *vertexFormat);
	static int getVariableFormat(const D3DXCONSTANT_DESC &desc);

protected:
	static void setCapsFromD3DCAPS9(RenderCaps &caps,const D3DCAPS9 &d3dcaps,bool renderToTexture,bool renderToDepthTexture);
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
	RenderState *mLastRenderState;
	Collection<D3D9Texture*> mLastTextures;
	Collection<SamplerState*> mLastSamplerStates;

	RenderCaps mCaps;
};

}
}

#endif
