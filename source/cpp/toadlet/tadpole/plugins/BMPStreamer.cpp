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

#include <toadlet/egg/EndianConversion.h>
#include <toadlet/egg/io/DataStream.h>
#include "BMPStreamer.h"

#if !defined(BI_RGB)
	#define BI_RGB 0L
#endif

namespace toadlet{
namespace tadpole{

typedef struct{
	uint32	biSize;
	int32	biWidth;
	int32	biHeight;
	uint16	biPlanes;
	uint16	biBitCount;
	uint32	biCompression;
	uint32	biSizeImage;
	int32	biXPelsPerMeter;
	int32	biYPelsPerMeter;
	uint32	biClrUsed;
	uint32	biClrImportant;
}BITMAPINFOHEADER;

const static int SIZEOF_BITMAPINFOHEADER=40;

typedef struct{
	uint16	bfType;
	uint32	bfSize;
	uint16	bfReserved1;
	uint16	bfReserved2;
	uint32	bfOffBits;
}BITMAPFILEHEADER;

const static int SIZEOF_BITMAPFILEHEADER=14;

Resource::ptr BMPStreamer::load(Stream::ptr stream,ResourceData *data,ProgressListener *listener){
	int i,j;
	BITMAPINFOHEADER bmih={0};
	BITMAPFILEHEADER bmfh={0};
	int usage=(data!=NULL)?((TextureData*)data)->usage:0;

	if(stream==NULL){
		Error::nullPointer(Categories::TOADLET_TADPOLE,
			"Stream is NULL");
		return NULL;
	}

	DataStream::ptr dataStream=new DataStream(stream);

	dataStream->read((tbyte*)&bmfh,SIZEOF_BITMAPFILEHEADER);
	#if defined(TOADLET_BIG_ENDIAN)
		littleUInt16InPlace(bmfh.bfType);
		littleUInt32InPlace(bmfh.bfSize);
		littleUInt16InPlace(bmfh.bfReserved1);
		littleUInt16InPlace(bmfh.bfReserved2);
		littleUInt32InPlace(bmfh.bfOffBits);
	#endif

	dataStream->read((tbyte*)&bmih,SIZEOF_BITMAPINFOHEADER);
	#if defined(TOADLET_BIG_ENDIAN)
		littleUInt32InPlace(bmih.biSize);
		littleInt32InPlace(bmih.biWidth);
		littleInt32InPlace(bmih.biHeight);
		littleUInt16InPlace(bmih.biPlanes);
		littleUInt16InPlace(bmih.biBitCount);
		littleUInt32InPlace(bmih.biCompression);
		littleUInt32InPlace(bmih.biSizeImage);
		littleInt32InPlace(bmih.biXPelsPerMeter);
		littleInt32InPlace(bmih.biYPelsPerMeter);
		littleUInt32InPlace(bmih.biClrUsed);
		littleUInt32InPlace(bmih.biClrImportant);
	#endif

	int width=bmih.biWidth;
	int height=bmih.biHeight;

	if(bmih.biPlanes!=1){
		Error::unknown(Categories::TOADLET_TADPOLE,
			String("BMPStreamer: Planes are ")+(int)bmih.biPlanes+", not 1");
		return NULL;
	}

	int pixelFormat=0;
	if(bmih.biBitCount==1 || bmih.biBitCount==2 || bmih.biBitCount==4 || bmih.biBitCount==8 || bmih.biBitCount==16 || bmih.biBitCount==24){
		pixelFormat=TextureFormat::Format_RGB_8;
	}
	else if(bmih.biBitCount==32){
		pixelFormat=TextureFormat::Format_RGBA_8;
	}
	else{
		Error::unknown(Categories::TOADLET_TADPOLE,
			String("BMPStreamer: Format is ")+(int)bmih.biBitCount+", not 1, 2, 4, 8, 24, or 32 bit");
		return NULL;
	}

	if(bmih.biCompression!=BI_RGB){
		Error::unknown(Categories::TOADLET_TADPOLE,
			String("BMPStreamer: Compression is ")+(int)bmih.biCompression+", not BI_RGB");
		return NULL;
	}

	TextureFormat::ptr textureFormat=new TextureFormat(TextureFormat::Dimension_D2,pixelFormat,width,height,1,0);
	tbyte *textureData=new tbyte[textureFormat->getDataSize()];

	if(bmih.biBitCount==1 || bmih.biBitCount==2 || bmih.biBitCount==4 || bmih.biBitCount==8){
		tbyte *palette=new tbyte[(1<<bmih.biBitCount)*4];
		stream->read(palette,(1<<bmih.biBitCount)*4);

		int byteWidth=(width*bmih.biBitCount)/8 + ((((width*bmih.biBitCount)%8)>0)?1:0);
		int rowSize=byteWidth + (4-byteWidth%4)%4;

		stream->seek(bmfh.bfOffBits);

		tbyte *rawData=new tbyte[rowSize*height];
		stream->read(rawData,rowSize*height);

		for(j=0;j<height;++j){
			for(i=0;i<width;++i){
				// First, calculate our byte index into rawData, then right shift to move the bits we want to least significant
				// Then, mod by the maximum value available to remove the excess left bits
				int index=(rawData[(i*bmih.biBitCount)/8 + j*rowSize] >> (8-((bmih.biBitCount*(i+1))%8))%8) % (1 << bmih.biBitCount);
				int imageDataOffset=i*3 + j*width*3;

				textureData[imageDataOffset+0]=palette[index*4+2];
				textureData[imageDataOffset+1]=palette[index*4+1];
				textureData[imageDataOffset+2]=palette[index*4+0];
			}
		}

		delete[] rawData;
		delete[] palette;
	}
	else if(bmih.biBitCount==24){
		stream->seek(bmfh.bfOffBits);

		int rowSize=(width*3) + (4-(width*3)%4)%4;

		tbyte *rawData=new tbyte[rowSize*height];
		stream->read((tbyte*)rawData,rowSize*height);

		for(j=0;j<height;++j){
			for(i=0;i<width;++i){
				int rawDataOffset=i*3 + j*rowSize;
				int imageDataOffset=i*3 + j*width*3;

				textureData[imageDataOffset+0]=rawData[rawDataOffset+2];
				textureData[imageDataOffset+1]=rawData[rawDataOffset+1];
				textureData[imageDataOffset+2]=rawData[rawDataOffset+0];
			}
		}

		delete[] rawData;
	}
	else if(bmih.biBitCount==32){
		stream->seek(bmfh.bfOffBits);

		stream->read((tbyte*)textureData,width*height*4);

		for(j=0;j<height;++j){
			for(i=0;i<width;++i){
				int imageDataOffset=i*4 + j*width*4;
				tbyte temp=textureData[imageDataOffset+2];

				textureData[imageDataOffset+2]=textureData[imageDataOffset+0];
				textureData[imageDataOffset+0]=temp;
			}
		}
	}

	Texture::ptr texture=mTextureManager->createTexture(usage,textureFormat,textureData);

	delete[] textureData;

	return texture;
} 

bool BMPStreamer::save(Stream::ptr stream,Resource::ptr resource,ResourceData *data,ProgressListener *listener){
	int rowSize;
	int i,j;
	int ihSize;
	int fhSize;

	BITMAPINFOHEADER bmih={0};
	BITMAPFILEHEADER bmfh={0};

	Texture::ptr texture=shared_static_cast<Texture>(resource);
	TextureFormat::ptr textureFormat=texture->getFormat();

	if(textureFormat->getDimension()!=TextureFormat::Dimension_D2){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"texture isn't 2 dimensional");
		return false;
	}

	if(!(textureFormat->getPixelFormat()==TextureFormat::Format_RGB_8 || textureFormat->getPixelFormat()==TextureFormat::Format_RGBA_8)){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"texture isn't in RGB/RGBA format");
		return false;
	}

	tbyte *textureData=new tbyte[textureFormat->getDataSize()];

	texture->read(textureFormat,textureData);

	int pixelFormat=textureFormat->getPixelFormat();
	int width=textureFormat->getWidth();
	int height=textureFormat->getHeight();

	rowSize=textureFormat->getXPitch() + (4-(textureFormat->getXPitch())%4)%4;

	//Find the header sizes, due to the padding of structures
	fhSize=	sizeof(bmfh.bfType) + sizeof(bmfh.bfSize) + sizeof(bmfh.bfReserved1) + sizeof(bmfh.bfReserved2) + sizeof(bmfh.bfOffBits);
	ihSize=	sizeof(bmih.biSize) + sizeof(bmih.biWidth) + sizeof(bmih.biHeight) + sizeof(bmih.biPlanes) + sizeof(bmih.biBitCount) +
		sizeof(bmih.biCompression) + sizeof(bmih.biSizeImage) + sizeof(bmih.biXPelsPerMeter) + sizeof(bmih.biYPelsPerMeter) +
		sizeof(bmih.biClrUsed) + sizeof(bmih.biClrImportant);

	DataStream::ptr dataStream=new DataStream(stream);

	bmfh.bfType=((int16)(((char)('B')) | ((int16)((char('M'))) << 8)));
	bmfh.bfSize=fhSize+ihSize+rowSize*height;
	bmfh.bfReserved1=0;
	bmfh.bfReserved2=0;
	bmfh.bfOffBits=fhSize+ihSize;

	dataStream->writeLUInt16(bmfh.bfType);
	dataStream->writeLUInt32(bmfh.bfSize);
	dataStream->writeLUInt16(bmfh.bfReserved1);
	dataStream->writeLUInt16(bmfh.bfReserved2);
	dataStream->writeLUInt32(bmfh.bfOffBits);

	bmih.biSize=ihSize;
	bmih.biWidth=width;
	bmih.biHeight=height;
	bmih.biPlanes=1;
	if(pixelFormat==TextureFormat::Format_RGB_8){
		bmih.biBitCount=24;
	}
	else if(pixelFormat==TextureFormat::Format_RGBA_8){
		bmih.biBitCount=32;
	}
	bmih.biCompression=BI_RGB; // Only non-compressed supported
	bmih.biSizeImage=0;
	bmih.biXPelsPerMeter=0;
	bmih.biYPelsPerMeter=0;
	bmih.biClrUsed=0;
	bmih.biClrImportant=0;

	// Write info header
	dataStream->writeLUInt32(bmih.biSize);
	dataStream->writeLInt32(bmih.biWidth);
	dataStream->writeLInt32(bmih.biHeight);
	dataStream->writeLUInt16(bmih.biPlanes);
	dataStream->writeLUInt16(bmih.biBitCount);
	dataStream->writeLUInt32(bmih.biCompression);
	dataStream->writeLUInt32(bmih.biSizeImage);
	dataStream->writeLInt32(bmih.biXPelsPerMeter);
	dataStream->writeLInt32(bmih.biYPelsPerMeter);
	dataStream->writeLUInt32(bmih.biClrUsed);
	dataStream->writeLUInt32(bmih.biClrImportant);

	if(pixelFormat==TextureFormat::Format_RGB_8){
		int rowSize=(width*3) + (4-(width*3)%4)%4;

		tbyte *rawData=new tbyte[rowSize*height];
		memset(rawData,0,rowSize*height);

		for(j=0;j<height;++j){
			for(i=0;i<width;++i){
				int rawDataOffset=i*3 + j*rowSize;
				int imageDataOffset=i*3 + j*width*3;

				rawData[rawDataOffset+2]=textureData[imageDataOffset+0];
				rawData[rawDataOffset+1]=textureData[imageDataOffset+1];
				rawData[rawDataOffset+0]=textureData[imageDataOffset+2];
			}
		}

		stream->write(rawData,rowSize*height);

		delete[] rawData;
	}
	else if(pixelFormat==TextureFormat::Format_RGBA_8){
		tbyte *rawData=new tbyte[width*height*4];
		memset(rawData,0,width*height*4);

		for(j=0;j<height;++j){
			for(i=0;i<width;++i){
				int rawDataOffset=i*4 + j*width*4;
				int imageDataOffset=i*4 + j*width*4;

				rawData[rawDataOffset+2]=textureData[imageDataOffset+0];
				rawData[rawDataOffset+1]=textureData[imageDataOffset+1];
				rawData[rawDataOffset+0]=textureData[imageDataOffset+2];
				rawData[rawDataOffset+3]=textureData[imageDataOffset+3];
			}
		}

		stream->write(rawData,width*height*4);

		delete[] rawData;
	}

	delete[] textureData;

	return true;
}

}
}
