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

#include <toadlet/egg/image/RGBHandler.h>
#include <toadlet/egg/io/MemoryStream.h>
#include <toadlet/egg/EndianConversion.h>
#include <toadlet/egg/Error.h>
#include <string.h> // memcpy

namespace toadlet{
namespace egg{
namespace image{
	
typedef struct{
	unsigned short imagic;
	unsigned short type;
	unsigned short dim;
	unsigned short xsize, ysize, zsize;
	unsigned int min, max;
	unsigned int wasteBytes;
	char name[80];
	unsigned long colorMap;
	io::MemoryStream *stream;
	unsigned char *tmp, *tmpR, *tmpG, *tmpB;
	unsigned int *rowStart;
	int *rowSize;
} ImageRecord;

ImageRecord *toadlet_rgb_openImage(io::MemoryStream *stream){
	ImageRecord *imageRecord;
	int x;

	imageRecord=new ImageRecord();
	imageRecord->stream=stream;

	stream->read((char*)imageRecord,12);

	bigUInt16InPlace(imageRecord->imagic);
	bigUInt16InPlace(imageRecord->type);
	bigUInt16InPlace(imageRecord->dim);
	bigUInt16InPlace(imageRecord->xsize);
	bigUInt16InPlace(imageRecord->ysize);
	bigUInt16InPlace(imageRecord->zsize);

	imageRecord->tmp=new unsigned char[imageRecord->xsize*256];
	imageRecord->tmpR=new unsigned char[imageRecord->xsize*256];
	imageRecord->tmpG=new unsigned char[imageRecord->xsize*256];
	imageRecord->tmpB=new unsigned char[imageRecord->xsize*256];
   
	if((imageRecord->type & 0xFF00)==0x0100){
		x=imageRecord->ysize*imageRecord->zsize;
		imageRecord->rowStart=new unsigned int[x];
		imageRecord->rowSize=new int[x];

		stream->seek(512);
		stream->read((char*)imageRecord->rowStart,x*sizeof(unsigned int));
		stream->read((char*)imageRecord->rowSize,x*sizeof(unsigned int));

		int i;
		unsigned int *stp=imageRecord->rowStart;
		unsigned int *sip=(unsigned int*)imageRecord->rowSize;
		for(i=0;i<x;i++,stp++,sip++){
			bigUInt32InPlace(*stp);
			bigUInt32InPlace(*sip);
		}
	}
	else{
		imageRecord->rowStart=0;
		imageRecord->rowSize=0;
	}
	
	return imageRecord;
}

void toadlet_rgb_closeImage(ImageRecord *imageRecord){
	delete[] imageRecord->tmp;
	delete[] imageRecord->tmpR;
	delete[] imageRecord->tmpG;
	delete[] imageRecord->tmpB;
	delete[] imageRecord->rowSize;
	delete[] imageRecord->rowStart;
	delete imageRecord;
}

void toadlet_rgb_getImageRow(ImageRecord *imageRecord,unsigned char *buf,int y,int z){
	unsigned char *iPtr, *oPtr, pixel;
	int count;

	if((imageRecord->type & 0xFF00) == 0x0100){
		imageRecord->stream->seek((int)imageRecord->rowStart[y+z*imageRecord->ysize]);
		imageRecord->stream->read((char*)imageRecord->tmp,(int)imageRecord->rowSize[y+z*imageRecord->ysize]);

		iPtr=imageRecord->tmp;
		oPtr=buf;

		while(true){
			pixel=*iPtr++;
			count=(int)(pixel&0x7F);
			if(!count){
				return;
			}
			if(pixel&0x80){
				while(count--){
					*oPtr++=*iPtr++;
				}
			}
			else{
				pixel=*iPtr++;
				while(count--){
					*oPtr++=pixel;
				}
			}
		}
	}
	else{
		imageRecord->stream->seek(512+(y*imageRecord->xsize)+(z*imageRecord->xsize*imageRecord->ysize));
		imageRecord->stream->read((char*)buf,imageRecord->xsize);
	}
}

RGBHandler::RGBHandler(){
}

RGBHandler::~RGBHandler(){
}

Image *RGBHandler::loadImage(io::Stream *stream){
	char buffer[1024];
	char *totalBuffer=NULL;
	int totalSize=0;
	int i;

	if(stream==NULL){
		Error::nullPointer(Categories::TOADLET_EGG,
			"InputStream is NULL");
		return NULL;
	}

	for(i=stream->read(buffer,1024);i>0;i=stream->read(buffer,1024)){
		char *newBuffer=new char[totalSize+i];
		if(totalBuffer!=NULL){
			memcpy(newBuffer,totalBuffer,totalSize);
		}
		memcpy(newBuffer+totalSize,buffer,i);
		delete[] totalBuffer;
		totalBuffer=newBuffer;
		totalSize+=i;
	}

	io::MemoryStream::ptr memoryStream(new io::MemoryStream(totalBuffer,totalSize,false));
	unsigned char *rbuf, *gbuf, *bbuf, *abuf;
	ImageRecord *imageRecord;
	int y;

	imageRecord=toadlet_rgb_openImage(memoryStream);
	if(imageRecord==NULL){
		Error::loadingImage(Categories::TOADLET_EGG,
			"RGBHandler: Error opening file");
		return NULL;
	}

	rbuf=new unsigned char[imageRecord->xsize];
	gbuf=new unsigned char[imageRecord->xsize];
	bbuf=new unsigned char[imageRecord->xsize];
	abuf=new unsigned char[imageRecord->xsize];

	Image *image=new Image();
	int format=Image::Format_UNKNOWN;
	switch(imageRecord->zsize){
		case 1:
			format=Image::Format_L_8;
		break;
		case 2:
			format=Image::Format_LA_8;
		break;
		case 3:
			format=Image::Format_RGB_8;
		break;
		case 4:
			format=Image::Format_RGBA_8;
		break;
	}
	image->reallocate(Image::Dimension_D2,format,imageRecord->xsize,imageRecord->ysize,false);
	unsigned char *lptr=image->getData();

	int width=image->getWidth();

	for(y=0;y<imageRecord->ysize;++y){
		if(imageRecord->zsize==4){
			toadlet_rgb_getImageRow(imageRecord,rbuf,y,0);
			toadlet_rgb_getImageRow(imageRecord,gbuf,y,1);
			toadlet_rgb_getImageRow(imageRecord,bbuf,y,2);
			toadlet_rgb_getImageRow(imageRecord,abuf,y,3);

			for(i=0;i<width;++i){
				lptr[i*4+0]=rbuf[i];
				lptr[i*4+1]=gbuf[i];
				lptr[i*4+2]=bbuf[i];
				lptr[i*4+3]=abuf[i];
			}
       		lptr+=width*4;
		}
		else if(imageRecord->zsize==3){
			toadlet_rgb_getImageRow(imageRecord,rbuf,y,0);
			toadlet_rgb_getImageRow(imageRecord,gbuf,y,1);
			toadlet_rgb_getImageRow(imageRecord,bbuf,y,2);

			for(i=0;i<width;++i){
				lptr[i*3+0]=rbuf[i];
				lptr[i*3+1]=gbuf[i];
				lptr[i*3+2]=bbuf[i];
			}
       		lptr+=width*3;
		}
		else if(imageRecord->zsize==2){
			toadlet_rgb_getImageRow(imageRecord,rbuf,y,0);
			toadlet_rgb_getImageRow(imageRecord,abuf,y,1);

			for(i=0;i<width;++i){
				lptr[i*2+0]=rbuf[i];
				lptr[i*2+1]=abuf[i];
			}
       		lptr+=width*2;
		}
		else if(imageRecord->zsize==1){
			toadlet_rgb_getImageRow(imageRecord,rbuf,y,0);

			for(i=0;i<width;++i){
				lptr[i]=rbuf[i];
			}
       		lptr+=width;
		}
	}

	toadlet_rgb_closeImage(imageRecord);

	delete[] totalBuffer;
	
	delete[] rbuf;
	delete[] gbuf;
	delete[] bbuf;
	delete[] abuf;

	return image;
}

bool RGBHandler::saveImage(Image *image,io::Stream *stream){
	Error::unimplemented(Categories::TOADLET_EGG,
		"RGBHandler::saveImage: Not implemented");
	return false;
}

}
}
}

