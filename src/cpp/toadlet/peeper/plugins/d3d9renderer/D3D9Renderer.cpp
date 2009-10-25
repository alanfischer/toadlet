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
#include "D3D9VertexBufferPeer.h"
#include "D3D9IndexBufferPeer.h"
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
	mTexturePerspective(false),

	mMirrorY(false)

	//mStatisticsSet,
	//mCapabilitySet
{
}

D3D9Renderer::~D3D9Renderer(){
	TOADLET_ASSERT(mShutdown);
}

bool D3D9Renderer::startup(RenderTarget *target,int *options){
	Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ALERT,
		"D3D9Renderer: Startup started");

	if(target==NULL){
		Error::nullPointer(Categories::TOADLET_PEEPER,
			"D3D9Renderer: NULL RenderTarget");
		return false;
	}

	D3D9RenderTarget *d3dtarget=(D3D9RenderTarget*)target->getRootRenderTarget();
	mD3D=d3dtarget->getDirect3D9();
	mD3DDevice=d3dtarget->getDirect3DDevice9();
	if(mD3DDevice==NULL){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D9Renderer: Invalid Device");
		return false;
	}

	mShutdown=false;
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
		"D3D9Renderer: Startup finished");

	return true;
}

bool D3D9Renderer::shutdown(){
	mShutdown=true;

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
	return NULL;
}

BufferPeer *D3D9Renderer::createBufferPeer(Buffer *buffer){
	if(buffer==NULL){
		Error::nullPointer(Categories::TOADLET_PEEPER,
			"Buffer is NULL");
		return NULL;
	}
	if(buffer->getType()==Buffer::Type_INDEX && mCapabilitySet.hardwareIndexBuffers==true){
		D3D9IndexBufferPeer *peer=new D3D9IndexBufferPeer(this,(IndexBuffer*)buffer);
		if(peer->isValid()==false){
			delete peer;
			peer=NULL;
		}
		return peer;
	}
	else if(buffer->getType()==Buffer::Type_VERTEX && mCapabilitySet.hardwareVertexBuffers==true){
		D3D9VertexBufferPeer *peer=new D3D9VertexBufferPeer(this,(VertexBuffer*)buffer);
		if(peer->isValid()==false){
			delete peer;
			peer=NULL;
		}
		return peer;
	}

	return NULL;
}

ProgramPeer *D3D9Renderer::createProgramPeer(Program *program){
	return NULL;
}

ShaderPeer *D3D9Renderer::createShaderPeer(Shader *shader){
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

	HRESULT result;

	if(vertexData->getNumVertexBuffers()>1){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D9Renderer does not support multiple streams yet");
		return;
	}

	D3DPRIMITIVETYPE d3dpt;
	int count;
	getPrimitiveTypeAndCount(d3dpt,count,indexData->getPrimitive(),indexData->getCount());

	// TODO: Remove redundant setting of vertex data
	if(vertexData->getNumVertexBuffers()>0){
		// First check to see if the buffers have peers
		if(vertexData->getVertexBuffer(0)->internal_getBufferPeer()!=NULL && (indexData->getIndexBuffer()==NULL || indexData->getIndexBuffer()->internal_getBufferPeer()!=NULL)){
			int numVertexes=0;
			int i;
			for(i=0;i<vertexData->getNumVertexBuffers();++i){
				VertexBuffer *vertexBuffer=vertexData->getVertexBuffer(i);
				if(numVertexes==0){
					numVertexes=vertexBuffer->getSize();
				}
				D3D9VertexBufferPeer *peer=(D3D9VertexBufferPeer*)vertexBuffer->internal_getBufferPeer();
				#if defined(TOADLET_HAS_DIRECT3DMOBILE)
					result=mD3DDevice->SetStreamSource(i,peer->vertexBuffer,vertexBuffer->getVertexFormat()->getVertexSize());
				#else
					result=mD3DDevice->SetStreamSource(i,peer->vertexBuffer,0,vertexBuffer->getVertexFormat()->getVertexSize());
				#endif
				TOADLET_CHECK_D3D9ERROR(result,"D3D9Renderer: SetStreamSource");
				#if !defined(TOADLET_HAS_DIRECT3DMOBILE)
					result=mD3DDevice->SetFVF(peer->fvf);
					TOADLET_CHECK_D3D9ERROR(result,"D3D9Renderer: SetFVF");
				#endif
			}

			IndexBuffer *indexBuffer=indexData->getIndexBuffer();
			if(indexBuffer!=NULL){
				D3D9IndexBufferPeer *peer=(D3D9IndexBufferPeer*)indexBuffer->internal_getBufferPeer();
				result=mD3DDevice->SetIndices(peer->indexBuffer);
				TOADLET_CHECK_D3D9ERROR(result,"D3D9Renderer: SetIndices");

				result=mD3DDevice->DrawIndexedPrimitive(d3dpt,0,0,numVertexes,indexData->getStart(),count);
				TOADLET_CHECK_D3D9ERROR(result,"D3D9Renderer: DrawIndexedPrimitive");
			}
			else{
				result=mD3DDevice->DrawPrimitive(d3dpt,indexData->getStart(),count);
				TOADLET_CHECK_D3D9ERROR(result,"D3D9Renderer: DrawPrimitive");
			}
		}
		#if !defined(TOADLET_HAS_DIRECT3DMOBILE)
			else{
				// Warning: This path is very unoptimized.  Use buffer peers if at all possible
				VertexBuffer *vertexBuffer=vertexData->getVertexBuffer(0);

				int numVertexes=vertexBuffer->getSize();
				int vertexSize=vertexBuffer->getVertexFormat()->getVertexSize();

				Collection<VertexElement> colorElements;
				result=mD3DDevice->SetFVF(D3D9VertexBufferPeer::getFVF(vertexBuffer,&colorElements));
				TOADLET_CHECK_D3D9ERROR(result,"D3D9Renderer: SetFVF");

				int i,j;
				for(i=0;i<colorElements.size();++i){
					const VertexElement &vertexElement=colorElements[i];
					for(j=0;j<numVertexes;++j){
						uint32 &color=*(uint32*)(vertexBuffer->internal_getData()+vertexSize*j+vertexElement.offset);
						color=(color&0xFF000000)|((color&0x000000FF)<<16)|(color&0x0000FF00)|((color&0x00FF0000)>>16);
					}
				}

				IndexBuffer *indexBuffer=indexData->getIndexBuffer();
				if(indexBuffer!=NULL){
					result=mD3DDevice->DrawIndexedPrimitiveUP(d3dpt,0,numVertexes,count,indexBuffer->internal_getData()+indexData->getStart()*indexBuffer->getIndexFormat(),D3D9IndexBufferPeer::getD3DFORMAT(indexBuffer->getIndexFormat()),vertexBuffer->internal_getData(),vertexSize);
					TOADLET_CHECK_D3D9ERROR(result,"D3D9Renderer: DrawIndexedPrimitiveUP");
				}
				else{
					result=mD3DDevice->DrawPrimitiveUP(d3dpt,count,vertexBuffer->internal_getData()+indexData->getStart()*vertexSize,vertexSize);
					TOADLET_CHECK_D3D9ERROR(result,"D3D9Renderer: DrawPrimitiveUP");
				}

				for(i=0;i<colorElements.size();++i){
					const VertexElement &vertexElement=colorElements[i];
					for(j=0;j<numVertexes;++j){
						uint32 &color=*(uint32*)(vertexBuffer->internal_getData()+vertexSize*j+vertexElement.offset);
						color=(color&0xFF000000)|((color&0x000000FF)<<16)|(color&0x0000FF00)|((color&0x00FF0000)>>16);
					}
				}
			}
		#endif
	}
}

void D3D9Renderer::setDefaultStates(){
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

	setAlphaTest(AlphaTest_NONE,0.5);
	setDepthWrite(true);
	setDepthTest(DepthTest_LEQUAL);
	setDithering(false);
	setFaceCulling(FaceCulling_BACK);
	setFogParameters(Fog_NONE,0,1.0,Colors::BLACK);
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
		mD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
		mD3DDevice->SetRenderState(D3DRS_SPECULARENABLE,true);
	}
}

void D3D9Renderer::setAlphaTest(const AlphaTest &alphaTest,scalar cutoff){
	if(mAlphaTest==alphaTest && mAlphaCutoff==cutoff){
		return;
	}

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

	mAlphaTest=alphaTest;
	mAlphaCutoff=cutoff;
}

void D3D9Renderer::setBlend(const Blend &blend){
	if(mBlend==blend){
		return;
	}

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

	mBlend.set(blend);
}

void D3D9Renderer::setFaceCulling(const FaceCulling &culling){
	if(mFaceCulling==culling){
		return;
	}

	switch(culling){
		case FaceCulling_NONE:
			mD3DDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
		break;
		case FaceCulling_CCW:
		case FaceCulling_FRONT:
			mD3DDevice->SetRenderState(D3DRS_CULLMODE,mMirrorY?D3DCULL_CW:D3DCULL_CCW);
		break;
		case FaceCulling_CW:
		case FaceCulling_BACK:
			mD3DDevice->SetRenderState(D3DRS_CULLMODE,mMirrorY?D3DCULL_CCW:D3DCULL_CW);
		break;
	}

	mFaceCulling=culling;
}

void D3D9Renderer::setDepthTest(const DepthTest &depthTest){
	if(mDepthTest==depthTest){
		return;
	}

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

		mD3DDevice->SetRenderState(D3DRS_ZFUNC,func);
	}

	mDepthTest=depthTest;
}

void D3D9Renderer::setDepthWrite(bool enable){
	if(mDepthWrite==enable){
		return;
	}

	mD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,enable);

	mDepthWrite=enable;
}

void D3D9Renderer::setDithering(bool dithering){
	if(mDithering==dithering){
		return;
	}

	HRESULT result=mD3DDevice->SetRenderState(D3DRS_DITHERENABLE,dithering);

	mDithering=dithering;

	TOADLET_CHECK_D3D9ERROR(result,"setDithering");
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

void D3D9Renderer::setFill(const Fill &fill){
	if(mFill==fill){
		return;
	}

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

	mD3DDevice->SetRenderState(D3DRS_FILLMODE,d3dfill);

	mFill=fill;
}

void D3D9Renderer::setLighting(bool lighting){
	if(mLighting==lighting){
		return;
	}

	mD3DDevice->SetRenderState(D3DRS_LIGHTING,lighting);

	mLighting=lighting;
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
	if(mTexturePerspective==texturePerspective){
		return;
	}

	#if defined(TOADLET_HAS_DIRECT3DMOBILE)
		mD3DDevice->SetRenderState(D3DMRS_TEXTUREPERSPECTIVE,texturePerspective);
	#endif

	mTexturePerspective=texturePerspective;
}

void D3D9Renderer::setTextureStage(int stage,TextureStage *textureStage){
	HRESULT result=S_OK;

	if(textureStage!=NULL){
		Texture *texture=textureStage->getTexture()->getRootTexture();
		if(texture!=NULL){
			D3D9Texture *d3dtexture=(D3D9Texture*)texture;

			result=mD3DDevice->SetTexture(stage,d3dtexture->mTexture);
			TOADLET_CHECK_D3D9ERROR(result,"SetTexture");
		}
		else{
			result=mD3DDevice->SetTexture(stage,NULL);
			TOADLET_CHECK_D3D9ERROR(result,"SetTexture");
		}

		result=mD3DDevice->SetTextureStageState(stage,D3DTSS_TEXCOORDINDEX,textureStage->getTexCoordIndex());
		TOADLET_CHECK_D3D9ERROR(result,"SetTextureStageState");

		// TODO: Only if the texture matrix is set, and we're not using shaders
		{
			result=mD3DDevice->SetTextureStageState(stage,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_COUNT2);
			TOADLET_CHECK_D3D9ERROR(result,"SetTextureStageState");

			toD3DMATRIX(cacheD3DMatrix,textureStage->getTextureMatrix());
			result=mD3DDevice->SetTransform((D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0+stage),&cacheD3DMatrix TOADLET_D3DMFMT);
			TOADLET_CHECK_D3D9ERROR(result,"SetTransform");
		}

		const TextureBlend &blend=textureStage->getBlend();
		if(blend.operation!=TextureBlend::Operation_UNSPECIFIED){
			DWORD mode=0;
			switch(blend.operation){
				case TextureBlend::Operation_REPLACE:
					mode=D3DTOP_DISABLE;
				break;
				case TextureBlend::Operation_MODULATE:
					mode=D3DTOP_MODULATE;
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
			if(textureStage->getAddressModeSpecified()){
				mD3DDevice->SetTextureStageState(stage,D3DMTSS_ADDRESSU,D3D9Texture::getD3DTADDRESS(textureStage->getSAddressMode()));
				mD3DDevice->SetTextureStageState(stage,D3DMTSS_ADDRESSV,D3D9Texture::getD3DTADDRESS(textureStage->getTAddressMode()));
				mD3DDevice->SetTextureStageState(stage,D3DMTSS_ADDRESSW,D3D9Texture::getD3DTADDRESS(textureStage->getRAddressMode()));
			}

			if(textureStage->getFilterSpecified()){
				mD3DDevice->SetTextureStageState(stage,D3DMTSS_MINFILTER,D3D9Texture::getD3DTEXF(textureStage->getMinFilter()));
				mD3DDevice->SetTextureStageState(stage,D3DMTSS_MIPFILTER,D3D9Texture::getD3DTEXF(textureStage->getMipFilter()));
				mD3DDevice->SetTextureStageState(stage,D3DMTSS_MAGFILTER,D3D9Texture::getD3DTEXF(textureStage->getMagFilter()));
			}
		#else
			if(textureStage->getAddressModeSpecified()){
				mD3DDevice->SetSamplerState(stage,D3DSAMP_ADDRESSU,D3D9Texture::getD3DTADDRESS(textureStage->getSAddressMode()));
				mD3DDevice->SetSamplerState(stage,D3DSAMP_ADDRESSV,D3D9Texture::getD3DTADDRESS(textureStage->getTAddressMode()));
				mD3DDevice->SetSamplerState(stage,D3DSAMP_ADDRESSW,D3D9Texture::getD3DTADDRESS(textureStage->getRAddressMode()));
			}

			if(textureStage->getFilterSpecified()){
				mD3DDevice->SetSamplerState(stage,D3DSAMP_MINFILTER,D3D9Texture::getD3DTEXF(textureStage->getMinFilter()));
				mD3DDevice->SetSamplerState(stage,D3DSAMP_MIPFILTER,D3D9Texture::getD3DTEXF(textureStage->getMipFilter()));
				mD3DDevice->SetSamplerState(stage,D3DSAMP_MAGFILTER,D3D9Texture::getD3DTEXF(textureStage->getMagFilter()));
			}
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
	if(mShading==shading){
		return;
	}

	switch(shading){
		case Shading_FLAT:
			mD3DDevice->SetRenderState(D3DRS_SHADEMODE,D3DSHADE_FLAT);
		break;
		case Shading_SMOOTH:
			mD3DDevice->SetRenderState(D3DRS_SHADEMODE,D3DSHADE_GOURAUD);
		break;
	}

	mShading=shading;
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
	if(mNormalize==normalize){
		return;
	}

	if(normalize!=Normalize_NONE){
		mD3DDevice->SetRenderState(D3DRS_NORMALIZENORMALS,TRUE);
	}
	else{
		mD3DDevice->SetRenderState(D3DRS_NORMALIZENORMALS,FALSE);
	}

	mNormalize=normalize;
}

void D3D9Renderer::setTexCoordGen(int stage,const TexCoordGen &texCoordGen,const Matrix4x4 &matrix){
	toD3DMATRIX(cacheD3DMatrix,matrix);

	switch(texCoordGen){
		case TexCoordGen_DISABLED:
			mD3DDevice->SetTextureStageState(stage,D3DTSS_TEXCOORDINDEX,0); // TODO: Replace TexCoordIndex stage?
			mD3DDevice->SetTextureStageState(stage,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE);
		break;
		case TexCoordGen_OBJECTSPACE:
			mD3DDevice->SetTextureStageState(stage,D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_PASSTHRU);
			mD3DDevice->SetTextureStageState(stage,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_COUNT4|D3DTTFF_PROJECTED);

			// TODO: This will interfear with setTextureStage
			mD3DDevice->SetTransform((D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0+stage),&cacheD3DMatrix TOADLET_D3DMFMT);
		break;
		case TexCoordGen_CAMERASPACE:
			mD3DDevice->SetTextureStageState(stage,D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_CAMERASPACEPOSITION);
			mD3DDevice->SetTextureStageState(stage,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_COUNT4|D3DTTFF_PROJECTED);

			// TODO: This will interfear with setTextureStage
			mD3DDevice->SetTransform((D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0+stage),&cacheD3DMatrix TOADLET_D3DMFMT);
		break;
	}
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
			toD3DVECTOR(d3dlight.Direction,light->getDirection());
			break;
		}
		case Light::Type_POSITION:{
			d3dlight.Type=D3DLIGHT_POINT;
			toD3DVECTOR(d3dlight.Position,light->getPosition());
			break;
		}
		case Light::Type_SPOT:{
			#if !defined(TOADLET_HAS_DIRECT3DMOBILE)
				d3dlight.Type=D3DLIGHT_SPOT;
				toD3DVECTOR(d3dlight.Position,light->getPosition());
			#endif
			break;
		}
	}

	toD3DCOLORVALUE(d3dlight.Diffuse,light->getDiffuseColor());
	toD3DCOLORVALUE(d3dlight.Specular,light->getSpecularColor());
	d3dlight.Attenuation1=scalarToFloat(light->getLinearAttenuation());
	d3dlight.Range=scalarToFloat(light->getRadius());

	mD3DDevice->SetLight(i,&d3dlight TOADLET_D3DMFMT);
}

void D3D9Renderer::setLightEnabled(int i,bool enable){
	mD3DDevice->LightEnable(i,enable);
}

void D3D9Renderer::setAmbientColor(const Color &ambient){
	mD3DDevice->SetRenderState(D3DRS_AMBIENT,toD3DCOLOR(ambient));
}

const StatisticsSet &D3D9Renderer::getStatisticsSet(){
	return mStatisticsSet;
}

const CapabilitySet &D3D9Renderer::getCapabilitySet(){
	return mCapabilitySet;
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
		capabilitySet.idealFormatBit=VertexElement::Format_BIT_FIXED_32;
	#else
		capabilitySet.idealFormatBit=VertexElement::Format_BIT_FLOAT_32;
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

void D3D9Renderer::copyFrameBufferToTexture(Texture *texture){
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
