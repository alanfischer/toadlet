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
#include "D3D10Texture.h"
#include "D3D10VertexFormat.h"
#include <toadlet/egg/MathConversion.h>
#include <toadlet/egg/Error.h>
#include <toadlet/peeper/MaterialState.h>
#include <toadlet/peeper/LightState.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/peeper/Viewport.h>

using namespace toadlet::egg;
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
	mD3DRenderTarget(NULL)
{}

D3D10RenderDevice::~D3D10RenderDevice(){
	destroy();
}

bool D3D10RenderDevice::create(RenderTarget *target,int *options){
	Logger::alert(Categories::TOADLET_PEEPER,
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

	RenderCaps &caps=mCaps;
	{
		caps.resetOnResize=false;
		caps.hardwareTextures=true;
		caps.hardwareIndexBuffers=true;
		caps.hardwareVertexBuffers=true;
		caps.pointSprites=false;
		caps.maxLights=0;
		caps.maxTextureStages=16;
		caps.maxTextureSize=8192;
		caps.textureDot3=false;
		caps.textureNonPowerOf2=true;
		caps.textureNonPowerOf2Restricted=true;
		caps.textureAutogenMipMaps=false;
		caps.renderToTexture=true;
		caps.renderToDepthTexture=true;
		caps.renderToTextureNonPowerOf2Restricted=true;
		caps.idealVertexFormatBit=VertexFormat::Format_BIT_FLOAT_32;
		caps.triangleFan=false;
	}

	Logger::alert(Categories::TOADLET_PEEPER,
		"created D3D10RenderDevice");

samp=NULL;
	char *effectstring=
"float4x4 ShaderMatrix;\n"
"float4x4 textureMatrix;\n"
"Texture2D diffuseTexture;\n"
"float4 diffuseColor;\n"
"float useTexture;\n"
"SamplerState samps[1];\n"
/*
"SamplerState samLinear{\n"
    "Filter = MIN_MAG_MIP_LINEAR;\n"
    "AddressU = Wrap;\n"
    "AddressV = Wrap;\n"
"};\n"
*/
"struct VS_OUTPUT{\n"
    "float4 Pos : SV_POSITION;\n"
    "float2 TexCoords : TEXCOORD0;\n"
"};\n"

"VS_OUTPUT VS( float4 Pos : POSITION, float2 TexCoords : TEXCOORD){\n"
  "VS_OUTPUT Output = (VS_OUTPUT)0;\n"
  "Output.Pos = mul(Pos, ShaderMatrix);\n"
  "Output.TexCoords=mul(TexCoords,textureMatrix);\n"
  "return Output;\n"
"}\n"

"float4 PS( VS_OUTPUT Input ) : SV_Target{\n"
//	"return diffuseTexture.Sample(samLinear,Input.TexCoords)*useTexture + diffuseColor*(1-useTexture);\n"
	"return diffuseTexture.Sample(samps[0],Input.TexCoords);\n"
"}\n"

"technique10 Render{\n"
    "pass P0{\n"
        "SetVertexShader( CompileShader( vs_4_0, VS() ) );\n"
        "SetGeometryShader( NULL );\n"
        "SetPixelShader( CompileShader( ps_4_0, PS() ) );\n"
    "}\n"
"}\n";

HMODULE library=LoadLibrary(TOADLET_D3D10_DLL_NAME);

ID3D10Blob *compiledEffect=NULL;
typedef HRESULT(WINAPI *D3D10CompileEffectFromMemory)(void *pData,SIZE_T DataLength,LPCSTR pSrcFileName,const D3D10_SHADER_MACRO *pDefines,ID3D10Include *pInclude,UINT HLSLFlags,UINT FXFlags,ID3D10Blob **ppCompiledEffect,ID3D10Blob **ppErrors);
void *compile=GetProcAddress(library,"D3D10CompileEffectFromMemory");
((D3D10CompileEffectFromMemory)compile)(effectstring,strlen(effectstring),0,0,0,0,0,&compiledEffect,0);

typedef HRESULT(WINAPI *D3D10CreateEffectFromMemory)(void *pData,SIZE_T DataLength,UINT FXFlags,ID3D10Device *pDevice,ID3D10EffectPool *pEffectPool,ID3D10Effect **ppEffect);
void *create=GetProcAddress(library,"D3D10CreateEffectFromMemory");
((D3D10CreateEffectFromMemory)create)(compiledEffect->GetBufferPointer(),compiledEffect->GetBufferSize(),0,mD3DDevice,NULL,&effect);

// Obtain the technique
technique = effect->GetTechniqueByName( "Render" );
technique->GetPassByIndex( 0 )->GetDesc( &passDesc);


	mDefaultState=RenderState::ptr(new D3D10RenderState(this));
	mDefaultState->setBlendState(BlendState());
	mDefaultState->setDepthState(DepthState());
	mDefaultState->setRasterizerState(RasterizerState());
	setRenderState(mDefaultState);

	return true;
}

void D3D10RenderDevice::destroy(){
	mDefaultState->destroy();
}

RenderDevice::DeviceStatus D3D10RenderDevice::getStatus(){
	return DeviceStatus_OK;
}

bool D3D10RenderDevice::reset(){
	// No device reset necessary

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

Program *D3D10RenderDevice::createProgram(){
	return NULL;
}

Shader *D3D10RenderDevice::createShader(){
	return NULL;
}

Query *D3D10RenderDevice::createQuery(){
	return new D3D10Query(this);
}

RenderState *D3D10RenderDevice::createRenderState(){
	return new D3D10RenderState(this);
}

bool D3D10RenderDevice::setRenderTarget(RenderTarget *target){
	D3D10RenderTarget *d3dtarget=NULL;
	if(target!=NULL){
		d3dtarget=(D3D10RenderTarget*)target->getRootRenderTarget();
		if(d3dtarget==NULL){
			Error::nullPointer(Categories::TOADLET_PEEPER,
				"RenderTarget is not a D3D10RenderTarget");
			return false;
		}
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

void D3D10RenderDevice::setModelMatrix(const Matrix4x4 &matrix){
	mModelMatrix.set(matrix);
}

void D3D10RenderDevice::setViewMatrix(const Matrix4x4 &matrix){
	mViewMatrix.set(matrix);
}

void D3D10RenderDevice::setProjectionMatrix(const Matrix4x4 &matrix){
	mProjectionMatrix.set(matrix);
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

	int i;
	for(i=0;i<mCaps.maxTextureStages;++i){
		setTexture(i,NULL);
	}

// Doing this temporarily to unbind any resources
D3D10_TECHNIQUE_DESC techDesc;
technique->GetDesc( &techDesc );
for( UINT p = 0; p < techDesc.Passes; ++p )
technique->GetPassByIndex(p)->Apply(0);
}

void D3D10RenderDevice::renderPrimitive(const VertexData::ptr &vertexData,const IndexData::ptr &indexData){
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

	// Set vertex buffer
	D3D10Buffer *vertexBuffer=(D3D10Buffer*)vertexData->getVertexBuffer(0)->getRootVertexBuffer();
	D3D10VertexFormat *vertexFormat=(D3D10VertexFormat*)vertexBuffer->mVertexFormat->getRootVertexFormat();

	mD3DDevice->IASetInputLayout(vertexFormat->getLayout());

	UINT stride=vertexBuffer->mVertexFormat->getVertexSize();
	UINT offset=0;
	mD3DDevice->IASetVertexBuffers(0,1,&vertexBuffer->mBuffer,&stride,&offset);

//	pWorldMatrixEffectVariable->SetMatrix(w);
//	pViewMatrixEffectVariable->SetMatrix(viewMatrix);
//	pProjectionMatrixEffectVariable->SetMatrix(projectionMatrix);
	Matrix4x4 shaderMatrix=mProjectionMatrix*mViewMatrix*mModelMatrix;
	#if defined(TOADLET_FIXED_POINT)
		float d3dmatrix[16];
		toD3DMatrix(d3dmatrix,shaderMatrix);
	#else
		float *d3dmatrix=shaderMatrix.data;
	#endif
	effect->GetVariableByName("ShaderMatrix")->AsMatrix()->SetMatrix(d3dmatrix);

	if(indexData->getIndexBuffer()!=NULL){
		D3D10Buffer *buffer=(D3D10Buffer*)(indexData->getIndexBuffer()->getRootIndexBuffer());
		mD3DDevice->IASetIndexBuffer(buffer->mBuffer,getIndexDXGI_FORMAT(buffer->getIndexFormat()),0);
	}

	mD3DDevice->IASetPrimitiveTopology(getD3D10_PRIMITIVE_TOPOLOGY(indexData->primitive));


D3D10_TECHNIQUE_DESC techDesc;
technique->GetDesc( &techDesc );
for( UINT p = 0; p < techDesc.Passes; ++p )
{
	technique->GetPassByIndex( p )->Apply(0);
mD3DDevice->VSSetSamplers(0,1,&samp);
mD3DDevice->PSSetSamplers(0,1,&samp);
mD3DDevice->GSSetSamplers(0,1,&samp);

	if(indexData->getIndexBuffer()){
		mD3DDevice->DrawIndexed(indexData->count,indexData->start,0);
	}
	else{
		mD3DDevice->Draw(indexData->count,indexData->start);
	}
}


//	mD3DDevice->Draw(numVertices,0);

/*
	HRESULT result;
	D3DPRIMITIVETYPE d3dpt;
	int count;
	getPrimitiveTypeAndCount(d3dpt,count,indexData->primitive,indexData->count);

	int i;
	int numVertexes=0;
	int numVertexBuffers=vertexData->vertexBuffers.size();
	for(i=0;i<numVertexBuffers;++i){
		D3D9VertexBuffer *d3dvertexBuffer=(D3D9VertexBuffer*)vertexData->vertexBuffers[i]->getRootVertexBuffer();
		if(numVertexes==0){
			numVertexes=d3dvertexBuffer->mSize;
		}
		#if defined(TOADLET_HAS_DIRECT3DMOBILE)
			result=mD3DDevice->SetStreamSource(i,d3dvertexBuffer->mVertexBuffer,d3dvertexBuffer->mVertexSize);
		#else
			result=mD3DDevice->SetStreamSource(i,d3dvertexBuffer->mVertexBuffer,0,d3dvertexBuffer->mVertexSize);
		#endif
		TOADLET_CHECK_D3D9ERROR(result,"D3D10RenderDevice: SetStreamSource");
		#if !defined(TOADLET_HAS_DIRECT3DMOBILE)
			result=mD3DDevice->SetFVF(d3dvertexBuffer->mFVF);
			TOADLET_CHECK_D3D9ERROR(result,"D3D10RenderDevice: SetFVF");
		#endif
	}

	IndexBuffer *indexBuffer=indexData->indexBuffer;
	if(indexBuffer!=NULL){
		D3D9IndexBuffer *d3dindexBuffer=(D3D9IndexBuffer*)indexBuffer->getRootIndexBuffer();
		result=mD3DDevice->SetIndices(d3dindexBuffer->mIndexBuffer);
		TOADLET_CHECK_D3D9ERROR(result,"D3D10RenderDevice: SetIndices");

		result=mD3DDevice->DrawIndexedPrimitive(d3dpt,0,0,numVertexes,indexData->start,count);
		TOADLET_CHECK_D3D9ERROR(result,"D3D10RenderDevice: DrawIndexedPrimitive");
	}
	else{
		result=mD3DDevice->DrawPrimitive(d3dpt,indexData->start,count);
		TOADLET_CHECK_D3D9ERROR(result,"D3D10RenderDevice: DrawPrimitive");
	}
*/
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
	setRenderState(mDefaultState);
}

bool D3D10RenderDevice::setRenderState(RenderState *renderState){
	D3D10RenderState *d3drenderState=NULL;
	if(renderState!=NULL){
		d3drenderState=(D3D10RenderState*)renderState->getRootRenderState();
		if(d3drenderState==NULL){
			Error::nullPointer(Categories::TOADLET_PEEPER,
				"RenderState is not a D3D10RenderState");
			return false;
		}
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

// These need to be moved/cleaned
if(d3drenderState->mD3DSamplerStates.size()>0)samp=d3drenderState->mD3DSamplerStates[0];
	mD3DDevice->VSSetSamplers(0,d3drenderState->mD3DSamplerStates.size(),d3drenderState->mD3DSamplerStates.begin());
	mD3DDevice->PSSetSamplers(0,d3drenderState->mD3DSamplerStates.size(),d3drenderState->mD3DSamplerStates.begin());
	mD3DDevice->GSSetSamplers(0,d3drenderState->mD3DSamplerStates.size(),d3drenderState->mD3DSamplerStates.begin());

if(d3drenderState->mTextureStates.size()>0 && d3drenderState->mTextureStates[0]!=NULL){
#if defined(TOADLET_FIXED_POINT)
	float d3dmatrix[16];
	toD3DMatrix(d3dmatrix,d3drenderState->mTextureStates[0]->matrix);
#else
	float *d3dmatrix=d3drenderState->mTextureStates[0]->matrix.data;
#endif
if(d3drenderState->mTextureStates.size()>0)effect->GetVariableByName("textureMatrix")->AsMatrix()->SetMatrix(d3dmatrix);
}

if(d3drenderState->mMaterialState!=NULL){
effect->GetVariableByName("diffuseColor")->AsVector()->SetFloatVector((float*)d3drenderState->mMaterialState->diffuse.getData());
}

	return true;
}

void D3D10RenderDevice::setTexture(int i,Texture *texture){
	if(i>0)return;

	ID3D10ShaderResourceView *textureView=NULL;
	Matrix4x4 textureMatrix;
	if(texture){
		textureView=((D3D10Texture*)(texture->getRootTexture()))->mShaderResourceView;
	}
effect->GetVariableByName("diffuseTexture")->AsShaderResource()->SetResource(textureView);
effect->GetVariableByName("useTexture")->AsScalar()->SetFloat(texture!=NULL);
}

void D3D10RenderDevice::getShadowBiasMatrix(const Texture *shadowTexture,Matrix4x4 &result){
	result.reset();
}

int D3D10RenderDevice::getCloseTextureFormat(int textureFormat,int usage){
	switch(textureFormat){
		case Texture::Format_R_8:
		case Texture::Format_RG_8:
		case Texture::Format_RGBA_8:
		case Texture::Format_RGB_F32:
		case Texture::Format_RGBA_F32:
		case Texture::Format_RGBA_DXT1:
		case Texture::Format_RGBA_DXT2:
		case Texture::Format_RGBA_DXT3:
		case Texture::Format_RGBA_DXT4:
		case Texture::Format_RGBA_DXT5:
			return textureFormat;
		case Texture::Format_DEPTH_8:
		case Texture::Format_DEPTH_16:
			return Texture::Format_DEPTH_16;
		case Texture::Format_DEPTH_24:
			return Texture::Format_DEPTH_24;
		default:
			return Texture::Format_RGBA_8;
	}
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
			Logger::error(Categories::TOADLET_PEEPER,
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
			Logger::error(Categories::TOADLET_PEEPER,
				"D3D10RenderDevice::getIndexDXGI_FORMAT: Invalid format");
			return DXGI_FORMAT_UNKNOWN;
		break;
	}
}

DXGI_FORMAT D3D10RenderDevice::getVertexDXGI_FORMAT(int format){
	switch(format){
		case VertexFormat::Format_BIT_UINT_8|VertexFormat::Format_BIT_COUNT_1:
			return DXGI_FORMAT_R8_UINT;
		case VertexFormat::Format_BIT_UINT_8|VertexFormat::Format_BIT_COUNT_2:
			return DXGI_FORMAT_R8G8_UINT;
		case VertexFormat::Format_BIT_UINT_8|VertexFormat::Format_BIT_COUNT_4:
		case VertexFormat::Format_COLOR_RGBA:
			return DXGI_FORMAT_R8G8B8A8_UINT;

		case VertexFormat::Format_BIT_INT_8|VertexFormat::Format_BIT_COUNT_1:
			return DXGI_FORMAT_R8_SINT;
		case VertexFormat::Format_BIT_INT_8|VertexFormat::Format_BIT_COUNT_2:
			return DXGI_FORMAT_R8G8_SINT;
		case VertexFormat::Format_BIT_INT_8|VertexFormat::Format_BIT_COUNT_4:
			return DXGI_FORMAT_R8G8B8A8_SINT;

		case VertexFormat::Format_BIT_INT_16|VertexFormat::Format_BIT_COUNT_1:
			return DXGI_FORMAT_R16_SINT;
		case VertexFormat::Format_BIT_INT_16|VertexFormat::Format_BIT_COUNT_2:
			return DXGI_FORMAT_R16G16_SINT;
		case VertexFormat::Format_BIT_INT_16|VertexFormat::Format_BIT_COUNT_4:
			return DXGI_FORMAT_R16G16B16A16_SINT;

		case VertexFormat::Format_BIT_INT_32|VertexFormat::Format_BIT_COUNT_1:
			return DXGI_FORMAT_R32_SINT;
		case VertexFormat::Format_BIT_INT_32|VertexFormat::Format_BIT_COUNT_2:
			return DXGI_FORMAT_R32G32_SINT;
		case VertexFormat::Format_BIT_INT_32|VertexFormat::Format_BIT_COUNT_4:
			return DXGI_FORMAT_R32G32B32A32_SINT;

		case VertexFormat::Format_BIT_FLOAT_32|VertexFormat::Format_BIT_COUNT_1:
			return DXGI_FORMAT_R32_FLOAT;
		case VertexFormat::Format_BIT_FLOAT_32|VertexFormat::Format_BIT_COUNT_2:
			return DXGI_FORMAT_R32G32_FLOAT;
		case VertexFormat::Format_BIT_FLOAT_32|VertexFormat::Format_BIT_COUNT_3:
			return DXGI_FORMAT_R32G32B32_FLOAT;
		case VertexFormat::Format_BIT_FLOAT_32|VertexFormat::Format_BIT_COUNT_4:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		default:
			Logger::error(Categories::TOADLET_PEEPER,
				"D3D10RenderDevice::getVertexDXGI_FORMAT: Invalid format");
			return DXGI_FORMAT_UNKNOWN;
		break;
	}
}

DXGI_FORMAT D3D10RenderDevice::getTextureDXGI_FORMAT(int format){
	switch(format){
		case Texture::Format_L_8:
			return DXGI_FORMAT_R8_UNORM;
		case Texture::Format_LA_8:
			return DXGI_FORMAT_R8G8_UNORM;
		case Texture::Format_RGBA_8:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		case Texture::Format_RGB_F32:
			return DXGI_FORMAT_R32G32B32_FLOAT;
		case Texture::Format_RGBA_F32:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case Texture::Format_RGBA_DXT1:
		case Texture::Format_RGBA_DXT2:
			return DXGI_FORMAT_BC1_UNORM;
		case Texture::Format_RGBA_DXT3:
		case Texture::Format_RGBA_DXT4:
			return DXGI_FORMAT_BC2_UNORM;
		case Texture::Format_RGBA_DXT5:
			return DXGI_FORMAT_BC3_UNORM;
		case Texture::Format_DEPTH_16:
			return DXGI_FORMAT_D16_UNORM;
		case Texture::Format_DEPTH_24:
			return DXGI_FORMAT_D24_UNORM_S8_UINT;
		default:
			return DXGI_FORMAT_UNKNOWN;
	}
}

D3D10_MAP D3D10RenderDevice::getD3D10_MAP(int access,int usage){
	if((usage&Buffer::Usage_BIT_STAGING)>0){
		return D3D10_MAP_READ_WRITE;
	}

	switch(access){
		case Buffer::Access_BIT_READ:
			return D3D10_MAP_READ;
		break;
		case Buffer::Access_BIT_WRITE:
			return D3D10_MAP_WRITE_DISCARD;
		break;
		case Buffer::Access_READ_WRITE:
			return D3D10_MAP_READ_WRITE;
		break;
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"D3D10RenderDevice::getD3D10_MAP: Invalid type");
			return (D3D10_MAP)0;
	}
}

D3D10_USAGE D3D10RenderDevice::getD3D10_USAGE(int usage){
	if((usage&Buffer::Usage_BIT_STATIC)>0){
		return D3D10_USAGE_IMMUTABLE;
	}
	else if((usage&Buffer::Usage_BIT_STREAM)>0){
		return D3D10_USAGE_DEFAULT;
	}
	else if((usage&Buffer::Usage_BIT_DYNAMIC)>0){
		return D3D10_USAGE_DYNAMIC;
	}
	else if((usage&Buffer::Usage_BIT_STAGING)>0){
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

	desc.DepthEnable=(state.test!=DepthState::DepthTest_NEVER);
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

char *D3D10RenderDevice::getSemanticName(int semantic){
	switch(semantic){
		case VertexFormat::Semantic_POSITION:
			return "POSITION";
		case VertexFormat::Semantic_NORMAL:
			return "NORMAL";
		case VertexFormat::Semantic_TEX_COORD:
			return "TEXCOORD";
		default:
			return "UNKNOWN";
	}
}

}
}
