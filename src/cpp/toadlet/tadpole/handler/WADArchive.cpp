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

#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/EndianConversion.h>
#include <toadlet/egg/io/DataInputStream.h>
#include <toadlet/tadpole/handler/WADArchive.h>
#include <string.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::egg::image;

namespace toadlet{
namespace tadpole{
namespace handler{

WADArchive::WADArchive(TextureManager *textureManager){
	mTextureManager=textureManager;
};

WADArchive::~WADArchive(){
	destroy();
}

void WADArchive::destroy(){
	if(mInputStream!=NULL){
		mInputStream->close();
		mInputStream=NULL;
	}
}

bool WADArchive::open(InputStream::ptr stream){
	destroy();

	mInputStream=DataInputStream::ptr(new DataInputStream(stream));
	mInputStream->read(header.identification,sizeof(header.identification));
	header.numlumps=mInputStream->readLittleInt32();
	header.infotableofs=mInputStream->readLittleInt32();

	if (strncmp(header.identification,"WAD2",4) &&
		strncmp(header.identification,"WAD3",4)){
		Error::unknown("invalid wad file");
		return false;
	}

	lumpinfos.resize(header.numlumps);
	mInputStream->seek(header.infotableofs);

	mInputStream->read((char*)&lumpinfos[0],sizeof(wlumpinfo)*header.numlumps);

	int i;
	for(i=0;i<header.numlumps;i++){
		littleInt32(lumpinfos[i].filepos);
		littleInt32(lumpinfos[i].size);
	}

	return true;
}

Resource::ptr WADArchive::openResource(const String &name){
	String lowername=name.toLower();

	int i;
	for(i=0;i<header.numlumps;i++){
		String lumpname=lumpinfos[i].name;
		if(lowername.equals(lumpname.toLower())){
			mInputStream->seek(lumpinfos[i].filepos);
			mInputStream->read((char*)mInBuffer,lumpinfos[i].size);

			wmiptex *tex=(wmiptex*)mInBuffer;

			int width=tex->width;
			littleInt32(width);

			int height=tex->height;
			littleInt32(height);

			Image::ptr image(new Image(Image::Dimension_D2,Image::Format_RGB_8,width,height));

			int off=tex->offsets[0];
			littleInt32(off);
			uint8 *src=mInBuffer+off;

			off=tex->offsets[3];
			littleInt32(off);
			uint8 *pal=mInBuffer+off+width*height/64+2;

			uint8 *data=image->getData();

			int j=0,k=0;
			for(j=0;j<height*width;j++){
				k=*(src+j);
				k=k*3;

				*(data+j*3+0)=*(k+pal+0);
				*(data+j*3+1)=*(k+pal+1);
				*(data+j*3+2)=*(k+pal+2);
			}

			return mTextureManager->createTexture(image);
		}
	}

	return NULL;
}

}
}
}

