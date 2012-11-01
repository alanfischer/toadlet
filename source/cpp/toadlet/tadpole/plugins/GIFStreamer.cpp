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

/// @todo: The GIFStreamer needs to be cleaned up in several ways:
//  - Delays seem to be too long
//  - Transparent backgrounds dont work properly
//  - Some of the disposes seem to be broken, so it doesn't clear out the previous image correctly
//  - Not all extensions are properly handled

extern "C"{
	#include <gif_lib.h>
}
#include <toadlet/egg/Log.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/EndianConversion.h>
#include "GIFStreamer.h"

namespace toadlet{
namespace tadpole{

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

int GIFStreamer::getNextImage(tbyte *&data,int &delay,GifFileType *gifFile,TextureFormat *&format,tbyte *&base,tbyte *&working){
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
			bool bNetscapeExt = FALSE;
			switch (ExtCode){
				case COMMENT_EXT_FUNC_CODE:
					break;
				case GRAPHICS_EXT_FUNC_CODE:{
					int flag = pExtension[0];

					delay=(pExtension[2] << 8) | pExtension[1];
					if(delay<10){
						delay=10;
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

GifFileType *GIFStreamer::openFile(Stream *stream){
	GifFileType *file=DGifOpen((void*)stream,&readGifData);
	return file;
}

int GIFStreamer::closeFile(GifFileType *file){
	int result=DGifCloseFile(file);
	return result;
}

Resource::ptr GIFStreamer::load(Stream::ptr stream,ResourceData *data,ProgressListener *listener){
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
	TextureFormat *format=NULL;
	tbyte *image=NULL,*base=NULL,*working=NULL;
	int delay=0;
	int result=0;

	do{
		image=NULL;
		delay=0;
		result=getNextImage(image,delay,file,format,base,working);
		if(image!=NULL && result!=GIF_ERROR){
			images.add(image);
			delays.add(delay);
		}
	}while(result!=GIF_ERROR && image!=NULL);

	if(result!=GIF_ERROR){
		result=DGifCloseFile(file);
	}
	
	Texture::ptr texture;
	TextureFormat::ptr textureFormat(format);
	if(result!=GIF_ERROR){
		texture=mTextureManager->createTexture(usage,textureFormat,images[0]);
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

