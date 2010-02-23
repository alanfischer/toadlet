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

#include <toadlet/egg/image/SPRHandler.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>

using namespace toadlet::egg::io;

namespace toadlet{
namespace egg{
namespace image{

#define SPRITE_VERSION 2

typedef enum{
	ST_SYNC=0,
	ST_RAND
} synctype_t;

typedef struct{
	int ident;
	int version;
	int type;
	int texFormat;
	float boundingradius;
	int width;
	int height;
	int numframes;
	float beamlength;
	synctype_t synctype;
} dsprite_t;

#define SPR_VP_PARALLEL_UPRIGHT		0
#define SPR_FACING_UPRIGHT			1
#define SPR_VP_PARALLEL				2
#define SPR_ORIENTED				3
#define SPR_VP_PARALLEL_ORIENTED	4

#define SPR_NORMAL					0
#define SPR_ADDITIVE				1
#define SPR_INDEXALPHA				2
#define SPR_ALPHTEST				3

typedef struct{
	int			origin[2];
	int			width;
	int			height;
} dspriteframe_t;

typedef struct{
	int			numframes;
} dspritegroup_t;

typedef struct{
	float	interval;
} dspriteinterval_t;

typedef enum{
	SPR_SINGLE=0,
	SPR_GROUP
} spriteframetype_t;

typedef struct{
	spriteframetype_t	type;
} dspriteframetype_t;

#define IDSPRITEHEADER	(('P'<<24)+('S'<<16)+('D'<<8)+'I')

SPRHandler::SPRHandler():
	//mDataStream,
	mSprite(NULL),
	mPalette(NULL),
	mNextFrame(0)
{}

SPRHandler::~SPRHandler(){
	closeFile();
}

bool SPRHandler::openFile(Stream *stream){
	DataStream::ptr dataStream(new DataStream(stream));

	dsprite_t *sprite=new dsprite_t;

	sprite->ident=dataStream->readLittleInt32();
	sprite->version=dataStream->readLittleInt32();
	if(sprite->ident!=IDSPRITEHEADER || sprite->version!=SPRITE_VERSION){
		delete sprite;
		delete dataStream;
		Error::unknown(Categories::TOADLET_EGG,"invalid sprite file or version");
		return false;
	}

	sprite->type=dataStream->readLittleInt32();
	sprite->texFormat=dataStream->readLittleInt32();
	sprite->boundingradius=dataStream->readLittleFloat();
	sprite->width=dataStream->readLittleInt32();
	sprite->height=dataStream->readLittleInt32();
	sprite->numframes=dataStream->readLittleInt32();
	sprite->beamlength=dataStream->readLittleFloat();
	sprite->synctype=(synctype_t)dataStream->readLittleInt32();

	short paletteSize=0;
	paletteSize=dataStream->readLittleInt16();

	byte *palette=new byte[paletteSize*3];
	dataStream->read(palette,paletteSize*3);

	mDataStream=dataStream;
	mSprite=sprite;
	mPalette=palette;
	mNextFrame=0;

	return true;
}

void SPRHandler::closeFile(){
	if(mDataStream!=NULL){
		mDataStream->close();
		mDataStream=NULL;
	}

	if(mSprite!=NULL){
		delete (dsprite_t*)mSprite;
		mSprite=NULL;
	}

	if(mPalette!=NULL){
		delete[] mPalette;
		mPalette=NULL;
	}

	mNextFrame=0;
}

Image *SPRHandler::getNextImage(){
	dspriteframetype_t frameType;
	dspriteframe_t frame;

	Image *image=NULL;

	if(mNextFrame<((dsprite_t*)mSprite)->numframes){
		frameType.type=(spriteframetype_t)mDataStream->readLittleInt32();
		if(frameType.type==SPR_SINGLE){
			frame.origin[0]=mDataStream->readLittleInt32();
			frame.origin[1]=mDataStream->readLittleInt32();
			frame.width=mDataStream->readLittleInt32();
			frame.height=mDataStream->readLittleInt32();

			int readSize=(frame.width*frame.height);
			byte *data=new byte[readSize];
			mDataStream->read(data,readSize);

			image=new Image(Image::Dimension_D2,Image::Format_RGBA_8,frame.width,frame.height,1);
			byte *imageData=image->getData();
			int x,y;
			for(y=0;y<frame.height;++y){
				for(x=0;x<frame.width;++x){
					int a=(y*frame.width+x);

					if(data[a]==255){
						imageData[a*4+0]=0;
						imageData[a*4+1]=0;
						imageData[a*4+2]=0;
						imageData[a*4+3]=0;
					}
					else{
						imageData[a*4+0]=mPalette[data[a]*3+0];
						imageData[a*4+1]=mPalette[data[a]*3+1];
						imageData[a*4+2]=mPalette[data[a]*3+2];
						imageData[a*4+3]=255;
					}
				}
			}
			delete[] data;
		}
		else{
			// ?
		}
	}

	mNextFrame++;

	return image;
}

Image *SPRHandler::loadImage(Stream *stream){
	if(stream==NULL){
		Error::nullPointer(Categories::TOADLET_EGG,
			"Stream is NULL");
		return NULL;
	}

	if(openFile(stream)==false){
		Error::loadingImage(Categories::TOADLET_EGG,
			"SPRHandler::loadImage: Invalid file");
		return NULL;
	}

	Image *image=getNextImage();

	closeFile();

	return image;
}

bool SPRHandler::saveImage(Image *image,Stream *stream){
	Error::unimplemented(Categories::TOADLET_EGG,
		"SPRHandler::saveImage: Not implemented");
	return false;
}

bool SPRHandler::loadAnimatedImage(Stream *stream,Collection<Image*> &images,Collection<int> &frameDelays){
	if(stream==NULL){
		Error::nullPointer(Categories::TOADLET_EGG,
			"Stream is NULL");
		return false;
	}

	if(openFile(stream)==false){
		Error::loadingImage(Categories::TOADLET_EGG,
			"SPRHandler::loadImage: Invalid file");
		return false;
	}

	Image *image=NULL;
	while((image=getNextImage())!=NULL){
		images.add(image);
		frameDelays.add(100); // Just default to 100ms
	}

	closeFile();

	return true;
}

}
}
}
