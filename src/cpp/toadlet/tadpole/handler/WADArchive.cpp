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
#include <toadlet/egg/io/DataStream.h>
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
	if(mStream!=NULL){
		mStream->close();
		mStream=NULL;
	}
}

bool WADArchive::open(Stream::ptr stream){
	destroy();

	mStream=DataStream::ptr(new DataStream(stream));
	mStream->read((byte*)mHeader.identification,sizeof(mHeader.identification));
	mHeader.numlumps=mStream->readLittleInt32();
	mHeader.infotableofs=mStream->readLittleInt32();

	if (strncmp(mHeader.identification,"WAD2",4) &&
		strncmp(mHeader.identification,"WAD3",4)){
		Error::unknown("invalid wad file");
		return false;
	}

	mLumpinfos.resize(mHeader.numlumps);
	mStream->seek(mHeader.infotableofs);

	mStream->read((byte*)&mLumpinfos[0],sizeof(wlumpinfo)*mHeader.numlumps);
	int i;
	for(i=0;i<mHeader.numlumps;i++){
		littleInt32(mLumpinfos[i].filepos);
		littleInt32(mLumpinfos[i].size);
		mNameMap[String(mLumpinfos[i].name).toLower()]=i;
	}

	return true;
}

Resource::ptr WADArchive::openResource(const String &name){
	Map<String,int>::iterator texindex=mNameMap.find(name.toLower());
	if(texindex==mNameMap.end()){
		return NULL;
	}
	
	wlumpinfo *info=&mLumpinfos[texindex->second];
	mStream->seek(info->filepos);
	mStream->read((byte*)mInBuffer,info->size);

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

	int j=0,k=0,j3=0,k3=0;
	for(j=0;j<height*width;j++){
		k=*(src+j);

		j3=j*3;
		k3=k*3;
		*(data+j3+0)=*(pal+k3+0);
		*(data+j3+1)=*(pal+k3+1);
		*(data+j3+2)=*(pal+k3+2);
	}

	return mTextureManager->createTexture(image);
}

Collection<String>::ptr WADArchive::getEntries(){
	Collection<String>::ptr entries(new Collection<String>());
	int i;
	for(i=0;i<mHeader.numlumps;i++){
		entries->add(mLumpinfos[i].name);
	}
	return entries;
}

}
}
}

