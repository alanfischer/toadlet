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

#include "D3D9Renderer.h"
#include "D3D9Texture.h"
#include "D3D9RenderTarget.h"
#include "D3D9SurfaceRenderTarget.h"
#include "D3D9VertexBuffer.h"
#include "D3D9IndexBuffer.h"
#include <toadlet/egg/MathConversion.h>
#include <toadlet/egg/Error.h>
#include <toadlet/peeper/LightEffect.h>
#include <toadlet/peeper/Light.h>
#include <toadlet/peeper/IndexBuffer.h>
#include <toadlet/peeper/VertexBuffer.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/peeper/Viewport.h>

using namespace toadlet::egg;
using namespace toadlet::egg::MathConversion;

#if defined(TOADLET_HAS_DIRECT3DMOBILE)
	#if defined(TOADLET_FIXED_POINT)
		#define	TOADLET_D3DMFMT ,D3DMFMT_D3DMVALUE_FIXED
	#else
		#define	TOADLET_D3DMFMT ,D3DMFMT_D3DMVALUE_FLOAT
	#endif
#else
	#define	TOADLET_D3DMFMT
#endif

namespace toadlet{
namespace peeper{

TOADLET_C_API Renderer* new_D3D9Renderer(){
	return new D3D9Renderer();
}

#if defined(TOADLET_BUILD_DYNAMIC)
	TOADLET_C_API Renderer* new_Renderer(){
		return new D3D9Renderer();
	}
#endif

D3D9Renderer::D3D9Renderer():
	mD3D(NULL),
	mD3DDevice(NULL),
	//mD3DCaps,
	mPrimaryRenderTarget(NULL),
	mRenderTarget(NULL),

	mFaceCulling(FaceCulling_NONE),
	mMirrorY(false)

	//mStatisticsSet,
	//mCapabilitySet
{
}

D3D9Renderer::~D3D9Renderer(){
	destroy();
}

bool D3D9Renderer::create(RenderTarget *target,int *options){
	Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ALERT,
		"creating D3D9Renderer");

	if(target==NULL){
		Error::nullPointer(Categories::TOADLET_PEEPER,
			"NULL RenderTarget");
		return false;
	}

	D3D9RenderTarget *d3dtarget=(D3D9RenderTarget*)target->getRootRenderTarget();
	mD3D=d3dtarget->getDirect3D9();
	mD3DDevice=d3dtarget->getDirect3DDevice9();
	if(mD3DDevice==NULL){
		Error::unknown(Categories::TOADLET_PEEPER,
			"invalid device");
		return false;
	}

	mPrimaryRenderTarget=target;
	mD3DPrimaryRenderTarget=d3dtarget;
	mRenderTarget=target;
	mD3DRenderTarget=d3dtarget;

	ZeroMemory(&mD3DCaps,sizeof(mD3DCaps));
	HRESULT result=mD3DDevice->GetDeviceCaps(&mD3DCaps);
	TOADLET_CHECK_D3D9ERROR(result,"Error getting caps");

	#if defined(TOADLET_HAS_DIRECT3DMOBILE)
		HRESULT renderToTextureResult=mD3D->CheckDeviceFormat( D3DMADAPTER_DEFAULT, D3DMDEVTYPE_DEFAULT, D3DMFMT_X8R8G8B8,
			D3DMUSAGE_RENDERTARGET, D3DMRTYPE_TEXTURE, D3DMFMT_X8R8G8B8);
		HRESULT renderToDepthTextureResult=E_FAIL;
	#else
		HRESULT renderToTextureResult=mD3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8,
			D3DUSAGE_RENDERTARGET, D3DRTYPE_TEXTURE, D3DFMT_X8R8G8B8);
		HRESULT renderToDepthTextureResult=mD3D->CheckDeviceFormat(	D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8,
			D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_TEXTURE, D3DFMT_D24S8);
	#endif

	setCapabilitySetFromCaps(mCapabilitySet,mD3DCaps,SUCCEEDED(renderToTextureResult),SUCCEEDED(renderToDepthTextureResult));

	setDefaultStates();

	Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ALERT,
		"created D3D9Renderer");

	return true;
}

bool D3D9Renderer::destroy(){
	return true;
}

Renderer::RendererStatus D3D9Renderer::getStatus(){
	HRESULT result=mD3DDevice->TestCooperativeLevel();

	switch(result){
		case D3DERR_DEVICELOST:
			return RendererStatus_UNAVAILABLE;
		case D3DERR_DEVICENOTRESET:
			return RendererStatus_NEEDSRESET;
		case D3DERR_DRIVERINTERNALERROR:
			return RendererStatus_UNAVAILABLE;
	}

	return RendererStatus_OK;
}

bool D3D9Renderer::reset(){
	mD3DPrimaryRenderTarget->reset();

	setDefaultStates();

	return true;
}

Texture *D3D9Renderer::createTexture(){
	return new D3D9Texture(this);
}

SurfaceRenderTarget *D3D9Renderer::createSurfaceRenderTarget(){
	return new D3D9SurfaceRenderTarget(this);
}

VertexBuffer *D3D9Renderer::createVertexBuffer(){
	return new D3D9VertexBuffer(this);
}

IndexBuffer *D3D9Renderer::createIndexBuffer(){
	return new D3D9IndexBuffer(this);
}

Program *D3D9Renderer::createProgram(){
	return NULL;
}

Shader *D3D9Renderer::createShader(){
	return NULL;
}

bool D3D9Renderer::setRenderTarget(RenderTarget *target){
	if(target==NULL){
		Error::nullPointer(Categories::TOADLET_PEEPER,
			"RenderTarget is NULL");
		return false;
	}

	D3D9RenderTarget *d3dtarget=(D3D9RenderTarget*)target->getRootRenderTarget();
	if(d3dtarget==NULL){
		Error::nullPointer(Categories::TOADLET_PEEPER,
			"RenderTarget is not a D3D9RenderTarget");
		return false;
	}

	mRenderTarget=target;
	mD3DRenderTarget=d3dtarget;

	mD3DRenderTarget->makeCurrent(mD3DDevice);

	return true;
}

void D3D9Renderer::setViewport(const Viewport &viewport){
	D3DVIEWPORT9 d3dviewport;
	d3dviewport.X=viewport.x;
	d3dviewport.Y=viewport.y;
	d3dviewport.Width=viewport.width;
	d3dviewport.Height=viewport.height;
	d3dviewport.MinZ=0.0f;
	d3dviewport.MaxZ=1.0f;

	int result=mD3DDevice->SetViewport(&d3dviewport);
	TOADLET_CHECK_D3D9ERROR(result,"setViewport");
}

void D3D9Renderer::clear(int clearFlags,const Color &clearColor){
	int d3dclear=0;

	if(clearFlags & ClearFlag_COLOR){
		d3dclear|=D3DCLEAR_TARGET;
	}
	if(clearFlags & ClearFlag_DEPTH){
		d3dclear|=D3DCLEAR_ZBUFFER;
	}
	if(clearFlags & ClearFlag_STENCIL){
		d3dclear|=D3DCLEAR_STENCIL;
	}

	D3DCOLOR color=toD3DCOLOR(clearColor);
	mD3DDevice->Clear(0,NULL,d3dclear,color,1.0f,0);
}

void D3D9Renderer::swap(){
	HRESULT result=mD3DDevice->Present(NULL,NULL,NULL,NULL);
	TOADLET_CHECK_D3D9ERROR(result,"swap");
}

void D3D9Renderer::setModelMatrix(const Matrix4x4 &matrix){
	toD3DMATRIX(cacheD3DMatrix,matrix);

	HRESULT result=mD3DDevice->SetTransform(D3DTS_WORLD,&cacheD3DMatrix TOADLET_D3DMFMT);
	TOADLET_CHECK_D3D9ERROR(result,"setModelMatrix");
}

void D3D9Renderer::setViewMatrix(const Matrix4x4 &matrix){
	toD3DMATRIX(cacheD3DMatrix,matrix);

	HRESULT result=mD3DDevice->SetTransform(D3DTS_VIEW,&cacheD3DMatrix TOADLET_D3DMFMT);
	TOADLET_CHECK_D3D9ERROR(result,"setViewMatrix");
}

void D3D9Renderer::setProjectionMatrix(const Matrix4x4 &matrix){
	if(mMirrorY){
		cacheMatrix4x4.set(matrix);
		cacheMatrix4x4.setAt(1,0,-cacheMatrix4x4.at(1,0));
		cacheMatrix4x4.setAt(1,1,-cacheMatrix4x4.at(1,1));
		cacheMatrix4x4.setAt(1,2,-cacheMatrix4x4.at(1,2));
		cacheMatrix4x4.setAt(1,3,-cacheMatrix4x4.at(1,3));
		toD3DMATRIX(cacheD3DMatrix,cacheMatrix4x4);
	}
	else{
		toD3DMATRIX(cacheD3DMatrix,matrix);
	}

	HRESULT result=mD3DDevice->SetTransform(D3DTS_PROJECTION,&cacheD3DMatrix TOADLET_D3DMFMT);
	TOADLET_CHECK_D3D9ERROR(result,"setProjectionMatrix");
}

void D3D9Renderer::beginScene(){
	HRESULT result=mD3DDevice->BeginScene();
	TOADLET_CHECK_D3D9ERROR(result,"beginScene");

	mStatisticsSet.reset();
}

void D3D9Renderer::endScene(){
	int i;
	for(i=0;i<mCapabilitySet.maxTextureStages;++i){
		setTextureStage(i,NULL);
	}

	HRESULT result=mD3DDevice->EndScene();
	TOADLET_CHECK_D3D9ERROR(result,"endScene");
}

void D3D9Renderer::renderPrimitive(const VertexData::ptr &vertexData,const IndexData::ptr &indexData){
	if(vertexData==NULL || indexData==NULL){
		Error::nullPointer(Categories::TOADLET_PEEPER,
			"VertexData or IndexData is NULL");
		return;
	}

	if(vertexData->vertexBuffers.size()>1){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D9Renderer does not support multiple streams yet");
		return;
	}

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
		TOADLET_CHECK_D3D9ERROR(result,"D3D9Renderer: SetStreamSource");
		#if !defined(TOADLET_HAS_DIRECT3DMOBILE)
			result=mD3DDevice->SetFVF(d3dvertexBuffer->mFVF);
			TOADLET_CHECK_D3D9ERROR(result,"D3D9Renderer: SetFVF");
		#endif
	}

	IndexBuffer *indexBuffer=indexData->indexBuffer;
	if(indexBuffer!=NULL){
		D3D9IndexBuffer *d3dindexBuffer=(D3D9IndexBuffer*)indexBuffer->getRootIndexBuffer();
		result=mD3DDevice->SetIndices(d3dindexBuffer->mIndexBuffer);
		TOADLET_CHECK_D3D9ERROR(result,"D3D9Renderer: SetIndices");

		result=mD3DDevice->DrawIndexedPrimitive(d3dpt,0,0,numVertexes,indexData->start,count);
		TOADLET_CHECK_D3D9ERROR(result,"D3D9Renderer: DrawIndexedPrimitive");
	}
	else{
		result=mD3DDevice->DrawPrimitive(d3dpt,indexData->start,count);
		TOADLET_CHECK_D3D9ERROR(result,"D3D9Renderer: DrawPrimitive");
	}
}

void D3D9Renderer::setDefaultStates(){
	setAlphaTest(AlphaTest_NONE,0.5);
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
}

void D3D9Renderer::setAlphaTest(const AlphaTest &alphaTest,scalar cutoff){
	if(alphaTest==AlphaTest_NONE){
		mD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE,false);
	}
	else{
		mD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE,true);

		mD3DDevice->SetRenderState(D3DRS_ALPHAREF,Math::toInt(cutoff*255));

		switch(alphaTest){
			case AlphaTest_LESS:
				mD3DDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_LESS);
			break;
			case AlphaTest_EQUAL:
				mD3DDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_EQUAL);
			break;
			case AlphaTest_LEQUAL:
				mD3DDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_LESSEQUAL);
			break;
			case AlphaTest_GREATER:
				mD3DDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);
			break;
			case AlphaTest_NOTEQUAL:
				mD3DDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_NOTEQUAL);
			break;
			case AlphaTest_GEQUAL:
				mD3DDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATEREQUAL);
			break;
			case AlphaTest_ALWAYS:
				mD3DDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_ALWAYS);
			break;
		}
	}
}

void D3D9Renderer::setBlend(const Blend &blend){
	if(blend.equals(Blend::Combination_DISABLED)){
		mD3DDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
		mD3DDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ZERO);
	}
	else{
		D3DBLEND src=getD3DBlendOperation(blend.source);
		D3DBLEND dest=getD3DBlendOperation(blend.dest);

		HRESULT hr;
		hr=mD3DDevice->SetRenderState(D3DRS_SRCBLEND,src);
		TOADLET_CHECK_D3D9ERROR(hr,"setBlendFunction");
		hr=mD3DDevice->SetRenderState(D3DRS_DESTBLEND,dest);
		TOADLET_CHECK_D3D9ERROR(hr,"setBlendFunction");
	}
}

void D3D9Renderer::setDepthTest(const DepthTest &depthTest){
	if(depthTest==DepthTest_NONE){
		mD3DDevice->SetRenderState(D3DRS_ZENABLE,D3DZB_FALSE);
	}
	else{
		mD3DDevice->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);

		DWORD func=0;

		switch(depthTest){
			case DepthTest_NEVER:
				func=D3DCMP_NEVER;
			break;
			case DepthTest_LESS:
				func=D3DCMP_LESS;
			break;
			case DepthTest_EQUAL:
				func=D3DCMP_EQUAL;
			break;
			case DepthTest_LEQUAL:
				func=D3DCMP_LESSEQUAL;
			break;
			case DepthTest_GREATER:
				func=D3DCMP_GREATER;
			break;
			case DepthTest_NOTEQUAL:
				func=D3DCMP_NOTEQUAL;
			break;
			case DepthTest_GEQUAL:
				func=D3DCMP_GREATEREQUAL;
			break;
			case DepthTest_ALWAYS:
				func=D3DCMP_ALWAYS;
			break;
		}

		HRESULT hr=mD3DDevice->SetRenderState(D3DRS_ZFUNC,func);
		TOADLET_CHECK_D3D9ERROR(hr,"setDepthTest");
	}
}

void D3D9Renderer::setDepthWrite(bool enable){
	HRESULT hr=mD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,enable);
	TOADLET_CHECK_D3D9ERROR(hr,"setDepthWrite");
}

void D3D9Renderer::setDithering(bool dithering){
	HRESULT hr=mD3DDevice->SetRenderState(D3DRS_DITHERENABLE,dithering);
	TOADLET_CHECK_D3D9ERROR(hr,"setDithering");
}

void D3D9Renderer::setFaceCulling(const FaceCulling &culling){
	switch(culling){
		case FaceCulling_NONE:
			mD3DDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
		break;
		case FaceCulling_FRONT:
			mD3DDevice->SetRenderState(D3DRS_CULLMODE,mMirrorY?D3DCULL_CW:D3DCULL_CCW);
		break;
		case FaceCulling_BACK:
			mD3DDevice->SetRenderState(D3DRS_CULLMODE,mMirrorY?D3DCULL_CCW:D3DCULL_CW);
		break;
	}

	mFaceCulling=culling;
}

void D3D9Renderer::setFogParameters(const Fog &fog,scalar nearDistance,scalar farDistance,const Color &color){
	if(fog==Fog_NONE){
		mD3DDevice->SetRenderState(D3DRS_FOGENABLE,FALSE);
	}
	else{
		float fNearDistance=scalarToFloat(nearDistance);
		float fFarDistance=scalarToFloat(farDistance);
	
		mD3DDevice->SetRenderState(D3DRS_FOGENABLE,TRUE);
	    mD3DDevice->SetRenderState(D3DRS_FOGCOLOR,toD3DCOLOR(color));
        mD3DDevice->SetRenderState(D3DRS_FOGVERTEXMODE,D3DFOG_LINEAR);
		mD3DDevice->SetRenderState(D3DRS_FOGSTART,*(DWORD*)(&fNearDistance));
		mD3DDevice->SetRenderState(D3DRS_FOGEND,*(DWORD*)(&fFarDistance));
	}
}

void D3D9Renderer::setFill(const Fill &fill){
	DWORD d3dfill;
	if(fill==Fill_POINT){
		d3dfill=D3DFILL_POINT;
	}
	else if(fill==Fill_LINE){
		d3dfill=D3DFILL_WIREFRAME;
	}
	else{
		d3dfill=D3DFILL_SOLID;
	}

	HRESULT hr=mD3DDevice->SetRenderState(D3DRS_FILLMODE,d3dfill);
	TOADLET_CHECK_D3D9ERROR(hr,"setFill");
}

void D3D9Renderer::setLighting(bool lighting){
	HRESULT hr=mD3DDevice->SetRenderState(D3DRS_LIGHTING,lighting);
	TOADLET_CHECK_D3D9ERROR(hr,"setLighting");
}

void D3D9Renderer::setLightEffect(const LightEffect &lightEffect){
	D3DMATERIAL9 material;

	toD3DCOLORVALUE(material.Ambient,lightEffect.ambient);
	toD3DCOLORVALUE(material.Diffuse,lightEffect.diffuse);
	toD3DCOLORVALUE(material.Specular,lightEffect.specular);
	#if !defined(TOADLET_HAS_DIRECT3DMOBILE) && defined(TOADLET_FIXED_POINT)
		material.Power=scalarToFloat(lightEffect.shininess);
	#else
		material.Power=lightEffect.shininess;
	#endif
	#if !defined(TOADLET_HAS_DIRECT3DMOBILE)
		toD3DCOLORVALUE(material.Emissive,lightEffect.emissive);
	#endif

	mD3DDevice->SetMaterial(&material TOADLET_D3DMFMT);

	mD3DDevice->SetRenderState(D3DRS_COLORVERTEX,lightEffect.trackColor);
	if(lightEffect.trackColor){
		mD3DDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE,D3DMCS_COLOR1);
		mD3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE,D3DMCS_COLOR1);
	}
	else{
		mD3DDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE,D3DMCS_MATERIAL);
		mD3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE,D3DMCS_MATERIAL);
	}
}

void D3D9Renderer::setDepthBias(scalar constant,scalar slope){
	float fconstant=scalarToFloat(constant);
	float fslope=scalarToFloat(slope);

	// TODO: We may need to scale these values to be more in-tune with the OpenGL ones
	mD3DDevice->SetRenderState(D3DRS_DEPTHBIAS,*(DWORD*)&fconstant);
	mD3DDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS,*(DWORD*)&fslope);
}

void D3D9Renderer::setPointParameters(bool sprite,scalar size,bool attenuated,scalar constant,scalar linear,scalar quadratic,scalar minSize,scalar maxSize){
	HRESULT result=S_OK;

	#if !defined(TOADLET_HAS_DIRECT3DMOBILE)
		// pointsize = size / sqrt(constant + linear*d + quadratic*d*d)
		// if a&b = 0, then quadratic = 1/(C*C) where C = first component of projMatrix * 1/2 screen width
		if(mCapabilitySet.pointSprites){
			mD3DDevice->SetRenderState(D3DRS_POINTSPRITEENABLE,sprite);
			mD3DDevice->SetRenderState(D3DRS_POINTSCALEENABLE,sprite);
		}

		float fSize=MathConversion::scalarToFloat(size);
		mD3DDevice->SetRenderState(D3DRS_POINTSIZE,*(DWORD*)(&fSize));
		
		if(attenuated){
			float fConstant=MathConversion::scalarToFloat(constant);
			float fLinear=MathConversion::scalarToFloat(linear);
			float fQuadratic=MathConversion::scalarToFloat(quadratic);

			mD3DDevice->SetRenderState(D3DRS_POINTSCALE_A,*(DWORD*)(&fConstant));
			mD3DDevice->SetRenderState(D3DRS_POINTSCALE_A,*(DWORD*)(&fLinear));
			mD3DDevice->SetRenderState(D3DRS_POINTSCALE_A,*(DWORD*)(&fQuadratic));
		}
	#endif
}

void D3D9Renderer::setTexturePerspective(bool texturePerspective){
	#if defined(TOADLET_HAS_DIRECT3DMOBILE)
		HRESULT hr=mD3DDevice->SetRenderState(D3DMRS_TEXTUREPERSPECTIVE,texturePerspective);
		TOADLET_CHECK_D3D9ERROR(hr,"setTexturePerspective");
	#endif
}

void D3D9Renderer::setTextureStage(int stage,TextureStage *textureStage){
	HRESULT result=S_OK;

	if(textureStage!=NULL){
		Texture *texture=textureStage->texture;
		if(texture!=NULL){
			D3D9Texture *d3dtexture=(D3D9Texture*)texture->getRootTexture();
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
}

void D3D9Renderer::setProgram(const Program *program){
}

void D3D9Renderer::setShading(const Shading &shading){
	switch(shading){
		case Shading_FLAT:
			mD3DDevice->SetRenderState(D3DRS_SHADEMODE,D3DSHADE_FLAT);
		break;
		case Shading_SMOOTH:
			mD3DDevice->SetRenderState(D3DRS_SHADEMODE,D3DSHADE_GOURAUD);
		break;
	}
}

void D3D9Renderer::setColorWrite(bool color){
	if(color){
		mD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE,	D3DCOLORWRITEENABLE_RED		|
															D3DCOLORWRITEENABLE_GREEN	|
															D3DCOLORWRITEENABLE_BLUE	|
															D3DCOLORWRITEENABLE_ALPHA);
	}
	else{
		mD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE,0);
	}
}

void D3D9Renderer::setNormalize(const Normalize &normalize){
	if(normalize!=Normalize_NONE){
		mD3DDevice->SetRenderState(D3DRS_NORMALIZENORMALS,TRUE);
	}
	else{
		mD3DDevice->SetRenderState(D3DRS_NORMALIZENORMALS,FALSE);
	}
}

void D3D9Renderer::setShadowComparisonMethod(bool enabled){
	if(enabled){
		mD3DDevice->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_MAX);
	}
	else{
		mD3DDevice->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);
	}
}

void D3D9Renderer::setLight(int i,Light *light){
	D3DLIGHT9 d3dlight;
	ZeroMemory(&d3dlight,sizeof(D3DLIGHT9));

	switch(light->getType()){
		case Light::Type_DIRECTION:{
			d3dlight.Type=D3DLIGHT_DIRECTIONAL;
			toD3DVECTOR(d3dlight.Direction,light->direction);
			break;
		}
		case Light::Type_POSITION:{
			d3dlight.Type=D3DLIGHT_POINT;
			toD3DVECTOR(d3dlight.Position,light->position);
			break;
		}
		case Light::Type_SPOT:{
			#if !defined(TOADLET_HAS_DIRECT3DMOBILE)
				d3dlight.Type=D3DLIGHT_SPOT;
				toD3DVECTOR(d3dlight.Position,light->position);
			#endif
			break;
		}
	}

	toD3DCOLORVALUE(d3dlight.Diffuse,light->diffuseColor);
	toD3DCOLORVALUE(d3dlight.Specular,light->specularColor);
	#if !defined(TOADLET_HAS_DIRECT3DMOBILE) && defined(TOADLET_FIXED_POINT)
		d3dlight.Attenuation1=scalarToFloat(light->linearAttenuation);
		d3dlight.Range=scalarToFloat(light->radius);
	#else
		d3dlight.Attenuation1=light->linearAttenuation;
		d3dlight.Range=light->radius;
	#endif

	mD3DDevice->SetLight(i,&d3dlight TOADLET_D3DMFMT);
}

void D3D9Renderer::setLightEnabled(int i,bool enable){
	HRESULT hr=mD3DDevice->LightEnable(i,enable);
	TOADLET_CHECK_D3D9ERROR(hr,"setLightEnabled");
}

void D3D9Renderer::setAmbientColor(const Color &ambient){
	HRESULT hr=mD3DDevice->SetRenderState(D3DRS_AMBIENT,toD3DCOLOR(ambient));
	TOADLET_CHECK_D3D9ERROR(hr,"setAmbientColor");
}

void D3D9Renderer::setCapabilitySetFromCaps(CapabilitySet &capabilitySet,const D3DCAPS9 &caps,bool renderToTexture,bool renderToDepthTexture){
	capabilitySet.resetOnResize=true;
	capabilitySet.hardwareTextures=true;
	capabilitySet.hardwareIndexBuffers=true;
	capabilitySet.hardwareVertexBuffers=true;
	#if defined(TOADLET_HAS_DIRECT3DMOBILE)
		capabilitySet.pointSprites=false;
	#else
		capabilitySet.pointSprites=(caps.FVFCaps & D3DFVFCAPS_PSIZE)!=0 && caps.MaxPointSize>1.0f;
	#endif
	capabilitySet.maxLights=caps.MaxActiveLights;
	capabilitySet.maxTextureStages=caps.MaxTextureBlendStages;
	capabilitySet.maxTextureSize=math::Math::minVal(caps.MaxTextureWidth,caps.MaxTextureHeight);
	capabilitySet.textureDot3=(caps.TextureOpCaps & D3DTEXOPCAPS_DOTPRODUCT3)!=0;
	capabilitySet.textureNonPowerOf2=(caps.TextureCaps & D3DPTEXTURECAPS_POW2)==0 && (caps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY)==0;
	#if defined(TOADLET_HAS_DIRECT3DMOBILE)
		capabilitySet.textureNonPowerOf2Restricted=false;
	#else
		capabilitySet.textureNonPowerOf2Restricted=(caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL)!=0;
	#endif
	capabilitySet.renderToTexture=renderToTexture;
	capabilitySet.renderToDepthTexture=renderToDepthTexture;
	capabilitySet.renderToTextureNonPowerOf2Restricted=capabilitySet.textureNonPowerOf2Restricted && capabilitySet.renderToTexture;
	#if defined(TOADLET_HAS_DIRECT3DMOBILE) && defined(TOADLET_FIXED_POINT)
		capabilitySet.idealVertexFormatBit=VertexElement::Format_BIT_FIXED_32;
	#else
		capabilitySet.idealVertexFormatBit=VertexElement::Format_BIT_FLOAT_32;
	#endif

	#if defined(TOADLET_HAS_DIRECT3DMOBILE)
		Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ALERT,
			String("D3DM has lockable textures:")+((caps.SurfaceCaps & D3DMSURFCAPS_LOCKTEXTURE)>0));
	#endif
}

DWORD D3D9Renderer::getD3DTextureBlendSource(TextureBlend::Source blend){
	switch(blend){
		case TextureBlend::Source_PREVIOUS:
			return D3DTA_CURRENT;
		case TextureBlend::Source_TEXTURE:
			return D3DTA_TEXTURE;
		case TextureBlend::Source_PRIMARY_COLOR:
			return D3DTA_DIFFUSE;
		default:
			return 0;
	}
}

D3DBLEND D3D9Renderer::getD3DBlendOperation(Blend::Operation blend){
	switch(blend){
		case Blend::Operation_ONE:
			return D3DBLEND_ONE;
		case Blend::Operation_ZERO:
			return D3DBLEND_ZERO;
		case Blend::Operation_DEST_COLOR:
			return D3DBLEND_DESTCOLOR;
		case Blend::Operation_SOURCE_COLOR:
			return D3DBLEND_SRCCOLOR;
		case Blend::Operation_ONE_MINUS_DEST_COLOR:
			return D3DBLEND_INVDESTCOLOR;
		case Blend::Operation_ONE_MINUS_SOURCE_COLOR:
			return D3DBLEND_INVSRCCOLOR;
		case Blend::Operation_DEST_ALPHA:
			return D3DBLEND_DESTALPHA;
		case Blend::Operation_SOURCE_ALPHA:
			return D3DBLEND_SRCALPHA;
		case Blend::Operation_ONE_MINUS_DEST_ALPHA:
			return D3DBLEND_INVDESTALPHA;
		case Blend::Operation_ONE_MINUS_SOURCE_ALPHA:
			return D3DBLEND_INVSRCALPHA;
		default:
			return (D3DBLEND)0;
	}
}

void D3D9Renderer::getPrimitiveTypeAndCount(D3DPRIMITIVETYPE &d3dpt,int &count,IndexData::Primitive prim,int numIndexes){
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

void D3D9Renderer::setMirrorY(bool mirrorY){
	mMirrorY=mirrorY;

	FaceCulling faceCulling=mFaceCulling;
	mFaceCulling=FaceCulling_NONE;
	setFaceCulling(faceCulling);
}

void D3D9Renderer::getShadowBiasMatrix(const Texture *shadowTexture,Matrix4x4 &result){
	int width=shadowTexture->getWidth();
	int height=shadowTexture->getHeight();
	scalar xoff=Math::HALF+Math::div(Math::HALF,Math::fromInt(width));
	scalar yoff=Math::HALF+Math::div(Math::HALF,Math::fromInt(height));
	result.set( Math::HALF, 0,           0,         xoff,
				0,          -Math::HALF, 0,         yoff,
				0,          0,           Math::ONE, 0,
				0,          0,           0,         Math::ONE);
}

}
}
