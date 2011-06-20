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

#include "D3D9RenderDevice.h"
#include "D3D9PixelBufferRenderTarget.h"
#include "D3D9RenderTarget.h"
#include "D3D9RenderState.h"
#include "D3D9ShaderState.h"
#include "D3D9Texture.h"
#include "D3D9IndexBuffer.h"
#include "D3D9VertexBuffer.h"
#include "D3D9VertexFormat.h"
#if !defined(TOADLET_SET_D3DM)
	#include "D3D9Query.h"
	#include "D3D9Shader.h"
#endif
#include <toadlet/egg/MathConversion.h>
#include <toadlet/egg/Error.h>
#include <toadlet/peeper/FogState.h>
#include <toadlet/peeper/MaterialState.h>
#include <toadlet/peeper/LightState.h>
#include <toadlet/peeper/PointState.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/peeper/Viewport.h>

using namespace toadlet::egg;
using namespace toadlet::egg::MathConversion;

#if defined(TOADLET_SET_D3DM)
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

#if defined(TOADLET_SET_D3DM)
	TOADLET_C_API RenderDevice* new_D3DMRenderDevice(){
		return new D3D9RenderDevice();
	}
#else
	TOADLET_C_API RenderDevice* new_D3D9RenderDevice(){
		return new D3D9RenderDevice();
	}
#endif

#if defined(TOADLET_BUILD_DYNAMIC)
	TOADLET_C_API RenderDevice* new_RenderDevice(){
		return new D3D9RenderDevice();
	}
#endif

D3D9RenderDevice::D3D9RenderDevice():
	mD3D(NULL),
	mD3DDevice(NULL),
	mD3DDevType((D3DDEVTYPE)0),
	mD3DAdapter(0),
	mD3DAdapterFormat(D3DFMT_UNKNOWN),
	//mD3DCaps,
	mPrimaryRenderTarget(NULL),
	mD3DPrimaryRenderTarget(NULL),
	mRenderTarget(NULL),
	mD3DRenderTarget(NULL)
{
}

D3D9RenderDevice::~D3D9RenderDevice(){
	destroy();
}

bool D3D9RenderDevice::create(RenderTarget *target,int *options){
	Logger::alert(Categories::TOADLET_PEEPER,
		"creating "+Categories::TOADLET_PEEPER+".D3D9RenderDevice");

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

	/// @todo: Get these from the D3D9RenderTarget
	#if defined(TOADLET_SET_D3DM)
		mD3DDevType=D3DMDEVTYPE_DEFAULT;
	#else
		mD3DDevType=D3DDEVTYPE_HAL;
	#endif
	mD3DAdapter=D3DADAPTER_DEFAULT;
	mD3DAdapterFormat=D3DFMT_X8R8G8B8;

	mPrimaryRenderTarget=target;
	mD3DPrimaryRenderTarget=d3dtarget;
	mRenderTarget=target;
	mD3DRenderTarget=d3dtarget;

	ZeroMemory(&mD3DCaps,sizeof(mD3DCaps));
	HRESULT result=mD3DDevice->GetDeviceCaps(&mD3DCaps);
	TOADLET_CHECK_D3D9ERROR(result,"Error getting caps");

	HRESULT renderToTextureResult=isD3DFORMATValid(D3DFMT_X8R8G8B8,D3DUSAGE_RENDERTARGET);
	HRESULT renderToDepthTextureResult=
	#if defined(TOADLET_SET_D3DM)
		E_FAIL;
	#else
		isD3DFORMATValid(D3DFMT_D24S8,D3DUSAGE_DEPTHSTENCIL);
	#endif
	setCapsFromD3DCAPS9(mCaps,mD3DCaps,SUCCEEDED(renderToTextureResult),SUCCEEDED(renderToDepthTextureResult));

	setDefaultState();

	Logger::alert(Categories::TOADLET_PEEPER,
		"created D3D9RenderDevice");

	return true;
}

void D3D9RenderDevice::destroy(){
	if(mD3D!=NULL){
		mD3D=NULL;
		mD3DDevice=NULL;
		
		mPrimaryRenderTarget=NULL;
		mD3DPrimaryRenderTarget=NULL;
		mRenderTarget=NULL;
		mD3DRenderTarget=NULL;
		
		Logger::alert(Categories::TOADLET_PEEPER,
			"destroyed D3D9RenderDevice");
	}
}

RenderDevice::DeviceStatus D3D9RenderDevice::getStatus(){
	HRESULT result=mD3DDevice->TestCooperativeLevel();

	switch(result){
		case D3DERR_DEVICELOST:
			return DeviceStatus_UNAVAILABLE;
		case D3DERR_DEVICENOTRESET:
			return DeviceStatus_NEEDSRESET;
		case D3DERR_DRIVERINTERNALERROR:
			return DeviceStatus_UNAVAILABLE;
	}

	return DeviceStatus_OK;
}

bool D3D9RenderDevice::reset(){
	mD3DPrimaryRenderTarget->reset();

	setDefaultState();

	return true;
}

bool D3D9RenderDevice::activateAdditionalContext(){
	D3DDEVICE_CREATION_PARAMETERS params={0};
	mD3DDevice->GetCreationParameters(&params);
	return (params.BehaviorFlags & D3DCREATE_MULTITHREADED)!=0;
}

Texture *D3D9RenderDevice::createTexture(){
	return new D3D9Texture(this);
}

PixelBufferRenderTarget *D3D9RenderDevice::createPixelBufferRenderTarget(){
	return new D3D9PixelBufferRenderTarget(this);
}

PixelBuffer *D3D9RenderDevice::createPixelBuffer(){
	return new D3D9PixelBuffer(this,false);
}

VertexFormat *D3D9RenderDevice::createVertexFormat(){
	return new D3D9VertexFormat(this);
}

VertexBuffer *D3D9RenderDevice::createVertexBuffer(){
	return new D3D9VertexBuffer(this);
}

IndexBuffer *D3D9RenderDevice::createIndexBuffer(){
	return new D3D9IndexBuffer(this);
}

Shader *D3D9RenderDevice::createShader(){
	return new D3D9Shader(this);
}

Query *D3D9RenderDevice::createQuery(){
	#if !defined(TOADLET_SET_D3DM)
		return new D3D9Query(this);
	#else
		Error::unimplemented("D3D9RenderDevice::createQuery is unavailable");
		return NULL;
	#endif
}

RenderState *D3D9RenderDevice::createRenderState(){
	return new D3D9RenderState(this);
}

ShaderState *D3D9RenderDevice::createShaderState(){
	return new D3D9ShaderState(this);
}

bool D3D9RenderDevice::setRenderTarget(RenderTarget *target){
	D3D9RenderTarget *d3dtarget=NULL;
	if(target!=NULL){
		d3dtarget=(D3D9RenderTarget*)target->getRootRenderTarget();
		if(d3dtarget==NULL){
			Error::nullPointer(Categories::TOADLET_PEEPER,
				"RenderTarget is not a D3D9RenderTarget");
			return false;
		}
	}

	if(mD3DRenderTarget!=NULL){
		mD3DRenderTarget->deactivate();
	}

	mRenderTarget=target;
	mD3DRenderTarget=d3dtarget;

	if(mD3DRenderTarget){
		mD3DRenderTarget->activate();
	}

	return true;
}

void D3D9RenderDevice::setViewport(const Viewport &viewport){
	D3DVIEWPORT9 d3dviewport;
	d3dviewport.X=viewport.x;
	d3dviewport.Y=viewport.y;
	d3dviewport.Width=viewport.width;
	d3dviewport.Height=viewport.height;
	d3dviewport.MinZ=0.0f;
	d3dviewport.MaxZ=1.0f;

	HRESULT result=mD3DDevice->SetViewport(&d3dviewport);
	TOADLET_CHECK_D3D9ERROR(result,"setViewport");
}

void D3D9RenderDevice::clear(int clearFlags,const Vector4 &clearColor){
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
	HRESULT result=mD3DDevice->Clear(0,NULL,d3dclear,color,1.0f,0);
	TOADLET_CHECK_D3D9ERROR(result,"clear");
}

void D3D9RenderDevice::swap(){
	mD3DRenderTarget->swap();
}
Matrix4x4 mm,vm,pm;
void D3D9RenderDevice::setModelMatrix(const Matrix4x4 &matrix){
	toD3DMATRIX(cacheD3DMatrix,matrix);
mm=matrix;
	HRESULT result=mD3DDevice->SetTransform(D3DTS_WORLD,&cacheD3DMatrix TOADLET_D3DMFMT);
	TOADLET_CHECK_D3D9ERROR(result,"setModelMatrix");
}

void D3D9RenderDevice::setViewMatrix(const Matrix4x4 &matrix){
	toD3DMATRIX(cacheD3DMatrix,matrix);
vm=matrix;
	HRESULT result=mD3DDevice->SetTransform(D3DTS_VIEW,&cacheD3DMatrix TOADLET_D3DMFMT);
	TOADLET_CHECK_D3D9ERROR(result,"setViewMatrix");
}

void D3D9RenderDevice::setProjectionMatrix(const Matrix4x4 &matrix){
	D3DMATRIX &d3dmatrix=cacheD3DMatrix;
pm=matrix;
	toD3DMATRIX(d3dmatrix,matrix);

	// Convert depth ranges from -1,1 to 0,1
	//  Thanks to the OGRE project
	int i;
	for(i=0;i<4;++i){
		d3dmatrix.m[i][2]=(d3dmatrix.m[i][2]+d3dmatrix.m[i][3])/2;
	};

	HRESULT result=mD3DDevice->SetTransform(D3DTS_PROJECTION,&d3dmatrix TOADLET_D3DMFMT);
	TOADLET_CHECK_D3D9ERROR(result,"setProjectionMatrix");
}

void D3D9RenderDevice::beginScene(){
	HRESULT result=mD3DDevice->BeginScene();
	TOADLET_CHECK_D3D9ERROR(result,"beginScene");
}

void D3D9RenderDevice::endScene(){
	int i;
	for(i=0;i<mCaps.maxTextureStages;++i){
		setTexture(i,NULL);
	}

	HRESULT result=mD3DDevice->EndScene();
	TOADLET_CHECK_D3D9ERROR(result,"endScene");
}

void D3D9RenderDevice::renderPrimitive(VertexData *vertexData,IndexData *indexData){
	if(vertexData==NULL || indexData==NULL){
		Error::nullPointer(Categories::TOADLET_PEEPER,
			"VertexData or IndexData is NULL");
		return;
	}

	if(vertexData->vertexBuffers.size()>1){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D9RenderDevice does not support multiple streams yet");
		return;
	}

Matrix4x4 shaderMatrix;
Math::transpose(shaderMatrix,pm*vm*mm);
mD3DDevice->SetVertexShaderConstantF(0,shaderMatrix.getData(),4);

	HRESULT result;
	D3DPRIMITIVETYPE d3dpt;
	int count;
	getPrimitiveTypeAndCount(d3dpt,count,indexData->primitive,indexData->count);
	if(count==0){
		return;
	}

	int i;
	int numVertexes=0;
	int numVertexBuffers=vertexData->vertexBuffers.size();
	for(i=0;i<numVertexBuffers;++i){
		D3D9VertexBuffer *d3dvertexBuffer=(D3D9VertexBuffer*)vertexData->vertexBuffers[i]->getRootVertexBuffer();
		D3D9VertexFormat *d3dvertexFormat=(D3D9VertexFormat*)d3dvertexBuffer->mVertexFormat->getRootVertexFormat();
		if(numVertexes==0){
			numVertexes=d3dvertexBuffer->mSize;
		}
		#if defined(TOADLET_SET_D3DM)
			result=mD3DDevice->SetStreamSource(i,d3dvertexBuffer->mVertexBuffer,d3dvertexFormat->mVertexSize);
		#else
			result=mD3DDevice->SetStreamSource(i,d3dvertexBuffer->mVertexBuffer,0,d3dvertexFormat->mVertexSize);
		#endif
		TOADLET_CHECK_D3D9ERROR(result,"D3D9RenderDevice: SetStreamSource");
		#if defined(TOADLET_SET_D3DM)
			result=mD3DDevice->SetFVF(d3dvertexFormat->getFVF());
			TOADLET_CHECK_D3D9ERROR(result,"D3D9RenderDevice: SetFVF");
		#else
			result=mD3DDevice->SetVertexDeclaration(d3dvertexFormat->getDeclaration());
			TOADLET_CHECK_D3D9ERROR(result,"D3D9RenderDevice: SetVertexDeclaration");
		#endif
	}

	IndexBuffer *indexBuffer=indexData->indexBuffer;
	if(indexBuffer!=NULL){
		D3D9IndexBuffer *d3dindexBuffer=(D3D9IndexBuffer*)indexBuffer->getRootIndexBuffer();
		result=mD3DDevice->SetIndices(d3dindexBuffer->mIndexBuffer);
		TOADLET_CHECK_D3D9ERROR(result,"D3D9RenderDevice: SetIndices");

		result=mD3DDevice->DrawIndexedPrimitive(d3dpt,0,0,numVertexes,indexData->start,count);
		TOADLET_CHECK_D3D9ERROR(result,"D3D9RenderDevice: DrawIndexedPrimitive");
	}
	else{
		result=mD3DDevice->DrawPrimitive(d3dpt,indexData->start,count);
		TOADLET_CHECK_D3D9ERROR(result,"D3D9RenderDevice: DrawPrimitive");
	}
}

bool D3D9RenderDevice::copyFrameBufferToPixelBuffer(PixelBuffer *dst){
	D3D9PixelBuffer *d3dpixelBuffer=(D3D9PixelBuffer*)dst->getRootPixelBuffer();

	IDirect3DSurface9 *renderSurface=NULL;
	#if defined(TOADLET_SET_D3DM)
		mD3DDevice->GetRenderTarget(&renderSurface);
	#else
		mD3DDevice->GetRenderTarget(0,&renderSurface);
	#endif

	bool result=copySurface(d3dpixelBuffer->getSurface(),renderSurface);

	renderSurface->Release();

	return result;
}

bool D3D9RenderDevice::copyPixelBuffer(PixelBuffer *dst,PixelBuffer *src){
	D3D9PixelBuffer *d3ddst=(D3D9PixelBuffer*)dst->getRootPixelBuffer();
	D3D9PixelBuffer *d3dsrc=(D3D9PixelBuffer*)src->getRootPixelBuffer();

	return copySurface(d3ddst->getSurface(),d3dsrc->getSurface());
}

bool D3D9RenderDevice::copySurface(IDirect3DSurface9 *dst,IDirect3DSurface9 *src){
	if(src==NULL || dst==NULL){
		Error::nullPointer(Categories::TOADLET_PEEPER,"invalid src or dst pointer");
		return false;
	}

	D3DSURFACE_DESC dstdesc,srcdesc;
	dst->GetDesc(&dstdesc);
	src->GetDesc(&srcdesc);

	RECT rect={0};
	rect.right=dstdesc.Width<srcdesc.Width?dstdesc.Width:srcdesc.Width;
	rect.bottom=dstdesc.Height<srcdesc.Height?dstdesc.Height:srcdesc.Height;

	HRESULT result=S_OK;
	if(srcdesc.Pool==D3DPOOL_SYSTEMMEM && dstdesc.Pool==D3DPOOL_DEFAULT){
		result=mD3DDevice->UpdateSurface(src,&rect,dst,NULL);
		TOADLET_CHECK_D3D9ERROR(result,"D3D9RenderDevice: UpdateSurface");
	}
	else if((srcdesc.Pool==D3DPOOL_DEFAULT && srcdesc.Usage==D3DUSAGE_RENDERTARGET) && dstdesc.Pool==D3DPOOL_SYSTEMMEM){
		result=mD3DDevice->GetRenderTargetData(src,dst);
		TOADLET_CHECK_D3D9ERROR(result,"D3D9RenderDevice: GetRenderTargetData");
	}
	else if((srcdesc.Pool==D3DPOOL_SYSTEMMEM && dstdesc.Pool==D3DPOOL_SYSTEMMEM) || (srcdesc.Pool!=D3DPOOL_SYSTEMMEM && dstdesc.Pool!=D3DPOOL_SYSTEMMEM)){
		result=mD3DDevice->StretchRect(src,&rect,dst,&rect,D3DTEXF_NONE);
		TOADLET_CHECK_D3D9ERROR(result,"D3D9RenderDevice: StretchRect");
	}
	else{
		return false;
	}

	return true;
}

void D3D9RenderDevice::setDefaultState(){
	setBlendState(BlendState::Combination_DISABLED);
	setDepthState(DepthState());
	setFogState(FogState());
	setNormalize(Normalize_RESCALE);
	setPointState(PointState());
	setRasterizerState(RasterizerState());

	int i;
	for(i=0;i<mCaps.maxTextureStages;++i){
		setSamplerState(i,NULL);
		setTextureState(i,NULL);
		setTexture(i,NULL);
	}

	setMaterialState(MaterialState());
	setAmbientColor(Math::ONE_VECTOR4);

	// D3D specific states
	mD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
	mD3DDevice->SetRenderState(D3DRS_SPECULARENABLE,true);
}

bool D3D9RenderDevice::setRenderState(RenderState *renderState){
	D3D9RenderState *d3drenderState=NULL;
	if(renderState!=NULL){
		d3drenderState=(D3D9RenderState*)renderState->getRootRenderState();
		if(d3drenderState==NULL){
			Error::nullPointer(Categories::TOADLET_PEEPER,
				"RenderState is not a D3D9RenderState");
			return false;
		}
	}

	if(d3drenderState->mBlendState!=NULL){
		setBlendState(*d3drenderState->mBlendState);
	}
	if(d3drenderState->mDepthState!=NULL){
		setDepthState(*d3drenderState->mDepthState);
	}
	if(d3drenderState->mRasterizerState!=NULL){
		setRasterizerState(*d3drenderState->mRasterizerState);
	}
	if(d3drenderState->mFogState!=NULL){
		setFogState(*d3drenderState->mFogState);
	}
	if(d3drenderState->mPointState!=NULL){
		setPointState(*d3drenderState->mPointState);
	}
	if(d3drenderState->mMaterialState!=NULL){
		setMaterialState(*d3drenderState->mMaterialState);
	}
	int i;
	for(i=0;i<d3drenderState->mSamplerStates.size();++i){
		setSamplerState(i,d3drenderState->mSamplerStates[i]);
	}
	for(i=0;i<d3drenderState->mTextureStates.size();++i){
		setTextureState(i,d3drenderState->mTextureStates[i]);
	}

	return true;
}

bool D3D9RenderDevice::setShaderState(ShaderState *shaderState){
	D3D9ShaderState *d3d9shaderState=NULL;
	if(shaderState!=NULL){
		d3d9shaderState=(D3D9ShaderState*)shaderState->getRootShaderState();
		if(d3d9shaderState==NULL){
			Error::nullPointer(Categories::TOADLET_PEEPER,
				"ShaderState is not a D3D9ShaderState");
			return false;
		}
	}

	return d3d9shaderState->activate();
}

void D3D9RenderDevice::setBlendState(const BlendState &state){
	if(state.equals(BlendState::Combination_DISABLED)){
		mD3DDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
		mD3DDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ZERO);
	}
	else{
		D3DBLEND src=getD3DBLEND(state.source);
		D3DBLEND dest=getD3DBLEND(state.dest);

		HRESULT hr;
		hr=mD3DDevice->SetRenderState(D3DRS_SRCBLEND,src);
		TOADLET_CHECK_D3D9ERROR(hr,"setBlendFunction");
		hr=mD3DDevice->SetRenderState(D3DRS_DESTBLEND,dest);
		TOADLET_CHECK_D3D9ERROR(hr,"setBlendFunction");
	}

	mD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE,state.colorWrite); // D3DCOLORWRITEENABLE values match with BlendState::ColorWrite values
}

void D3D9RenderDevice::setDepthState(const DepthState &state){
	HRESULT hr=S_OK;

	if(state.test==DepthState::DepthTest_NEVER){
		mD3DDevice->SetRenderState(D3DRS_ZENABLE,D3DZB_FALSE);
	}
	else{
		mD3DDevice->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);
		mD3DDevice->SetRenderState(D3DRS_ZFUNC,getD3DCMPFUNC(state.test));
	}

	hr=mD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,state.write);
	TOADLET_CHECK_D3D9ERROR(hr,"setDepthState");
}

void D3D9RenderDevice::setFogState(const FogState &state){
	if(state.fog==FogState::FogType_NONE){
		mD3DDevice->SetRenderState(D3DRS_FOGENABLE,FALSE);
	}
	else{
		float fNearDistance=scalarToFloat(state.nearDistance);
		float fFarDistance=scalarToFloat(state.farDistance);
	
		mD3DDevice->SetRenderState(D3DRS_FOGENABLE,TRUE);
	    mD3DDevice->SetRenderState(D3DRS_FOGCOLOR,toD3DCOLOR(state.color));
        mD3DDevice->SetRenderState(D3DRS_FOGVERTEXMODE,getD3DFOGMODE(state.fog));
        mD3DDevice->SetRenderState(D3DRS_FOGDENSITY,state.density);
		mD3DDevice->SetRenderState(D3DRS_FOGSTART,*(DWORD*)(&fNearDistance));
		mD3DDevice->SetRenderState(D3DRS_FOGEND,*(DWORD*)(&fFarDistance));
	}
}

void D3D9RenderDevice::setMaterialState(const MaterialState &state){
	D3DMATERIAL9 material;

	mD3DDevice->SetRenderState(D3DRS_LIGHTING,state.lighting);

	toD3DCOLORVALUE(material.Ambient,state.ambient);
	toD3DCOLORVALUE(material.Diffuse,state.diffuse);
	toD3DCOLORVALUE(material.Specular,state.specular);
	#if !defined(TOADLET_SET_D3DM) && defined(TOADLET_FIXED_POINT)
		material.Power=scalarToFloat(state.shininess);
	#else
		material.Power=state.shininess;
	#endif
	#if !defined(TOADLET_SET_D3DM)
		toD3DCOLORVALUE(material.Emissive,state.emissive);
	#endif

	mD3DDevice->SetMaterial(&material TOADLET_D3DMFMT);

	mD3DDevice->SetRenderState(D3DRS_COLORVERTEX,state.trackColor);
	if(state.trackColor){
		mD3DDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE,D3DMCS_COLOR1);
		mD3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE,D3DMCS_COLOR1);
	}
	else{
		mD3DDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE,D3DMCS_MATERIAL);
		mD3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE,D3DMCS_MATERIAL);
	}

	mD3DDevice->SetRenderState(D3DRS_SHADEMODE,getD3DSHADEMODE(state.shade));

	if(state.alphaTest==MaterialState::AlphaTest_NEVER){
		mD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE,false);
	}
	else{
		mD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE,true);
		mD3DDevice->SetRenderState(D3DRS_ALPHAREF,Math::toInt(state.alphaCutoff*255));
		mD3DDevice->SetRenderState(D3DRS_ALPHAFUNC,getD3DCMPFUNC(state.alphaTest));
	}
}

void D3D9RenderDevice::setPointState(const PointState &state){
	HRESULT result=S_OK;

	#if !defined(TOADLET_SET_D3DM)
		// pointsize = size / sqrt(constant + linear*d + quadratic*d*d)
		// if a&b = 0, then quadratic = 1/(C*C) where C = first component of projMatrix * 1/2 screen width
		if(mCaps.pointSprites){
			mD3DDevice->SetRenderState(D3DRS_POINTSPRITEENABLE,state.sprite);
			mD3DDevice->SetRenderState(D3DRS_POINTSCALEENABLE,state.attenuated);

			float fMinSize=MathConversion::scalarToFloat(state.minSize);
			float fMaxSize=MathConversion::scalarToFloat(state.maxSize);
			mD3DDevice->SetRenderState(D3DRS_POINTSIZE_MIN,*(DWORD*)(&fMinSize));
			mD3DDevice->SetRenderState(D3DRS_POINTSIZE_MAX,*(DWORD*)(&fMaxSize));
		}

		float fSize=MathConversion::scalarToFloat(state.size);
		mD3DDevice->SetRenderState(D3DRS_POINTSIZE,*(DWORD*)(&fSize));
		
		if(state.attenuated){
			float fConstant=MathConversion::scalarToFloat(state.constant);
			float fLinear=MathConversion::scalarToFloat(state.linear);
			float fQuadratic=MathConversion::scalarToFloat(state.quadratic);

			mD3DDevice->SetRenderState(D3DRS_POINTSCALE_A,*(DWORD*)(&fConstant));
			mD3DDevice->SetRenderState(D3DRS_POINTSCALE_B,*(DWORD*)(&fLinear));
			mD3DDevice->SetRenderState(D3DRS_POINTSCALE_C,*(DWORD*)(&fQuadratic));
		}
	#endif
}

void D3D9RenderDevice::setRasterizerState(const RasterizerState &state){
	mD3DDevice->SetRenderState(D3DRS_CULLMODE,getD3DCULL(state.cull));

	mD3DDevice->SetRenderState(D3DRS_FILLMODE,getD3DFILLMODE(state.fill));

	float fconstant=scalarToFloat(state.depthBiasConstant) * 1e6f; // This value is what the Wine project uses
	float fslope=scalarToFloat(state.depthBiasSlope);
	mD3DDevice->SetRenderState(D3DRS_DEPTHBIAS,*(DWORD*)&fconstant);
	mD3DDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS,*(DWORD*)&fslope);

	#if !defined(TOADLET_SET_D3DM)
		mD3DDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS,state.multisample);
		mD3DDevice->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE,state.multisample);
	#endif

	mD3DDevice->SetRenderState(D3DRS_DITHERENABLE,state.dither);
}

void D3D9RenderDevice::setSamplerState(int i,SamplerState *state){
	if(state!=NULL){
		#if defined(TOADLET_SET_D3DM)
			mD3DDevice->SetTextureStageState(i,D3DMTSS_ADDRESSU,getD3DTADDRESS(state->uAddress));
			mD3DDevice->SetTextureStageState(i,D3DMTSS_ADDRESSV,getD3DTADDRESS(state->vAddress));
			mD3DDevice->SetTextureStageState(i,D3DMTSS_ADDRESSW,getD3DTADDRESS(state->wAddress));

			mD3DDevice->SetTextureStageState(i,D3DMTSS_MINFILTER,getD3DTEXF(state->minFilter));
			mD3DDevice->SetTextureStageState(i,D3DMTSS_MIPFILTER,getD3DTEXF(state->mipFilter));
			mD3DDevice->SetTextureStageState(i,D3DMTSS_MAGFILTER,getD3DTEXF(state->magFilter));
		#else
			mD3DDevice->SetSamplerState(i,D3DSAMP_ADDRESSU,getD3DTADDRESS(state->uAddress));
			mD3DDevice->SetSamplerState(i,D3DSAMP_ADDRESSV,getD3DTADDRESS(state->vAddress));
			mD3DDevice->SetSamplerState(i,D3DSAMP_ADDRESSW,getD3DTADDRESS(state->wAddress));

			mD3DDevice->SetSamplerState(i,D3DSAMP_MINFILTER,getD3DTEXF(state->minFilter));
			mD3DDevice->SetSamplerState(i,D3DSAMP_MIPFILTER,getD3DTEXF(state->mipFilter));
			mD3DDevice->SetSamplerState(i,D3DSAMP_MAGFILTER,getD3DTEXF(state->magFilter));

			mD3DDevice->SetSamplerState(i,D3DSAMP_BORDERCOLOR,toD3DCOLOR(state->borderColor));

			mD3DDevice->SetSamplerState(i,D3DSAMP_MAXMIPLEVEL,state->maxLOD);
		#endif

		if(i==0){
			#if defined(TOADLET_SET_D3DM)
				mD3DDevice->SetRenderState(D3DMRS_TEXTUREPERSPECTIVE,state->perspective);
			#endif
		}
	}
}

void D3D9RenderDevice::setTextureState(int i,TextureState *state){
	if(state!=NULL){
		// Setup texture blending
		if(state->colorOperation!=TextureState::Operation_UNSPECIFIED){
			mD3DDevice->SetTextureStageState(i,D3DTSS_COLOROP,getD3DTOP(state->colorOperation,state->colorSource3));
			if(state->colorSource1!=TextureState::Source_UNSPECIFIED && state->colorSource2!=TextureState::Source_UNSPECIFIED){
				mD3DDevice->SetTextureStageState(i,D3DTSS_COLORARG1,getD3DTA(state->colorSource1));
				mD3DDevice->SetTextureStageState(i,D3DTSS_COLORARG2,getD3DTA(state->colorSource2));
			}
		}
		else{
			mD3DDevice->SetTextureStageState(i,D3DTSS_COLOROP,D3DTOP_MODULATE);
		}
		if(state->alphaOperation!=TextureState::Operation_UNSPECIFIED){
			mD3DDevice->SetTextureStageState(i,D3DTSS_ALPHAOP,getD3DTOP(state->alphaOperation,state->alphaSource3));
			if(state->alphaSource1!=TextureState::Source_UNSPECIFIED && state->alphaSource2!=TextureState::Source_UNSPECIFIED){
				mD3DDevice->SetTextureStageState(i,D3DTSS_ALPHAARG1,getD3DTA(state->alphaSource1));
				mD3DDevice->SetTextureStageState(i,D3DTSS_ALPHAARG2,getD3DTA(state->alphaSource2));
			}
		}
		else{
			mD3DDevice->SetTextureStageState(i,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
		}

		mD3DDevice->SetTextureStageState(i,D3DTSS_CONSTANT,toD3DCOLOR(state->constantColor));

		// Setup calculations
		TextureState::CalculationType calculation=state->calculation;
		if(calculation!=TextureState::CalculationType_DISABLED){
			if(calculation==TextureState::CalculationType_NORMAL){
				/// @todo: Get this working with 3D Texture coordinates.  Doesnt seem to currently
				//  I have tried just switching to D3DTTFF_COUNT3, but nothing changed, I'm pretty sure it has
				//  something to do with the setup of the cacheD3DMatrix below
				mD3DDevice->SetTextureStageState(i,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_COUNT2);

				toD3DMATRIX(cacheD3DMatrix,state->matrix);

				#if defined(TOADLET_SET_D3DM) && defined(TOADLET_FIXED_POINT)
					scalar t;
				#else
					float t;
				#endif
				t=cacheD3DMatrix._31; cacheD3DMatrix._31=cacheD3DMatrix._41; cacheD3DMatrix._41=t;
				t=cacheD3DMatrix._32; cacheD3DMatrix._32=cacheD3DMatrix._42; cacheD3DMatrix._42=t;
				t=cacheD3DMatrix._33; cacheD3DMatrix._33=cacheD3DMatrix._43; cacheD3DMatrix._43=t;
				t=cacheD3DMatrix._34; cacheD3DMatrix._34=cacheD3DMatrix._44; cacheD3DMatrix._44=t;
			}
			else if(calculation==TextureState::CalculationType_OBJECTSPACE){
				mD3DDevice->SetTextureStageState(i,D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_PASSTHRU);
				mD3DDevice->SetTextureStageState(i,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_COUNT4|D3DTTFF_PROJECTED);

				toD3DMATRIX(cacheD3DMatrix,state->matrix);
			}
			else if(calculation==TextureState::CalculationType_CAMERASPACE){
				mD3DDevice->SetTextureStageState(i,D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_CAMERASPACEPOSITION);
				mD3DDevice->SetTextureStageState(i,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_COUNT4|D3DTTFF_PROJECTED);

				toD3DMATRIX(cacheD3DMatrix,state->matrix);
			}

			mD3DDevice->SetTransform((D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0+i),&cacheD3DMatrix TOADLET_D3DMFMT);
		}
		else{
			mD3DDevice->SetTextureStageState(i,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE);
		}

		mD3DDevice->SetTextureStageState(i,D3DTSS_TEXCOORDINDEX,state->texCoordIndex);
	}
	else{
		mD3DDevice->SetTextureStageState(i,D3DTSS_COLOROP,D3DTOP_DISABLE);

		mD3DDevice->SetTextureStageState(i,D3DTSS_CONSTANT,0);

		mD3DDevice->SetTextureStageState(i,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE);

		mD3DDevice->SetTextureStageState(i,D3DTSS_TEXCOORDINDEX,0);
	}
}

void D3D9RenderDevice::setTexture(int i,Texture *texture){
	HRESULT result=S_OK;

	D3D9Texture *d3dtexture=texture!=NULL?(D3D9Texture*)texture->getRootTexture():NULL;
	if(d3dtexture!=NULL){
		result=mD3DDevice->SetTexture(i,d3dtexture->mTexture);
	}
	else{
		result=mD3DDevice->SetTexture(i,NULL);
	}
	TOADLET_CHECK_D3D9ERROR(result,"SetTexture");
}

void D3D9RenderDevice::setNormalize(const Normalize &normalize){
	if(normalize!=Normalize_NONE){
		mD3DDevice->SetRenderState(D3DRS_NORMALIZENORMALS,TRUE);
	}
	else{
		mD3DDevice->SetRenderState(D3DRS_NORMALIZENORMALS,FALSE);
	}
}

void D3D9RenderDevice::setLightState(int i,const LightState &light){
	D3DLIGHT9 d3dlight;
	ZeroMemory(&d3dlight,sizeof(D3DLIGHT9));

	switch(light.type){
		case LightState::Type_DIRECTION:{
			d3dlight.Type=D3DLIGHT_DIRECTIONAL;
			toD3DVECTOR(d3dlight.Direction,light.direction);
			break;
		}
		case LightState::Type_POINT:{
			d3dlight.Type=D3DLIGHT_POINT;
			toD3DVECTOR(d3dlight.Position,light.position);
			break;
		}
		case LightState::Type_SPOT:{
			#if !defined(TOADLET_SET_D3DM)
				d3dlight.Type=D3DLIGHT_SPOT;
				toD3DVECTOR(d3dlight.Position,light.position);
				toD3DVECTOR(d3dlight.Direction,light.direction);
				d3dlight.Falloff=light.spotFalloff;
				d3dlight.Phi=light.spotOuterRadius;
				d3dlight.Theta=light.spotInnerRadius;
			#endif
			break;
		}
	}

	toD3DCOLORVALUE(d3dlight.Diffuse,light.diffuseColor);
	toD3DCOLORVALUE(d3dlight.Specular,light.specularColor);
	#if !defined(TOADLET_SET_D3DM) && defined(TOADLET_FIXED_POINT)
		d3dlight.Attenuation0=scalarToFloat(light.constantAttenuation);
		d3dlight.Attenuation1=scalarToFloat(light.linearAttenuation);
		d3dlight.Attenuation2=scalarToFloat(light.quadraticAttenuation);
		d3dlight.Range=scalarToFloat(light.radius);
	#else
		d3dlight.Attenuation0=light.constantAttenuation;
		d3dlight.Attenuation1=light.linearAttenuation;
		d3dlight.Attenuation2=light.quadraticAttenuation;
		d3dlight.Range=light.radius;
	#endif

	mD3DDevice->SetLight(i,&d3dlight TOADLET_D3DMFMT);
}

void D3D9RenderDevice::setLightEnabled(int i,bool enable){
	HRESULT hr=mD3DDevice->LightEnable(i,enable);
	TOADLET_CHECK_D3D9ERROR(hr,"setLightEnabled");
}

void D3D9RenderDevice::setAmbientColor(const Vector4 &ambient){
	HRESULT hr=mD3DDevice->SetRenderState(D3DRS_AMBIENT,toD3DCOLOR(ambient));
	TOADLET_CHECK_D3D9ERROR(hr,"setAmbientColor");
}

void D3D9RenderDevice::setCapsFromD3DCAPS9(RenderCaps &caps,const D3DCAPS9 &d3dcaps,bool renderToTexture,bool renderToDepthTexture){
	caps.resetOnResize=true;
	#if defined(TOADLET_SET_D3DM)
		caps.pointSprites=false;
	#else
		caps.pointSprites=(d3dcaps.FVFCaps & D3DFVFCAPS_PSIZE)!=0 && d3dcaps.MaxPointSize>1.0f;
	#endif
	caps.maxLights=d3dcaps.MaxActiveLights;
	caps.maxTextureStages=d3dcaps.MaxTextureBlendStages;
	caps.maxTextureSize=math::Math::minVal(d3dcaps.MaxTextureWidth,d3dcaps.MaxTextureHeight);
	caps.textureDot3=(d3dcaps.TextureOpCaps & D3DTEXOPCAPS_DOTPRODUCT3)!=0;
	caps.textureNonPowerOf2=(d3dcaps.TextureCaps & D3DPTEXTURECAPS_POW2)==0 && (d3dcaps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY)==0;
	#if defined(TOADLET_SET_D3DM)
		caps.textureNonPowerOf2Restricted=false;
	#else
		caps.textureNonPowerOf2Restricted=(d3dcaps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL)!=0;
	#endif
	#if defined(TOADLET_SET_D3DM)
		caps.textureAutogenMipMaps=false;
	#else
		caps.textureAutogenMipMaps=(d3dcaps.Caps2 & D3DCAPS2_CANAUTOGENMIPMAP)!=0;
	#endif
	caps.renderToTexture=renderToTexture;
	caps.renderToDepthTexture=renderToDepthTexture;
	caps.renderToTextureNonPowerOf2Restricted=caps.textureNonPowerOf2Restricted && caps.renderToTexture;
	#if defined(TOADLET_SET_D3DM) && defined(TOADLET_FIXED_POINT)
		caps.idealVertexFormatBit=VertexFormat::Format_BIT_FIXED_32;
	#else
		caps.idealVertexFormatBit=VertexFormat::Format_BIT_FLOAT_32;
	#endif
	caps.triangleFan=true;
	caps.fill=true;
	#if !defined(TOADLET_SET_D3DM)
		caps.texturePerspective=true;
		caps.cubeMap=true;
	#endif

	#if defined(TOADLET_SET_D3DM)
		Logger::alert(Categories::TOADLET_PEEPER,
			String("D3DM has lockable textures:")+((d3dcaps.SurfaceCaps & D3DMSURFCAPS_LOCKTEXTURE)>0));
	#endif
}

void D3D9RenderDevice::getPrimitiveTypeAndCount(D3DPRIMITIVETYPE &d3dpt,int &count,IndexData::Primitive prim,int numIndexes){
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

void D3D9RenderDevice::getShadowBiasMatrix(const Texture *shadowTexture,Matrix4x4 &result){
	int width=shadowTexture->getWidth();
	int height=shadowTexture->getHeight();
	scalar xoff=Math::HALF+Math::div(Math::HALF,Math::fromInt(width));
	scalar yoff=Math::HALF+Math::div(Math::HALF,Math::fromInt(height));
	result.set( Math::HALF, 0,           0,         xoff,
				0,          -Math::HALF, 0,         yoff,
				0,          0,           float(1<<24)/*Math::ONE*/, -400000.0f/*0*/,
				0,          0,           0,         Math::ONE);
}

int D3D9RenderDevice::getCloseTextureFormat(int format,int usage){
	int closeFormat=getCloseTextureFormat(format);
	DWORD d3dusage=getD3DUSAGE(closeFormat,usage);
	D3DFORMAT d3dformat=getD3DFORMAT(closeFormat);
	if(isD3DFORMATValid(d3dformat,d3dusage)==false){
		return Texture::Format_BGRA_8;
	}
	else{
		return closeFormat;
	}
}

int D3D9RenderDevice::getCloseTextureFormat(int format){
	switch(format){
		#if defined(TOADLET_SET_D3DM)
			case Texture::Format_L_8:
				else return Texture::Format_BGR_8;
			case Texture::Format_A_8:
			case Texture::Format_LA_8:
				else return return Texture::Format_BGRA_8;
		#else
			case Texture::Format_L_8:
			case Texture::Format_R_8:
			case Texture::Format_RG_8:
				return Texture::Format_BGR_8;
			case Texture::Format_A_8:
			case Texture::Format_LA_8:
				return Texture::Format_BGRA_8;
		#endif
		case Texture::Format_RGB_8:
			return Texture::Format_BGR_8;
		case Texture::Format_RGBA_8:
			return Texture::Format_BGRA_8;
		case Texture::Format_RGB_5_6_5:
			return Texture::Format_BGR_5_6_5;
		case Texture::Format_RGBA_5_5_5_1:
			return Texture::Format_BGRA_5_5_5_1;
		case Texture::Format_RGBA_4_4_4_4:
			return Texture::Format_BGRA_4_4_4_4;
		default:
			return format;
	}
}

bool D3D9RenderDevice::getShaderProfileSupported(const String &profile){
	return profile=="hlsl";
}

bool D3D9RenderDevice::isD3DFORMATValid(D3DFORMAT textureFormat,DWORD usage){
	return SUCCEEDED(mD3D->CheckDeviceFormat(mD3DAdapter,mD3DDevType,mD3DAdapterFormat,usage,D3DRTYPE_TEXTURE,textureFormat));
}

DWORD D3D9RenderDevice::getD3DTOP(TextureState::Operation operation,TextureState::Source alphaSource){
	switch(operation){
		case TextureState::Operation_REPLACE:
			return D3DTOP_SELECTARG1;
		case TextureState::Operation_MODULATE:
			return D3DTOP_MODULATE;
		case TextureState::Operation_MODULATE_2X:
			return D3DTOP_MODULATE2X;
		case TextureState::Operation_MODULATE_4X:
			return D3DTOP_MODULATE4X;
		case TextureState::Operation_ADD:
			return D3DTOP_ADD;
		case TextureState::Operation_DOTPRODUCT:
			return D3DTOP_DOTPRODUCT3;
		case TextureState::Operation_ALPHABLEND:
			if(alphaSource==TextureState::Source_PREVIOUS){
				return D3DTOP_BLENDCURRENTALPHA;
			}
			else if(alphaSource==TextureState::Source_TEXTURE){
				return D3DTOP_BLENDTEXTUREALPHA;
			}
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"invalid type");
			return D3DTOP_DISABLE;
	}
}

D3DFORMAT D3D9RenderDevice::getD3DFORMAT(int format){
	switch(format){
		#if !defined(TOADLET_SET_D3DM)
			case Texture::Format_L_8:
				return D3DFMT_L8;
			case Texture::Format_A_8:
				return D3DFMT_A8;
			case Texture::Format_LA_8:
				return D3DFMT_A8L8;
		#endif
		case Texture::Format_BGR_8:
			return D3DFMT_R8G8B8;
		case Texture::Format_BGRA_8:
			return D3DFMT_A8R8G8B8;
		case Texture::Format_BGR_5_6_5:
			return D3DFMT_R5G6B5;
		case Texture::Format_BGRA_5_5_5_1:
			return D3DFMT_A1R5G5B5;
		case Texture::Format_BGRA_4_4_4_4:
			return D3DFMT_A4R4G4B4;
		case Texture::Format_DEPTH_16:
			return D3DFMT_D16;
		case Texture::Format_DEPTH_24:
			return D3DFMT_D24S8;
		case Texture::Format_DEPTH_32:
			return D3DFMT_D32;
		case Texture::Format_RGBA_DXT1:
			return D3DFMT_DXT1;
		case Texture::Format_RGBA_DXT2:
			return D3DFMT_DXT2;
		case Texture::Format_RGBA_DXT3:
			return D3DFMT_DXT3;
		case Texture::Format_RGBA_DXT4:
			return D3DFMT_DXT4;
		case Texture::Format_RGBA_DXT5:
			return D3DFMT_DXT5;
		default:
			return D3DFMT_UNKNOWN;
	}
}

DWORD D3D9RenderDevice::getD3DUSAGE(int textureFormat,int usage){
	DWORD d3dusage=
		#if defined(TOADLET_SET_D3DM)
			D3DUSAGE_LOCKABLE;
		#else
			0;
		#endif
	if((usage&Texture::Usage_BIT_RENDERTARGET)>0){
		if((textureFormat&Texture::Format_BIT_DEPTH)>0){
			d3dusage|=D3DUSAGE_DEPTHSTENCIL;
		}
		else{
			d3dusage|=D3DUSAGE_RENDERTARGET;
		}
	}
	if((usage&Texture::Usage_BIT_STREAM)>0){
		d3dusage|=D3DUSAGE_DYNAMIC;
	}
	return d3dusage;
}

D3DPOOL D3D9RenderDevice::getD3DPOOL(int usage){
	D3DPOOL d3dpool;
	#if defined(TOADLET_SET_D3DM)
		d3dpool=D3DPOOL_MANAGED;
	#else
		if((usage&Buffer::Usage_BIT_STAGING)>0){
			d3dpool=D3DPOOL_SYSTEMMEM;
		}
		else if((usage&(Buffer::Usage_BIT_STREAM|Buffer::Usage_BIT_DYNAMIC|Texture::Usage_BIT_RENDERTARGET))>0){
			d3dpool=D3DPOOL_DEFAULT;
		}
		else{
			d3dpool=D3DPOOL_MANAGED;
		}
	#endif
	return d3dpool;
}

DWORD D3D9RenderDevice::getD3DTADDRESS(SamplerState::AddressType address){
	switch(address){
		case SamplerState::AddressType_REPEAT:
			return D3DTADDRESS_WRAP;
		case SamplerState::AddressType_CLAMP_TO_EDGE:
			return D3DTADDRESS_CLAMP;
		case SamplerState::AddressType_CLAMP_TO_BORDER:
			return D3DTADDRESS_BORDER;
		case SamplerState::AddressType_MIRRORED_REPEAT:
			return D3DTADDRESS_MIRROR;
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"invalid address");
			return D3DTADDRESS_WRAP;
	}
}

DWORD D3D9RenderDevice::getD3DTEXF(SamplerState::FilterType filter){
	switch(filter){
		case SamplerState::FilterType_NONE:
			return D3DTEXF_NONE;
		case SamplerState::FilterType_NEAREST:
			return D3DTEXF_POINT;
		case SamplerState::FilterType_LINEAR:
			return D3DTEXF_LINEAR;
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"invalid filter");
			return D3DTEXF_NONE;
	}
}

DWORD D3D9RenderDevice::getD3DTA(TextureState::Source blend){
	switch(blend){
		case TextureState::Source_PREVIOUS:
			return D3DTA_CURRENT;
		case TextureState::Source_TEXTURE:
			return D3DTA_TEXTURE;
		case TextureState::Source_PRIMARY_COLOR:
			return D3DTA_DIFFUSE;
		case TextureState::Source_CONSTANT_COLOR:
			return D3DTA_CONSTANT;
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"invalid source");
			return 0;
	}
}

D3DBLEND D3D9RenderDevice::getD3DBLEND(BlendState::Operation state){
	switch(state){
		case BlendState::Operation_ONE:
			return D3DBLEND_ONE;
		case BlendState::Operation_ZERO:
			return D3DBLEND_ZERO;
		case BlendState::Operation_DEST_COLOR:
			return D3DBLEND_DESTCOLOR;
		case BlendState::Operation_SOURCE_COLOR:
			return D3DBLEND_SRCCOLOR;
		case BlendState::Operation_ONE_MINUS_DEST_COLOR:
			return D3DBLEND_INVDESTCOLOR;
		case BlendState::Operation_ONE_MINUS_SOURCE_COLOR:
			return D3DBLEND_INVSRCCOLOR;
		case BlendState::Operation_DEST_ALPHA:
			return D3DBLEND_DESTALPHA;
		case BlendState::Operation_SOURCE_ALPHA:
			return D3DBLEND_SRCALPHA;
		case BlendState::Operation_ONE_MINUS_DEST_ALPHA:
			return D3DBLEND_INVDESTALPHA;
		case BlendState::Operation_ONE_MINUS_SOURCE_ALPHA:
			return D3DBLEND_INVSRCALPHA;
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"invalid operation");
			return D3DBLEND_ZERO;
	}
}

D3DCULL D3D9RenderDevice::getD3DCULL(RasterizerState::CullType type){
	switch(type){
		case RasterizerState::CullType_NONE:
			return D3DCULL_NONE;
		case RasterizerState::CullType_BACK:
			return D3DCULL_CW;
		case RasterizerState::CullType_FRONT:
			return D3DCULL_CCW;
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"invalid cull type");
			return D3DCULL_NONE;
		break;
	}
}

D3DFILLMODE D3D9RenderDevice::getD3DFILLMODE(RasterizerState::FillType type){
	switch(type){
		case RasterizerState::FillType_POINT:
			return D3DFILL_POINT;
		case RasterizerState::FillType_LINE:
			return D3DFILL_WIREFRAME;
		case RasterizerState::FillType_SOLID:
			return D3DFILL_SOLID;
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"invalid fill type");
			return D3DFILL_POINT;
		break;
	}
}

D3DSHADEMODE D3D9RenderDevice::getD3DSHADEMODE(MaterialState::ShadeType type){
	switch(type){
		case MaterialState::ShadeType_FLAT:
			return D3DSHADE_FLAT;
		case MaterialState::ShadeType_GOURAUD:
			return D3DSHADE_GOURAUD;
		case MaterialState::ShadeType_PHONG:
			return D3DSHADE_PHONG;
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"invalid shade type");
			return D3DSHADE_FLAT;
		break;
	}
}

D3DFOGMODE D3D9RenderDevice::getD3DFOGMODE(FogState::FogType type){
	switch(type){
		case FogState::FogType_NONE:
			return D3DFOG_NONE;
		case FogState::FogType_LINEAR:
			return D3DFOG_LINEAR;
		case FogState::FogType_EXP:
			return D3DFOG_EXP;
		case FogState::FogType_EXP2:
			return D3DFOG_EXP2;
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"invalid fog type");
			return D3DFOG_NONE;
		break;
	}
}

D3DCMPFUNC D3D9RenderDevice::getD3DCMPFUNC(MaterialState::AlphaTest type){
	switch(type){
		case MaterialState::AlphaTest_NEVER:
			return D3DCMP_NEVER;
		case MaterialState::AlphaTest_LESS:
			return D3DCMP_LESS;
		case MaterialState::AlphaTest_EQUAL:
			return D3DCMP_EQUAL;
		case MaterialState::AlphaTest_LEQUAL:
			return D3DCMP_LESSEQUAL;
		case MaterialState::AlphaTest_GREATER:
			return D3DCMP_GREATER;
		case MaterialState::AlphaTest_NOTEQUAL:
			return D3DCMP_NOTEQUAL;
		case MaterialState::AlphaTest_GEQUAL:
			return D3DCMP_GREATEREQUAL;
		case MaterialState::AlphaTest_ALWAYS:
			return D3DCMP_ALWAYS;
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"invalid alpha test");
			return D3DCMP_NEVER;
		break;
	}
}

D3DCMPFUNC D3D9RenderDevice::getD3DCMPFUNC(DepthState::DepthTest type){
	switch(type){
		case DepthState::DepthTest_NEVER:
			return D3DCMP_NEVER;
		case DepthState::DepthTest_LESS:
			return D3DCMP_LESS;
		case DepthState::DepthTest_EQUAL:
			return D3DCMP_EQUAL;
		case DepthState::DepthTest_LEQUAL:
			return D3DCMP_LESSEQUAL;
		case DepthState::DepthTest_GREATER:
			return D3DCMP_GREATER;
		case DepthState::DepthTest_NOTEQUAL:
			return D3DCMP_NOTEQUAL;
		case DepthState::DepthTest_GEQUAL:
			return D3DCMP_GREATEREQUAL;
		case DepthState::DepthTest_ALWAYS:
			return D3DCMP_ALWAYS;
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"invalid depth test");
			return D3DCMP_NEVER;
		break;
	}
}

DWORD D3D9RenderDevice::getFVF(VertexFormat *vertexFormat){
	DWORD fvf=0;

	int i;
	int texCoordCount=0;
	for(i=0;i<vertexFormat->getNumElements();++i){
		int semantic=vertexFormat->getSemantic(i);
		int index=vertexFormat->getIndex(i);
		int format=vertexFormat->getFormat(i);
		#if defined(TOADLET_SET_D3DM)
			if(semantic==VertexFormat::Semantic_POSITION && format==(VertexFormat::Format_BIT_FIXED_32|VertexFormat::Format_BIT_COUNT_3)){
				fvf|=D3DMFVF_XYZ_FIXED;
			}
			else if(semantic==VertexFormat::Semantic_POSITION && format==(VertexFormat::Format_BIT_FLOAT_32|VertexFormat::Format_BIT_COUNT_3)){
				fvf|=D3DMFVF_XYZ_FLOAT;
			}
			else if(semantic==VertexFormat::Semantic_NORMAL && format==(VertexFormat::Format_BIT_FIXED_32|VertexFormat::Format_BIT_COUNT_3)){
				fvf|=D3DMFVF_NORMAL_FIXED;
			}
			else if(semantic==VertexFormat::Semantic_NORMAL && format==(VertexFormat::Format_BIT_FLOAT_32|VertexFormat::Format_BIT_COUNT_3)){
				fvf|=D3DMFVF_NORMAL_FLOAT;
			}
		#else
			if(semantic==VertexFormat::Semantic_POSITION && format==(VertexFormat::Format_BIT_FLOAT_32|VertexFormat::Format_BIT_COUNT_3)){
				fvf|=D3DFVF_XYZ;
			}
			else if(semantic==VertexFormat::Semantic_NORMAL && format==(VertexFormat::Format_BIT_FLOAT_32|VertexFormat::Format_BIT_COUNT_3)){
				fvf|=D3DFVF_NORMAL;
			}
		#endif
		else if(semantic==VertexFormat::Semantic_COLOR && format==VertexFormat::Format_COLOR_RGBA){
			if(index==0){
				fvf|=D3DFVF_DIFFUSE;
			}
			else if(index==1){
				fvf|=D3DFVF_SPECULAR;
			}
		}
		else if(semantic==VertexFormat::Semantic_TEXCOORD){
			if((format&VertexFormat::Format_BIT_COUNT_1)>0){
				fvf|=D3DFVF_TEXCOORDSIZE1(texCoordCount);
			}
			else if((format&VertexFormat::Format_BIT_COUNT_2)>0){
				fvf|=D3DFVF_TEXCOORDSIZE2(texCoordCount);
			}
			else if((format&VertexFormat::Format_BIT_COUNT_3)>0){
				fvf|=D3DFVF_TEXCOORDSIZE3(texCoordCount);
			}
			else if((format&VertexFormat::Format_BIT_COUNT_4)>0){
				#if defined(TOADLET_SET_D3DM)
					Logger::error(Categories::TOADLET_PEEPER,
						"D3D9VertexBuffer: Invalid tex coord count");
				#else
					fvf|=D3DFVF_TEXCOORDSIZE4(texCoordCount);
				#endif
			}

			#if defined(TOADLET_SET_D3DM)
				if((format&VertexFormat::Format_BIT_FIXED_32)>0){
					fvf|=D3DMFVF_TEXCOORDFIXED(texCoordCount);
				}
				else if((format&VertexFormat::Format_BIT_FLOAT_32)>0){
					fvf|=D3DMFVF_TEXCOORDFLOAT(texCoordCount);
				}
			#endif

			texCoordCount++;
		}
		else{
			Logger::error(Categories::TOADLET_PEEPER,
				String("invalid vertex element:")+semantic+","+format);
		}
	}

	switch(texCoordCount){
		case 0:
			fvf|=D3DFVF_TEX0;
		break;
		case 1:
			fvf|=D3DFVF_TEX1;
		break;
		case 2:
			fvf|=D3DFVF_TEX2;
		break;
		case 3:
			fvf|=D3DFVF_TEX3;
		break;
		case 4:
			fvf|=D3DFVF_TEX4;
		break;
		#if !defined(TOADLET_SET_D3DM)
			case 5:
				fvf|=D3DFVF_TEX5;
			break;
			case 6:
				fvf|=D3DFVF_TEX6;
			break;
			case 7:
				fvf|=D3DFVF_TEX7;
			break;
			case 8:
				fvf|=D3DFVF_TEX8;
			break;
		#endif
		default:
			Logger::error(Categories::TOADLET_PEEPER,
				String("invalid tex coord number:")+texCoordCount);
	}

	return fvf;
}

}
}
