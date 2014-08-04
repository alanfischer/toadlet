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

#include <toadlet/tadpole/Engine.h>
#include "PointTextureCreator.h"

namespace toadlet{
namespace tadpole{

bool PointTextureCreator::createPointTexture(TextureFormat *format,tbyte *data,int xoffset,int yoffset,int width,int height,float colorOffset,float colorFactor,float alphaOffset,float alphaFactor,float falloff){
	int px=xoffset + width/2,py=yoffset + height/2;
	int twidth=format->getWidth(),theight=format->getHeight(),txpitch=format->getXPitch();
	int hwidth=width/2;
	int pixelFormat=format->getPixelFormat();
	int ps=TextureFormat::getPixelSize(pixelFormat);
	int cs=TextureFormat::getColorBits(pixelFormat)/8;
	int as=TextureFormat::getAlphaBits(pixelFormat)/8;

	if(ps<=0 || (cs<=0 && as<=0)){
		return false;
	}

	int x,y;
	for(y=0;y<height;y++){
		for(x=0;x<width;x++){
			int tx=Math::intClamp(0,twidth-1,x + xoffset);
			int ty=Math::intClamp(0,theight-1,y + yoffset);

			scalar v=Math::length(Vector2(Math::fromInt(tx-px),Math::fromInt(ty-py)));
			v=Math::div(v,Math::fromInt(hwidth));
			v=Math::clamp(0,Math::ONE,v);

			float fv=1.0f - pow(v,Math::toFloat(falloff));
			float c=colorFactor * fv + colorOffset;
			float a=alphaFactor * fv + alphaOffset;

			int dataOffset=ty*txpitch + tx*ps;
			if(cs>0){
				for(int i=0;i<cs;++i){
					data[dataOffset+i] = Math::intClamp(0,255,data[dataOffset+i] + c * 255);
				}
			}
			if(as>0){
				for(int i=0;i<as;++i){
					data[dataOffset+cs+i] = Math::intClamp(0,255,data[dataOffset+cs+i] + a * 255);
				}
			}
		}
	}

	return true;
}

Texture::ptr PointTextureCreator::createPointTexture(TextureFormat *format,float colorOffset,float colorFactor,float alphaOffset,float alphaFactor,float falloff){
	tbyte *data=new tbyte[format->getDataSize()];
	memset(data,0,format->getDataSize());
	bool result=createPointTexture(format,data,0,0,format->getWidth(),format->getHeight(),colorOffset,colorFactor,alphaOffset,alphaFactor,falloff);
	if(result==false){
		delete[] data;
		Error::unknown(Categories::TOADLET_TADPOLE,"unable to createPointTexture");
		return NULL;
	}

	Texture::ptr texture=mEngine->getTextureManager()->createTexture(format,data);
	delete[] data;
	return texture;
}

}
}
