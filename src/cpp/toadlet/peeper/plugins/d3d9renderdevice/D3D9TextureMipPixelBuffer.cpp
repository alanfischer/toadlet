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

#include "D3D9TextureMipPixelBuffer.h"
#include "D3D9RenderDevice.h"
#include <toadlet/egg/Error.h>

namespace toadlet{
namespace peeper{

D3D9TextureMipPixelBuffer::D3D9TextureMipPixelBuffer(D3D9Texture *texture,int level,int cubeSide):D3D9PixelBuffer(texture->mDevice,false),
	mTexture(NULL),
	mLevel(0),
	mCubeSide(0)
{
	mTexture=texture;
	mLevel=level;
	mCubeSide=cubeSide;
	
	resetCreate();
}

D3D9TextureMipPixelBuffer::~D3D9TextureMipPixelBuffer(){
	destroy();
}

bool D3D9TextureMipPixelBuffer::create(int usage,int access,TextureFormat::ptr format){
	return false;
}

void D3D9TextureMipPixelBuffer::destroy(){
	resetDestroy();
}

void D3D9TextureMipPixelBuffer::resetCreate(){
	IDirect3DBaseTexture9 *d3dtexture=mTexture->mTexture;

	switch(mTexture->mFormat->dimension){
		case TextureFormat::Dimension_D1:
		case TextureFormat::Dimension_D2:
			((IDirect3DTexture9*)d3dtexture)->GetSurfaceLevel(mLevel,&mSurface);
		break;
		#if !defined(TOADLET_SET_D3DM)
			case TextureFormat::Dimension_D3:
				((IDirect3DVolumeTexture9*)d3dtexture)->GetVolumeLevel(mLevel,&mVolume);
			break;
			case TextureFormat::Dimension_CUBE:
				((IDirect3DCubeTexture9*)d3dtexture)->GetCubeMapSurface((D3DCUBEMAP_FACES)mCubeSide,mLevel,&mSurface);
			break;
		#endif
	}

	mFormat=TextureFormat::ptr(new TextureFormat(mTexture->mFormat));
	
	if(mSurface!=NULL){
		D3DSURFACE_DESC desc;
		mSurface->GetDesc(&desc);
		mFormat->width=desc.Width;
		mFormat->height=desc.Height;
		mFormat->depth=1;
	}
	else if(mVolume!=NULL){
		D3DVOLUME_DESC desc;
		mVolume->GetDesc(&desc);
		mFormat->width=desc.Width;
		mFormat->height=desc.Height;
		mFormat->depth=desc.Depth;
	}
}

void D3D9TextureMipPixelBuffer::resetDestroy(){
	if(mSurface!=NULL){
		mSurface->Release();
		mSurface=NULL;
	}
	else if(mVolume!=NULL){
		mVolume->Release();
		mVolume=NULL;
	}
}

}
}
