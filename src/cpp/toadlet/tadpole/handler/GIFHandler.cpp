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

/// @todo: The GIFHandler needs to be cleaned up in several ways:
//  - Delays seem to be too long
//  - Transparent backgrounds dont work properly
//  - Some of the disposes seem to be broken, so it doesn't clear out the previous image correctly
//  - Not all extensions are properly handled

extern "C"{
	#include <gif_lib.h>
}
#include <toadlet/egg/EndianConversion.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/image/Image.h>
#include <toadlet/tadpole/handler/GIFHandler.h>

#if defined(TOADLET_PLATFORM_WIN32) && defined(TOADLET_LIBGIF_NAME)
	#pragma comment(lib,TOADLET_LIBGIF_NAME)
#endif`1

namespace toadlet{
namespace tadpole{
namespace handler{

int readGifData(GifFileType *file,GifByteType *data,int amount){
	Stream *stream=(Stream*)file->UserData;
	return stream->read(data,amount);
}

int writeGifData(GifFileType *file,GifByteType *data,int amount){
	Stream *stream=(Stream*)file->UserData;
	return stream->write(data,amount);
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

void setImagePortion(Image *dest,int x,int y,int width,int height,int red,int green,int blue){
	tbyte *data=dest->getData();
	int i,j;
	for(j=y;j<y+height;++j){
		for(i=x;i<x+width;++i){
			if(dest->getFormat()==Image::Format_RGB_8){
				data[(j*width+i)*3+0]=red;
				data[(j*width+i)*3+1]=green;
				data[(j*width+i)*3+2]=blue;
			}
			else{
				data[(j*width+i)*4+0]=red;
				data[(j*width+i)*4+1]=green;
				data[(j*width+i)*4+2]=blue;
				data[(j*width+i)*4+3]=0;
			}
		}
	}
}

int GIFHandler::getNextImage(Image *&image,int &frameDelay,GifFileType *gifFile,Image *&base,Image *&working){
	GifRecordType RecordType;
	GifByteType *pLine=NULL;
	GifByteType *pExtension=NULL;
	int dispose=GIF_DISPOSE_RESTORE;
	int pass=0;
	int transparent=GIF_NOT_TRANSPARENT;

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

				const int width=gifFile->Image.Width;
				const int height=gifFile->Image.Height;

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

				if(working==NULL){
					working=Image::createAndReallocate(Image::Dimension_D2,format,width,height);
					if(working==NULL){
						return GIF_ERROR;
					}

					memset(working->getData(),0,pixelSize*width*height);

					GifColorType *color=gifFile->SColorMap->Colors + gifFile->SBackGroundColor;
					setImagePortion(working,gifFile->Image.Left,gifFile->Image.Top,gifFile->Image.Width,gifFile->Image.Height,color->Red,color->Green,color->Blue);
				}

				if(base==NULL){
					base=Image::createAndReallocate(Image::Dimension_D2,format,width,height);
					if(base==NULL){
						return GIF_ERROR;
					}
					
					memset(working->getData(),0,pixelSize*width*height);

					GifColorType *color=gifFile->SColorMap->Colors + gifFile->SBackGroundColor;
					setImagePortion(working,gifFile->Image.Left,gifFile->Image.Top,gifFile->Image.Width,gifFile->Image.Height,color->Red,color->Green,color->Blue);
				}

				pLine=new GifByteType[width*pixelSize];

				unsigned int workingWidth=working->getWidth();
				unsigned int workingHeight=working->getHeight();

				if(gifFile->Image.Interlace){
					// Need to perform 4 passes on the images:
					for(pass=0;pass<4;++pass){
						for(i=InterlacedOffset[pass];i<height;i+=InterlacedJumps[pass]){
							if(DGifGetLine(gifFile,pLine,width)==GIF_ERROR){
								delete[] pLine;
								return GIF_ERROR;
							}
							copyGIFLine(working->getData(),workingWidth,workingHeight,x,y,width,height,width,i,pLine,transparent,pColorTable,pixelSize,format);
 						}
					}
				}
				else{
					// Non-interlaced image
					for(i=0;i<height;++i){
						if(DGifGetLine(gifFile,pLine,width)==GIF_ERROR){
							delete[] pLine;
							return GIF_ERROR;
						}
						copyGIFLine(working->getData(),workingWidth,workingHeight,x,y,width,height,width,i,pLine,transparent,pColorTable,pixelSize,format);
					}
				}
				delete[] pLine;

				image=flipImage(working);

				if(dispose == GIF_DISPOSE_LEAVE){
					memcpy(base->getData(),working->getData(),working->getPixelSize()*working->getWidth()*working->getHeight());
				}
 				else if(dispose == GIF_DISPOSE_BACKGND){
					GifColorType *color=gifFile->SColorMap->Colors + gifFile->SBackGroundColor;
					setImagePortion(working,gifFile->Image.Left,gifFile->Image.Top,gifFile->Image.Width,gifFile->Image.Height,color->Red,color->Green,color->Blue);
				}
				else if(dispose == GIF_DISPOSE_RESTORE){
					const int x = gifFile->Image.Left;
					const int y = gifFile->Image.Top;
					const int width = gifFile->Image.Width;
					const int height = gifFile->Image.Height;
					const int stride = width*pixelSize;

					int j;
					for(j=y;j<y+height;++j){
						memcpy(working->getData()+y*stride+x,base->getData()+y*stride+x,stride);
					}
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
					int flag = pExtension[0];

					frameDelay=(pExtension[2] << 8) | pExtension[1];
					if(frameDelay<10){
						frameDelay=10;
					}
			
					if((flag&GIF_TRANSPARENT)!=0){
						transparent=pExtension[3];
					}
					else{
						transparent=GIF_NOT_TRANSPARENT;
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

GifFileType *GIFHandler::openFile(Stream *stream){
	GifFileType *file=DGifOpen((void*)stream,&readGifData);
	return file;
}

int GIFHandler::closeFile(GifFileType *file){
	int result=DGifCloseFile(file);
	return result;
}

Resource::ptr GIFHandler::load(Stream::ptr stream,ResourceData *data,ProgressListener *listener){
	if(stream==NULL){
		Error::nullPointer(Categories::TOADLET_TADPOLE,
			"Stream is NULL");
		return false;
	}

	GifFileType *file=openFile(stream);

	if(file==NULL){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"GIFHandler::loadAnimatedImage: Invalid file");
		return false;
	}

	Collection<int> frameDelays;
	Collection<Image::ptr> images;
	Image *image=NULL,*base=NULL,*working=NULL;
	int frameDelay=0;
	int result=0;

	do{
		image=NULL;
		frameDelay=0;
		result=getNextImage(image,frameDelay,file,base,working);
		if(image!=NULL && result!=GIF_ERROR){
			images.add(Image::ptr(image));
			frameDelays.add(frameDelay);
		}
	}while(result!=GIF_ERROR && image!=NULL);

	if(result!=GIF_ERROR){
		result=DGifCloseFile(file);
	}
	
	Texture::ptr texture;
	if(result!=GIF_ERROR){
		Image::ptr image=images[0];
		if(images.size()>1){
			Image::ptr image3d(Image::createAndReallocate(Image::Dimension_D3,image->getFormat(),image->getWidth(),image->getHeight(),images.size()));
			int i;
			for(i=0;i<images.size();++i){
				memcpy(image3d->getData()+image3d->getSlicePitch()*i,images[i]->getData(),image3d->getSlicePitch());
			}
			texture=mTextureManager->createTexture(image3d);
		}
		else{
			texture=mTextureManager->createTexture(image);
		}
	}
	
	delete base;
	delete working;
	
	if(result==GIF_ERROR){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"GIFHandler::loadAnimatedImage: Error loading image");
		return NULL;
	}

	return texture;
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

