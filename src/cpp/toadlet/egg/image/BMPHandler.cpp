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

#include <toadlet/egg/image/BMPHandler.h>
#include <toadlet/egg/io/DataStream.h>
#include <toadlet/egg/Error.h>
#include <string.h> // memset

#if !defined(BI_RGB)
	#define BI_RGB 0L
#endif

using namespace toadlet::egg::io;

namespace toadlet{
namespace egg{
namespace image{

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

typedef struct{
	uint16	bfType;
	uint32	bfSize;
	uint16	bfReserved1;
	uint16	bfReserved2;
	uint32	bfOffBits;
}BITMAPFILEHEADER;

BMPHandler::BMPHandler(){
}

BMPHandler::~BMPHandler(){
}

Image *BMPHandler::loadImage(Stream *stream){
	int i,j;
	BITMAPINFOHEADER bmih={0};
	BITMAPFILEHEADER bmfh={0};

	if(stream==NULL){
		Error::nullPointer(Categories::TOADLET_EGG,
			"Stream is NULL");
		return NULL;
	}

	DataStream::ptr dataStream(new DataStream(stream));

	int amount=0;
	amount+=dataStream->readLittleUInt16(bmfh.bfType);
	amount+=dataStream->readLittleUInt32(bmfh.bfSize);
	amount+=dataStream->readLittleUInt16(bmfh.bfReserved1);
	amount+=dataStream->readLittleUInt16(bmfh.bfReserved2);
	amount+=dataStream->readLittleUInt32(bmfh.bfOffBits);
	amount+=dataStream->readLittleUInt32(bmih.biSize);
	amount+=dataStream->readLittleInt32(bmih.biWidth);
	amount+=dataStream->readLittleInt32(bmih.biHeight);
	amount+=dataStream->readLittleUInt16(bmih.biPlanes);
	amount+=dataStream->readLittleUInt16(bmih.biBitCount);
	amount+=dataStream->readLittleUInt32(bmih.biCompression);
	amount+=dataStream->readLittleUInt32(bmih.biSizeImage);
	amount+=dataStream->readLittleInt32(bmih.biXPelsPerMeter);
	amount+=dataStream->readLittleInt32(bmih.biYPelsPerMeter);
	amount+=dataStream->readLittleUInt32(bmih.biClrUsed);
	amount+=dataStream->readLittleUInt32(bmih.biClrImportant);

	int width=bmih.biWidth;
	int height=bmih.biHeight;

	if(bmih.biPlanes!=1){
		Error::loadingImage(Categories::TOADLET_EGG,
			String("BMPHandler: Planes are ")+(int)bmih.biPlanes+", not 1");
		return NULL;
	}

	int format;

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

	Image *image=new Image(Image::Dimension_D2,format,width,height);

	if(bmih.biBitCount==1 || bmih.biBitCount==2 || bmih.biBitCount==4 || bmih.biBitCount==8){
		unsigned char *palette=new unsigned char[(1<<bmih.biBitCount)*4];
		amount+=stream->read((char*)palette,(1<<bmih.biBitCount)*4);

		int byteWidth=(width*bmih.biBitCount)/8 + ((((width*bmih.biBitCount)%8)>0)?1:0);
		int rowSize=byteWidth + (4-byteWidth%4)%4;

		if(amount<(int)bmfh.bfOffBits){
			char *temp=new char[bmfh.bfOffBits-amount];
			stream->read(temp,bmfh.bfOffBits-amount);
			delete[] temp;
		}

		unsigned char *rawData=new unsigned char[rowSize*height];
		stream->read((char*)rawData,rowSize*height);

		unsigned char *imageData=image->getData();

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
		if(amount<(int)bmfh.bfOffBits){
			char *temp=new char[bmfh.bfOffBits-amount];
			stream->read(temp,bmfh.bfOffBits-amount);
			delete[] temp;
		}

		int rowSize=(width*3) + (4-(width*3)%4)%4;

		unsigned char *rawData=new unsigned char[rowSize*height];
		stream->read((char*)rawData,rowSize*height);

		unsigned char *imageData=image->getData();

		for(j=0;j<height;++j){
			for(i=0;i<width;++i){
				int rawDataOffset=i*3 + j*rowSize;
				int imageDataOffset=i*3 + j*width*3;

				imageData[imageDataOffset+0]=rawData[rawDataOffset+0];
				imageData[imageDataOffset+1]=rawData[rawDataOffset+1];
				imageData[imageDataOffset+2]=rawData[rawDataOffset+2];
			}
		}

		delete[] rawData;
	}
	else if(bmih.biBitCount==32){
		if(amount<(int)bmfh.bfOffBits){
			char *temp=new char[bmfh.bfOffBits-amount];
			stream->read(temp,bmfh.bfOffBits-amount);
			delete[] temp;
		}

		unsigned char *imageData=image->getData();
		stream->read((char*)imageData,width*height*4);

		for(j=0;j<height;++j){
			for(i=0;i<width;++i){
				int imageDataOffset=i*4 + j*width*4;
				unsigned char temp=imageData[imageDataOffset+2];

				imageData[imageDataOffset+2]=imageData[imageDataOffset+0];
				imageData[imageDataOffset+0]=temp;
			}
		}
	}

	return image;
} 

bool BMPHandler::saveImage(Image *image,Stream *stream){
	int rowSize;
	int i,j;
	int ihSize;
	int fhSize;

	BITMAPINFOHEADER bmih={0};
	BITMAPFILEHEADER bmfh={0};

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

	dataStream->writeLittleUInt16(bmfh.bfType);
	dataStream->writeLittleUInt32(bmfh.bfSize);
	dataStream->writeLittleUInt16(bmfh.bfReserved1);
	dataStream->writeLittleUInt16(bmfh.bfReserved2);
	dataStream->writeLittleUInt32(bmfh.bfOffBits);

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
	dataStream->writeLittleUInt32(bmih.biSize);
	dataStream->writeLittleInt32(bmih.biWidth);
	dataStream->writeLittleInt32(bmih.biHeight);
	dataStream->writeLittleUInt16(bmih.biPlanes);
	dataStream->writeLittleUInt16(bmih.biBitCount);
	dataStream->writeLittleUInt32(bmih.biCompression);
	dataStream->writeLittleUInt32(bmih.biSizeImage);
	dataStream->writeLittleInt32(bmih.biXPelsPerMeter);
	dataStream->writeLittleInt32(bmih.biYPelsPerMeter);
	dataStream->writeLittleUInt32(bmih.biClrUsed);
	dataStream->writeLittleUInt32(bmih.biClrImportant);

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

		stream->write((char*)rawData,rowSize*height);

		delete[] rawData;
	}
	else if(image->getFormat()==Image::Format_RGBA_8){
		unsigned char *rawData=new unsigned char[width*height*4];
		memset(rawData,0,width*height*4);

		unsigned char *imageData=image->getData();

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

		stream->write((char*)imageData,width*height*4);

		delete[] rawData;
	}

	return true;
}

}
}
}
