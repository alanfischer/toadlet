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

#ifndef TOADLET_PEEPER_D3D10RENDERDEVICE_H
#define TOADLET_PEEPER_D3D10RENDERDEVICE_H

#include "D3D10Includes.h"
#include "D3D10RenderTarget.h"
#include <toadlet/peeper/RenderDevice.h>
#include <toadlet/peeper/BlendState.h>
#include <toadlet/peeper/DepthState.h>
#include <toadlet/peeper/RasterizerState.h>
#include <toadlet/peeper/SamplerState.h>
#include <toadlet/peeper/RenderCaps.h>
#include <toadlet/peeper/RenderState.h>
#include <toadlet/peeper/IndexData.h>

namespace toadlet{
namespace peeper{

class D3D10Buffer;
class D3D10Shader;
class D3D10ShaderState;
class D3D10VertexFormat;

class D3D10RenderDevice:public RenderDevice{
public:
	D3D10RenderDevice();
	virtual ~D3D10RenderDevice();

	// Startup/Shutdown
	bool create(RenderTarget *target,int *options);
	void destroy();
	DeviceStatus getStatus();
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
	void setDefaultState();
	bool setRenderState(RenderState *renderState);
	bool setShaderState(ShaderState *shaderState);
	void setBuffer(int i,VariableBuffer *buffer);
	void setTexture(int stage,Texture *texture){}

	// Old fixed states
	void setMatrix(MatrixType type,const Matrix4x4 &matrix){}
	void setAmbientColor(const Vector4 &ambient){}
	void setLightState(int i,const LightState &light){}
	void setLightEnabled(int i,bool enable){}

	// Misc operations
	int getCloseTextureFormat(int textureFormat,int usage);
	bool getShaderProfileSupported(const egg::String &profile);
	void getShadowBiasMatrix(const Texture *shadowTexture,Matrix4x4 &result);
	bool getRenderCaps(RenderCaps &caps){caps.set(mCaps);return true;}

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
	static int getVariableFormat(const D3D10_SHADER_TYPE_DESC &type);

protected:
	void vertexFormatCreated(D3D10VertexFormat *format);
	void vertexFormatDestroyed(D3D10VertexFormat *format);
	void shaderCreated(D3D10Shader *shader);
	void shaderDestroyed(D3D10Shader *shader);

	ID3D10Device *mD3DDevice;
	RenderTarget *mPrimaryRenderTarget;
	D3D10RenderTarget *mD3DPrimaryRenderTarget;
	RenderTarget *mRenderTarget;
	D3D10RenderTarget *mD3DRenderTarget;

	D3D10ShaderState *mLastShaderState;
	egg::Collection<D3D10Shader*> mVertexShaders;
	egg::Collection<D3D10VertexFormat*> mVertexFormats;

	RenderCaps mCaps;

	RenderState::ptr mDefaultState;

	friend class D3D10Buffer;
	friend class D3D10Shader;
	friend class D3D10VertexFormat;
};

}
}

#endif
