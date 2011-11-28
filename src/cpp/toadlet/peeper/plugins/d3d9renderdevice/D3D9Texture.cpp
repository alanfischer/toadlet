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
#include "D3D9Texture.h"
#include "D3D9TextureMipPixelBuffer.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>

namespace toadlet{
namespace peeper{

/// @todo: Add a dynamic usage flag, and also use D3DLOCK_DISCARD when using that
D3D9Texture::D3D9Texture(D3D9RenderDevice *renderDevice):BaseResource(),
	mDevice(NULL),

	mUsage(0),
	//mFormat,

	mD3DFormat(D3DFMT_X8R8G8B8),
	mD3DUsage(0),
	mD3DPool(D3DPOOL_MANAGED),
	mTexture(NULL),
	mManuallyGenerateMipLevels(false)
	//mBuffers
	//mBackingBuffer
{
	mDevice=renderDevice;
}

D3D9Texture::~D3D9Texture(){
	if(mTexture!=NULL){
		destroy();
	}
}

bool D3D9Texture::create(int usage,TextureFormat::ptr format,byte *mipDatas[]){
	if(format->isPowerOf2()==false &&
		mDevice->getCaps().textureNonPowerOf2==false &&
		(mDevice->getCaps().textureNonPowerOf2Restricted==false || (usage&Usage_BIT_NPOT_RESTRICTED)==0))
	{
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D9Texture: Cannot load a non power of 2 texture");
		return false;
	}

	mUsage=usage;
	mFormat=format;

	bool result=createContext(false);

	if(result && mipDatas!=NULL){
		int specifiedMipLevels=mFormat->getMipMax()>0?mFormat->getMipMax():1;
		int level;
		for(level=0;level<specifiedMipLevels;++level){
			TextureFormat::ptr format(new TextureFormat(mFormat,level));
			load(format,mipDatas[level]);
		}
	}

	return result;
}

void D3D9Texture::destroy(){
	destroyContext(false);

	mBuffers.clear();
}

void D3D9Texture::resetCreate(){
	if(needsReset()){
		createContext(true);
	}
}

void D3D9Texture::resetDestroy(){
	if(needsReset()){
		destroyContext(true);
	}
}

bool D3D9Texture::createContext(bool restore){
	IDirect3DDevice9 *device=mDevice->getDirect3DDevice9();

	int width=mFormat->getWidth(),height=mFormat->getHeight(),depth=mFormat->getDepth(),mipMax=mFormat->getMipMax();
	mD3DUsage=mDevice->getD3DUSAGE(mFormat->getPixelFormat(),mUsage);
	mD3DPool=mDevice->getD3DPOOL(mUsage);
	mD3DFormat=mDevice->getD3DFORMAT(mFormat->getPixelFormat());
	mManuallyGenerateMipLevels=(mUsage&Usage_BIT_AUTOGEN_MIPMAPS)>0;
	#if !defined(TOADLET_SET_D3DM)
		if(mManuallyGenerateMipLevels && mDevice->isD3DFORMATValid(mD3DFormat,D3DUSAGE_AUTOGENMIPMAP)){
			mD3DUsage|=D3DUSAGE_AUTOGENMIPMAP;
			mManuallyGenerateMipLevels=false;
		}
	#endif

	HRESULT result=E_FAIL;
	switch(mFormat->getDimension()){
		case TextureFormat::Dimension_D1:
		case TextureFormat::Dimension_D2:{
			IDirect3DTexture9 *texture=NULL;
			result=device->CreateTexture(width,height,mipMax,mD3DUsage,mD3DFormat,mD3DPool,&texture TOADLET_SHAREDHANDLE);
			mTexture=texture;
		}break;
		#if !defined(TOADLET_SET_D3DM)
			case TextureFormat::Dimension_D3:{
				IDirect3DVolumeTexture9 *texture=NULL;
				result=device->CreateVolumeTexture(width,height,depth,mipMax,mD3DUsage,mD3DFormat,mD3DPool,&texture TOADLET_SHAREDHANDLE);
				mTexture=texture;
			}break;
			case TextureFormat::Dimension_CUBE:{
				IDirect3DCubeTexture9 *texture=NULL;
				result=device->CreateCubeTexture(width,mipMax,mD3DUsage,mD3DFormat,mD3DPool,&texture TOADLET_SHAREDHANDLE);
				mTexture=texture;
			}break;
		#endif
	}

	if(mBackingBuffer!=NULL){
		IDirect3DSurface9 *surface=NULL;
		((IDirect3DTexture9*)mTexture)->GetSurfaceLevel(0,&surface);
		mDevice->copySurface(surface,mBackingBuffer->getSurface());
		surface->Release();
		mBackingBuffer->destroy();
		mBackingBuffer=NULL;
	}

	if(restore){
		int i;
		for(i=0;i<mBuffers.size();++i){
			mBuffers[i]->resetCreate();
		}
	}

	TOADLET_CHECK_D3D9ERROR(result,"CreateTexture");

	if(FAILED(result)){
		Error::unknown("error in createContext");
		return false;
	}

	return true;
}

bool D3D9Texture::destroyContext(bool backup){
	int i;
	if(backup){
		for(i=0;i<mBuffers.size();++i){
			mBuffers[i]->resetDestroy();
		}
	}
	else{
		for(i=0;i<mBuffers.size();++i){
			mBuffers[i]->destroy();
		}
	}

	if(backup && (mUsage&Usage_BIT_DYNAMIC)==0){
		D3D9PixelBuffer::ptr buffer((D3D9PixelBuffer*)(mDevice->createPixelBuffer()));
		if(buffer->create(Buffer::Usage_BIT_DYNAMIC,Buffer::Access_READ_WRITE,mFormat)){
			IDirect3DSurface9 *surface=NULL;
			((IDirect3DTexture9*)mTexture)->GetSurfaceLevel(0,&surface);
			mDevice->copySurface(buffer->getSurface(),surface);
			surface->Release();
			mBackingBuffer=buffer;
		}
	}
	else if(mBackingBuffer!=NULL){
		mBackingBuffer->destroy();
		mBackingBuffer=NULL;
	}

	HRESULT result=S_OK;
	if(mTexture!=NULL){
		result=mTexture->Release();
		mTexture=NULL;
	}

	return SUCCEEDED(result);
}

PixelBuffer::ptr D3D9Texture::getMipPixelBuffer(int level,int cubeSide){
	if(mTexture==NULL){
		return NULL;
	}

	int index=level;
	if(mFormat->getDimension()==TextureFormat::Dimension_CUBE){
		index=level*6+cubeSide;
	}

	if(mBuffers.size()<=index){
		mBuffers.resize(index+1);
	}

	if(mBuffers[index]==NULL){
		PixelBuffer::ptr buffer(new D3D9TextureMipPixelBuffer(this,level,cubeSide));
		mBuffers[index]=buffer;
	}

	return mBuffers[index];
}

bool D3D9Texture::load(TextureFormat *format,byte *data){
	TOADLET_ASSERT(format->getMipMin()==format->getMipMax());

	if(mTexture==NULL){
		return false;
	}

	if((mD3DUsage&D3DUSAGE_RENDERTARGET)>0){
		// Can not load to a RenderTarget
		return false;
	}

	int dimension=format->getDimension();
	int xPitch=format->getXPitch(),yPitch=format->getYPitch();
	int mipLevel=format->getMipMin();
	int faces=format->getDimension()==TextureFormat::Dimension_CUBE?6:1;

	int face;
	for(face=0;face<faces;++face){
		HRESULT result=S_OK;
		tbyte *lockData=NULL;
		int lockXPitch=format->getXPitch(),lockYPitch=format->getYPitch();
		if(dimension==TextureFormat::Dimension_D1 || dimension==TextureFormat::Dimension_D2){
			IDirect3DTexture9 *texture=(IDirect3DTexture9*)mTexture;
			RECT rect;
			setRECT(rect,format);
			D3DLOCKED_RECT lockedRect={0};
			result=texture->LockRect(mipLevel,&lockedRect,&rect,0);
			TOADLET_CHECK_D3D9ERROR(result,"LockRect");
			lockData=(tbyte*)lockedRect.pBits;
			lockXPitch=lockedRect.Pitch;
		}
		#if !defined(TOADLET_SET_D3DM)
			else if(dimension==TextureFormat::Dimension_D3){
				IDirect3DVolumeTexture9 *texture=(IDirect3DVolumeTexture9*)mTexture;
				D3DBOX box;
				setD3DBOX(box,format);
				D3DLOCKED_BOX lockedBox={0};
				result=texture->LockBox(mipLevel,&lockedBox,&box,0);
				TOADLET_CHECK_D3D9ERROR(result,"LockBox");
				lockData=(tbyte*)lockedBox.pBits;
				lockXPitch=lockedBox.RowPitch;
				lockYPitch=lockedBox.SlicePitch;
			}
			else if(dimension==TextureFormat::Dimension_CUBE){
				IDirect3DCubeTexture9 *texture=(IDirect3DCubeTexture9*)mTexture;
				RECT rect;
				setRECT(rect,format);
				D3DLOCKED_RECT lockedRect={0};
				result=texture->LockRect((D3DCUBEMAP_FACES)face,mipLevel,&lockedRect,&rect,0);
				TOADLET_CHECK_D3D9ERROR(result,"LockRect");
				lockData=(tbyte*)lockedRect.pBits;
				lockXPitch=lockedRect.Pitch;
			}
		#endif

		if(SUCCEEDED(result)){
			int i,j;
			for(j=0;j<format->getDepth();++j){
				for(i=0;i<format->getHeight();++i){
					memcpy(lockData+lockYPitch*j+lockXPitch*i,data+yPitch*j+xPitch*i,xPitch);
				}
			}

			if(dimension==TextureFormat::Dimension_D1 || dimension==TextureFormat::Dimension_D2){
				IDirect3DTexture9 *texture=(IDirect3DTexture9*)mTexture;
				texture->UnlockRect(mipLevel);
			}
			#if !defined(TOADLET_SET_D3DM)
				else if(dimension==TextureFormat::Dimension_D3){
					IDirect3DVolumeTexture9 *texture=(IDirect3DVolumeTexture9*)mTexture;
					texture->UnlockBox(mipLevel);
				}
				else if(dimension==TextureFormat::Dimension_CUBE){
					IDirect3DCubeTexture9 *texture=(IDirect3DCubeTexture9*)mTexture;
					texture->UnlockRect((D3DCUBEMAP_FACES)face,mipLevel);
				}
			#endif
		}
	}

	if(mManuallyGenerateMipLevels){
		#if !defined(TOADLET_SET_D3DM)
			mTexture->GenerateMipSubLevels();
		#endif
	}

	return true;
}

bool D3D9Texture::read(TextureFormat *format,byte *data){
	TOADLET_ASSERT(format->getMipMin()==format->getMipMax());

	if(mTexture==NULL){
		return false;
	}

	int dimension=format->getDimension();
	int xPitch=format->getXPitch(),yPitch=format->getYPitch();
	int mipLevel=format->getMipMin();
	int faces=format->getDimension()==TextureFormat::Dimension_CUBE?6:1;

	HRESULT result=S_OK;
	int face;
	for(face=0;face<faces;++face){
		tbyte *lockData=NULL;
		int lockXPitch=format->getXPitch(),lockYPitch=format->getYPitch();
		if(dimension==TextureFormat::Dimension_D1 || dimension==TextureFormat::Dimension_D2){
			IDirect3DTexture9 *texture=(IDirect3DTexture9*)mTexture;
			RECT rect;
			setRECT(rect,format);
			D3DLOCKED_RECT lockedRect={0};
			result=texture->LockRect(mipLevel,&lockedRect,&rect,0);
			TOADLET_CHECK_D3D9ERROR(result,"LockRect");
			lockData=(tbyte*)lockedRect.pBits;
			lockXPitch=lockedRect.Pitch;
		}
		#if !defined(TOADLET_SET_D3DM)
			else if(dimension==TextureFormat::Dimension_D3){
				IDirect3DVolumeTexture9 *texture=(IDirect3DVolumeTexture9*)mTexture;
				D3DBOX box;
				setD3DBOX(box,format);
				D3DLOCKED_BOX lockedBox={0};
				result=texture->LockBox(mipLevel,&lockedBox,&box,0);
				TOADLET_CHECK_D3D9ERROR(result,"LockBox");
				lockData=(tbyte*)lockedBox.pBits;
				lockXPitch=lockedBox.RowPitch;
				lockYPitch=lockedBox.SlicePitch;
			}
			else if(dimension==TextureFormat::Dimension_CUBE){
				IDirect3DCubeTexture9 *texture=(IDirect3DCubeTexture9*)mTexture;
				RECT rect;
				setRECT(rect,format);
				D3DLOCKED_RECT lockedRect={0};
				result=texture->LockRect((D3DCUBEMAP_FACES)face,mipLevel,&lockedRect,&rect,0);
				TOADLET_CHECK_D3D9ERROR(result,"LockRect");
				lockData=(tbyte*)lockedRect.pBits;
				lockXPitch=lockedRect.Pitch;
			}
		#endif

		if(SUCCEEDED(result)){
			int i,j;
			for(j=0;j<format->getDepth();++j){
				for(i=0;i<format->getHeight();++i){
					memcpy(data+yPitch*j+xPitch*i,lockData+lockYPitch*j+lockXPitch*i,xPitch);
				}
			}

			if(dimension==TextureFormat::Dimension_D1 || dimension==TextureFormat::Dimension_D2){
				IDirect3DTexture9 *texture=(IDirect3DTexture9*)mTexture;
				texture->UnlockRect(mipLevel);
			}
			#if !defined(TOADLET_SET_D3DM)
				else if(dimension==TextureFormat::Dimension_D3){
					IDirect3DVolumeTexture9 *texture=(IDirect3DVolumeTexture9*)mTexture;
					texture->UnlockBox(mipLevel);
				}
				else if(dimension==TextureFormat::Dimension_CUBE){
					IDirect3DCubeTexture9 *texture=(IDirect3DCubeTexture9*)mTexture;
					texture->UnlockRect((D3DCUBEMAP_FACES)face,mipLevel);
				}
			#endif
		}
	}

	return SUCCEEDED(result);
}

bool D3D9Texture::needsReset(){
	#if defined(TOADLET_SET_D3DM)
		return false;
	#else
		return mD3DPool==D3DPOOL_DEFAULT;
	#endif
}

void D3D9Texture::setRECT(RECT &rect,TextureFormat *format){
	rect.left=format->getXMin();rect.right=format->getXMax();
	rect.top=format->getYMin();rect.bottom=format->getYMax();
}

void D3D9Texture::setD3DBOX(D3DBOX &box,TextureFormat *format){
	box.Left=format->getXMin();box.Right=format->getXMax();
	box.Top=format->getYMin();box.Bottom=format->getYMax();
	box.Front=format->getZMin();box.Back=format->getZMax();
}

}
}
