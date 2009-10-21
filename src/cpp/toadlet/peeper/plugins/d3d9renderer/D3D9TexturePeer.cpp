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
#include "D3D9TexturePeer.h"
#include <toadlet/egg/Error.h>

using namespace toadlet::egg;
using namespace toadlet::egg::image;

namespace toadlet{
namespace peeper{

D3D9TexturePeer::D3D9TexturePeer(D3D9Renderer *renderer,Texture *texture):
	baseTexture(NULL)
{
	const CapabilitySet &capabilitySet=renderer->getCapabilitySet();
	IDirect3DDevice9 *device=renderer->getDirect3DDevice9();
	IDirect3D9 *d3d=NULL; device->GetDirect3D(&d3d);

	int width=texture->getWidth();
	int height=texture->getHeight();
	int depth=texture->getDepth();
	Texture::Dimension dimension=texture->getDimension();
	int textureFormat=texture->getFormat();
	D3DFORMAT d3dformat=getD3DFORMAT(textureFormat);
	if(!isD3DFORMATValid(d3d,d3dformat,D3DFMT_X8R8G8B8)){
		d3dformat=D3DFMT_X8R8G8B8;
	}

	if(	(math::Math::isPowerOf2(width)==false || math::Math::isPowerOf2(height)==false) &&
		capabilitySet.textureNonPowerOf2==false &&
		capabilitySet.textureNonPowerOf2Restricted==false
	){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D9TexturePeer: Cannot load a non power of 2 texture");
		return;
	}

	Image *image=texture->getImage();
	if(image!=NULL){
		HRESULT result=E_FAIL;
		UINT levels=texture->getAutoGenerateMipMaps()?0:1;
		DWORD usage=0;
		D3DPOOL pool=D3DPOOL_MANAGED;
		switch(dimension){
			case Texture::Dimension_D1:
			case Texture::Dimension_D2:{
				IDirect3DTexture9 *texture=NULL;
				result=device->CreateTexture(width,height,levels,usage,d3dformat,pool,&texture,NULL);
				baseTexture=texture;
			}break;
			case Texture::Dimension_D3:{
				IDirect3DVolumeTexture9 *texture=NULL;
				result=device->CreateVolumeTexture(width,height,depth,levels,usage,d3dformat,pool,&texture,NULL);
				baseTexture=texture;
			}break;
			case Texture::Dimension_CUBEMAP:{
				IDirect3DCubeTexture9 *texture=NULL;
				result=device->CreateCubeTexture(width,levels,usage,d3dformat,pool,&texture,NULL);
				baseTexture=texture;
			}break;
		}

		if(FAILED(result)){
			TOADLET_CHECK_D3D9ERROR(result,"CreateTexture");
			return;
		}

		if(dimension==Texture::Dimension_D1 || dimension==Texture::Dimension_D2){
			IDirect3DTexture9 *texture=(IDirect3DTexture9*)baseTexture;

			D3DLOCKED_RECT rect={0};
			result=texture->LockRect(0,&rect,NULL,D3DLOCK_DISCARD);
			if(FAILED(result)){
				TOADLET_CHECK_D3D9ERROR(result,"LockRect");
				return;
			}

			int pixelSize=image->getPixelSize();
			unsigned char *dst=(unsigned char*)rect.pBits;
			unsigned char *src=(unsigned char*)image->getData();

			int i,j;
			if(textureFormat==Texture::Format_A_8 && d3dformat==D3DFMT_A8L8){
				for(i=0;i<height;++i){
					for(j=0;j<width;++j){
						*(uint16*)(dst+rect.Pitch*i+j*2)=A8toA8L8(*(uint8*)(src+width*pixelSize*i+j*1));
					}
				}
			}
			else if(textureFormat==Texture::Format_RGBA_8 && d3dformat==D3DFMT_A8R8G8B8){
				for(i=0;i<height;++i){
					for(j=0;j<width;++j){
						*(uint32*)(dst+rect.Pitch*i+j*4)=RGBA8toA8R8G8B8(*(uint32*)(src+width*pixelSize*i+j*pixelSize));
					}
				}
			}
			else if(textureFormat==Texture::Format_RGB_8 && d3dformat==D3DFMT_X8R8G8B8){
				for(i=0;i<height;++i){
					for(j=0;j<width;++j){
						*(uint32*)(dst+rect.Pitch*i+j*4)=RGB8toX8R8G8B8(src+width*pixelSize*i+j*pixelSize);
					}
				}
			}
			else{
				for(i=0;i<height;++i){
					memcpy(dst+rect.Pitch*i,src+width*pixelSize*i,width*pixelSize);
				}
			}

			texture->UnlockRect(0);
		}
	}
}

D3D9TexturePeer::~D3D9TexturePeer(){
	if(baseTexture!=NULL){
		baseTexture->Release();
		baseTexture=NULL;
	}
}

bool D3D9TexturePeer::isD3DFORMATValid(IDirect3D9 *d3d,D3DFORMAT textureFormat,D3DFORMAT adapterFormat){
	return SUCCEEDED(d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,adapterFormat,0,D3DRTYPE_TEXTURE,textureFormat));
}

D3DFORMAT D3D9TexturePeer::getD3DFORMAT(int textureFormat){
	D3DFORMAT format=D3DFMT_UNKNOWN;

	if(textureFormat==Texture::Format_L_8){
		format=D3DFMT_L8;
	}
	else if(textureFormat==Texture::Format_A_8){
		format=D3DFMT_A8L8; // Use A8L8 and fill L8 with white
	}
	else if(textureFormat==Texture::Format_LA_8){
		format=D3DFMT_A8L8;
	}
	else if(textureFormat==Texture::Format_RGB_8){
		format=D3DFMT_R8G8B8;
	}
	else if(textureFormat==Texture::Format_RGBA_8){
		format=D3DFMT_A8R8G8B8;
	}
	else if(textureFormat==Texture::Format_RGB_5_6_5){
		format=D3DFMT_R5G6B5;
	}
	else if(textureFormat==Texture::Format_RGBA_5_5_5_1){
		format=D3DFMT_A1R5G5B5;
	}
	else if(textureFormat==Texture::Format_RGBA_4_4_4_4){
		format=D3DFMT_A4R4G4B4;
	}

	if(format==D3DFMT_UNKNOWN){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D9TexturePeer::getD3DFORMAT: Invalid type");
	}

	return format;
}

DWORD D3D9TexturePeer::getD3DTADDRESS(Texture::AddressMode addressMode){
	DWORD taddress=0;

	switch(addressMode){
		case Texture::AddressMode_REPEAT:
			taddress=D3DTADDRESS_WRAP;
		break;
		case Texture::AddressMode_CLAMP_TO_EDGE:
			taddress=D3DTADDRESS_CLAMP;
		break;
		case Texture::AddressMode_CLAMP_TO_BORDER:
			taddress=D3DTADDRESS_BORDER;
		break;
		case Texture::AddressMode_MIRRORED_REPEAT:
			taddress=D3DTADDRESS_MIRROR;
		break;
	}

	if(taddress==0){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D9TexturePeer::getD3DTADDRESS: Invalid address mode");
	}

	return taddress;
}

DWORD D3D9TexturePeer::getD3DTEXF(Texture::Filter filter){
	DWORD texf=D3DTEXF_NONE;

	switch(filter){
		case Texture::Filter_NONE:
			texf=D3DTEXF_NONE;
		break;
		case Texture::Filter_NEAREST:
			texf=D3DTEXF_POINT;
		break;
		case Texture::Filter_LINEAR:
			texf=D3DTEXF_LINEAR;
		break;
	}

	return texf;
}

}
}
