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

#include "NormalizationTextureCreator.h"

namespace toadlet{
namespace tadpole{

Texture::ptr NormalizationTextureCreator::createNormalizationTexture(int size){
	TextureFormat::ptr format(new TextureFormat(TextureFormat::Dimension_CUBE,TextureFormat::Format_RGB_8,size,size,TextureFormat::CubeSide_MAX,1));
	tbyte *data=new tbyte[format->getDataSize()];

	Vector3 HALF_VECTOR3(Math::HALF,Math::HALF,Math::HALF);

	float offset = 0.5f;
	float halfSize = size * 0.5f;
	Vector3 temp;
	int pos=0;

	int i,j;
	for(j=0;j<size;++j){
		for(i=0;i<size;++i){
			temp.set(halfSize,(j+offset-halfSize),-(i+offset-halfSize));
			Math::normalize(temp);
			Math::madd(temp,Math::HALF,HALF_VECTOR3);
			data[pos+0]=(tbyte)(temp.x*255);
			data[pos+1]=(tbyte)(temp.y*255);
			data[pos+2]=(tbyte)(temp.z*255);

			pos+=3;
		}
	}

	for(j=0;j<size;++j){
		for(i=0;i<size;++i){
			temp.set(-halfSize,(j+offset-halfSize),(i+offset-halfSize));
			Math::normalize(temp);
			Math::madd(temp,Math::HALF,HALF_VECTOR3);

			data[pos+0]=(tbyte)(temp.x*255);
			data[pos+1]=(tbyte)(temp.y*255);
			data[pos+2]=(tbyte)(temp.z*255);

			pos+=3;
		}
	}

	for(j=0;j<size;++j){
		for(i=0;i<size;++i){
			temp.set((i+offset-halfSize),-halfSize,(j+offset-halfSize));
			Math::normalize(temp);
			Math::madd(temp,Math::HALF,HALF_VECTOR3);

			data[pos+0]=(tbyte)(temp.x*255);
			data[pos+1]=(tbyte)(temp.y*255);
			data[pos+2]=(tbyte)(temp.z*255);

			pos+=3;
		}
	}

	for(j=0;j<size;++j){
		for(i=0;i<size;++i){
			temp.set((i+offset-halfSize),halfSize,-(j+offset-halfSize));
			Math::normalize(temp);
			Math::madd(temp,Math::HALF,HALF_VECTOR3);

			data[pos+0]=(tbyte)(temp.x*255);
			data[pos+1]=(tbyte)(temp.y*255);
			data[pos+2]=(tbyte)(temp.z*255);

			pos+=3;
		}
	}

	for(j=0;j<size;++j){
		for(i=0;i<size;++i){
			temp.set((i+offset-halfSize),(j+offset-halfSize),halfSize);
			Math::normalize(temp);
			Math::madd(temp,Math::HALF,HALF_VECTOR3);

			data[pos+0]=(tbyte)(temp.x*255);
			data[pos+1]=(tbyte)(temp.y*255);
			data[pos+2]=(tbyte)(temp.z*255);

			pos+=3;
		}
	}

	for(j=0;j<size;++j){
		for(i=0;i<size;++i){
			temp.set(-(i+offset-halfSize),(j+offset-halfSize),-halfSize);
			Math::normalize(temp);
			Math::madd(temp,Math::HALF,HALF_VECTOR3);

			data[pos+0]=(tbyte)(temp.x*255);
			data[pos+1]=(tbyte)(temp.y*255);
			data[pos+2]=(tbyte)(temp.z*255);

			pos+=3;
		}
	}

	return mEngine->getTextureManager()->createTexture(Texture::Usage_BIT_STATIC,format,data);
}

}
}
