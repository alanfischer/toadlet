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
#include "D3D9TextureMipPixelBuffer.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>

using namespace toadlet::egg;
using namespace toadlet::egg::image;

namespace toadlet{
namespace peeper{

/// @todo: Add a dynamic usage flag, and also use D3DLOCK_DISCARD when using that
D3D9Texture::D3D9Texture(D3D9Renderer *renderer):BaseResource(),
	mRenderer(NULL),

	mUsage(0),
	mDimension(Dimension_UNKNOWN),
	mFormat(0),
	mWidth(0),
	mHeight(0),
	mDepth(0),
	mMipLevels(0),

	mInternalFormat(0),
	mD3DFormat(D3DFMT_X8R8G8B8),
	mD3DUsage(0),
	mD3DPool(D3DPOOL_MANAGED),
	mTexture(NULL),
	mManuallyGenerateMipLevels(false)
	//mBuffers
	//mBackingBuffer
{
	mRenderer=renderer;
}

D3D9Texture::~D3D9Texture(){
	if(mTexture!=NULL){
		destroy();
	}
}

bool D3D9Texture::create(int usage,Dimension dimension,int format,int width,int height,int depth,int mipLevels,byte *mipDatas[]){
	destroy();

	if((Math::isPowerOf2(width)==false || Math::isPowerOf2(height)==false || (dimension!=Dimension_CUBE && Math::isPowerOf2(depth)==false)) &&
		mRenderer->getCapabilityState().textureNonPowerOf2==false &&
		(mRenderer->getCapabilityState().textureNonPowerOf2Restricted==false || (usage&Usage_BIT_NPOT_RESTRICTED)==0))
	{
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D9Texture: Cannot load a non power of 2 texture");
		return false;
	}

	int closestFormat=mRenderer->getClosestTextureFormat(format);
	if(format!=closestFormat){
		if(mRenderer->getStrictFormats()){
			Error::unknown(Categories::TOADLET_PEEPER,
				"D3D9Texture: Invalid format");
			return false;
		}
		else{
			format=closestFormat;
		}
	}

	mUsage=usage;
	mDimension=dimension;
	mFormat=format;
	mWidth=width;
	mHeight=height;
	mDepth=depth;
	mMipLevels=mipLevels;

	createContext(false);

	int specifiedMipLevels=mMipLevels>0?mMipLevels:1;
	if(mipDatas!=NULL){
		int level;
		for(level=0;level<specifiedMipLevels;++level){
			load(width,height,depth,level,mipDatas[level]);
			width/=2;height/=2;depth/=2;
		}
	}

	return mTexture!=NULL;
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
	IDirect3DDevice9 *device=mRenderer->getDirect3DDevice9();

	/// @todo: Should we remove mInternalFormat and instead just change mFormat?
	///  Seems somewhat pointless to already change mFormat in create (on non strict)
	///  And then to change it again here.
	mInternalFormat=mRenderer->getClosestTextureFormat(mFormat);
	mD3DFormat=D3D9Renderer::getD3DFORMAT(mInternalFormat);
	if(!mRenderer->isD3DFORMATValid(mD3DFormat,mD3DUsage)){
		Logger::alert("invalid format, using BGRA_8");
		mInternalFormat=Format_BGRA_8;
		mD3DFormat=D3DFMT_X8R8G8B8;
	}

	mD3DUsage=
		#if defined(TOADLET_SET_D3DM)
			D3DUSAGE_LOCKABLE;
		#else
			0;
		#endif
	if((mUsage&Usage_BIT_RENDERTARGET)>0){
		if((mFormat&Format_BIT_DEPTH)>0){
			mD3DUsage|=D3DUSAGE_DEPTHSTENCIL;
		}
		else{
			mD3DUsage|=D3DUSAGE_RENDERTARGET;
		}
	}
	if((mUsage&Usage_BIT_STREAM)>0){
		mD3DUsage|=D3DUSAGE_DYNAMIC;
	}

	#if defined(TOADLET_SET_D3DM)
		mD3DPool=D3DPOOL_MANAGED;
	#else
		if((mUsage&Usage_BIT_STAGING)>0){
			mD3DPool=D3DPOOL_SYSTEMMEM;
		}
		else if((mUsage&Usage_BIT_RENDERTARGET)>0){
			mD3DPool=D3DPOOL_DEFAULT;
		}
		else{
			mD3DPool=D3DPOOL_MANAGED;
		}
	#endif

	mManuallyGenerateMipLevels=(mUsage&Usage_BIT_AUTOGEN_MIPMAPS)>0;
	#if !defined(TOADLET_SET_D3DM)
		if(mManuallyGenerateMipLevels && mRenderer->isD3DFORMATValid(mD3DFormat,D3DUSAGE_AUTOGENMIPMAP)){
			mD3DUsage|=D3DUSAGE_AUTOGENMIPMAP;
			mManuallyGenerateMipLevels=false;
		}
	#endif

	HRESULT result=E_FAIL;
	switch(mDimension){
		case Texture::Dimension_D1:
		case Texture::Dimension_D2:{
			IDirect3DTexture9 *texture=NULL;
			result=device->CreateTexture(mWidth,mHeight,mMipLevels,mD3DUsage,mD3DFormat,mD3DPool,&texture TOADLET_SHAREDHANDLE);
			mTexture=texture;
		}break;
		#if !defined(TOADLET_SET_D3DM)
			case Texture::Dimension_D3:{
				IDirect3DVolumeTexture9 *texture=NULL;
				result=device->CreateVolumeTexture(mWidth,mHeight,mDepth,mMipLevels,mD3DUsage,mD3DFormat,mD3DPool,&texture TOADLET_SHAREDHANDLE);
				mTexture=texture;
			}break;
			case Texture::Dimension_CUBE:{
				IDirect3DCubeTexture9 *texture=NULL;
				result=device->CreateCubeTexture(mWidth,mMipLevels,mD3DUsage,mD3DFormat,mD3DPool,&texture TOADLET_SHAREDHANDLE);
				mTexture=texture;
			}break;
		#endif
	}

	if(mBackingBuffer!=NULL){
		IDirect3DSurface9 *surface=NULL;
		((IDirect3DTexture9*)mTexture)->GetSurfaceLevel(0,&surface);
		mRenderer->copySurface(surface,mBackingBuffer->getSurface());
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

	return SUCCEEDED(result);
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
		D3D9PixelBuffer::ptr buffer((D3D9PixelBuffer*)(mRenderer->createPixelBuffer()));
		if(buffer->create(Buffer::Usage_BIT_DYNAMIC,Buffer::Access_READ_WRITE,mFormat,mWidth,mHeight,mDepth)){
			IDirect3DSurface9 *surface=NULL;
			((IDirect3DTexture9*)mTexture)->GetSurfaceLevel(0,&surface);
			mRenderer->copySurface(buffer->getSurface(),surface);
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
	if(mDimension==Dimension_CUBE){
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

bool D3D9Texture::load(int width,int height,int depth,int mipLevel,byte *mipData){
	if(mTexture==NULL){
		return false;
	}

	if((mD3DUsage&D3DUSAGE_RENDERTARGET)>0){
		// Can not load to a RenderTarget
		return false;
	}

	width=width>0?width:1;height=height>0?height:1;depth=depth>0?depth:1;

	if(mipLevel==0 && (width!=mWidth || height!=mHeight || depth!=mDepth)){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D9Texture: data of incorrect dimensions");
		return false;
	}

	int format=mFormat;
	int rowPitch=ImageFormatConversion::getRowPitch(format,width);
	int slicePitch=rowPitch*height;

	HRESULT result=S_OK;
	if(mDimension==Texture::Dimension_D1 || mDimension==Texture::Dimension_D2){
		IDirect3DTexture9 *texture=(IDirect3DTexture9*)mTexture;

		D3DLOCKED_RECT rect={0};
		result=texture->LockRect(mipLevel,&rect,NULL,0);
		TOADLET_CHECK_D3D9ERROR(result,"LockRect");
		if(SUCCEEDED(result)){
			ImageFormatConversion::convert(mipData,format,rowPitch,slicePitch,(uint8*)rect.pBits,mInternalFormat,rect.Pitch,rect.Pitch*height,width,height,depth);
			texture->UnlockRect(mipLevel);
		}
	}
	#if !defined(TOADLET_SET_D3DM)
		else if(mDimension==Texture::Dimension_D3){
			IDirect3DVolumeTexture9 *texture=(IDirect3DVolumeTexture9*)mTexture;

			D3DLOCKED_BOX box={0};
			result=texture->LockBox(mipLevel,&box,NULL,0);
			TOADLET_CHECK_D3D9ERROR(result,"LockBox");
			if(SUCCEEDED(result)){
				ImageFormatConversion::convert(mipData,format,rowPitch,slicePitch,(uint8*)box.pBits,mInternalFormat,box.RowPitch,box.SlicePitch,width,height,depth);
				texture->UnlockBox(mipLevel);
			}
		}
		else if(mDimension==Texture::Dimension_CUBE){
			IDirect3DCubeTexture9 *texture=(IDirect3DCubeTexture9*)mTexture;

			int i;
			for(i=0;i<6;++i){
				D3DLOCKED_RECT rect={0};
				result=texture->LockRect((D3DCUBEMAP_FACES)i,mipLevel,&rect,NULL,0);
				TOADLET_CHECK_D3D9ERROR(result,"LockRect");
				if(SUCCEEDED(result)){
					ImageFormatConversion::convert((mipData+slicePitch*i),format,rowPitch,slicePitch,(uint8*)rect.pBits,mInternalFormat,rect.Pitch,rect.Pitch*height,width,height,1);
					texture->UnlockRect((D3DCUBEMAP_FACES)i,mipLevel);
				}
			}
		}
	#endif
	else{
		Error::unimplemented(Categories::TOADLET_PEEPER,
			"D3D9Texture: unimplemented");
		return false;
	}

	if(mManuallyGenerateMipLevels){
		#if !defined(TOADLET_SET_D3DM)
			mTexture->GenerateMipSubLevels();
		#endif
	}

	return true;
}

bool D3D9Texture::read(int width,int height,int depth,int mipLevel,byte *mipData){
	if(mTexture==NULL){
		return false;
	}

	width=width>0?width:1;height=height>0?height:1;depth=depth>0?depth:1;

	if(mipLevel==0 && (width!=mWidth || height!=mHeight || depth!=mDepth)){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D9Texture: data of incorrect dimensions");
		return false;
	}

	int format=mFormat;
	int rowPitch=ImageFormatConversion::getRowPitch(format,width);
	int slicePitch=rowPitch*height;

	HRESULT result=S_OK;
	if(mDimension==Texture::Dimension_D1 || mDimension==Texture::Dimension_D2){
		IDirect3DTexture9 *texture=(IDirect3DTexture9*)mTexture;

		D3DLOCKED_RECT rect={0};
		result=texture->LockRect(mipLevel,&rect,NULL,D3DLOCK_READONLY);
		TOADLET_CHECK_D3D9ERROR(result,"LockRect");
		if(SUCCEEDED(result)){
			ImageFormatConversion::convert((uint8*)rect.pBits,mInternalFormat,rect.Pitch,rect.Pitch*height,mipData,format,rowPitch,slicePitch,width,height,depth);
			texture->UnlockRect(mipLevel);
		}
	}
	#if !defined(TOADLET_SET_D3DM)
		else if(mDimension==Texture::Dimension_D3){
			if(mD3DPool==D3DPOOL_DEFAULT){
				return false;
			}

			IDirect3DVolumeTexture9 *texture=(IDirect3DVolumeTexture9*)mTexture;

			D3DLOCKED_BOX box={0};
			result=texture->LockBox(mipLevel,&box,NULL,D3DLOCK_READONLY);
			TOADLET_CHECK_D3D9ERROR(result,"LockBox");
			if(SUCCEEDED(result)){
				ImageFormatConversion::convert((uint8*)box.pBits,mInternalFormat,box.RowPitch,box.SlicePitch,mipData,format,rowPitch,slicePitch,width,height,depth);
				texture->UnlockBox(mipLevel);
			}
		}
	#endif
	else{
		Error::unimplemented(Categories::TOADLET_PEEPER,
			"D3D9Texture: unimplemented");
		return false;
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

}
}
