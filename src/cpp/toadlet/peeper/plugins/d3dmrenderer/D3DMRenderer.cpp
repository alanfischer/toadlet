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

#include "D3DMRenderer.h"
#include "D3DMTexturePeer.h"
#include "D3DMRenderTexturePeer.h"
#include "D3DMVertexBufferPeer.h"
#include "D3DMIndexBufferPeer.h"
#include <toadlet/egg/Error.h>
#include <toadlet/peeper/Texture.h>
#include <toadlet/peeper/LightEffect.h>
#include <toadlet/peeper/Light.h>
#include <toadlet/peeper/IndexBuffer.h>
#include <toadlet/peeper/RenderContext.h>
#include <toadlet/peeper/VertexBuffer.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/peeper/Viewport.h>

using namespace toadlet::egg;
using namespace toadlet::egg::image;

#if defined(TOADLET_FIXED_POINT)
	const static D3DMFORMAT TOADLET_PEEPER_D3DM_FORMAT=D3DMFMT_D3DMVALUE_FIXED;
#else
	const static D3DMFORMAT TOADLET_PEEPER_D3DM_FORMAT=D3DMFMT_D3DMVALUE_FLOAT;
#endif

namespace toadlet{
namespace peeper{

TOADLET_C_API Renderer* new_D3DMRenderer(){
	return new D3DMRenderer();
}

#if defined(TOADLET_BUILD_DYNAMIC)
	TOADLET_C_API Renderer* new_Renderer(){
		return new D3DMRenderer();
	}
#endif

D3DMRenderer::D3DMRenderer():
	mD3DDevice(NULL),
	//mD3DCaps,
	mD3DRenderContextPeer(NULL),
	mRenderContext(NULL),
	mRenderTarget(NULL),
	mShutdown(true),

	mAlphaTest(AlphaTest_NONE),
	mAlphaCutoff(0),
	//mBlend,
	mDepthTest(DepthTest_NONE),
	mDepthWrite(false),
	mDithering(false),
	mFaceCulling(FaceCulling_NONE),
	mFill(Fill_SOLID),
	mLighting(false),
	mNormalize(Normalize_NONE),
	mShading(Shading_SMOOTH),

	mMirrorY(false)

	//mStatisticsSet,
	//mCapabilitySet
{
}

D3DMRenderer::~D3DMRenderer(){
	TOADLET_ASSERT(mShutdown);
}

bool D3DMRenderer::startup(RenderContext *renderContext,int *options){
	Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ALERT,
		"D3DMRenderer: Startup started");

	if(renderContext==NULL){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3DMRenderer: RenderContext passed in is NULL");
		return false;
	}

	#if defined(TOADLET_RTTI)
		D3DMRenderContextPeer *d3dRenderContextPeer=dynamic_cast<D3DMRenderContextPeer*>(renderContext->internal_getRenderTargetPeer());
	#else
		D3DMRenderContextPeer *d3dRenderContextPeer=(D3DMRenderContextPeer*)renderContext->internal_getRenderTargetPeer();
	#endif
	if(d3dRenderContextPeer==NULL){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3DMRenderer: RenderContext passed in does not have a D3DMRenderContextPeer");
		return false;
	}

	mShutdown=false;

	mD3DRenderContextPeer=d3dRenderContextPeer;
	mRenderTarget=renderContext;

	mD3DDevice=mD3DRenderContextPeer->getDirect3DMobileDevice();
	if(mD3DDevice==NULL){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3DMRenderer: Invalid Direct3DMobileDevice");
		return false;
	}

	ZeroMemory(&mD3DCaps,sizeof(D3DMCAPS));
	HRESULT result=mD3DDevice->GetDeviceCaps(&mD3DCaps);
	TOADLET_CHECK_D3DMERROR(result,"Error getting caps");

	IDirect3DMobile *d3d=NULL;
	mD3DDevice->GetDirect3D(&d3d);

	HRESULT renderToTextureResult=d3d->CheckDeviceFormat(	D3DMADAPTER_DEFAULT, D3DMDEVTYPE_DEFAULT, D3DMFMT_UNKNOWN,
		D3DMUSAGE_RENDERTARGET, D3DMRTYPE_TEXTURE, D3DMFMT_UNKNOWN);

	d3d->Release();
	d3d=NULL;

	setCapabilitySetFromCaps(mCapabilitySet,mD3DCaps,SUCCEEDED(renderToTextureResult),false);

	setDefaultStates();

	Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ALERT,
		"D3DMRenderer: Startup finished");

	return true;
}

bool D3DMRenderer::shutdown(){
	mShutdown=true;

	return true;
}

Renderer::RendererStatus D3DMRenderer::getStatus(){
	HRESULT result=mD3DDevice->TestCooperativeLevel();

	switch(result){
		case D3DMERR_DEVICELOST:
			return RendererStatus_UNAVAILABLE;
		case D3DMERR_DEVICENOTRESET:
			return RendererStatus_NEEDSRESET;
		case D3DMERR_DRIVERINTERNALERROR:
			return RendererStatus_UNAVAILABLE;
	}

	return RendererStatus_OK;
}

bool D3DMRenderer::reset(){
	mD3DRenderContextPeer->reset();

	setDefaultStates();

	return true;
}

TexturePeer *D3DMRenderer::createTexturePeer(Texture *texture){
	if(texture==NULL){
		Error::nullPointer(Categories::TOADLET_PEEPER,
			"Texture is NULL");
		return NULL;
	}

	switch(texture->getType()){
		case Texture::Type_NORMAL:{
			return new D3DMTexturePeer(this,texture);
		}
		case Texture::Type_RENDER:{
			RenderTexture *renderTexture=(RenderTexture*)texture;
			D3DMRenderTexturePeer *peer=new D3DMRenderTexturePeer(this,renderTexture);
			if(peer->isValid()==false){
				delete peer;
				peer=NULL;
			}
			return peer;
		}
		case Texture::Type_ANIMATED:{
			// Dont load animated textures, since they are just a collection of normal textures
			return NULL;
		}
	}

	return NULL;
}

BufferPeer *D3DMRenderer::createBufferPeer(Buffer *buffer){
	if(buffer==NULL){
		Error::nullPointer(Categories::TOADLET_PEEPER,
			"Buffer is NULL");
		return NULL;
	}

	if(buffer->getType()==Buffer::Type_INDEX && mCapabilitySet.hardwareIndexBuffers==true){
		D3DMIndexBufferPeer *peer=new D3DMIndexBufferPeer(this,(IndexBuffer*)buffer);
		if(peer->isValid()==false){
			delete peer;
			peer=NULL;
		}
		return peer;
	}
	else if(buffer->getType()==Buffer::Type_VERTEX && mCapabilitySet.hardwareVertexBuffers==true){
		D3DMVertexBufferPeer *peer=new D3DMVertexBufferPeer(this,(VertexBuffer*)buffer);
		if(peer->isValid()==false){
			delete peer;
			peer=NULL;
		}
		return peer;
	}

	return NULL;
}

ProgramPeer *D3DMRenderer::createProgramPeer(Program *program){
	return NULL;
}

ShaderPeer *D3DMRenderer::createShaderPeer(Shader *shader){
	return NULL;
}

void D3DMRenderer::setViewport(const Viewport &viewport){
	D3DMVIEWPORT d3dviewport;
	d3dviewport.X=viewport.x;
	d3dviewport.Y=viewport.y;
	d3dviewport.Width=viewport.width;
	d3dviewport.Height=viewport.height;
	d3dviewport.MinZ=0.0f;
	d3dviewport.MaxZ=1.0f;

	int result=mD3DDevice->SetViewport(&d3dviewport);
	TOADLET_CHECK_D3DMERROR(result,"setViewport");
}

void D3DMRenderer::clear(int clearFlags,const Color &clearColor){
	int d3dclear=0;

	if(clearFlags & ClearFlag_COLOR){
		d3dclear|=D3DMCLEAR_TARGET;
	}
	if(clearFlags & ClearFlag_DEPTH){
		d3dclear|=D3DMCLEAR_ZBUFFER;
	}
	if(clearFlags & ClearFlag_STENCIL){
		d3dclear|=D3DMCLEAR_STENCIL;
	}

	HRESULT result=mD3DDevice->Clear(0,NULL,d3dclear,toD3DMCOLOR(clearColor),1.0f,0);
	TOADLET_CHECK_D3DMERROR(result,"clearRenderTargetBuffers");
}

void D3DMRenderer::swap(){
	HRESULT result=mD3DDevice->Present(NULL,NULL,NULL,NULL);
	TOADLET_CHECK_D3DMERROR(result,"swapBuffers");
}

RenderContext *D3DMRenderer::getRenderContext(){
	return mRenderContext;
}

bool D3DMRenderer::setRenderTarget(RenderTarget *target){
	#if defined(TOADLET_RTTI)
		D3DMRenderTargetPeer *peer=dynamic_cast<D3DMRenderTargetPeer*>(target->internal_getRenderTargetPeer());
	#else
		D3DMRenderTargetPeer *peer=(D3DMRenderTargetPeer*)target->internal_getRenderTargetPeer();
	#endif
	if(peer!=NULL){
		peer->makeCurrent(mD3DDevice);
	}
	else{
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3DMRenderer: Null render target peer, or incorrect peer type");
		return false;
	}

	mRenderTarget=target;

	return true;
}

RenderTarget *D3DMRenderer::getRenderTarget(){
	return mRenderTarget;
}

void D3DMRenderer::setModelMatrix(const Matrix4x4 &matrix){
	toD3DMMATRIX(cacheD3DMatrix,matrix);

	HRESULT result=mD3DDevice->SetTransform(D3DMTS_WORLD,&cacheD3DMatrix,TOADLET_PEEPER_D3DM_FORMAT);
	TOADLET_CHECK_D3DMERROR(result,"setModelMatrix");
}

void D3DMRenderer::setViewMatrix(const Matrix4x4 &matrix){
	toD3DMMATRIX(cacheD3DMatrix,matrix);

	HRESULT result=mD3DDevice->SetTransform(D3DMTS_VIEW,&cacheD3DMatrix,TOADLET_PEEPER_D3DM_FORMAT);
	TOADLET_CHECK_D3DMERROR(result,"setViewMatrix");
}

void D3DMRenderer::setProjectionMatrix(const Matrix4x4 &matrix){
	if(mMirrorY){
		Matrix4x4 mirror(matrix);
		mirror.setAt(1,0,-matrix.at(1,0));
		mirror.setAt(1,1,-matrix.at(1,1));
		mirror.setAt(1,2,-matrix.at(1,2));
		mirror.setAt(1,3,-matrix.at(1,3));
		toD3DMMATRIX(cacheD3DMatrix,mirror);
	}
	else{
		toD3DMMATRIX(cacheD3DMatrix,matrix);
	}

	HRESULT result=mD3DDevice->SetTransform(D3DMTS_PROJECTION,&cacheD3DMatrix,TOADLET_PEEPER_D3DM_FORMAT);
	TOADLET_CHECK_D3DMERROR(result,"setProjectionMatrix");
}

void D3DMRenderer::beginScene(){
	HRESULT result=mD3DDevice->BeginScene();
	TOADLET_CHECK_D3DMERROR(result,"beginScene");

	mStatisticsSet.reset();
}

void D3DMRenderer::endScene(){
	int i;
	for(i=0;i<mCapabilitySet.maxTextureStages;++i){
		setTextureStage(i,NULL);
	}

	HRESULT result=mD3DDevice->EndScene();
	TOADLET_CHECK_D3DMERROR(result,"endScene");
}

void D3DMRenderer::renderPrimitive(const VertexData::ptr &vertexData,const IndexData::ptr &indexData){
	if(vertexData==NULL || indexData==NULL){
		Error::nullPointer(Categories::TOADLET_PEEPER,
			"VertexData or IndexData is NULL");
		return;
	}

	HRESULT result;

	if(vertexData->getNumVertexBuffers()>1){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3DMRenderer does not support multiple streams");
		return;
	}

	// TODO: Remove redundant setting of vertex data
	VertexBuffer *vertexBuffer=vertexData->getVertexBuffer(0);

	// First check to see if the buffers have peers
	if(vertexBuffer->internal_getBufferPeer()!=NULL && (indexData->getIndexBuffer()==NULL || indexData->getIndexBuffer()->internal_getBufferPeer()!=NULL)){
		D3DMVertexBufferPeer *vertexPeer=(D3DMVertexBufferPeer*)vertexBuffer->internal_getBufferPeer();
		result=mD3DDevice->SetStreamSource(0,vertexPeer->vertexBuffer,vertexBuffer->getVertexFormat()->getVertexSize());
		TOADLET_CHECK_D3DMERROR(result,"D3DMRenderer: SetStreamSource");

		D3DMPRIMITIVETYPE d3dpt;
		int count;
		getPrimitiveTypeAndCount(d3dpt,count,indexData->getPrimitive(),indexData->getCount());

		IndexBuffer *indexBuffer=indexData->getIndexBuffer();
		if(indexBuffer!=NULL){
			D3DMIndexBufferPeer *indexPeer=(D3DMIndexBufferPeer*)indexBuffer->internal_getBufferPeer();
			result=mD3DDevice->SetIndices(indexPeer->indexBuffer);
			TOADLET_CHECK_D3DMERROR(result,"D3DMRenderer: SetIndices");

			result=mD3DDevice->DrawIndexedPrimitive(d3dpt,0,0,vertexBuffer->getSize(),indexData->getStart(),count);
			TOADLET_CHECK_D3DMERROR(result,"D3DMRenderer: DrawIndexedPrimitive");
		}
		else{
			result=mD3DDevice->DrawPrimitive(d3dpt,indexData->getStart(),count);
			TOADLET_CHECK_D3DMERROR(result,"D3DMRenderer: DrawPrimitive");
		}
	}
	else{
		// There is no DrawIndexedPrimitiveUP, DrawPrimitiveUP in D3DM
		Error::unknown(Categories::TOADLET_PEEPER,
			"Peerless buffers are not supported in the D3DMRenderer");
	}
}

void D3DMRenderer::setDefaultStates(){
	// General states
	mAlphaTest=AlphaTest_GEQUAL;
	mAlphaCutoff=0;
	mDepthWrite=false;
	mDepthTest=DepthTest_NONE;
	mDithering=true;
	mFaceCulling=FaceCulling_NONE;
	mLighting=true;
	mBlend=Blend(Blend::Combination_ALPHA);
	mShading=Shading_FLAT;
	mNormalize=Normalize_NONE;
	mTexturePerspective=false;

	setAlphaTest(AlphaTest_NONE,0.5f);
	setDepthWrite(true);
	setDepthTest(DepthTest_LEQUAL);
	setDithering(false);
	setFaceCulling(FaceCulling_BACK);
	setFogParameters(Fog_NONE,0,0.5f,Colors::BLACK);
	setLighting(false);
	setBlend(Blend::Combination_DISABLED);
	setShading(Shading_SMOOTH);
	setNormalize(Normalize_RESCALE);
	setTexturePerspective(true);

	int i;
	for(i=0;i<mCapabilitySet.maxTextureStages;++i){
		setTextureStage(i,NULL);
	}

	setLightEffect(LightEffect());
	setAmbientColor(Colors::WHITE);
	// We leave the current lights enabled because the SceneManager does not re-set the lights between layers

	// D3D specific states
	{
		mD3DDevice->SetRenderState(D3DMRS_ALPHABLENDENABLE,true);
		mD3DDevice->SetRenderState(D3DMRS_SPECULARENABLE,true);
	}
}

void D3DMRenderer::setAlphaTest(const AlphaTest &alphaTest,scalar cutoff){
	if(mAlphaTest==alphaTest && mAlphaCutoff==cutoff){
		return;
	}

	if(alphaTest==AlphaTest_NONE){
		mD3DDevice->SetRenderState(D3DMRS_ALPHATESTENABLE,false);
	}
	else{
		mD3DDevice->SetRenderState(D3DMRS_ALPHATESTENABLE,true);

		mD3DDevice->SetRenderState(D3DMRS_ALPHAREF,cutoff*255.0);

		switch(alphaTest){
			case AlphaTest_LESS:
				mD3DDevice->SetRenderState(D3DMRS_ALPHAFUNC,D3DMCMP_LESS);
			break;
			case AlphaTest_EQUAL:
				mD3DDevice->SetRenderState(D3DMRS_ALPHAFUNC,D3DMCMP_EQUAL);
			break;
			case AlphaTest_LEQUAL:
				mD3DDevice->SetRenderState(D3DMRS_ALPHAFUNC,D3DMCMP_LESSEQUAL);
			break;
			case AlphaTest_GREATER:
				mD3DDevice->SetRenderState(D3DMRS_ALPHAFUNC,D3DMCMP_GREATER);
			break;
			case AlphaTest_NOTEQUAL:
				mD3DDevice->SetRenderState(D3DMRS_ALPHAFUNC,D3DMCMP_NOTEQUAL);
			break;
			case AlphaTest_GEQUAL:
				mD3DDevice->SetRenderState(D3DMRS_ALPHAFUNC,D3DMCMP_GREATEREQUAL);
			break;
			case AlphaTest_ALWAYS:
				mD3DDevice->SetRenderState(D3DMRS_ALPHAFUNC,D3DMCMP_ALWAYS);
			break;
		}
	}

	mAlphaTest=alphaTest;
	mAlphaCutoff=cutoff;
}

void D3DMRenderer::setBlend(const Blend &blend){
	if(mBlend==blend){
		return;
	}

	if(blend.equals(Blend::Combination_DISABLED)){
		mD3DDevice->SetRenderState(D3DMRS_SRCBLEND,D3DMBLEND_ONE);
		mD3DDevice->SetRenderState(D3DMRS_DESTBLEND,D3DMBLEND_ZERO);
	}
	else{
		D3DMBLEND src=getD3DBlendOperation(blend.source);
		D3DMBLEND dest=getD3DBlendOperation(blend.dest);

		HRESULT hr;
		hr=mD3DDevice->SetRenderState(D3DMRS_SRCBLEND,src);
		TOADLET_CHECK_D3DMERROR(hr,"setBlendFunction");
		hr=mD3DDevice->SetRenderState(D3DMRS_DESTBLEND,dest);
		TOADLET_CHECK_D3DMERROR(hr,"setBlendFunction");
	}

	mBlend=blend;
}

void D3DMRenderer::setFaceCulling(const FaceCulling &culling){
	if(mFaceCulling==culling){
		return;
	}

	switch(culling){
		case FaceCulling_NONE:
			mD3DDevice->SetRenderState(D3DMRS_CULLMODE,D3DMCULL_NONE);
		break;
		case FaceCulling_CCW:
		case FaceCulling_FRONT:
			mD3DDevice->SetRenderState(D3DMRS_CULLMODE,mMirrorY?D3DMCULL_CW:D3DMCULL_CCW);
		break;
		case FaceCulling_CW:
		case FaceCulling_BACK:
			mD3DDevice->SetRenderState(D3DMRS_CULLMODE,mMirrorY?D3DMCULL_CCW:D3DMCULL_CW);
		break;
	}

	mFaceCulling=culling;
}

void D3DMRenderer::setDepthTest(const DepthTest &depthTest){
	if(mDepthTest==depthTest){
		return;
	}

	if(depthTest==DepthTest_NONE){
		mD3DDevice->SetRenderState(D3DMRS_ZENABLE,D3DMZB_FALSE);
	}
	else{
		mD3DDevice->SetRenderState(D3DMRS_ZENABLE,D3DMZB_TRUE);

		DWORD func=0;

		switch(depthTest){
			case DepthTest_NEVER:
				func=D3DMCMP_NEVER;
			break;
			case DepthTest_LESS:
				func=D3DMCMP_LESS;
			break;
			case DepthTest_EQUAL:
				func=D3DMCMP_EQUAL;
			break;
			case DepthTest_LEQUAL:
				func=D3DMCMP_LESSEQUAL;
			break;
			case DepthTest_GREATER:
				func=D3DMCMP_GREATER;
			break;
			case DepthTest_NOTEQUAL:
				func=D3DMCMP_NOTEQUAL;
			break;
			case DepthTest_GEQUAL:
				func=D3DMCMP_GREATEREQUAL;
			break;
			case DepthTest_ALWAYS:
				func=D3DMCMP_ALWAYS;
			break;
		}

		mD3DDevice->SetRenderState(D3DMRS_ZFUNC,func);
	}

	mDepthTest=depthTest;
}

void D3DMRenderer::setDepthWrite(bool enable){
	if(mDepthWrite==enable){
		return;
	}

	mD3DDevice->SetRenderState(D3DMRS_ZWRITEENABLE,enable);

	mDepthWrite=enable;
}

void D3DMRenderer::setDithering(bool dithering){
	if(mDithering==dithering){
		return;
	}

	HRESULT result=mD3DDevice->SetRenderState(D3DMRS_DITHERENABLE,dithering);

	mDithering=dithering;

	TOADLET_CHECK_D3DMERROR(result,"setDithering");
}

void D3DMRenderer::setFogParameters(const Fog &fog,scalar nearDistance,scalar farDistance,const Color &color){
	if(fog==Fog_NONE){
		mD3DDevice->SetRenderState(D3DMRS_FOGENABLE,FALSE);
	}
	else{
		mD3DDevice->SetRenderState(D3DMRS_FOGENABLE,TRUE);
	    mD3DDevice->SetRenderState(D3DMRS_FOGCOLOR,toD3DMCOLOR(color));

        mD3DDevice->SetRenderState(D3DMRS_FOGVERTEXMODE,D3DMFOG_LINEAR);
#		if defined(TOADLET_FIXED_POINT)
			float fNearDistance=Math::fixedToFloat(nearDistance);
			float fFarDistance=Math::fixedToFloat(farDistance);
			mD3DDevice->SetRenderState(D3DMRS_FOGSTART,*(DWORD*)(&fNearDistance));
	        mD3DDevice->SetRenderState(D3DMRS_FOGEND,*(DWORD*)(&fFarDistance));
#		else
			mD3DDevice->SetRenderState(D3DMRS_FOGSTART,*(DWORD*)(&nearDistance));
	        mD3DDevice->SetRenderState(D3DMRS_FOGEND,*(DWORD*)(&farDistance));
#		endif
	}
}

void D3DMRenderer::setLightEffect(const LightEffect &lightEffect){
	D3DMMATERIAL material;

	material.Ambient=toD3DMCOLORVALUE(lightEffect.ambient);
	material.Diffuse=toD3DMCOLORVALUE(lightEffect.diffuse);
	material.Specular=toD3DMCOLORVALUE(lightEffect.specular);
	material.Power=lightEffect.shininess;

	mD3DDevice->SetMaterial(&material,TOADLET_PEEPER_D3DM_FORMAT);

	mD3DDevice->SetRenderState(D3DMRS_COLORVERTEX,lightEffect.trackColor);
	if(lightEffect.trackColor){
		mD3DDevice->SetRenderState(D3DMRS_AMBIENTMATERIALSOURCE,D3DMMCS_COLOR1);
		mD3DDevice->SetRenderState(D3DMRS_DIFFUSEMATERIALSOURCE,D3DMMCS_COLOR1);
	}
	else{
		mD3DDevice->SetRenderState(D3DMRS_AMBIENTMATERIALSOURCE,D3DMMCS_MATERIAL);
		mD3DDevice->SetRenderState(D3DMRS_DIFFUSEMATERIALSOURCE,D3DMMCS_MATERIAL);
	}
}

void D3DMRenderer::setFill(const Fill &fill){
	if(mFill==fill){
		return;
	}

	DWORD d3dfill;
	if(fill==Fill_POINT){
		d3dfill=D3DMFILL_POINT;
	}
	else if(fill==Fill_LINE){
		d3dfill=D3DMFILL_WIREFRAME;
	}
	else{
		d3dfill=D3DMFILL_SOLID;
	}

	mD3DDevice->SetRenderState(D3DMRS_FILLMODE,d3dfill);

	mFill=fill;
}

void D3DMRenderer::setLighting(bool lighting){
	if(mLighting==lighting){
		return;
	}

	mD3DDevice->SetRenderState(D3DMRS_LIGHTING,lighting);

	mLighting=lighting;
}

void D3DMRenderer::setDepthBias(scalar constant,scalar slope){
	// TODO: We may need to scale these values to be more in-tune with the OpenGL ones
#	if defined(TOADLET_FIXED_POINT)
		float fConstant=Math::fixedToFloat(constant);
		float fSlope=Math::fixedToFloat(slope);
		mD3DDevice->SetRenderState(D3DMRS_DEPTHBIAS,*(DWORD*)&fConstant);
		mD3DDevice->SetRenderState(D3DMRS_SLOPESCALEDEPTHBIAS,*(DWORD*)&fSlope);
#	else
		mD3DDevice->SetRenderState(D3DMRS_DEPTHBIAS,*(DWORD*)&constant);
		mD3DDevice->SetRenderState(D3DMRS_SLOPESCALEDEPTHBIAS,*(DWORD*)&slope);
#	endif
}

void D3DMRenderer::setTexturePerspective(bool texturePerspective){
	if(mTexturePerspective==texturePerspective){
		return;
	}

	mD3DDevice->SetRenderState(D3DMRS_TEXTUREPERSPECTIVE,texturePerspective);

	mTexturePerspective=texturePerspective;
}

void D3DMRenderer::setTextureStage(int stage,TextureStage *textureStage){
	HRESULT result=S_OK;

	if(textureStage!=NULL){
		Texture *texture=textureStage->getTexture();
		if(texture!=NULL && texture->internal_getTexturePeer()!=NULL){
			D3DMTexturePeer *peer=(D3DMTexturePeer*)texture->internal_getTexturePeer();

			result=mD3DDevice->SetTexture(stage,peer->baseTexture);
			TOADLET_CHECK_D3DMERROR(result,"SetTexture");
		}
		else{
			result=mD3DDevice->SetTexture(stage,NULL);
			TOADLET_CHECK_D3DMERROR(result,"SetTexture");
		}

		result=mD3DDevice->SetTextureStageState(stage,D3DMTSS_TEXCOORDINDEX,textureStage->getTexCoordIndex());
		TOADLET_CHECK_D3DMERROR(result,"SetTextureStageState");

		toD3DMMATRIX(cacheD3DMatrix,textureStage->getTextureMatrix());

		result=mD3DDevice->SetTransform((D3DMTRANSFORMSTATETYPE)(D3DMTS_TEXTURE0+stage),&cacheD3DMatrix,TOADLET_PEEPER_D3DM_FORMAT);
		TOADLET_CHECK_D3DMERROR(result,"SetTransform");

		const TextureBlend &blend=textureStage->getBlend();
		if(blend.operation!=TextureBlend::Operation_UNSPECIFIED){
			DWORD mode=0;
			switch(blend.operation){
				case TextureBlend::Operation_REPLACE:
					mode=D3DMTOP_DISABLE;
				break;
				case TextureBlend::Operation_MODULATE:
					mode=D3DMTOP_MODULATE;
				break;
				case TextureBlend::Operation_ADD:
					mode=D3DMTOP_ADD;
				break;
				case TextureBlend::Operation_DOTPRODUCT:
					mode=D3DMTOP_DOTPRODUCT3;
				break;
			}

			result=mD3DDevice->SetTextureStageState(stage,D3DMTSS_COLOROP,mode);
			TOADLET_CHECK_D3DMERROR(result,"SetTextureStageState");
			result=mD3DDevice->SetTextureStageState(stage,D3DMTSS_ALPHAOP,mode);
			TOADLET_CHECK_D3DMERROR(result,"SetTextureStageState");
			if(blend.source1!=TextureBlend::Source_UNSPECIFIED && blend.source2!=TextureBlend::Source_UNSPECIFIED){
				mD3DDevice->SetTextureStageState(stage,D3DMTSS_COLORARG1,getD3DTextureBlendSource(blend.source1));
				mD3DDevice->SetTextureStageState(stage,D3DMTSS_COLORARG2,getD3DTextureBlendSource(blend.source2));
			}
		}
		else{
			result=mD3DDevice->SetTextureStageState(stage,D3DMTSS_COLOROP,D3DMTOP_MODULATE);
			TOADLET_CHECK_D3DMERROR(result,"SetTextureStageState");
			result=mD3DDevice->SetTextureStageState(stage,D3DMTSS_ALPHAOP,D3DMTOP_MODULATE);
			TOADLET_CHECK_D3DMERROR(result,"SetTextureStageState");
		}

		if(textureStage->getAddressModeSpecified()){
			mD3DDevice->SetTextureStageState(stage,D3DMTSS_ADDRESSU,D3DMTexturePeer::getD3DTADDRESS(textureStage->getSAddressMode()));
			mD3DDevice->SetTextureStageState(stage,D3DMTSS_ADDRESSV,D3DMTexturePeer::getD3DTADDRESS(textureStage->getTAddressMode()));
			mD3DDevice->SetTextureStageState(stage,D3DMTSS_ADDRESSW,D3DMTexturePeer::getD3DTADDRESS(textureStage->getRAddressMode()));
		}

		if(textureStage->getFilterSpecified()){
			mD3DDevice->SetTextureStageState(stage,D3DMTSS_MINFILTER,D3DMTexturePeer::getD3DTEXF(textureStage->getMinFilter()));
			mD3DDevice->SetTextureStageState(stage,D3DMTSS_MIPFILTER,D3DMTexturePeer::getD3DTEXF(textureStage->getMipFilter()));
			mD3DDevice->SetTextureStageState(stage,D3DMTSS_MAGFILTER,D3DMTexturePeer::getD3DTEXF(textureStage->getMagFilter()));
		}
	}
	else{
		int result=mD3DDevice->SetTexture(stage,NULL);
		TOADLET_CHECK_D3DMERROR(result,"SetTexture");

		result=mD3DDevice->SetTextureStageState(stage,D3DMTSS_TEXCOORDINDEX,0);
		TOADLET_CHECK_D3DMERROR(result,"SetTextureStageState");
	}
}

void D3DMRenderer::setProgram(const Program *program){
}

void D3DMRenderer::setShading(const Shading &shading){
	if(mShading==shading){
		return;
	}

	switch(shading){
		case Shading_FLAT:
			mD3DDevice->SetRenderState(D3DMRS_SHADEMODE,D3DMSHADE_FLAT);
		break;
		case Shading_SMOOTH:
			mD3DDevice->SetRenderState(D3DMRS_SHADEMODE,D3DMSHADE_GOURAUD);
		break;
	}

	mShading=shading;
}

void D3DMRenderer::setColorWrite(bool color){
	if(color){
		mD3DDevice->SetRenderState(D3DMRS_COLORWRITEENABLE,	D3DMCOLORWRITEENABLE_RED		|
															D3DMCOLORWRITEENABLE_GREEN	|
															D3DMCOLORWRITEENABLE_BLUE	|
															D3DMCOLORWRITEENABLE_ALPHA);
	}
	else{
		mD3DDevice->SetRenderState(D3DMRS_COLORWRITEENABLE,0);
	}
}

void D3DMRenderer::setNormalize(const Normalize &normalize){
	if(mNormalize==normalize){
		return;
	}

	if(normalize!=Normalize_NONE){
		mD3DDevice->SetRenderState(D3DMRS_NORMALIZENORMALS,TRUE);
	}
	else{
		mD3DDevice->SetRenderState(D3DMRS_NORMALIZENORMALS,FALSE);
	}

	mNormalize=normalize;
}

void D3DMRenderer::setTexCoordGen(int stage,const TexCoordGen &tcg,const Matrix4x4 &matrix){
	toD3DMMATRIX(cacheD3DMatrix,matrix);

	switch(tcg){
		case TexCoordGen_DISABLED:
			mD3DDevice->SetTextureStageState(stage,D3DMTSS_TEXCOORDINDEX,0); // TODO: Replace TexCoordIndex stage?
			mD3DDevice->SetTextureStageState(stage,D3DMTSS_TEXTURETRANSFORMFLAGS,D3DMTTFF_DISABLE);
		break;
		case TexCoordGen_OBJECTSPACE:
			mD3DDevice->SetTextureStageState(stage,D3DMTSS_TEXCOORDINDEX,D3DMTSS_TCI_PASSTHRU);
			mD3DDevice->SetTextureStageState(stage,D3DMTSS_TEXTURETRANSFORMFLAGS,D3DMTTFF_PROJECTED);

			// TODO: This will interfear with setTextureStage
			mD3DDevice->SetTransform((D3DMTRANSFORMSTATETYPE)(D3DMTS_TEXTURE0+stage),&cacheD3DMatrix,TOADLET_PEEPER_D3DM_FORMAT);
		break;
		case TexCoordGen_CAMERASPACE:
			mD3DDevice->SetTextureStageState(stage,D3DMTSS_TEXCOORDINDEX,D3DMTSS_TCI_CAMERASPACEPOSITION);
			mD3DDevice->SetTextureStageState(stage,D3DMTSS_TEXTURETRANSFORMFLAGS,D3DMTTFF_PROJECTED);

			// TODO: This will interfear with setTextureStage
			mD3DDevice->SetTransform((D3DMTRANSFORMSTATETYPE)(D3DMTS_TEXTURE0+stage),&cacheD3DMatrix,TOADLET_PEEPER_D3DM_FORMAT);
		break;
	}
}

void D3DMRenderer::getShadowBiasMatrix(const Texture *shadowTexture,Matrix4x4 &result){
	int width=shadowTexture->getWidth();
	int height=shadowTexture->getHeight();
	scalar xoff=Math::HALF+Math::div(Math::HALF,Math::fromInt(width));
	scalar yoff=Math::HALF+Math::div(Math::HALF,Math::fromInt(height));
	result.set( Math::HALF, 0,           0,         xoff,
				0,          -Math::HALF, 0,         yoff,
				0,          0,           Math::ONE, 0,
				0,          0,           0,         Math::ONE);
}

void D3DMRenderer::setShadowComparisonMethod(bool enabled){
	if(enabled){
		mD3DDevice->SetRenderState(D3DMRS_BLENDOP,D3DMBLENDOP_MAX);
	}
	else{
		mD3DDevice->SetRenderState(D3DMRS_BLENDOP,D3DMBLENDOP_ADD);
	}
}

void D3DMRenderer::setLight(int i,Light *light){
	D3DMLIGHT d3dlight;
	ZeroMemory(&d3dlight,sizeof(D3DMLIGHT));

	switch(light->getType()){
		case Light::Type_DIRECTION:{
			d3dlight.Type=D3DMLIGHT_DIRECTIONAL;
			d3dlight.Direction=toD3DMVECTOR(light->getDirection());
			break;
		}
		case Light::Type_POSITION:{
			d3dlight.Type=D3DMLIGHT_POINT;
			d3dlight.Position=toD3DMVECTOR(light->getPosition());
			break;
		}
		case Light::Type_SPOT:{
			break;
		}
	}

	d3dlight.Diffuse=toD3DMCOLORVALUE(light->getDiffuseColor());
	d3dlight.Specular=toD3DMCOLORVALUE(light->getSpecularColor());
	d3dlight.Attenuation1=light->getLinearAttenuation();
	d3dlight.Range=light->getRadius();

	mD3DDevice->SetLight(i,&d3dlight,TOADLET_PEEPER_D3DM_FORMAT);
}

void D3DMRenderer::setLightEnabled(int i,bool enable){
	mD3DDevice->LightEnable(i,enable);
}

void D3DMRenderer::setAmbientColor(const Color &ambient){
	mD3DDevice->SetRenderState(D3DMRS_AMBIENT,toD3DMCOLOR(ambient));
}

const StatisticsSet &D3DMRenderer::getStatisticsSet(){
	return mStatisticsSet;
}

const CapabilitySet &D3DMRenderer::getCapabilitySet(){
	return mCapabilitySet;
}

void D3DMRenderer::setCapabilitySetFromCaps(CapabilitySet &capabilitySet,const D3DMCAPS &caps,bool renderToTexture,bool renderToDepthTexture){
	capabilitySet.hardwareTextures=true;
	capabilitySet.hardwareIndexBuffers=true;
	capabilitySet.hardwareVertexBuffers=true;
	capabilitySet.pointSprites=false;
	capabilitySet.maxLights=caps.MaxActiveLights;
	capabilitySet.maxTextureStages=caps.MaxTextureBlendStages;
	capabilitySet.maxTextureSize=Math::minVal(caps.MaxTextureWidth,caps.MaxTextureHeight);
	capabilitySet.textureDot3=(caps.TextureOpCaps & D3DMTEXOPCAPS_DOTPRODUCT3)!=0;
	capabilitySet.textureNonPowerOf2=(caps.TextureCaps & D3DMPTEXTURECAPS_POW2)==0 && (caps.TextureCaps & D3DMPTEXTURECAPS_SQUAREONLY)==0;
	capabilitySet.textureNonPowerOf2Restricted=false;
	capabilitySet.renderToTexture=renderToTexture;
	capabilitySet.renderToDepthTexture=renderToDepthTexture;
	capabilitySet.renderToTextureNonPowerOf2Restricted=capabilitySet.textureNonPowerOf2Restricted && capabilitySet.renderToTexture;
#	if defined(TOADLET_FIXED_POINT)
		capabilitySet.idealFormatBit=VertexElement::Format_BIT_FIXED_32;
#	else
		capabilitySet.idealFormatBit=VertexElement::Format_BIT_FLOAT_32;
#	endif

	Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ALERT,
		String("D3DM has lockable textures:")+((caps.SurfaceCaps & D3DMSURFCAPS_LOCKTEXTURE)>0));
}

DWORD D3DMRenderer::getD3DTextureBlendSource(TextureBlend::Source blend){
	switch(blend){
		case TextureBlend::Source_PREVIOUS:
			return D3DMTA_CURRENT;
		case TextureBlend::Source_TEXTURE:
			return D3DMTA_TEXTURE;
		case TextureBlend::Source_PRIMARY_COLOR:
			return D3DMTA_DIFFUSE;
		default:
			return 0;
	}
}

D3DMBLEND D3DMRenderer::getD3DBlendOperation(Blend::Operation blend){
	switch(blend){
		case Blend::Operation_ONE:
			return D3DMBLEND_ONE;
		case Blend::Operation_ZERO:
			return D3DMBLEND_ZERO;
		case Blend::Operation_DEST_COLOR:
			return D3DMBLEND_DESTCOLOR;
		case Blend::Operation_SOURCE_COLOR:
			return D3DMBLEND_SRCCOLOR;
		case Blend::Operation_ONE_MINUS_DEST_COLOR:
			return D3DMBLEND_INVDESTCOLOR;
		case Blend::Operation_ONE_MINUS_SOURCE_COLOR:
			return D3DMBLEND_INVSRCCOLOR;
		case Blend::Operation_DEST_ALPHA:
			return D3DMBLEND_DESTALPHA;
		case Blend::Operation_SOURCE_ALPHA:
			return D3DMBLEND_SRCALPHA;
		case Blend::Operation_ONE_MINUS_DEST_ALPHA:
			return D3DMBLEND_INVDESTALPHA;
		case Blend::Operation_ONE_MINUS_SOURCE_ALPHA:
			return D3DMBLEND_INVSRCALPHA;
		default:
			return (D3DMBLEND)0;
	}
}

void D3DMRenderer::getPrimitiveTypeAndCount(D3DMPRIMITIVETYPE &d3dpt,int &count,IndexData::Primitive prim,int numIndexes){
	switch(prim){
		case IndexData::Primitive_POINTS:
			d3dpt=D3DMPT_POINTLIST;
			count=numIndexes;
		break;
		case IndexData::Primitive_LINES:
			d3dpt=D3DMPT_LINELIST;
			count=numIndexes/2;
		break;
		case IndexData::Primitive_LINESTRIP:
			d3dpt=D3DMPT_LINESTRIP;
			count=numIndexes-1;
		break;
		case IndexData::Primitive_TRIS:
			d3dpt=D3DMPT_TRIANGLELIST;
			count=numIndexes/3;
		break;
		case IndexData::Primitive_TRISTRIP:
			d3dpt=D3DMPT_TRIANGLESTRIP;
			count=numIndexes-2;
		break;
		case IndexData::Primitive_TRIFAN:
			d3dpt=D3DMPT_TRIANGLEFAN;
			count=numIndexes-2;
		break;
	}
}

void D3DMRenderer::setMirrorY(bool mirrorY){
	mMirrorY=mirrorY;

	FaceCulling faceCulling=mFaceCulling;
	mFaceCulling=FaceCulling_NONE;
	setFaceCulling(faceCulling);
}

void D3DMRenderer::copyFrameBufferToTexture(Texture *texture){
/*
	D3D9TexturePeer *peer=(D3D9TexturePeer*)texture->getTexturePeer();
	if(peer==NULL){
		throw std::runtime_error("invalid peer in copyBufferToTexture");
	}

	int width,height;
	texture->getSize(width,height);

	glBindTexture(peer->dimension,peer->textureHandle);
	glCopyTexSubImage2D(peer->dimension,0,0,0,0,0,width,height);

	if(texture->getMipMap() && glGenerateMipmapEXT!=NULL){
		glGenerateMipmapEXT(peer->dimension);
	}

	glBindTexture(peer->dimension,0);

	TOADLET_CHECK_ERROR("copyBufferToTexture");
*/
}

}
}
