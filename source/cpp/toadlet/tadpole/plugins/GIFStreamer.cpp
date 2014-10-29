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
#include <toadlet/egg/EndianConversion.h>
#include "GIFStreamer.h"

namespace toadlet{
namespace tadpole{

const int InterlacedOffset[] = { 0, 4, 2, 1 };	// The way Interlaced image should
const int InterlacedJumps[] = { 8, 8, 4, 2 };	//  be read - offsets and jumps...

GifByteType Netscape20ext[] = "\x0bNETSCAPE2.0";
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

int readGifData(GifFileType *file,GifByteType *data,int amount){
	Stream *stream=(Stream*)file->UserData;
	return stream->read(data,amount);
}

int writeGifData(GifFileType *file,GifByteType *data,int amount){
	Stream *stream=(Stream*)file->UserData;
	return stream->write(data,amount);
}

void copyGIFLine(	unsigned char *dest,
					unsigned int iwidth,unsigned int iheight,
					unsigned int xoff,unsigned int yoff,
					unsigned int twidth,unsigned int theight,
					unsigned int amt,unsigned int yoff2,
					unsigned char *src,int trans,GifColorType *colorTable,int pixelSize,int pixelFormat){

	dest=dest + (iwidth*pixelSize*(yoff+yoff2)) + xoff*pixelSize;

	int i;
	for(i=0;i<amt;++i){
		if(pixelFormat==TextureFormat::Format_RGB_8){
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

void setImagePortion(TextureFormat *format,tbyte *data,int x,int y,int width,int height,int red,int green,int blue){
	int i,j;
	for(j=y;j<y+height;++j){
		for(i=x;i<x+width;++i){
			if(format->getPixelFormat()==TextureFormat::Format_RGB_8){
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

int GIFStreamer::getNextImage(GifFileType *gifFile,TextureFormat::ptr &format,tbyte *&base,tbyte *&working,int &delay){
	GifRecordType RecordType;
	GifByteType *pLine=NULL;
	GifByteType *pExtension=NULL;
	int dispose=GIF_DISPOSE_RESTORE;
	int pass=0;
	int transparent=GIF_NOT_TRANSPARENT;
	int loopCount=0; // This is currently set, but unused

	do{
		int i, ExtCode;
		if(DGifGetRecordType(gifFile,&RecordType)==GIF_ERROR){
			return -1;
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

				int pixelFormat=TextureFormat::Format_UNKNOWN;
				int pixelSize=0;
				if(transparent==GIF_NOT_TRANSPARENT){
					pixelFormat=TextureFormat::Format_RGB_8;
					pixelSize=3;
				}
				else{
					pixelFormat=TextureFormat::Format_RGBA_8;
					pixelSize=4;
				}

				if(format==NULL){
					format=new TextureFormat(TextureFormat::Dimension_D2,pixelFormat,width,height,1,0);
				}

				if(working==NULL){
					working=new tbyte[format->getDataSize()];
					memset(working,0,format->getDataSize());

					GifColorType *color=gifFile->SColorMap->Colors + gifFile->SBackGroundColor;
					setImagePortion(format,working,gifFile->Image.Left,gifFile->Image.Top,gifFile->Image.Width,gifFile->Image.Height,color->Red,color->Green,color->Blue);
				}

				if(base==NULL){
					base=new tbyte[format->getDataSize()];
					memset(base,0,format->getDataSize());
	
					GifColorType *color=gifFile->SColorMap->Colors + gifFile->SBackGroundColor;
					setImagePortion(format,base,gifFile->Image.Left,gifFile->Image.Top,gifFile->Image.Width,gifFile->Image.Height,color->Red,color->Green,color->Blue);
				}

				pLine=new GifByteType[width*pixelSize];

				unsigned int workingWidth=format->getWidth();
				unsigned int workingHeight=format->getHeight();

				if(gifFile->Image.Interlace){
					// Need to perform 4 passes on the images:
					for(pass=0;pass<4;++pass){
						for(i=InterlacedOffset[pass];i<height;i+=InterlacedJumps[pass]){
							if(DGifGetLine(gifFile,pLine,width)==GIF_ERROR){
								delete[] pLine;
								return GIF_ERROR;
							}
							copyGIFLine(working,workingWidth,workingHeight,x,y,width,height,width,i,pLine,transparent,pColorTable,pixelSize,pixelFormat);
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
						copyGIFLine(working,workingWidth,workingHeight,x,y,width,height,width,i,pLine,transparent,pColorTable,pixelSize,pixelFormat);
					}
				}
				delete[] pLine;

				if(dispose == GIF_DISPOSE_LEAVE){
					memcpy(base,working,format->getDataSize());
				}
 				else if(dispose == GIF_DISPOSE_BACKGND){
					GifColorType *color=gifFile->SColorMap->Colors + gifFile->SBackGroundColor;
					setImagePortion(format,working,gifFile->Image.Left,gifFile->Image.Top,gifFile->Image.Width,gifFile->Image.Height,color->Red,color->Green,color->Blue);
				}
				else if(dispose == GIF_DISPOSE_RESTORE){
					const int x = gifFile->Image.Left;
					const int y = gifFile->Image.Top;
					const int width = gifFile->Image.Width;
					const int height = gifFile->Image.Height;
					const int stride = width*pixelSize;

					int j;
					for(j=y;j<y+height;++j){
						memcpy(working+y*stride+x,base+y*stride+x,stride);
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
			bool bNetscapeExt = false;
			switch (ExtCode){
				case COMMENT_EXT_FUNC_CODE:
					break;
				case GRAPHICS_EXT_FUNC_CODE:{
					int flag = pExtension[1];

					delay=(pExtension[2] | (pExtension[3] << 8));
					if(delay<10){
						delay=10;
					}
			
					transparent = (flag & GIF_TRANSPARENT) ? pExtension[4] : GIF_NOT_TRANSPARENT;
					dispose = (flag >> GIF_DISPOSE_SHIFT) & GIF_DISPOSE_MASK;

					break;
				}
				case PLAINTEXT_EXT_FUNC_CODE:
					break;
				case APPLICATION_EXT_FUNC_CODE:
					if (memcmp(pExtension, Netscape20ext, Netscape20ext[0]) == 0) {
						bNetscapeExt = true;
					}
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
						loopCount=(pExtension[2] | (pExtension[3] << 8)) - 1;
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

GifFileType *GIFStreamer::openFile(Stream *stream){
#if defined(GIFLIB_MAJOR) && (GIFLIB_MAJOR >= 5)
	GifFileType *file=DGifOpen((void*)stream,&readGifData,NULL);
#else
	GifFileType *file=DGifOpen((void*)stream,&readGifData);
#endif
	return file;
}

int GIFStreamer::closeFile(GifFileType *file){
#if defined(GIFLIB_MAJOR) && (GIFLIB_MAJOR >= 5)
	int result=DGifCloseFile(file,NULL);
#else
	int result=DGifCloseFile(file);
#endif
	return result;
}

Resource::ptr GIFStreamer::load(Stream::ptr stream,ResourceData *data){
	int usage=(data!=NULL)?((TextureData*)data)->usage:0;

	if(stream==NULL){
		Error::nullPointer(Categories::TOADLET_TADPOLE,
			"Stream is NULL");
		return NULL;
	}

	GifFileType *file=openFile(stream);

	if(file==NULL){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"GIFStreamer::loadAnimatedImage: Invalid file");
		return NULL;
	}

	Collection<tbyte*> images;
	Collection<int> delays;
	TextureFormat::ptr format=NULL;
	tbyte *base=NULL,*working=NULL;
	int delay=0;
	int result=0;

	do{
		delay=0;
		result=getNextImage(file,format,base,working,delay);
		if(working!=NULL && result!=GIF_ERROR){
			tbyte *image=new tbyte[format->getDataSize()];
			memcpy(image,working,format->getDataSize());
			images.push_back(image);
			delays.push_back(delay);
		}
	}while(result==GIF_OK && working!=NULL);

	if(result!=GIF_ERROR){
		result=closeFile(file);
	}
	
	Texture::ptr texture;
	if(result!=GIF_ERROR){
		texture=mTextureManager->createTexture(usage,format,images[0]);
	}
	
	delete[] base;
	delete[] working;
	int i;
	for(i=0;i<images.size();++i){
		delete[] images[i];
	}
	
	if(result==GIF_ERROR){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"GIFStreamer::loadAnimatedImage: Error loading image");
		return NULL;
	}

	return texture;
}

}
}

