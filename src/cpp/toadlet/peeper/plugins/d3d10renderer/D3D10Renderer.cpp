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
#include "D3D10Texture.h"
#include "D3D10RenderTarget.h"
#include "D3D10SurfaceRenderTarget.h"
#include "D3D10Buffer.h"
#include <toadlet/egg/MathConversion.h>
#include <toadlet/egg/Error.h>
#include <toadlet/peeper/LightEffect.h>
#include <toadlet/peeper/Light.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/peeper/Viewport.h>

#include <d3dx10.h>
//#pragma comment(lib,"d3dx10.lib")

using namespace toadlet::egg;
using namespace toadlet::egg::MathConversion;

namespace toadlet{
namespace peeper{

TOADLET_C_API Renderer* new_D3D10Renderer(){
	return new D3D10Renderer();
}

#if defined(TOADLET_BUILD_DYNAMIC)
	TOADLET_C_API Renderer* new_Renderer(){
		return new D3D10Renderer();
	}
#endif

D3D10Renderer::D3D10Renderer():
	mD3DDevice(NULL),
	mPrimaryRenderTarget(NULL),
	mRenderTarget(NULL)

	//mStatisticsSet,
	//mCapabilitySet
{
}

D3D10Renderer::~D3D10Renderer(){
	destroy();
}


LPD3D10EFFECT g_lpEffect = NULL;
LPD3D10EFFECTTECHNIQUE pTechnique=NULL;

bool D3D10Renderer::create(RenderTarget *target,int *options){
	Logger::alert(Categories::TOADLET_PEEPER,
		"creating D3D10Renderer");

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
	mCapabilitySet.maxTextureStages=0;
	mCapabilitySet.maxTextureSize=0;
	mCapabilitySet.textureDot3=false;
	mCapabilitySet.textureNonPowerOf2=true;
	mCapabilitySet.textureNonPowerOf2Restricted=true;
	mCapabilitySet.textureAutogenMipMaps=true;
	mCapabilitySet.renderToTexture=true;
	mCapabilitySet.renderToDepthTexture=true;
	mCapabilitySet.renderToTextureNonPowerOf2Restricted=true;
	mCapabilitySet.idealVertexFormatBit=VertexElement::Format_BIT_FLOAT_32;

	setDefaultStates();

	Logger::alert(Categories::TOADLET_PEEPER,
		"created D3D10Renderer");



// Create the effect
HRESULT res;
LPD3D10BLOB errorBuffer;
//10CreateEffectFromFile( TEXT("D3D10_Tut4.fx"), NULL, NULL, TEXT("fx_4_0"),D3D10_SHADER_ENABLE_STRICTNESS, 0, mD3DDevice, NULL, NULL, &g_lpEffect, &errorBuffer, &res );
if( FAILED(res ))
{
	TCHAR* error = (TCHAR*)errorBuffer->GetBufferPointer();
//	return FALSE;
}
// Obtain the technique
pTechnique = g_lpEffect->GetTechniqueByName( "Render" );


	// Define the input layout (replaces VertexDeclaration from D3D9)
	D3D10_INPUT_ELEMENT_DESC elementDesc[] =
	{
		{ TEXT("POSITION"), 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D10_INPUT_PER_VERTEX_DATA, 0 },  
		{ TEXT("COLOR"),    0, DXGI_FORMAT_R32G32B32_FLOAT,  0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },  
		{ TEXT("TEXCOORD"), 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(elementDesc)/sizeof(elementDesc[0]);

	// Create the input layout
	ID3D10InputLayout* pVertexLayout = NULL;
	D3D10_PASS_DESC PassDesc;
	pTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
	if( FAILED( mD3DDevice->CreateInputLayout( elementDesc, numElements, PassDesc.pIAInputSignature, 
		PassDesc.IAInputSignatureSize, &pVertexLayout ) ) )
		return FALSE;
	// Set the input layout
	mD3DDevice->IASetInputLayout( pVertexLayout );



	return true;
}

bool D3D10Renderer::destroy(){
	return true;
}

Renderer::RendererStatus D3D10Renderer::getStatus(){
	return RendererStatus_OK;
}

bool D3D10Renderer::reset(){
	// No device reset necessary

	setDefaultStates();

	return true;
}

Texture *D3D10Renderer::createTexture(){
	return new D3D10Texture(this);
}

SurfaceRenderTarget *D3D10Renderer::createSurfaceRenderTarget(){
	return NULL;//new D3D10SurfaceRenderTarget(this);
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
	return NULL;
}

bool D3D10Renderer::setRenderTarget(RenderTarget *target){
	if(target==NULL){
		Error::nullPointer(Categories::TOADLET_PEEPER,
			"RenderTarget is NULL");
		return false;
	}

	D3D10RenderTarget *d3dtarget=(D3D10RenderTarget*)target->getRootRenderTarget();
	if(d3dtarget==NULL){
		Error::nullPointer(Categories::TOADLET_PEEPER,
			"RenderTarget is not a D3D10RenderTarget");
		return false;
	}

	mRenderTarget=target;
	mD3DRenderTarget=d3dtarget;

	mD3DRenderTarget->makeCurrent(mD3DDevice);

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
	int i;
	for(i=0;i<mCapabilitySet.maxTextureStages;++i){
		setTextureStage(i,NULL);
	}
}

inline void toD3DMATRIX(D3DMATRIX &r,const Matrix4x4 &s){
	r.m[0][0]=s.at(0,0);
	r.m[1][0]=s.at(0,1);
	r.m[2][0]=s.at(0,2);
	r.m[3][0]=s.at(0,3);
	r.m[0][1]=s.at(1,0);
	r.m[1][1]=s.at(1,1);
	r.m[2][1]=s.at(1,2);
	r.m[3][1]=s.at(1,3);
	r.m[0][2]=s.at(2,0);
	r.m[1][2]=s.at(2,1);
	r.m[2][2]=s.at(2,2);
	r.m[3][2]=s.at(2,3);
	r.m[0][3]=s.at(3,0);
	r.m[1][3]=s.at(3,1);
	r.m[2][3]=s.at(3,2);
	r.m[3][3]=s.at(3,3);
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
	UINT stride=vertexBuffer->mVertexFormat->getVertexSize();
	UINT offset=0;
mD3DDevice->IASetVertexBuffers(0,1,&vertexBuffer->mBuffer,&stride,&offset);

//	pWorldMatrixEffectVariable->SetMatrix(w);
//	pViewMatrixEffectVariable->SetMatrix(viewMatrix);
//	pProjectionMatrixEffectVariable->SetMatrix(projectionMatrix);
//Matrix4x4 shaderMatrix=mViewMatrix*mProjectionMatrix*mModelMatrix;
Matrix4x4 shaderMatrix=mProjectionMatrix*mViewMatrix*mModelMatrix;
//	g_ShaderMatrix = ViewMatrix * PerspectiveMatrix * WorldMatrix;
D3DMATRIX result;
toD3DMATRIX(result,shaderMatrix);
//Math::transpose(result,shaderMatrix);

		//handover the matrix to the effect.
		LPD3D10EFFECTMATRIXVARIABLE lpEffectMatrixVar = g_lpEffect->GetVariableByName("ShaderMatrix")->AsMatrix();
		lpEffectMatrixVar->SetMatrix((float*)&result);

	D3D10Buffer *buffer=(D3D10Buffer*)(indexData->getIndexBuffer()->getRootIndexBuffer());
	mD3DDevice->IASetIndexBuffer(buffer->mBuffer,DXGI_FORMAT_R16_UINT,0);

	mD3DDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

D3D10_TECHNIQUE_DESC techDesc;
pTechnique->GetDesc( &techDesc );
for( UINT p = 0; p < techDesc.Passes; ++p )
{
	pTechnique->GetPassByIndex( p )->Apply(0);
	mD3DDevice->Draw(indexData->getCount(),0);
//	pd3dDevice->Draw( 4, 0 );
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

bool D3D10Renderer::copyToSurface(Surface *surface){
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

void D3D10Renderer::setFogParameters(const Fog &fog,scalar nearDistance,scalar farDistance,const Color &color){
}

void D3D10Renderer::setFill(const Fill &fill){
}

void D3D10Renderer::setLighting(bool lighting){
}

void D3D10Renderer::setLightEffect(const LightEffect &lightEffect){
}

void D3D10Renderer::setDepthBias(scalar constant,scalar slope){
}

void D3D10Renderer::setPointParameters(bool sprite,scalar size,bool attenuated,scalar constant,scalar linear,scalar quadratic,scalar minSize,scalar maxSize){
}

void D3D10Renderer::setTexturePerspective(bool texturePerspective){
}

void D3D10Renderer::setTextureStage(int stage,TextureStage *textureStage){
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

		// TODO: Only if we're not using shaders
		TextureStage::Calculation calculation=textureStage->calculation;
		if(calculation!=TextureStage::Calculation_DISABLED){
			if(calculation==TextureStage::Calculation_NORMAL){
				// TODO: Get this working with 3D Texture coordinates.  Doesnt seem to currently
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

void D3D10Renderer::setColorWrite(bool color){
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
/*
void D3D10Renderer::getPrimitiveTypeAndCount(D3DPRIMITIVETYPE &d3dpt,int &count,IndexData::Primitive prim,int numIndexes){
	switch(prim){
		case IndexData::Primitive_POINTS:
			d3dpt=D3DPT_POINTLIST;
			count=numIndexes;
		break;
		case IndexData::Primitive_LINES:
			d3dpt=D3DPT_LINELIST;
			count=numIndexes/2;
		break;
		case IndexData::Primitive_LINESTRIP:
			d3dpt=D3DPT_LINESTRIP;
			count=numIndexes-1;
		break;
		case IndexData::Primitive_TRIS:
			d3dpt=D3DPT_TRIANGLELIST;
			count=numIndexes/3;
		break;
		case IndexData::Primitive_TRISTRIP:
			d3dpt=D3DPT_TRIANGLESTRIP;
			count=numIndexes-2;
		break;
		case IndexData::Primitive_TRIFAN:
			d3dpt=D3DPT_TRIANGLEFAN;
			count=numIndexes-2;
		break;
	}
}
*/
void D3D10Renderer::setMirrorY(bool mirrorY){
/*	mMirrorY=mirrorY;

	FaceCulling faceCulling=mFaceCulling;
	mFaceCulling=FaceCulling_NONE;
	setFaceCulling(faceCulling);
*/}

void D3D10Renderer::getShadowBiasMatrix(const Texture *shadowTexture,Matrix4x4 &result){
}

}
}
