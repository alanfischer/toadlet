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
#include "D3D9SurfaceRenderTarget.h"
#include <toadlet/egg/Error.h>

using namespace toadlet::egg;
using namespace toadlet::egg::image;

namespace toadlet{
namespace peeper{

D3D9SurfaceRenderTarget::D3D9SurfaceRenderTarget(D3D9Renderer *renderer):D3D9RenderTarget(),
	mRenderer(NULL),
	mWidth(0),
	mHeight(0),
	mNeedsCompile(false)
	//mSurfaces,
	//mSurfaceAttachments,
	//mOwnedSurfaces
{
	mRenderer=renderer;
}

D3D9SurfaceRenderTarget::~D3D9SurfaceRenderTarget(){
	destroy();
}

bool D3D9SurfaceRenderTarget::create(){
	destroy();

	mWidth=0;
	mHeight=0;
	mNeedsCompile=true;

	return true;
}

bool D3D9SurfaceRenderTarget::destroy(){
	int i;
	for(i=0;i<mOwnedSurfaces.size();++i){
		mOwnedSurfaces[i]->destroy();
	}
	mOwnedSurfaces.clear();

	return true;
}

bool D3D9SurfaceRenderTarget::makeCurrent(IDirect3DDevice9 *device){
	if(mNeedsCompile){
		compile();
	}

	bool result=false;
	#if defined(TOADLET_HAS_DIRECT3DMOBILE)
		if(mSurfaces.size()>=2 && mSurfaces[0]!=NULL && mSurfaces[1]!=NULL){
			D3D9Surface *renderSurface=(D3D9Surface*)mSurfaces[0]->getRootSurface();
			D3D9Surface *depthSurface=(D3D9Surface*)mSurfaces[1]->getRootSurface();
			device->SetRenderTarget(renderSurface->getSurface(),depthSurface->getSurface());
			result=true;
		}
	#else
		int i;
		for(i=0;i<mSurfaces.size();++i){
			D3D9Surface *surface=(D3D9Surface*)mSurfaces[i]->getRootSurface();
			Attachment attachment=mSurfaceAttachments[i];
			if(attachment==Attachment_DEPTH_STENCIL){
				device->SetDepthStencilSurface(surface->getSurface());
			}
			else{
				device->SetRenderTarget(0,surface->getSurface());
			}
		}
		result=true;
	#endif
	return result;
}

bool D3D9SurfaceRenderTarget::attach(Surface::ptr surface,Attachment attachment){
	#if defined(TOADLET_HAS_DIRECT3DMOBILE)
		mSurfaces.resize(2);
		mSurfaceAttachments.resize(2);
		if(attachment==Attachment_DEPTH_STENCIL){
			mSurfaces.setAt(1,surface);
			mSurfaceAttachments.setAt(1,attachment);
		}
		else{
			mSurfaces.setAt(0,surface);
			mSurfaceAttachments.setAt(0,attachment);
		}
	#else
		mSurfaces.add(surface);
		mSurfaceAttachments.add(attachment);
	#endif

	mWidth=surface->getWidth();
	mHeight=surface->getHeight();
	mNeedsCompile=true;

	return true;
}

bool D3D9SurfaceRenderTarget::remove(Surface::ptr surface){
	int i;
	for(i=0;i<mSurfaces.size();++i){
		if(mSurfaces[i]==surface){
			break;
		}
	}
	if(i==mSurfaces.size()){
		return false;
	}

	mSurfaces.removeAt(i);
	mSurfaceAttachments.removeAt(i);
	mNeedsCompile=true;

	return true;
}

bool D3D9SurfaceRenderTarget::compile(){
	Surface::ptr depth;
	Surface::ptr color;

	int i;
	for(i=0;i<mSurfaceAttachments.size();++i){
		if(mSurfaceAttachments[i]==Attachment_DEPTH_STENCIL){
			depth=mSurfaces[i];
		}
		else{
			color=mSurfaces[i];
		}
	}

	if(color!=NULL && depth==NULL){
		// No Depth-Stencil surface, so add one
		attach(createBufferSurface(Texture::Format_DEPTH_16,mWidth,mHeight),Attachment_DEPTH_STENCIL);
	}

	mNeedsCompile=false;

	return true;
}

Surface::ptr D3D9SurfaceRenderTarget::createBufferSurface(int format,int width,int height){
	IDirect3DSurface9 *d3dsurface=NULL;

	D3DFORMAT d3dformat=D3D9Renderer::getD3DFORMAT(format);

	HRESULT result;
	if((format&Texture::Format_BIT_DEPTH)>0){
		#if defined(TOADLET_HAS_DIRECT3DMOBILE)
			result=mRenderer->getDirect3DDevice9()->CreateDepthStencilSurface(width,height,d3dformat,D3DMULTISAMPLE_NONE,&d3dsurface TOADLET_SHAREDHANDLE);
		#else
			result=mRenderer->getDirect3DDevice9()->CreateDepthStencilSurface(width,height,d3dformat,D3DMULTISAMPLE_NONE,NULL,FALSE,&d3dsurface TOADLET_SHAREDHANDLE);
		#endif
		if(FAILED(result)){
			TOADLET_CHECK_D3D9ERROR(result,"CreateDepthStencilSurface");
			return NULL;
		}
	}
	else{
		#if defined(TOADLET_HAS_DIRECT3DMOBILE)
			result=mRenderer->getDirect3DDevice9()->CreateRenderTarget(width,height,d3dformat,D3DMULTISAMPLE_NONE,FALSE,&d3dsurface TOADLET_SHAREDHANDLE);
		#else
			result=mRenderer->getDirect3DDevice9()->CreateRenderTarget(width,height,d3dformat,D3DMULTISAMPLE_NONE,NULL,FALSE,&d3dsurface TOADLET_SHAREDHANDLE);
		#endif
		if(FAILED(result)){
			TOADLET_CHECK_D3D9ERROR(result,"CreateRenderTarget");
			return NULL;
		}
	}

	D3D9Surface::ptr surface(new D3D9Surface(d3dsurface));
	mOwnedSurfaces.add(surface);
	return surface;
}

}
}
