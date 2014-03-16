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

#include <toadlet/egg/io/DataStream.h>
#include "TGAStreamer.h"

namespace toadlet{
namespace tadpole{

enum ColorMap{
	ColorMap_NO=0,
	ColorMap_YES=1,
};

enum Encoding{
	Encoding_NONE=0,
	Encoding_COLORMAP=1,
	Encoding_TRUECOLOR=2,
	Encoding_BW=3,
	Encoding_RLE_COLORMAP=9,
	Encoding_RLE_TRUECOLOR=10,
	Encoding_RLE_BW=11,
};

Resource::ptr TGAStreamer::load(Stream::ptr stream,ResourceData *data){
	int usage=(data!=NULL)?((TextureData*)data)->usage:0;

	if(stream==NULL){
		Error::nullPointer(Categories::TOADLET_TADPOLE,
			"Stream is NULL");
		return NULL;
	}

	DataStream::ptr dataStream=new DataStream(stream);

	int idLength=dataStream->readUInt8();
	int colorMapType=dataStream->readUInt8();
	int imageType=dataStream->readUInt8();
	int colorMapStart=dataStream->readLUInt16();
	int colorMapLength=dataStream->readLUInt16();
	int colorMapBPP=dataStream->readUInt8();

	int x=dataStream->readLUInt16();
	int y=dataStream->readLUInt16();
	int width=dataStream->readLUInt16();
	int height=dataStream->readLUInt16();
	int bpp=dataStream->readUInt8();
	int descriptor=dataStream->readUInt8();

	TOADLET_IGNORE_UNUSED_VARIABLE_WARNING(colorMapStart);
	TOADLET_IGNORE_UNUSED_VARIABLE_WARNING(colorMapLength);
	TOADLET_IGNORE_UNUSED_VARIABLE_WARNING(colorMapBPP);
	TOADLET_IGNORE_UNUSED_VARIABLE_WARNING(x);
	TOADLET_IGNORE_UNUSED_VARIABLE_WARNING(y);
	
	if(colorMapType>1){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"Invalid color map type");
		return NULL;
	}
	
	if(!(imageType==Encoding_TRUECOLOR || imageType==Encoding_RLE_TRUECOLOR)){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"Only truecolor currently supported");
		return NULL;
	}

	if(descriptor>32){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"Interleaved not currently supported");
		return NULL;
	}

	int pixelFormat=0;
	int pixelSize=0;
	if(bpp==1 || bpp==2 || bpp==4 || bpp==8 || bpp==16 || bpp==24){
		pixelFormat=TextureFormat::Format_BGR_8;
		pixelSize=3;
	}
	else if(bpp==32){
		pixelFormat=TextureFormat::Format_BGRA_8;
		pixelSize=4;
	}
	else{
		Error::unknown(Categories::TOADLET_TADPOLE,
			String("TGAStreamer: Format is ")+bpp+", not 1, 2, 4, 8, 24, or 32 bit");
		return NULL;
	}

	TextureFormat::ptr textureFormat=new TextureFormat(TextureFormat::Dimension_D2,pixelFormat,width,height,1,0);
	int textureSize=textureFormat->getDataSize();
	tbyte *textureData=new tbyte[textureSize];

	int loop=0;
	switch(imageType){
		case(Encoding_TRUECOLOR):{
			int offset=idLength+18;

			stream->reset();
			stream->seek(offset);
			stream->read(textureData,textureSize);
		}break;
		case(Encoding_RLE_TRUECOLOR):{
			int offset=idLength+18;

			stream->reset();
			stream->seek(offset);
			
			tbyte *data=new tbyte[stream->length()];
			stream->read(data,stream->length());

			tbyte *c=data;

			int index=0;
			while(index<textureSize){
				if(*c&0x80){ // RLE high bit=1
					int length=*c-127;
					c++;

					for(loop=0;loop!=length;++loop,index+=pixelSize){
						memcpy(textureData+index,c,pixelSize);
					}

					c+=pixelSize;
				}
				else{
					int length=*c+1;
					c++;

					for(loop=0;loop!=length;++loop,index+=pixelSize,c+=pixelSize){
						memcpy(textureData+index,c,pixelSize);
					}
				}
			}

			delete[] data;
		}break;
	}

	Texture::ptr texture=mTextureManager->createTexture(usage,textureFormat,textureData);

	delete[] textureData;

	return texture;
}

}
}
