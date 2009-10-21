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

extern "C"{
	#include <gif_lib.h>
}
#include <toadlet/egg/image/GIFHandler.h>
#include <toadlet/egg/io/DataInputStream.h>
#include <toadlet/egg/io/DataOutputStream.h>
#include <toadlet/egg/EndianConversion.h>
#include <toadlet/egg/Error.h>
#include <string.h> // memcpy

#if defined(TOADLET_PLATFORM_WIN32) && defined(TOADLET_LIBGIF_NAME)
	#pragma comment(lib,TOADLET_LIBGIF_NAME)
#endif

using namespace toadlet::egg::io;

namespace toadlet{
namespace egg{
namespace image{

int readGifData(GifFileType *file,GifByteType *data,int amount){
	InputStream *in=(InputStream*)file->UserData;
	return in->read((char*)data,amount);
}

int writeGifData(GifFileType *file,GifByteType *data,int amount){
	OutputStream *out=(OutputStream*)file->UserData;
	return out->write((char*)data,amount);
}

const int InterlacedOffset[] = { 0, 4, 2, 1 };	// The way Interlaced image should
const int InterlacedJumps[] = { 8, 8, 4, 2 };	//  be read - offsets and jumps...

GifByteType szNetscape20ext[] = "\x0bNETSCAPE2.0";
#define NSEXT_LOOP			0x01	// Loop Count field code
#define GIF_TRANSPARENT		0x01
#define GIF_USER_INPUT		0x02
#define GIF_DISPOSE_MASK	0x07
#define GIF_DISPOSE_SHIFT	2

#define GIF_NOT_TRANSPARENT	-1

#define GIF_DISPOSE_NONE	0	// No disposal specified. The decoder is
								//  not required to take any action.
#define GIF_DISPOSE_LEAVE	1	// Do not dispose. The graphic is to be left
								//  in place.
#define GIF_DISPOSE_BACKGND	2	// Restore to background color. The area used by the
								//  graphic must be restored to the background color.
#define GIF_DISPOSE_RESTORE	3	// Restore to previous. The decoder is required to
								//  restore the area overwritten by the graphic with
								//  what was there prior to rendering the graphic.

void copyGIFLine(	unsigned char *dest,
					unsigned int iwidth,unsigned int iheight,
					unsigned int xoff,unsigned int yoff,
					unsigned int twidth,unsigned int theight,
					unsigned int amt,unsigned int yoff2,
					unsigned char *src,int trans,GifColorType *colorTable,int pixelSize,int format){

	dest=dest + (iwidth*pixelSize*(yoff+yoff2)) + xoff*pixelSize;

	int i;
	for(i=0;i<amt;++i){
		if(format==Image::Format_RGB_8){
			dest[i*3+0]=colorTable[src[i]].Red;
			dest[i*3+1]=colorTable[src[i]].Green;
			dest[i*3+2]=colorTable[src[i]].Blue;
		}
		else{
			dest[i*4+0]=colorTable[src[i]].Red;
			dest[i*4+1]=colorTable[src[i]].Green;
			dest[i*4+2]=colorTable[src[i]].Blue;
			dest[i*4+3]=(src[i]==trans)?0:255;
		}
	}
}

GIFHandler::GIFHandler(){
	mWorking=0;
	mBase=0;
	mDelayMilliseconds=0;
}

GIFHandler::~GIFHandler(){
	resetReader();
}

void GIFHandler::resetReader(){
	if(mWorking){
		delete mWorking;
		mWorking=0;
	}
	if(mBase){
		delete mBase;
		mBase=0;
	}
	mDelayMilliseconds=0;
}

int GIFHandler::getNextImage(GifFileType *gifFile,Image *&image,int &delayMilliseconds){
	GifRecordType RecordType;
	GifByteType *pLine=0;
	GifByteType *pExtension=0;
	int dispose = 0;
	int pass = 0;
	int transparent = GIF_NOT_TRANSPARENT;

	do{
		int i, ExtCode;
		if(DGifGetRecordType(gifFile,&RecordType)==GIF_ERROR){
			break;
		}

		switch (RecordType){
		case IMAGE_DESC_RECORD_TYPE:
			if (DGifGetImageDesc(gifFile) != GIF_ERROR){
				const int x = gifFile->Image.Left;
				const int y = gifFile->Image.Top;

				GifColorType* pColorTable;
				if (gifFile->Image.ColorMap == NULL){
					pColorTable = gifFile->SColorMap->Colors;
				}
				else{
					pColorTable = gifFile->Image.ColorMap->Colors;
				}

				const int Width = gifFile->Image.Width;
				const int Height = gifFile->Image.Height;

				int format=Image::Format_UNKNOWN;
				int pixelSize=0;
				if(transparent==GIF_NOT_TRANSPARENT){
					format=Image::Format_RGB_8;
					pixelSize=3;
				}
				else{
					format=Image::Format_RGBA_8;
					pixelSize=4;
				}

				if(mWorking==NULL){
					mWorking=new Image(Image::Dimension_D2,format,Width,Height);
					memset(mWorking->getData(),0,pixelSize*Width*Height);
				}

				pLine=new GifByteType[Width];

				unsigned int workingWidth=mWorking->getWidth();
				unsigned int workingHeight=mWorking->getHeight();

				if(gifFile->Image.Interlace){
					// Need to perform 4 passes on the images:
					for (pass = 0; pass < 4; ++pass){
						for (i = InterlacedOffset[pass]; i < Height; i += InterlacedJumps[pass]){
							if (DGifGetLine(gifFile, pLine, Width) == GIF_ERROR){
								delete[] pLine;
								return GIF_ERROR;
							}
							copyGIFLine(mWorking->getData(),workingWidth,workingHeight,x,y,Width,Height,Width,i,pLine,transparent,pColorTable,pixelSize,format);
						}
					}
				}
				else{
					// Non-interlaced image
					for (i = 0; i < Height; ++i){
						if (DGifGetLine(gifFile, pLine, Width) == GIF_ERROR){
							delete[] pLine;
							return GIF_ERROR;
						}
						copyGIFLine(mWorking->getData(),workingWidth,workingHeight,x,y,Width,Height,Width,i,pLine,transparent,pColorTable,pixelSize,format);
					}
				}
				delete[] pLine;

				if(mBase==NULL){
					mBase=mWorking->clone();
				}

				image=flipImage(mWorking);
				delayMilliseconds=mDelayMilliseconds;

				// Prepare second image with next starting
				if(dispose == GIF_DISPOSE_BACKGND){
					/* Untested
					GifColorType *color=gifFile->SColorMap->Colors + gifFile->SBackGroundColor;
					const int x = gifFile->Image.Left;
					const int y = gifFile->Image.Top;
					const int Width = gifFile->Image.Width;
					const int Height = gifFile->Image.Height;

					int i,j;
					for(i=y;i<y+Width;++i){
						for(j=x;j<x+Height;++j){
							if(mWorking->type==Image::TYPE_RGB){
								mWorking->data[(i*mWorking->width+j)*3+0]=color->Red;
								mWorking->data[(i*mWorking->width+j)*3+1]=color->Green;
								mWorking->data[(i*mWorking->width+j)*3+2]=color->Blue;
							}
							else{
								mWorking->data[(i*mWorking->width+j)*4+0]=color->Red;
								mWorking->data[(i*mWorking->width+j)*4+1]=color->Green;
								mWorking->data[(i*mWorking->width+j)*4+2]=color->Blue;
								mWorking->data[(i*mWorking->width+j)*4+3]=0;
							}
						}
					}
					*/
				}
				else if(dispose == GIF_DISPOSE_RESTORE){
					memcpy(mBase->getData(),mWorking->getData(),mWorking->getPixelSize()*mWorking->getWidth()*mWorking->getHeight());
				}
				else{
					// Dont do anything
				}

				return GIF_OK;
			}
			break;
		case EXTENSION_RECORD_TYPE:{
			if (DGifGetExtension(gifFile,&ExtCode,&pExtension)==GIF_ERROR){
				return GIF_ERROR;
			}
			bool bNetscapeExt = FALSE;
			switch (ExtCode){
				case COMMENT_EXT_FUNC_CODE:
					break;
				case GRAPHICS_EXT_FUNC_CODE:{
					int flag = pExtension[1];
					int16 d=*(int16*)&pExtension[2];
					littleInt16InPlace(d);

					// Not really correct probably, but sometimes zero's crop up when they shouldnt...
					if(d!=0){
						mDelayMilliseconds=d*10;
					}

					transparent = (flag & GIF_TRANSPARENT) ? pExtension[4] : GIF_NOT_TRANSPARENT;
					dispose = (flag >> GIF_DISPOSE_SHIFT) & GIF_DISPOSE_MASK;

					break;
				}
				case PLAINTEXT_EXT_FUNC_CODE:
					break;
				case APPLICATION_EXT_FUNC_CODE:
					break;
				default:
					break;
			}
			do{
				if(DGifGetExtensionNext(gifFile,&pExtension)==GIF_ERROR){
					return GIF_ERROR;
				}
				// Process Netscape 2.0 extension (GIF looping)
				if(pExtension && bNetscapeExt){
					GifByteType bLength=pExtension[0];
					int iSubCode=pExtension[1] & 0x07;
					if(bLength==3 && iSubCode==NSEXT_LOOP){
						//UINT uLoopCount=MAKEWORD(pExtension[2],pExtension[3]);
						//m_uLoopCount=uLoopCount-1;
					}
				}
			}
			while (pExtension);
			break;
		}
		case TERMINATE_RECORD_TYPE:
			break;
		default: // Should be trapped by DGifGetRecordType
			break;
		}
	}
	while (RecordType != TERMINATE_RECORD_TYPE);
	return GIF_OK;
}

GifFileType *GIFHandler::openFile(InputStream *in){
	GifFileType *file=DGifOpen((void*)in,&readGifData);
	return file;
}

int GIFHandler::closeFile(GifFileType *file){
	int result=DGifCloseFile(file);
	return result;
}

Image *GIFHandler::loadImage(InputStream *in){
	if(in==NULL){
		Error::nullPointer(Categories::TOADLET_EGG,
			"InputStream is NULL");
		return NULL;
	}

	GifFileType *file=openFile(in);

	if(file==NULL){
		Error::loadingImage(Categories::TOADLET_EGG,
			"GIFHandler::loadImage: Invalid file");
		return NULL;
	}

	resetReader();

	Image *image=0;
	int delayMilliseconds=0;
	int result=0;

	result=getNextImage(file,image,delayMilliseconds);
	if(result==GIF_ERROR){
		delete image;

		Error::loadingImage(Categories::TOADLET_EGG,
			"GIFHandler::loadImage: Error in getNextImage");
		return NULL;
	}

	result=closeFile(file);
	if(result==GIF_ERROR){
		delete image;

		Error::loadingImage(Categories::TOADLET_EGG,
			String("GIFHandler::loadImage: Error closing file: ")+result);
		return NULL;
	}

	return image;
}

bool GIFHandler::saveImage(Image *image,OutputStream *out){
	Error::unimplemented(Categories::TOADLET_EGG,
		"GIFHandler::saveImage: Not implemented");
	return false;
}

bool GIFHandler::loadAnimatedImage(InputStream *in,Collection<Image*> &images,Collection<int> &delaysMilliseconds){
	if(in==NULL){
		Error::nullPointer(Categories::TOADLET_EGG,
			"InputStream is NULL");
		return false;
	}

	GifFileType *file=openFile(in);

	if(file==NULL){
		Error::loadingImage(Categories::TOADLET_EGG,
			"GIFHandler::loadAnimatedImage: Invalid file");
		return false;
	}

	resetReader();

	Image *image;
	int delayMilliseconds;
	int result;

	do{
		image=0;
		delayMilliseconds=0;
		result=getNextImage(file,image,delayMilliseconds);
		if(image!=0 && result!=GIF_ERROR){
			images.add(image);
			delaysMilliseconds.add(delayMilliseconds);
		}
	}while(result!=GIF_ERROR && image!=0);

	if(result==GIF_ERROR){
		Error::loadingImage(Categories::TOADLET_EGG,
			"GIFHandler::loadAnimatedImage: Error in getNextImage");
		return false;
	}

	result=DGifCloseFile(file);
	if(result==GIF_ERROR){
		Error::loadingImage(Categories::TOADLET_EGG,
			"GIFHandler::loadAnimatedImage: Error closing file");
		return false;
	}

	return true;
}

Image *GIFHandler::flipImage(Image *image){
	unsigned int width=image->getWidth();
	unsigned int height=image->getHeight();
	unsigned int pixelSize=image->getPixelSize();

	int amt=width*pixelSize;

	Image *image2=new Image();
	image2->reallocate(image,false);

	unsigned char *data1=image->getData();
	unsigned char *data2=image2->getData();

	int j;
	for(j=0;j<height;++j){
		unsigned char *line1=data1+width*j*pixelSize;
		unsigned char *line2=data2+width*(height-1-j)*pixelSize;
		memcpy(line2,line1,amt);
	}

	return image2;
}

}
}
}

