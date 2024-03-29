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

#include "D3D10RenderDevice.h"
#include "D3D10Buffer.h"
#include "D3D10TextureMipPixelBuffer.h"
#include "D3D10PixelBufferRenderTarget.h"
#include "D3D10Query.h"
#include "D3D10RenderState.h"
#include "D3D10RenderTarget.h"
#include "D3D10Shader.h"
#include "D3D10ShaderState.h"
#include "D3D10Texture.h"
#include "D3D10VertexFormat.h"
#include <toadlet/egg/MathConversion.h>
#include <toadlet/peeper/MaterialState.h>
#include <toadlet/peeper/LightState.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/peeper/Viewport.h>

using namespace toadlet::egg::MathConversion;

namespace toadlet{
namespace peeper{

#if defined(TOADLET_SET_D3D10)
	TOADLET_C_API RenderDevice* new_D3D10RenderDevice(){
		return new D3D10RenderDevice();
	}
#else
	TOADLET_C_API RenderDevice* new_D3D11RenderDevice(){
		return new D3D10RenderDevice();
	}
#endif

#if defined(TOADLET_BUILD_DYNAMIC)
	TOADLET_C_API RenderDevice* new_RenderDevice(){
		return new D3D10RenderDevice();
	}
#endif

D3D10RenderDevice::D3D10RenderDevice():
	mD3DDevice(NULL),
	mPrimaryRenderTarget(NULL),
	mD3DPrimaryRenderTarget(NULL),
	mRenderTarget(NULL),
	mD3DRenderTarget(NULL),

	mLastShaderState(NULL)
	//mVertexShaders,
	//mVertexFormats
{}

bool D3D10RenderDevice::create(RenderTarget *target,int options){
	Log::alert(Categories::TOADLET_PEEPER,
		"creating "+Categories::TOADLET_PEEPER+".D3D10RenderDevice");

	if(target==NULL){
		Error::nullPointer(Categories::TOADLET_PEEPER,
			"NULL RenderTarget");
		return false;
	}

	D3D10RenderTarget *d3dtarget=(D3D10RenderTarget*)target->getRootRenderTarget();
	mD3DDevice=d3dtarget->getD3D10Device();
	if(mD3DDevice==NULL){
		Error::unknown(Categories::TOADLET_PEEPER,
			"invalid device");
		return false;
	}

	mPrimaryRenderTarget=target;
	mD3DPrimaryRenderTarget=d3dtarget;
	mRenderTarget=target;
	mD3DRenderTarget=d3dtarget;

	if(mD3DRenderTarget!=NULL){
		mD3DRenderTarget->activate();
	}

	RenderCaps &caps=mCaps;
	{
		caps.resetOnResize=true;
		caps.maxTextureStages=16;
		caps.maxTextureSize=8192;
		caps.textureDot3=false;
		caps.textureNonPowerOf2=true;
		caps.textureNonPowerOf2Restricted=true;
		caps.textureAutogenMipMaps=true;
		caps.renderToTexture=true;
		caps.renderToDepthTexture=true;
		caps.renderToTextureNonPowerOf2Restricted=true;
		caps.idealVertexFormatType=VertexFormat::Format_TYPE_FLOAT_32;
		caps.triangleFan=false;

		caps.hasShader[Shader::ShaderType_VERTEX]=true;
		caps.hasShader[Shader::ShaderType_FRAGMENT]=true;
		caps.hasShader[Shader::ShaderType_GEOMETRY]=true;
	}

	mDefaultState=new D3D10RenderState(this);
	mDefaultState->setBlendState(BlendState());
	mDefaultState->setDepthState(DepthState());
	mDefaultState->setRasterizerState(RasterizerState());

	setDefaultState();
	
	Log::alert(Categories::TOADLET_PEEPER,
		"created D3D10RenderDevice");

	return true;
}

void D3D10RenderDevice::destroy(){
	mDefaultState->destroy();
}

RenderDevice::DeviceStatus D3D10RenderDevice::activate(){
	return DeviceStatus_OK;
}

bool D3D10RenderDevice::reset(){
	mD3DPrimaryRenderTarget->reset();

	setDefaultState();

	return true;
}

bool D3D10RenderDevice::activateAdditionalContext(){
	return (mD3DDevice->GetCreationFlags() & D3D10_CREATE_DEVICE_SINGLETHREADED)==0;
}

Texture *D3D10RenderDevice::createTexture(){
	return new D3D10Texture(this);
}

PixelBufferRenderTarget *D3D10RenderDevice::createPixelBufferRenderTarget(){
	return new D3D10PixelBufferRenderTarget(this);
}

PixelBuffer *D3D10RenderDevice::createPixelBuffer(){
	return new D3D10TextureMipPixelBuffer(this);
}

VertexFormat *D3D10RenderDevice::createVertexFormat(){
	return new D3D10VertexFormat(this);
}

VertexBuffer *D3D10RenderDevice::createVertexBuffer(){
	return new D3D10Buffer(this);
}

IndexBuffer *D3D10RenderDevice::createIndexBuffer(){
	return new D3D10Buffer(this);
}

VariableBuffer *D3D10RenderDevice::createVariableBuffer(){
	return new D3D10Buffer(this);
}

Shader *D3D10RenderDevice::createShader(){
	return new D3D10Shader(this);
}

Query *D3D10RenderDevice::createQuery(){
	return new D3D10Query(this);
}

RenderState *D3D10RenderDevice::createRenderState(){
	return new D3D10RenderState(this);
}

ShaderState *D3D10RenderDevice::createShaderState(){
	return new D3D10ShaderState(this);
}

bool D3D10RenderDevice::setRenderTarget(RenderTarget *target){
	D3D10RenderTarget *d3dtarget=target!=NULL?(D3D10RenderTarget*)target->getRootRenderTarget():NULL;
	if(d3dtarget==NULL || mRenderTarget==target){
		return false;
	}

	if(mD3DRenderTarget!=NULL){
		mD3DRenderTarget->deactivate();
	}

	mRenderTarget=target;
	mD3DRenderTarget=d3dtarget;

	if(mD3DRenderTarget!=NULL){
		mD3DRenderTarget->activate();
	}

	return true;
}

void D3D10RenderDevice::setViewport(const Viewport &viewport){
	D3D10_VIEWPORT d3dviewport;
	d3dviewport.TopLeftX=viewport.x;
	d3dviewport.TopLeftY=viewport.y;
	d3dviewport.Width=viewport.width;
	d3dviewport.Height=viewport.height;
	d3dviewport.MinDepth=0.0f;
	d3dviewport.MaxDepth=1.0f;

	mD3DDevice->RSSetViewports(1,&d3dviewport);
}

void D3D10RenderDevice::clear(int clearFlags,const Vector4 &clearColor){
	mD3DRenderTarget->clear(clearFlags,clearColor);
}

void D3D10RenderDevice::swap(){
	mD3DRenderTarget->swap();
}

void D3D10RenderDevice::beginScene(){
}

void D3D10RenderDevice::endScene(){
	ID3D10Buffer *buffer=NULL;
	UINT stride=0;
	UINT offset=0;
	mD3DDevice->IASetVertexBuffers(0,1,&buffer,&stride,&offset);

	mD3DDevice->IASetIndexBuffer(NULL,(DXGI_FORMAT)0,0);

	mD3DDevice->IASetInputLayout(NULL);

	setShaderState(NULL);
}

void D3D10RenderDevice::renderPrimitive(VertexData *vertexData,IndexData *indexData){
	if(vertexData==NULL || indexData==NULL){
		Error::nullPointer(Categories::TOADLET_PEEPER,
			"VertexData or IndexData is NULL");
		return;
	}

	if(vertexData->vertexBuffers.size()>1){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D10RenderDevice does not support multiple streams yet");
		return;
	}

	if(	mLastShaderState==NULL || 
		mLastShaderState->getShader(Shader::ShaderType_VERTEX)==NULL ||
		mLastShaderState->getShader(Shader::ShaderType_FRAGMENT)==NULL
	){
		Log::warning(Categories::TOADLET_PEEPER,
			"D3D10RenderDevice requires a vertex and fragment shader");
		return;
	}

	D3D10VertexFormat *d3dVertexFormat=(D3D10VertexFormat*)vertexData->getVertexFormat()->getRootVertexFormat();

	// Set input layout
	ID3D10InputLayout *id3dLayout=NULL;
	if(mLastShaderState!=NULL){
		Shader *shader=mLastShaderState->getShader(Shader::ShaderType_VERTEX);
		if(shader!=NULL){
			id3dLayout=((D3D10Shader*)shader->getRootShader())->findInputLayout(d3dVertexFormat);
		}
	}

	mD3DDevice->IASetInputLayout(id3dLayout);

	// Set vertex buffer
	D3D10Buffer *d3dVertexBuffer=(D3D10Buffer*)vertexData->getVertexBuffer(0)->getRootVertexBuffer();
	d3dVertexFormat=(D3D10VertexFormat*)d3dVertexBuffer->mVertexFormat->getRootVertexFormat();

	UINT stride=d3dVertexBuffer->mVertexFormat->getVertexSize();
	UINT offset=0;
	mD3DDevice->IASetVertexBuffers(0,1,&d3dVertexBuffer->mBuffer,&stride,&offset);

	if(indexData->getIndexBuffer()!=NULL){
		D3D10Buffer *d3dIndexBuffer=(D3D10Buffer*)(indexData->getIndexBuffer()->getRootIndexBuffer());
		mD3DDevice->IASetIndexBuffer(d3dIndexBuffer->mBuffer,getIndexDXGI_FORMAT(d3dIndexBuffer->getIndexFormat()),0);
	}

	mD3DDevice->IASetPrimitiveTopology(getD3D10_PRIMITIVE_TOPOLOGY(indexData->primitive));

	if(indexData->getIndexBuffer()){
		mD3DDevice->DrawIndexed(indexData->count,indexData->start,0);
	}
	else{
		mD3DDevice->Draw(indexData->count,indexData->start);
	}
}

bool D3D10RenderDevice::copyFrameBufferToPixelBuffer(PixelBuffer *dst){
/*	D3D9Surface *d3dsurface=(D3D9Surface*)surface->getRootSurface();

	IDirect3DSurface9 *currentSurface=NULL;
	#if defined(TOADLET_HAS_DIRECT3DMOBILE)
		mD3DDevice->GetRenderTarget(&currentSurface);
	#else
		mD3DDevice->GetRenderTarget(0,&currentSurface);
	#endif

	D3DSURFACE_DESC desc;
	HRESULT result=currentSurface->GetDesc(&desc);
	TOADLET_CHECK_D3D9ERROR(result,"D3D10RenderDevice: GetDesc");

	RECT rect={0};
	rect.right=desc.Width<d3dsurface->mWidth?desc.Width:d3dsurface->mWidth;
	rect.bottom=desc.Height<d3dsurface->mHeight?desc.Height:d3dsurface->mHeight;

	result=mD3DDevice->StretchRect(currentSurface,&rect,d3dsurface->mSurface,&rect,D3DTEXF_NONE);
	TOADLET_CHECK_D3D9ERROR(result,"D3D10RenderDevice: StretchRect");

	currentSurface->Release();

	return true;
*/
return false;
}

bool D3D10RenderDevice::copyPixelBuffer(PixelBuffer *dst,PixelBuffer *src){
	D3D10TextureMipPixelBuffer *d3dDst=(D3D10TextureMipPixelBuffer*)dst->getRootPixelBuffer();
	D3D10TextureMipPixelBuffer *d3dSrc=(D3D10TextureMipPixelBuffer*)src->getRootPixelBuffer();

	int dstSubRes=D3D10CalcSubresource(d3dDst->mLevel,0,0);
	int srcSubRes=D3D10CalcSubresource(d3dSrc->mLevel,0,0);

	mD3DDevice->CopySubresourceRegion(
		d3dDst->mD3DTexture,dstSubRes,
		0,0,0,
		d3dSrc->mD3DTexture,srcSubRes,
		NULL
	);

	return false;
}

void D3D10RenderDevice::setDefaultState(){
	if(mNullShaderResources.size()>0){
		mD3DDevice->VSSetShaderResources(0,mNullShaderResources.size(),&mNullShaderResources[0]);
		mD3DDevice->PSSetShaderResources(0,mNullShaderResources.size(),&mNullShaderResources[0]);
		mD3DDevice->GSSetShaderResources(0,mNullShaderResources.size(),&mNullShaderResources[0]);
		mNullShaderResources.clear();
	}

	setRenderState(mDefaultState);
}

bool D3D10RenderDevice::setRenderState(RenderState *renderState){
	D3D10RenderState *d3drenderState=renderState!=NULL?(D3D10RenderState*)renderState->getRootRenderState():NULL;
	if(d3drenderState==NULL){
		return false;
	}

	if(d3drenderState->mD3DBlendState!=NULL){
		mD3DDevice->OMSetBlendState(d3drenderState->mD3DBlendState,NULL,-1);
	}
	if(d3drenderState->mD3DDepthStencilState!=NULL){
		mD3DDevice->OMSetDepthStencilState(d3drenderState->mD3DDepthStencilState,-1);
	}
	if(d3drenderState->mD3DRasterizerState!=NULL){
		mD3DDevice->RSSetState(d3drenderState->mD3DRasterizerState);
	}
	int j;
	for(j=0;j<Shader::ShaderType_MAX;++j){
		if(d3drenderState->mD3DSamplerStates[j].size()>0){
			switch(j){
				case Shader::ShaderType_VERTEX:
					mD3DDevice->VSSetSamplers(0,d3drenderState->mD3DSamplerStates[j].size(),&d3drenderState->mD3DSamplerStates[j][0]);
				break;
				case Shader::ShaderType_FRAGMENT:
					mD3DDevice->PSSetSamplers(0,d3drenderState->mD3DSamplerStates[j].size(),&d3drenderState->mD3DSamplerStates[j][0]);
				break;
				case Shader::ShaderType_GEOMETRY:
					mD3DDevice->GSSetSamplers(0,d3drenderState->mD3DSamplerStates[j].size(),&d3drenderState->mD3DSamplerStates[j][0]);
				break;
			}
		}
	}

	return true;
}

bool D3D10RenderDevice::setShaderState(ShaderState *shaderState){
	D3D10ShaderState *d3dshaderState=shaderState!=NULL?(D3D10ShaderState*)shaderState->getRootShaderState():NULL;

	bool result=true;
	if(d3dshaderState!=NULL){
		result=d3dshaderState->activate();
	}
	else{
		mD3DDevice->VSSetShader(NULL);
		mD3DDevice->PSSetShader(NULL);
		mD3DDevice->GSSetShader(NULL);
	}

	mLastShaderState=d3dshaderState;

	return result;
}

void D3D10RenderDevice::setBuffer(Shader::ShaderType shaderType,int i,VariableBuffer *buffer){
	D3D10Buffer *d3dbuffer=buffer!=NULL?(D3D10Buffer*)buffer->getRootVariableBuffer():NULL;
	ID3D10Buffer *constantBuffer=NULL;
	if(d3dbuffer!=NULL){
		constantBuffer=d3dbuffer->mBuffer;
	}

	switch(shaderType){
		case Shader::ShaderType_VERTEX:
			mD3DDevice->VSSetConstantBuffers(i,1,&constantBuffer);
		break;
		case Shader::ShaderType_FRAGMENT:
			mD3DDevice->PSSetConstantBuffers(i,1,&constantBuffer);
		break;
		case Shader::ShaderType_GEOMETRY:
			mD3DDevice->GSSetConstantBuffers(i,1,&constantBuffer);
		break;
	}
}

void D3D10RenderDevice::setTexture(Shader::ShaderType shaderType,int i,Texture *texture){
	D3D10Texture *d3dtexture=texture!=NULL?(D3D10Texture*)texture->getRootTexture():NULL;
	ID3D10ShaderResourceView *resourceview=NULL;
	if(d3dtexture!=NULL){
		resourceview=d3dtexture->mShaderResourceView;
	}

	switch(shaderType){
		case Shader::ShaderType_VERTEX:
			mD3DDevice->VSSetShaderResources(i,1,&resourceview);
		break;
		case Shader::ShaderType_FRAGMENT:
			mD3DDevice->PSSetShaderResources(i,1,&resourceview);
		break;
		case Shader::ShaderType_GEOMETRY:
			mD3DDevice->GSSetShaderResources(i,1,&resourceview);
		break;
	}

	if(mNullShaderResources.size()<=i){
		mNullShaderResources.resize(i+1,NULL);
	}
}

void D3D10RenderDevice::getShadowBiasMatrix(Matrix4x4 &result,const Texture *shadowTexture,scalar bias){
	result.reset();
}

int D3D10RenderDevice::getClosePixelFormat(int pixelFormat,int usage){
	switch(pixelFormat){
		case TextureFormat::Format_R_8:
		case TextureFormat::Format_RG_8:
		case TextureFormat::Format_RGBA_8:
		case TextureFormat::Format_RGB_F32:
		case TextureFormat::Format_RGBA_F32:
		case TextureFormat::Format_RGB_DXT1:
		case TextureFormat::Format_RGBA_DXT2:
		case TextureFormat::Format_RGBA_DXT3:
		case TextureFormat::Format_RGBA_DXT4:
		case TextureFormat::Format_RGBA_DXT5:
			return pixelFormat;
		case TextureFormat::Format_DEPTH_8:
		case TextureFormat::Format_DEPTH_16:
			return TextureFormat::Format_DEPTH_16;
		case TextureFormat::Format_DEPTH_24:
			return TextureFormat::Format_DEPTH_24;
		default:
			return TextureFormat::Format_RGBA_8;
	}
}

bool D3D10RenderDevice::getShaderProfileSupported(const String &profile){
	return profile=="hlsl";
}

D3D10_PRIMITIVE_TOPOLOGY D3D10RenderDevice::getD3D10_PRIMITIVE_TOPOLOGY(IndexData::Primitive primitive){
	switch(primitive){
		case IndexData::Primitive_POINTS:
			return D3D10_PRIMITIVE_TOPOLOGY_POINTLIST;
		case IndexData::Primitive_LINES:
			return D3D10_PRIMITIVE_TOPOLOGY_LINELIST;
		case IndexData::Primitive_LINESTRIP:
			return D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP;
		case IndexData::Primitive_TRIS:
			return D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		case IndexData::Primitive_TRISTRIP:
			return D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"D3D10RenderDevice::getD3D10_PRIMITIVE_FORMAT: Invalid format");
			return D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED;
		break;
	}
}

DXGI_FORMAT D3D10RenderDevice::getIndexDXGI_FORMAT(IndexBuffer::IndexFormat format){
	switch(format){
		case IndexBuffer::IndexFormat_UINT8:
		case IndexBuffer::IndexFormat_UINT16:
			return DXGI_FORMAT_R16_UINT;
		break;
		case IndexBuffer::IndexFormat_UINT32:
			return DXGI_FORMAT_R32_UINT;
		break;
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"D3D10RenderDevice::getIndexDXGI_FORMAT: Invalid format");
			return DXGI_FORMAT_UNKNOWN;
		break;
	}
}

DXGI_FORMAT D3D10RenderDevice::getVertexDXGI_FORMAT(int format){
	switch(format){
		case VertexFormat::Format_TYPE_UINT_8|VertexFormat::Format_COUNT_1:
			return DXGI_FORMAT_R8_UINT;
		case VertexFormat::Format_TYPE_UINT_8|VertexFormat::Format_COUNT_2:
			return DXGI_FORMAT_R8G8_UINT;
		case VertexFormat::Format_TYPE_UINT_8|VertexFormat::Format_COUNT_4:
			return DXGI_FORMAT_R8G8B8A8_UINT;
		case VertexFormat::Format_TYPE_COLOR_RGBA:
			return DXGI_FORMAT_R8G8B8A8_UNORM;

		case VertexFormat::Format_TYPE_INT_8|VertexFormat::Format_COUNT_1:
			return DXGI_FORMAT_R8_SINT;
		case VertexFormat::Format_TYPE_INT_8|VertexFormat::Format_COUNT_2:
			return DXGI_FORMAT_R8G8_SINT;
		case VertexFormat::Format_TYPE_INT_8|VertexFormat::Format_COUNT_4:
			return DXGI_FORMAT_R8G8B8A8_SINT;

		case VertexFormat::Format_TYPE_INT_16|VertexFormat::Format_COUNT_1:
			return DXGI_FORMAT_R16_SINT;
		case VertexFormat::Format_TYPE_INT_16|VertexFormat::Format_COUNT_2:
			return DXGI_FORMAT_R16G16_SINT;
		case VertexFormat::Format_TYPE_INT_16|VertexFormat::Format_COUNT_4:
			return DXGI_FORMAT_R16G16B16A16_SINT;

		case VertexFormat::Format_TYPE_INT_32|VertexFormat::Format_COUNT_1:
			return DXGI_FORMAT_R32_SINT;
		case VertexFormat::Format_TYPE_INT_32|VertexFormat::Format_COUNT_2:
			return DXGI_FORMAT_R32G32_SINT;
		case VertexFormat::Format_TYPE_INT_32|VertexFormat::Format_COUNT_4:
			return DXGI_FORMAT_R32G32B32A32_SINT;

		case VertexFormat::Format_TYPE_FLOAT_32|VertexFormat::Format_COUNT_1:
			return DXGI_FORMAT_R32_FLOAT;
		case VertexFormat::Format_TYPE_FLOAT_32|VertexFormat::Format_COUNT_2:
			return DXGI_FORMAT_R32G32_FLOAT;
		case VertexFormat::Format_TYPE_FLOAT_32|VertexFormat::Format_COUNT_3:
			return DXGI_FORMAT_R32G32B32_FLOAT;
		case VertexFormat::Format_TYPE_FLOAT_32|VertexFormat::Format_COUNT_4:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"D3D10RenderDevice::getVertexDXGI_FORMAT: Invalid format");
			return DXGI_FORMAT_UNKNOWN;
		break;
	}
}

DXGI_FORMAT D3D10RenderDevice::getTextureDXGI_FORMAT(int format){
	switch(format){
		case TextureFormat::Format_L_8:
			return DXGI_FORMAT_R8_UNORM;
		case TextureFormat::Format_LA_8:
			return DXGI_FORMAT_R8G8_UNORM;
		case TextureFormat::Format_RGBA_8:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		case TextureFormat::Format_RGB_F32:
			return DXGI_FORMAT_R32G32B32_FLOAT;
		case TextureFormat::Format_RGBA_F32:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case TextureFormat::Format_RGB_DXT1:
			return DXGI_FORMAT_BC1_UNORM;
		case TextureFormat::Format_RGBA_DXT2:
		case TextureFormat::Format_RGBA_DXT3:
			return DXGI_FORMAT_BC2_UNORM;
		case TextureFormat::Format_RGBA_DXT4:
		case TextureFormat::Format_RGBA_DXT5:
			return DXGI_FORMAT_BC3_UNORM;
		case TextureFormat::Format_DEPTH_16:
			return DXGI_FORMAT_D16_UNORM;
		case TextureFormat::Format_DEPTH_24:
			return DXGI_FORMAT_D24_UNORM_S8_UINT;
		default:
			return DXGI_FORMAT_UNKNOWN;
	}
}

D3D10_MAP D3D10RenderDevice::getD3D10_MAP(int access,int usage){
	if((usage&Buffer::Usage_BIT_STAGING)!=0){
		return D3D10_MAP_READ_WRITE;
	}

	if((access&Buffer::Access_READ_WRITE)==Buffer::Access_READ_WRITE){
		return D3D10_MAP_READ_WRITE;
	}
	else if((access&Buffer::Access_BIT_READ)!=0){
		return D3D10_MAP_READ;
	}
	else if((access&Buffer::Access_BIT_WRITE)!=0){
		return D3D10_MAP_WRITE_DISCARD;
	}
	else{
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D10RenderDevice::getD3D10_MAP: Invalid type");
		return (D3D10_MAP)0;
	}
}

D3D10_USAGE D3D10RenderDevice::getD3D10_USAGE(int usage){
	if((usage&Buffer::Usage_BIT_STATIC)!=0){
		return D3D10_USAGE_IMMUTABLE;
	}
	else if((usage&Buffer::Usage_BIT_STREAM)!=0){
		return D3D10_USAGE_DEFAULT;
	}
	else if((usage&Buffer::Usage_BIT_DYNAMIC)!=0){
		return D3D10_USAGE_DYNAMIC;
	}
	else if((usage&Buffer::Usage_BIT_STAGING)!=0){
		return D3D10_USAGE_STAGING;
	}
	else{
		return D3D10_USAGE_DEFAULT;
	}
}

D3D10_BLEND D3D10RenderDevice::getD3D10_BLEND(BlendState::Operation operation,bool alpha){
	switch(operation){
		case BlendState::Operation_ONE:
			return D3D10_BLEND_ONE;
		case BlendState::Operation_ZERO:
			return D3D10_BLEND_ZERO;
		case BlendState::Operation_DEST_ALPHA:
			return D3D10_BLEND_DEST_ALPHA;
		case BlendState::Operation_SOURCE_ALPHA:
			return D3D10_BLEND_SRC_ALPHA;
		case BlendState::Operation_ONE_MINUS_DEST_ALPHA:
			return D3D10_BLEND_INV_DEST_ALPHA;
		case BlendState::Operation_ONE_MINUS_SOURCE_ALPHA:
			return D3D10_BLEND_INV_SRC_ALPHA;
	}
	if(alpha==false){
		switch(operation){
			case BlendState::Operation_DEST_COLOR:
				return D3D10_BLEND_DEST_COLOR;
			case BlendState::Operation_SOURCE_COLOR:
				return D3D10_BLEND_SRC_COLOR;
			case BlendState::Operation_ONE_MINUS_DEST_COLOR:
				return D3D10_BLEND_INV_DEST_COLOR;
			case BlendState::Operation_ONE_MINUS_SOURCE_COLOR:
				return D3D10_BLEND_INV_SRC_COLOR;
		}
	}
	return D3D10_BLEND_ZERO;
}

D3D10_COMPARISON_FUNC D3D10RenderDevice::getD3D10_COMPARISON_FUNC(DepthState::DepthTest test){
	switch(test){
		case DepthState::DepthTest_NEVER:
			return D3D10_COMPARISON_NEVER;
		case DepthState::DepthTest_LESS:
			return D3D10_COMPARISON_LESS;
		case DepthState::DepthTest_EQUAL:
			return D3D10_COMPARISON_EQUAL;
		case DepthState::DepthTest_LEQUAL:
			return D3D10_COMPARISON_LESS_EQUAL;
		case DepthState::DepthTest_GREATER:
			return D3D10_COMPARISON_GREATER;
		case DepthState::DepthTest_NOTEQUAL:
			return D3D10_COMPARISON_NOT_EQUAL;
		case DepthState::DepthTest_GEQUAL:
			return D3D10_COMPARISON_GREATER_EQUAL;
		case DepthState::DepthTest_ALWAYS:
			return D3D10_COMPARISON_ALWAYS;
		default:
			return D3D10_COMPARISON_ALWAYS;
	}
}

D3D10_FILL_MODE D3D10RenderDevice::getD3D10_FILL_MODE(RasterizerState::FillType type){
	switch(type){
		case RasterizerState::FillType_POINT:
		case RasterizerState::FillType_LINE:
			return D3D10_FILL_WIREFRAME;
		case RasterizerState::FillType_SOLID:
		default:
			return D3D10_FILL_SOLID;
	}
}

D3D10_CULL_MODE D3D10RenderDevice::getD3D10_CULL_MODE(RasterizerState::CullType type){
	switch(type){
		case RasterizerState::CullType_NONE:
			return D3D10_CULL_NONE;
		case RasterizerState::CullType_FRONT:
			return D3D10_CULL_FRONT;
		case RasterizerState::CullType_BACK:
			return D3D10_CULL_BACK;
		default:
			return D3D10_CULL_NONE;
	}
}

D3D10_FILTER D3D10RenderDevice::getD3D10_FILTER(SamplerState::FilterType minFilter,SamplerState::FilterType magFilter,SamplerState::FilterType mipFilter){
	switch(minFilter){
		case SamplerState::FilterType_NONE:
		case SamplerState::FilterType_NEAREST:
			switch(magFilter){
				case SamplerState::FilterType_NONE:
				case SamplerState::FilterType_NEAREST:
					switch(mipFilter){
						case SamplerState::FilterType_NONE:
						case SamplerState::FilterType_NEAREST:
							return D3D10_FILTER_MIN_MAG_MIP_POINT;
						case SamplerState::FilterType_LINEAR:
							return D3D10_FILTER_MIN_MAG_POINT_MIP_LINEAR;
						default:
						break;
					}
				case SamplerState::FilterType_LINEAR:
					switch(mipFilter){
						case SamplerState::FilterType_NONE:
						case SamplerState::FilterType_NEAREST:
							return D3D10_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
						case SamplerState::FilterType_LINEAR:
							return D3D10_FILTER_MIN_POINT_MAG_MIP_LINEAR;
						default:
						break;
					}
				default:
				break;
			}
		break;
		case SamplerState::FilterType_LINEAR:
			switch(magFilter){
				case SamplerState::FilterType_NONE:
				case SamplerState::FilterType_NEAREST:
					switch(mipFilter){
						case SamplerState::FilterType_NONE:
						case SamplerState::FilterType_NEAREST:
							return D3D10_FILTER_MIN_LINEAR_MAG_MIP_POINT;
						case SamplerState::FilterType_LINEAR:
							return D3D10_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
						default:
						break;
					}
				case SamplerState::FilterType_LINEAR:
					switch(mipFilter){
						case SamplerState::FilterType_NONE:
						case SamplerState::FilterType_NEAREST:
							return D3D10_FILTER_MIN_MAG_LINEAR_MIP_POINT;
						case SamplerState::FilterType_LINEAR:
							return D3D10_FILTER_MIN_MAG_MIP_LINEAR;
						default:
						break;
					}
				default:
				break;
			}
		default:
		break;
	}

	return D3D10_FILTER_MIN_MAG_MIP_POINT;
}

D3D10_TEXTURE_ADDRESS_MODE D3D10RenderDevice::getD3D10_TEXTURE_ADDRESS_MODE(SamplerState::AddressType type){
	switch(type){
		case SamplerState::AddressType_REPEAT:
			return D3D10_TEXTURE_ADDRESS_WRAP;
		case SamplerState::AddressType_CLAMP_TO_EDGE:
			return D3D10_TEXTURE_ADDRESS_CLAMP;
		case SamplerState::AddressType_CLAMP_TO_BORDER:
			return D3D10_TEXTURE_ADDRESS_BORDER;
		case SamplerState::AddressType_MIRRORED_REPEAT:
			return D3D10_TEXTURE_ADDRESS_MIRROR;
		default:
			return D3D10_TEXTURE_ADDRESS_WRAP;
	}
}

void D3D10RenderDevice::getD3D10_BLEND_DESC(D3D10_BLEND_DESC &desc,const BlendState &state){
	memset(&desc,0,sizeof(desc));

	desc.BlendEnable[0]=!(state.source==BlendState::Operation_ONE && state.dest==BlendState::Operation_ZERO);
	desc.SrcBlend=getD3D10_BLEND(state.source,false);
	desc.DestBlend=getD3D10_BLEND(state.dest,false);
	desc.BlendOp=D3D10_BLEND_OP_ADD;
	desc.SrcBlendAlpha=getD3D10_BLEND(state.source,true);
	desc.DestBlendAlpha=getD3D10_BLEND(state.dest,true);
	desc.BlendOpAlpha=D3D10_BLEND_OP_ADD;
	desc.RenderTargetWriteMask[0]=state.colorWrite; // BlendState::ColorWrite bits match D3D10_COLOR_WRITE_ENABLE
}

void D3D10RenderDevice::getD3D10_DEPTH_STENCIL_DESC(D3D10_DEPTH_STENCIL_DESC &desc,const DepthState &state){
	memset(&desc,0,sizeof(desc));

	desc.DepthEnable=(state.test!=DepthState::DepthTest_ALWAYS);
	desc.DepthWriteMask=(state.write?D3D10_DEPTH_WRITE_MASK_ALL:D3D10_DEPTH_WRITE_MASK_ZERO);
    desc.DepthFunc=getD3D10_COMPARISON_FUNC(state.test);
}

void D3D10RenderDevice::getD3D10_RASTERIZER_DESC(D3D10_RASTERIZER_DESC &desc,const RasterizerState &state){
	memset(&desc,0,sizeof(desc));

	desc.FillMode=getD3D10_FILL_MODE(state.fill);
	desc.CullMode=getD3D10_CULL_MODE(state.cull);
	desc.FrontCounterClockwise=TRUE;
	desc.DepthBias=state.depthBiasConstant;
	desc.SlopeScaledDepthBias=state.depthBiasSlope;
    desc.DepthClipEnable=TRUE;
    desc.MultisampleEnable=state.multisample;
}

void D3D10RenderDevice::getD3D10_SAMPLER_DESC(D3D10_SAMPLER_DESC &desc,const SamplerState &state){
	memset(&desc,0,sizeof(desc));

	desc.Filter=getD3D10_FILTER(state.minFilter,state.magFilter,state.mipFilter);
	desc.AddressU=getD3D10_TEXTURE_ADDRESS_MODE(state.uAddress);
	desc.AddressV=getD3D10_TEXTURE_ADDRESS_MODE(state.vAddress);
	desc.AddressW=getD3D10_TEXTURE_ADDRESS_MODE(state.wAddress);
	toD3DColor(desc.BorderColor,state.borderColor);
	desc.MinLOD=state.minLOD;
	desc.MaxLOD=state.maxLOD;
}

int D3D10RenderDevice::getVariableFormat(const D3D10_SHADER_TYPE_DESC &type){
	int format=0;

	switch(type.Type){
		case D3D10_SVT_BOOL:
			format|=VariableBufferFormat::Format_TYPE_UINT_8;
		break;
		case D3D10_SVT_INT:
			format|=VariableBufferFormat::Format_TYPE_INT_32;
		break;
		case D3D10_SVT_FLOAT:
			format|=VariableBufferFormat::Format_TYPE_FLOAT_32;
		break;
	}

	if(type.Rows>1){
		if(type.Rows==2){
			if(type.Columns==2){
				format|=VariableBufferFormat::Format_COUNT_2X2;
			}
			else if(type.Columns==3){
				format|=VariableBufferFormat::Format_COUNT_2X3;
			}
			else if(type.Columns==4){
				format|=VariableBufferFormat::Format_COUNT_2X4;
			}
		}
		if(type.Rows==3){
			if(type.Columns==2){
				format|=VariableBufferFormat::Format_COUNT_3X2;
			}
			else if(type.Columns==3){
				format|=VariableBufferFormat::Format_COUNT_3X3;
			}
			else if(type.Columns==4){
				format|=VariableBufferFormat::Format_COUNT_3X4;
			}
		}
		if(type.Rows==4){
			if(type.Columns==2){
				format|=VariableBufferFormat::Format_COUNT_4X2;
			}
			else if(type.Columns==3){
				format|=VariableBufferFormat::Format_COUNT_4X3;
			}
			else if(type.Columns==4){
				format|=VariableBufferFormat::Format_COUNT_4X4;
			}
		}
	}
	else{
		format|=(type.Columns<<VariableBufferFormat::Format_SHIFT_COUNTS);
	}

	return format;
}

void D3D10RenderDevice::vertexFormatCreated(D3D10VertexFormat *format){
	#if defined(TOADLET_THREADSAFE)
		mMutex.lock();
	#endif

	int handle=mVertexFormats.size();
	format->mRenderHandle=handle;
	mVertexFormats.resize(handle+1);
	mVertexFormats[handle]=format;

	#if defined(TOADLET_THREADSAFE)
		mMutex.unlock();
	#endif
}

void D3D10RenderDevice::vertexFormatDestroyed(D3D10VertexFormat *format){
	#if defined(TOADLET_THREADSAFE)
		mMutex.unlock();
	#endif

	int handle=format->mRenderHandle;
	if(handle==-1 || mVertexFormats[handle]!=format){
		#if defined(TOADLET_THREADSAFE)
			mMutex.unlock();
		#endif

		return;
	}

	int i;
	for(i=mVertexShaders.size()-1;i>=0;--i){
		D3D10Shader *shader=mVertexShaders[i];
		if(handle<shader->mLayouts.size()){
			ID3D10InputLayout *layout=shader->mLayouts[handle];
			if(layout!=NULL){
				layout->Release();
			}
			shader->mLayouts.erase(shader->mLayouts.begin()+handle);
		}
	}
	for(i=handle;i<mVertexFormats.size();++i){
		mVertexFormats[i]->mRenderHandle--;
	}
	mVertexFormats.erase(mVertexFormats.begin()+handle);

	#if defined(TOADLET_THREADSAFE)
		mMutex.unlock();
	#endif
}

void D3D10RenderDevice::shaderCreated(D3D10Shader *shader){
	#if defined(TOADLET_THREADSAFE)
		mMutex.lock();
	#endif

	if(shader->mShaderType==Shader::ShaderType_VERTEX){
		mVertexShaders.push_back(shader);
	}

	#if defined(TOADLET_THREADSAFE)
		mMutex.unlock();
	#endif
}

void D3D10RenderDevice::shaderDestroyed(D3D10Shader *shader){
	#if defined(TOADLET_THREADSAFE)
		mMutex.lock();
	#endif

	if(shader->mShaderType==Shader::ShaderType_VERTEX){
		mVertexShaders.erase(stlit::remove(mVertexShaders.begin(),mVertexShaders.end(),shader),mVertexShaders.end());
	}

	#if defined(TOADLET_THREADSAFE)
		mMutex.unlock();
	#endif
}

}
}
