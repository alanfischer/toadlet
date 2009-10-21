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
#include <toadlet/egg/Error.h>

using namespace toadlet::egg;
using namespace toadlet::egg::image;

namespace toadlet{
namespace peeper{

D3DMTexturePeer::D3DMTexturePeer(D3DMRenderer *renderer,Texture *texture):
	baseTexture(NULL)
{
	const CapabilitySet &capabilitySet=renderer->getCapabilitySet();
	IDirect3DMobileDevice *device=renderer->getDirect3DMobileDevice();

	int width=texture->getWidth();
	int height=texture->getHeight();
	int depth=texture->getDepth();
	Texture::Dimension dimension=texture->getDimension();
	int textureFormat=texture->getFormat();
	D3DMFORMAT format=getD3DFORMAT(textureFormat);

	if(dimension==Texture::Dimension_D3 || dimension==Texture::Dimension_CUBEMAP){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3DMTexturePeer: Cannot load a 3d or cubemap texture");
		return;
	}

	if(	(Math::isPowerOf2(width)==false || Math::isPowerOf2(height)==false) &&
		capabilitySet.textureNonPowerOf2==false &&
		capabilitySet.textureNonPowerOf2Restricted==false
	){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3DMTexturePeer: Cannot load a non power of 2 texture");
		return;
	}

	Image *image=texture->getImage();
	if(image!=NULL){
		HRESULT result=E_FAIL;

		UINT levels=texture->getAutoGenerateMipMaps()?0:1;
		DWORD usage=D3DMUSAGE_LOCKABLE;
		D3DMPOOL pool=D3DMPOOL_MANAGED;
		{
			IDirect3DMobileTexture *texture=NULL;
			result=device->CreateTexture(width,height,levels,usage,format,pool,&texture);
			baseTexture=texture;
		}
		if(FAILED(result)){
			TOADLET_CHECK_D3DMERROR(result,"CreateTexture");
			return;
		}

		{
			IDirect3DMobileTexture *texture=(IDirect3DMobileTexture*)baseTexture;
			D3DMLOCKED_RECT rect={0};
			result=texture->LockRect(0,&rect,NULL,D3DMLOCK_DISCARD);
			if(FAILED(result)){
				TOADLET_CHECK_D3DMERROR(result,"LockRect");
				return;
			}

			int pixelSize=image->getPixelSize();
			unsigned char *dst=(unsigned char*)rect.pBits;
			unsigned char *src=(unsigned char*)image->getData();

			int i,j;
			if(textureFormat==Texture::Format_RGBA_8 && format==D3DMFMT_A8R8G8B8){
				for(i=0;i<height;++i){
					for(j=0;j<width;++j){
						*(uint32*)(dst+rect.Pitch*i+j*4)=RGBA8toA8R8G8B8(*(uint32*)(src+width*pixelSize*i+j*pixelSize));
					}
				}
			}
			else if(textureFormat==Texture::Format_L_8 && format==D3DMFMT_R8G8B8){
				for(i=0;i<height;++i){
					for(j=0;j<width;++j){
						*(uint16*)(dst+rect.Pitch*i+j*2)=L8toR8G8B8(*(uint8*)(src+width*pixelSize*i+j*pixelSize));
					}
				}
			}
			else if(textureFormat==Texture::Format_A_8 && format==D3DMFMT_A8R8G8B8){
				for(i=0;i<height;++i){
					for(j=0;j<width;++j){
						*(uint16*)(dst+rect.Pitch*i+j*2)=A8toA8R8G8B8(*(uint8*)(src+width*pixelSize*i+j*pixelSize));
					}
				}
			}
			else if(textureFormat==Texture::Format_LA_8 && format==D3DMFMT_A8R8G8B8){
				for(i=0;i<height;++i){
					for(j=0;j<width;++j){
						*(uint16*)(dst+rect.Pitch*i+j*2)=LA8toA8R8G8B8(*(uint16*)(src+width*pixelSize*i+j*pixelSize));
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

D3DMTexturePeer::~D3DMTexturePeer(){
	if(baseTexture!=NULL){
		baseTexture->Release();
		baseTexture=NULL;
	}
}

D3DMFORMAT D3DMTexturePeer::getD3DFORMAT(int textureFormat){
	D3DMFORMAT format=D3DMFMT_UNKNOWN;

	if(textureFormat==Texture::Format_L_8){
		format=D3DMFMT_R5G6B5;
	}
	else if(textureFormat==Texture::Format_A_8){
		format=D3DMFMT_A8R8G8B8;
	}
	else if(textureFormat==Texture::Format_LA_8){
		format=D3DMFMT_A8R8G8B8;
	}
	else if(textureFormat==Texture::Format_RGB_8){
		format=D3DMFMT_R8G8B8;
	}
	else if(textureFormat==Texture::Format_RGBA_8){
		format=D3DMFMT_A8R8G8B8;
	}
	else if(textureFormat==Texture::Format_RGB_5_6_5){
		format=D3DMFMT_R5G6B5;
	}
	else if(textureFormat==Texture::Format_RGBA_5_5_5_1){
		format=D3DMFMT_A1R5G5B5;
	}
	else if(textureFormat==Texture::Format_RGBA_4_4_4_4){
		format=D3DMFMT_A4R4G4B4;
	}

	if(format==D3DMFMT_UNKNOWN){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3DMTexturePeer::getD3DFORMAT: Invalid type");
	}

	return format;
}

DWORD D3DMTexturePeer::getD3DTADDRESS(Texture::AddressMode addressMode){
	DWORD taddress=0;

	switch(addressMode){
		case Texture::AddressMode_REPEAT:
			taddress=D3DMTADDRESS_WRAP;
		break;
		case Texture::AddressMode_CLAMP_TO_EDGE:
			taddress=D3DMTADDRESS_CLAMP;
		break;
		case Texture::AddressMode_CLAMP_TO_BORDER:
			taddress=D3DMTADDRESS_BORDER;
		break;
		case Texture::AddressMode_MIRRORED_REPEAT:
			taddress=D3DMTADDRESS_MIRROR;
		break;
	}

	if(taddress==0){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3DMTexturePeer::getD3DTADDRESS: Invalid address mode");
	}

	return taddress;
}

DWORD D3DMTexturePeer::getD3DTEXF(Texture::Filter filter){
	DWORD texf=D3DMTEXF_NONE;

	switch(filter){
		case Texture::Filter_NONE:
			texf=D3DMTEXF_NONE;
		break;
		case Texture::Filter_NEAREST:
			texf=D3DMTEXF_POINT;
		break;
		case Texture::Filter_LINEAR:
			texf=D3DMTEXF_LINEAR;
		break;
	}

	return texf;
}

}
}
