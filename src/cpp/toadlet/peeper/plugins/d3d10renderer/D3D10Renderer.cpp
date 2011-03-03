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

#include "D3D10Renderer.h"
#include "D3D10Buffer.h"
#include "D3D10TextureMipPixelBuffer.h"
#include "D3D10PixelBufferRenderTarget.h"
#include "D3D10Query.h"
#include "D3D10RenderTarget.h"
#include "D3D10Texture.h"
#include "D3D10VertexFormat.h"
#include <toadlet/egg/MathConversion.h>
#include <toadlet/egg/Error.h>
#include <toadlet/peeper/LightEffect.h>
#include <toadlet/peeper/Light.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/peeper/Viewport.h>

using namespace toadlet::egg;
using namespace toadlet::egg::MathConversion;

namespace toadlet{
namespace peeper{

#if defined(TOADLET_SET_D3D10)
	TOADLET_C_API Renderer* new_D3D10Renderer(){
		return new D3D10Renderer();
	}
#else
	TOADLET_C_API Renderer* new_D3D11Renderer(){
		return new D3D10Renderer();
	}
#endif

#if defined(TOADLET_BUILD_DYNAMIC)
	TOADLET_C_API Renderer* new_Renderer(){
		return new D3D10Renderer();
	}
#endif

D3D10Renderer::D3D10Renderer():
	mD3DDevice(NULL),
	mPrimaryRenderTarget(NULL),
	mD3DPrimaryRenderTarget(NULL),
	mRenderTarget(NULL),
	mD3DRenderTarget(NULL),
	mStrict(false)

	//mStatisticsSet,
	//mCapabilitySet
{
}

D3D10Renderer::~D3D10Renderer(){
	destroy();
}

bool D3D10Renderer::create(RenderTarget *target,int *options){
	Logger::alert(Categories::TOADLET_PEEPER,
		"creating "+Categories::TOADLET_PEEPER+".D3D10Renderer");

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

	mCapabilitySet.resetOnResize=false;
	mCapabilitySet.hardwareTextures=true;
	mCapabilitySet.hardwareIndexBuffers=true;
	mCapabilitySet.hardwareVertexBuffers=true;
	mCapabilitySet.pointSprites=false;
	mCapabilitySet.maxLights=0;
	mCapabilitySet.maxTextureStages=16;
	mCapabilitySet.maxTextureSize=8192;
	mCapabilitySet.textureDot3=false;
	mCapabilitySet.textureNonPowerOf2=true;
	mCapabilitySet.textureNonPowerOf2Restricted=true;
	mCapabilitySet.textureAutogenMipMaps=false;
	mCapabilitySet.renderToTexture=true;
	mCapabilitySet.renderToDepthTexture=true;
	mCapabilitySet.renderToTextureNonPowerOf2Restricted=true;
	mCapabilitySet.idealVertexFormatBit=VertexFormat::Format_BIT_FLOAT_32;
	mCapabilitySet.triangleFan=false;

	setDefaultStates();

	Logger::alert(Categories::TOADLET_PEEPER,
		"created D3D10Renderer");

	D3D10_RASTERIZER_DESC desc;
	desc.FillMode=D3D10_FILL_SOLID;
	desc.CullMode=D3D10_CULL_NONE;
	desc.FrontCounterClockwise=TRUE;
	desc.DepthBias=0;
	desc.DepthBiasClamp=0;
	desc.SlopeScaledDepthBias=0;
	desc.DepthClipEnable=TRUE;
	desc.ScissorEnable=FALSE;
	desc.MultisampleEnable=FALSE;
	desc.AntialiasedLineEnable=FALSE;

	ID3D10RasterizerState *state=NULL;
	mD3DDevice->CreateRasterizerState(&desc,&state);
	mD3DDevice->RSSetState(state);

	D3D10_DEPTH_STENCIL_DESC ddesc;
	ddesc.DepthEnable=TRUE;
	ddesc.DepthWriteMask=D3D10_DEPTH_WRITE_MASK_ALL;
	ddesc.DepthFunc=D3D10_COMPARISON_LESS;
	ddesc.StencilEnable=FALSE;
	ddesc.StencilReadMask=D3D10_DEFAULT_STENCIL_READ_MASK;
	ddesc.StencilWriteMask=D3D10_DEFAULT_STENCIL_WRITE_MASK;
	ddesc.FrontFace.StencilFailOp=D3D10_STENCIL_OP_KEEP;
	ddesc.FrontFace.StencilDepthFailOp=D3D10_STENCIL_OP_KEEP;
	ddesc.FrontFace.StencilPassOp=D3D10_STENCIL_OP_KEEP;
	ddesc.FrontFace.StencilFunc=D3D10_COMPARISON_ALWAYS;
	ddesc.BackFace.StencilFailOp=D3D10_STENCIL_OP_KEEP;
	ddesc.BackFace.StencilDepthFailOp=D3D10_STENCIL_OP_KEEP;
	ddesc.BackFace.StencilPassOp=D3D10_STENCIL_OP_KEEP;
	ddesc.BackFace.StencilFunc=D3D10_COMPARISON_ALWAYS;

	ID3D10DepthStencilState *dstate=NULL;
	mD3DDevice->CreateDepthStencilState(&ddesc,&dstate);
	mD3DDevice->OMSetDepthStencilState(dstate,0);

	char *effectstring=
"float4x4 ShaderMatrix;\n"
"Texture2D diffuseTexture;\n"
"SamplerState samLinear{\n"
    "Filter = MIN_MAG_MIP_LINEAR;\n"
    "AddressU = Wrap;\n"
    "AddressV = Wrap;\n"
"};\n"

"struct VS_OUTPUT{\n"
    "float4 Pos : SV_POSITION;\n"
    "float2 TexCoords : TEXCOORD0;\n"
"};\n"

"VS_OUTPUT VS( float4 Pos : POSITION, float2 TexCoords : TEXCOORD){\n"
  "VS_OUTPUT Output = (VS_OUTPUT)0;\n"
  "Output.Pos = mul(Pos, ShaderMatrix);\n"
  "Output.TexCoords = TexCoords;\n"
  "return Output;\n"
"}\n"

"float4 PS( VS_OUTPUT Input ) : SV_Target{\n"
    "return diffuseTexture.Sample(samLinear,Input.TexCoords);\n"
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

texture=NULL;

	return true;
}

void D3D10Renderer::destroy(){}

Renderer::RendererStatus D3D10Renderer::getStatus(){
	return RendererStatus_OK;
}

bool D3D10Renderer::reset(){
	// No device reset necessary

	setDefaultStates();

	return true;
}

bool D3D10Renderer::activateAdditionalContext(){
	return (mD3DDevice->GetCreationFlags() & D3D10_CREATE_DEVICE_SINGLETHREADED)==0;
}

Texture *D3D10Renderer::createTexture(){
	return new D3D10Texture(this);
}

PixelBufferRenderTarget *D3D10Renderer::createPixelBufferRenderTarget(){
	return new D3D10PixelBufferRenderTarget(this);
}

PixelBuffer *D3D10Renderer::createPixelBuffer(){
	return new D3D10TextureMipPixelBuffer(this);
}

VertexFormat *D3D10Renderer::createVertexFormat(){
	return new D3D10VertexFormat(this);
}

VertexBuffer *D3D10Renderer::createVertexBuffer(){
	return new D3D10Buffer(this);
}

IndexBuffer *D3D10Renderer::createIndexBuffer(){
	return new D3D10Buffer(this);
}

Program *D3D10Renderer::createProgram(){
	return NULL;
}

Shader *D3D10Renderer::createShader(){
	return NULL;
}

Query *D3D10Renderer::createQuery(){
	return new D3D10Query(this);
}

bool D3D10Renderer::setRenderTarget(RenderTarget *target){
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

void D3D10Renderer::setViewport(const Viewport &viewport){
	D3D10_VIEWPORT d3dviewport;
	d3dviewport.TopLeftX=viewport.x;
	d3dviewport.TopLeftY=viewport.y;
	d3dviewport.Width=viewport.width;
	d3dviewport.Height=viewport.height;
	d3dviewport.MinDepth=0.0f;
	d3dviewport.MaxDepth=1.0f;

	mD3DDevice->RSSetViewports(1,&d3dviewport);
}

void D3D10Renderer::clear(int clearFlags,const Color &clearColor){
	mD3DRenderTarget->clear(clearFlags,clearColor);
}

void D3D10Renderer::swap(){
	mD3DRenderTarget->swap();
}

void D3D10Renderer::setModelMatrix(const Matrix4x4 &matrix){
	mModelMatrix.set(matrix);
}

void D3D10Renderer::setViewMatrix(const Matrix4x4 &matrix){
	mViewMatrix.set(matrix);
}

void D3D10Renderer::setProjectionMatrix(const Matrix4x4 &matrix){
/*
	if(mMirrorY){
		cacheMatrix4x4.set(matrix);
		cacheMatrix4x4.setAt(1,0,-cacheMatrix4x4.at(1,0));
		cacheMatrix4x4.setAt(1,1,-cacheMatrix4x4.at(1,1));
		cacheMatrix4x4.setAt(1,2,-cacheMatrix4x4.at(1,2));
		cacheMatrix4x4.setAt(1,3,-cacheMatrix4x4.at(1,3));
		mProjectionMatrix.set(cacheMatrix4x4);
	}
	else{
*/		mProjectionMatrix.set(matrix);
//	}
}

void D3D10Renderer::beginScene(){
	mStatisticsSet.reset();
}

void D3D10Renderer::endScene(){
	ID3D10Buffer *buffer=NULL;
	UINT stride=0;
	UINT offset=0;
	mD3DDevice->IASetVertexBuffers(0,1,&buffer,&stride,&offset);

	mD3DDevice->IASetIndexBuffer(NULL,(DXGI_FORMAT)0,0);

	mD3DDevice->IASetInputLayout(NULL);

	int i;
	for(i=0;i<mCapabilitySet.maxTextureStages;++i){
		setTextureStage(i,NULL);
	}
}

void D3D10Renderer::renderPrimitive(const VertexData::ptr &vertexData,const IndexData::ptr &indexData){
	if(vertexData==NULL || indexData==NULL){
		Error::nullPointer(Categories::TOADLET_PEEPER,
			"VertexData or IndexData is NULL");
		return;
	}

	if(vertexData->vertexBuffers.size()>1){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D10Renderer does not support multiple streams yet");
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
		TOADLET_CHECK_D3D9ERROR(result,"D3D10Renderer: SetStreamSource");
		#if !defined(TOADLET_HAS_DIRECT3DMOBILE)
			result=mD3DDevice->SetFVF(d3dvertexBuffer->mFVF);
			TOADLET_CHECK_D3D9ERROR(result,"D3D10Renderer: SetFVF");
		#endif
	}

	IndexBuffer *indexBuffer=indexData->indexBuffer;
	if(indexBuffer!=NULL){
		D3D9IndexBuffer *d3dindexBuffer=(D3D9IndexBuffer*)indexBuffer->getRootIndexBuffer();
		result=mD3DDevice->SetIndices(d3dindexBuffer->mIndexBuffer);
		TOADLET_CHECK_D3D9ERROR(result,"D3D10Renderer: SetIndices");

		result=mD3DDevice->DrawIndexedPrimitive(d3dpt,0,0,numVertexes,indexData->start,count);
		TOADLET_CHECK_D3D9ERROR(result,"D3D10Renderer: DrawIndexedPrimitive");
	}
	else{
		result=mD3DDevice->DrawPrimitive(d3dpt,indexData->start,count);
		TOADLET_CHECK_D3D9ERROR(result,"D3D10Renderer: DrawPrimitive");
	}
*/
}

bool D3D10Renderer::copyFrameBufferToPixelBuffer(PixelBuffer *dst){
/*	D3D9Surface *d3dsurface=(D3D9Surface*)surface->getRootSurface();

	IDirect3DSurface9 *currentSurface=NULL;
	#if defined(TOADLET_HAS_DIRECT3DMOBILE)
		mD3DDevice->GetRenderTarget(&currentSurface);
	#else
		mD3DDevice->GetRenderTarget(0,&currentSurface);
	#endif

	D3DSURFACE_DESC desc;
	HRESULT result=currentSurface->GetDesc(&desc);
	TOADLET_CHECK_D3D9ERROR(result,"D3D10Renderer: GetDesc");

	RECT rect={0};
	rect.right=desc.Width<d3dsurface->mWidth?desc.Width:d3dsurface->mWidth;
	rect.bottom=desc.Height<d3dsurface->mHeight?desc.Height:d3dsurface->mHeight;

	result=mD3DDevice->StretchRect(currentSurface,&rect,d3dsurface->mSurface,&rect,D3DTEXF_NONE);
	TOADLET_CHECK_D3D9ERROR(result,"D3D10Renderer: StretchRect");

	currentSurface->Release();

	return true;
*/
return false;
}

bool D3D10Renderer::copyPixelBuffer(PixelBuffer *dst,PixelBuffer *src){
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

void D3D10Renderer::setDefaultStates(){
/*	setAlphaTest(AlphaTest_NONE,0.5);
	setBlend(Blend::Combination_DISABLED);
	setDepthWrite(true);
	setDepthTest(DepthTest_LEQUAL);
	setDithering(false);
	setFaceCulling(FaceCulling_BACK);
	setFogParameters(Fog_NONE,0,1.0,Colors::BLACK);
	setLighting(false);
	setShading(Shading_SMOOTH);
	setNormalize(Normalize_RESCALE);
	#if defined(TOADLET_HAS_DIRECT3DMOBILE)
		setTexturePerspective(true);
	#endif

	int i;
	for(i=0;i<mCapabilitySet.maxTextureStages;++i){
		setTextureStage(i,NULL);
	}

	setLightEffect(LightEffect());
	setAmbientColor(Colors::WHITE);
	// We leave the current lights enabled because the SceneManager does not re-set the lights between layers

	// D3D specific states
	{
		mD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
		mD3DDevice->SetRenderState(D3DRS_SPECULARENABLE,true);
	}
*/}

void D3D10Renderer::setAlphaTest(const AlphaTest &alphaTest,scalar cutoff){
}

void D3D10Renderer::setBlend(const Blend &blend){
}

void D3D10Renderer::setDepthTest(const DepthTest &depthTest){
}

void D3D10Renderer::setDepthWrite(bool enable){
}

void D3D10Renderer::setDithering(bool dithering){
}

void D3D10Renderer::setFaceCulling(const FaceCulling &culling){
}

void D3D10Renderer::setFogState(const FogState &state){
}

void D3D10Renderer::setFill(const Fill &fill){
}

void D3D10Renderer::setLighting(bool lighting){
}

void D3D10Renderer::setLightEffect(const LightEffect &lightEffect){
}

void D3D10Renderer::setDepthBias(scalar constant,scalar slope){
}

void D3D10Renderer::setPointState(const PointState &state){
}

void D3D10Renderer::setTexturePerspective(bool texturePerspective){
}

void D3D10Renderer::setTextureStage(int stage,TextureStage *textureStage){
	if(textureStage!=NULL && stage==0 && textureStage->texture!=NULL){
		texture=((D3D10Texture*)(textureStage->texture->getRootTexture(0)))->mShaderResourceView;
	}
	else if(stage==0){
		texture=NULL;
	}
effect->GetVariableByName("diffuseTexture")->AsShaderResource()->SetResource(texture);

/*	HRESULT result=S_OK;

	if(textureStage!=NULL){
		Texture *texture=textureStage->texture;
		if(texture!=NULL){
			D3D9Texture *d3dtexture=(D3D9Texture*)texture->getRootTexture(textureStage->frame);
			result=mD3DDevice->SetTexture(stage,d3dtexture->mTexture);
			TOADLET_CHECK_D3D9ERROR(result,"SetTexture");
		}
		else{
			result=mD3DDevice->SetTexture(stage,NULL);
			TOADLET_CHECK_D3D9ERROR(result,"SetTexture");
		}

		result=mD3DDevice->SetTextureStageState(stage,D3DTSS_TEXCOORDINDEX,textureStage->texCoordIndex);
		TOADLET_CHECK_D3D9ERROR(result,"SetTextureStageState");

		/// @todo: Only if we're not using shaders
		TextureStage::Calculation calculation=textureStage->calculation;
		if(calculation!=TextureStage::Calculation_DISABLED){
			if(calculation==TextureStage::Calculation_NORMAL){
				/// @todo: Get this working with 3D Texture coordinates.  Doesnt seem to currently
				//  I have tried just switching to D3DTTFF_COUNT3, but nothing changed, I'm pretty sure it has
				//  something to do with the setup of the cacheD3DMatrix below
				result=mD3DDevice->SetTextureStageState(stage,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_COUNT2);
				TOADLET_CHECK_D3D9ERROR(result,"SetTextureStageState");

				toD3DMATRIX(cacheD3DMatrix,textureStage->matrix);
				#if defined(TOADLET_HAS_DIRECT3DMOBILE) && defined(TOADLET_FIXED_POINT)
					scalar t;
				#else
					float t;
				#endif
				t=cacheD3DMatrix._31; cacheD3DMatrix._31=cacheD3DMatrix._41; cacheD3DMatrix._41=t;
				t=cacheD3DMatrix._32; cacheD3DMatrix._32=cacheD3DMatrix._42; cacheD3DMatrix._42=t;
				t=cacheD3DMatrix._33; cacheD3DMatrix._33=cacheD3DMatrix._43; cacheD3DMatrix._43=t;
				t=cacheD3DMatrix._34; cacheD3DMatrix._34=cacheD3DMatrix._44; cacheD3DMatrix._44=t;
			}
			else if(calculation==TextureStage::Calculation_OBJECTSPACE){
				mD3DDevice->SetTextureStageState(stage,D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_PASSTHRU);
				mD3DDevice->SetTextureStageState(stage,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_COUNT4|D3DTTFF_PROJECTED);
			}
			else if(calculation==TextureStage::Calculation_CAMERASPACE){
				mD3DDevice->SetTextureStageState(stage,D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_CAMERASPACEPOSITION);
				mD3DDevice->SetTextureStageState(stage,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_COUNT4|D3DTTFF_PROJECTED);
			}

			result=mD3DDevice->SetTransform((D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0+stage),&cacheD3DMatrix TOADLET_D3DMFMT);
			TOADLET_CHECK_D3D9ERROR(result,"SetTransform");
		}
		else{
			mD3DDevice->SetTextureStageState(stage,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE);
		}

		const TextureBlend &blend=textureStage->blend;
		if(blend.operation!=TextureBlend::Operation_UNSPECIFIED){
			DWORD mode=0;
			switch(blend.operation){
				case TextureBlend::Operation_REPLACE:
					mode=D3DTOP_DISABLE;
				break;
				case TextureBlend::Operation_MODULATE:
					mode=D3DTOP_MODULATE;
				break;
				case TextureBlend::Operation_MODULATE_2X:
					mode=D3DTOP_MODULATE2X;
				break;
				case TextureBlend::Operation_MODULATE_4X:
					mode=D3DTOP_MODULATE4X;
				break;
				case TextureBlend::Operation_ADD:
					mode=D3DTOP_ADD;
				break;
				case TextureBlend::Operation_DOTPRODUCT:
					mode=D3DTOP_DOTPRODUCT3;
				break;
			}

			result=mD3DDevice->SetTextureStageState(stage,D3DTSS_COLOROP,mode);
			TOADLET_CHECK_D3D9ERROR(result,"SetTextureStageState");
			result=mD3DDevice->SetTextureStageState(stage,D3DTSS_ALPHAOP,mode);
			TOADLET_CHECK_D3D9ERROR(result,"SetTextureStageState");
			if(blend.source1!=TextureBlend::Source_UNSPECIFIED && blend.source2!=TextureBlend::Source_UNSPECIFIED){
				mD3DDevice->SetTextureStageState(stage,D3DTSS_COLORARG1,getD3DTextureBlendSource(blend.source1));
				mD3DDevice->SetTextureStageState(stage,D3DTSS_COLORARG2,getD3DTextureBlendSource(blend.source2));
			}
		}
		else{
			result=mD3DDevice->SetTextureStageState(stage,D3DTSS_COLOROP,D3DTOP_MODULATE);
			TOADLET_CHECK_D3D9ERROR(result,"SetTextureStageState");
			result=mD3DDevice->SetTextureStageState(stage,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
			TOADLET_CHECK_D3D9ERROR(result,"SetTextureStageState");
		}

		#if defined(TOADLET_HAS_DIRECT3DMOBILE)
			mD3DDevice->SetTextureStageState(stage,D3DMTSS_ADDRESSU,D3D9Texture::getD3DTADDRESS(textureStage->uAddressMode));
			mD3DDevice->SetTextureStageState(stage,D3DMTSS_ADDRESSV,D3D9Texture::getD3DTADDRESS(textureStage->vAddressMode));
			mD3DDevice->SetTextureStageState(stage,D3DMTSS_ADDRESSW,D3D9Texture::getD3DTADDRESS(textureStage->wAddressMode));

			mD3DDevice->SetTextureStageState(stage,D3DMTSS_MINFILTER,D3D9Texture::getD3DTEXF(textureStage->minFilter));
			mD3DDevice->SetTextureStageState(stage,D3DMTSS_MIPFILTER,D3D9Texture::getD3DTEXF(textureStage->mipFilter));
			mD3DDevice->SetTextureStageState(stage,D3DMTSS_MAGFILTER,D3D9Texture::getD3DTEXF(textureStage->magFilter));
		#else
			mD3DDevice->SetSamplerState(stage,D3DSAMP_ADDRESSU,D3D9Texture::getD3DTADDRESS(textureStage->uAddressMode));
			mD3DDevice->SetSamplerState(stage,D3DSAMP_ADDRESSV,D3D9Texture::getD3DTADDRESS(textureStage->vAddressMode));
			mD3DDevice->SetSamplerState(stage,D3DSAMP_ADDRESSW,D3D9Texture::getD3DTADDRESS(textureStage->wAddressMode));

			mD3DDevice->SetSamplerState(stage,D3DSAMP_MINFILTER,D3D9Texture::getD3DTEXF(textureStage->minFilter));
			mD3DDevice->SetSamplerState(stage,D3DSAMP_MIPFILTER,D3D9Texture::getD3DTEXF(textureStage->mipFilter));
			mD3DDevice->SetSamplerState(stage,D3DSAMP_MAGFILTER,D3D9Texture::getD3DTEXF(textureStage->magFilter));
		#endif
	}
	else{
		result=mD3DDevice->SetTexture(stage,NULL);
		TOADLET_CHECK_D3D9ERROR(result,"disableTextureStage");

		result=mD3DDevice->SetTextureStageState(stage,D3DTSS_TEXCOORDINDEX,0);
		TOADLET_CHECK_D3D9ERROR(result,"disableTextureStage");
	}
*/}

void D3D10Renderer::setProgram(const Program *program){
}

void D3D10Renderer::setShading(const Shading &shading){
}

void D3D10Renderer::setColorWrite(bool r,bool g,bool b,bool a){
}

void D3D10Renderer::setNormalize(const Normalize &normalize){
}

void D3D10Renderer::setShadowComparisonMethod(bool enabled){
}

void D3D10Renderer::setLight(int i,Light *light){
}

void D3D10Renderer::setLightEnabled(int i,bool enable){
}

void D3D10Renderer::setAmbientColor(const Color &ambient){
}

void D3D10Renderer::setMirrorY(bool mirrorY){
/*	mMirrorY=mirrorY;

	FaceCulling faceCulling=mFaceCulling;
	mFaceCulling=FaceCulling_NONE;
	setFaceCulling(faceCulling);
*/}

void D3D10Renderer::getShadowBiasMatrix(const Texture *shadowTexture,Matrix4x4 &result){
}

int D3D10Renderer::getClosestTextureFormat(int textureFormat){
	switch(textureFormat){
		case Texture::Format_L_8:
		case Texture::Format_LA_8:
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

D3D10_PRIMITIVE_TOPOLOGY D3D10Renderer::getD3D10_PRIMITIVE_TOPOLOGY(IndexData::Primitive primitive){
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
				"D3D10Renderer::getD3D10_PRIMITIVE_FORMAT: Invalid format");
			return D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED;
		break;
	}
}

DXGI_FORMAT D3D10Renderer::getIndexDXGI_FORMAT(IndexBuffer::IndexFormat format){
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
				"D3D10Renderer::getIndexDXGI_FORMAT: Invalid format");
			return DXGI_FORMAT_UNKNOWN;
		break;
	}
}

DXGI_FORMAT D3D10Renderer::getVertexDXGI_FORMAT(int format){
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
				"D3D10Renderer::getVertexDXGI_FORMAT: Invalid format");
			return DXGI_FORMAT_UNKNOWN;
		break;
	}
}

DXGI_FORMAT D3D10Renderer::getTextureDXGI_FORMAT(int format){
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
			Error::unknown(Categories::TOADLET_PEEPER,
				"D3D10Renderer::getDXGI_FORMAT: Invalid type");
			return DXGI_FORMAT_UNKNOWN;
	}
}

D3D10_MAP D3D10Renderer::getD3D10_MAP(int access,int usage){
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
				"D3D10Renderer::getD3D10_MAP: Invalid type");
			return (D3D10_MAP)0;
	}
}

D3D10_USAGE D3D10Renderer::getD3D10_USAGE(int usage){
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
}

char *D3D10Renderer::getSemanticName(int semantic){
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
