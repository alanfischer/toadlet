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
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/io/DataStream.h>
#include <toadlet/tadpole/handler/BMPHandler.h>

#if !defined(BI_RGB)
	#define BI_RGB 0L
#endif

namespace toadlet{
namespace tadpole{
namespace handler{

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

Resource::ptr BMPHandler::load(Stream::ptr stream,ResourceData *data,ProgressListener *listener){
	int i,j;
	BITMAPINFOHEADER bmih={0};
	BITMAPFILEHEADER bmfh={0};

	if(stream==NULL){
		Error::nullPointer(Categories::TOADLET_EGG,
			"Stream is NULL");
		return NULL;
	}

	DataStream::ptr dataStream(new DataStream(stream));

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
		Error::loadingImage(Categories::TOADLET_EGG,
			String("BMPHandler: Planes are ")+(int)bmih.biPlanes+", not 1");
		return NULL;
	}

	int format=0;
	if(bmih.biBitCount==1 || bmih.biBitCount==2 || bmih.biBitCount==4 || bmih.biBitCount==8 || bmih.biBitCount==16 || bmih.biBitCount==24){
		format=Image::Format_RGB_8;
	}
	else if(bmih.biBitCount==32){
		format=Image::Format_RGBA_8;
	}
	else{
		Error::loadingImage(Categories::TOADLET_EGG,
			String("BMPHandler: Format is ")+(int)bmih.biBitCount+", not 1, 2, 4, 8, 24, or 32 bit");
		return NULL;
	}

	if(bmih.biCompression!=BI_RGB){
		Error::loadingImage(Categories::TOADLET_EGG,
			String("BMPHandler: Compression is ")+(int)bmih.biCompression+", not BI_RGB");
		return NULL;
	}

	Image::ptr image(Image::createAndReallocate(Image::Dimension_D2,format,width,height));
	if(image==NULL){
		return NULL;
	}
	
	tbyte *imageData=image->getData();

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

				imageData[imageDataOffset+0]=palette[index*4+2];
				imageData[imageDataOffset+1]=palette[index*4+1];
				imageData[imageDataOffset+2]=palette[index*4+0];
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

				imageData[imageDataOffset+0]=rawData[rawDataOffset+2];
				imageData[imageDataOffset+1]=rawData[rawDataOffset+1];
				imageData[imageDataOffset+2]=rawData[rawDataOffset+0];
			}
		}

		delete[] rawData;
	}
	else if(bmih.biBitCount==32){
		stream->seek(bmfh.bfOffBits);

		stream->read((tbyte*)imageData,width*height*4);

		for(j=0;j<height;++j){
			for(i=0;i<width;++i){
				int imageDataOffset=i*4 + j*width*4;
				tbyte temp=imageData[imageDataOffset+2];

				imageData[imageDataOffset+2]=imageData[imageDataOffset+0];
				imageData[imageDataOffset+0]=temp;
			}
		}
	}

	if(image!=NULL){
		return mTextureManager->createTexture(image);
	}
	else{
		return NULL;
	}
} 

bool BMPHandler::save(Stream::ptr stream,Resource::ptr resource,ResourceData *data,ProgressListener *listener){
	int rowSize;
	int i,j;
	int ihSize;
	int fhSize;

	BITMAPINFOHEADER bmih={0};
	BITMAPFILEHEADER bmfh={0};

	Texture::ptr texture=shared_static_cast<Texture>(resource);
	Image::ptr image=mTextureManager->createImage(texture);

	if(image->getDimension()!=Image::Dimension_D2){
		Error::loadingImage(Categories::TOADLET_EGG,
			"BMPHandler: Image isn't D2");
		return false;
	}

	if(!(image->getFormat()==Image::Format_RGB_8 || image->getFormat()==Image::Format_RGBA_8)){
		Error::loadingImage(Categories::TOADLET_EGG,
			"BMPHandler: Image isn't in RGB/RGBA format");
		return false;
	}

	int width=image->getWidth();
	int height=image->getHeight();

	rowSize=width*image->getFormat() + (4-(width*image->getFormat())%4)%4;

	//Find the header sizes, due to the padding of structures
	fhSize=	sizeof(bmfh.bfType) + sizeof(bmfh.bfSize) + sizeof(bmfh.bfReserved1) + sizeof(bmfh.bfReserved2) + sizeof(bmfh.bfOffBits);
	ihSize=	sizeof(bmih.biSize) + sizeof(bmih.biWidth) + sizeof(bmih.biHeight) + sizeof(bmih.biPlanes) + sizeof(bmih.biBitCount) +
		sizeof(bmih.biCompression) + sizeof(bmih.biSizeImage) + sizeof(bmih.biXPelsPerMeter) + sizeof(bmih.biYPelsPerMeter) +
		sizeof(bmih.biClrUsed) + sizeof(bmih.biClrImportant);

	DataStream::ptr dataStream(new DataStream(stream));

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
	if(image->getFormat()==Image::Format_RGB_8){
		bmih.biBitCount=24;
	}
	else if(image->getFormat()==Image::Format_RGBA_8){
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

	if(image->getFormat()==Image::Format_RGB_8){
		int rowSize=(width*3) + (4-(width*3)%4)%4;

		unsigned char *rawData=new unsigned char[rowSize*height];
		memset(rawData,0,rowSize*height);

		unsigned char *imageData=image->getData();

		for(j=0;j<height;++j){
			for(i=0;i<width;++i){
				int rawDataOffset=i*3 + j*rowSize;
				int imageDataOffset=i*3 + j*width*3;

				rawData[rawDataOffset+2]=imageData[imageDataOffset+0];
				rawData[rawDataOffset+1]=imageData[imageDataOffset+1];
				rawData[rawDataOffset+0]=imageData[imageDataOffset+2];
			}
		}

		stream->write(rawData,rowSize*height);

		delete[] rawData;
	}
	else if(image->getFormat()==Image::Format_RGBA_8){
		tbyte *rawData=new tbyte[width*height*4];
		memset(rawData,0,width*height*4);

		tbyte *imageData=image->getData();

		for(j=0;j<height;++j){
			for(i=0;i<width;++i){
				int rawDataOffset=i*4 + j*width*4;
				int imageDataOffset=i*4 + j*width*4;

				rawData[rawDataOffset+2]=imageData[imageDataOffset+0];
				rawData[rawDataOffset+1]=imageData[imageDataOffset+1];
				rawData[rawDataOffset+0]=imageData[imageDataOffset+2];
				rawData[rawDataOffset+3]=imageData[imageDataOffset+3];
			}
		}

		stream->write(imageData,width*height*4);

		delete[] rawData;
	}

	return true;
}

}
}
}
