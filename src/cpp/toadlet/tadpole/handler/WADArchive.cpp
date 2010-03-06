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
using namespace toadlet::peeper;

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
		mLumpinfos[i].filepos=littleInt32(mLumpinfos[i].filepos);
		mLumpinfos[i].size=littleInt32(mLumpinfos[i].size);
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

	return createTexture(mTextureManager,(wmiptex*)mInBuffer);
}

Collection<String>::ptr WADArchive::getEntries(){
	Collection<String>::ptr entries(new Collection<String>());
	int i;
	for(i=0;i<mHeader.numlumps;i++){
		entries->add(mLumpinfos[i].name);
	}
	return entries;
}

peeper::Texture::ptr WADArchive::createTexture(toadlet::tadpole::TextureManager *textureManager,wmiptex *miptex){
	int width=littleInt32(miptex->width);
	int height=littleInt32(miptex->height);
	int size=width*height;

	if(size<=0 || littleInt32(miptex->offsets[0])==0){
		return NULL;
	}

	int datasize=size + (size/4) + (size/16) + (size/64);
	byte *pal=(byte*)miptex + littleInt32(miptex->offsets[0]) + datasize + 2;

	int format=Texture::Format_RGB_8;
	if(miptex->name[0]=='{'){
		format=Texture::Format_RGBA_8;
	}
	
	Texture::ptr texture=textureManager->createTexture(0,Texture::Dimension_D2,format,width,height,0,4);
	if(texture!=NULL){
		textureManager->manage(texture,miptex->name);

		Image::ptr image(new Image(Image::Dimension_D2,format,width,height));

		int hwidth=width,hheight=height;
		int mipLevel;
		for(mipLevel=0;mipLevel<4;++mipLevel){
			byte *src=(byte*)miptex + littleInt32(miptex->offsets[mipLevel]);
			byte *data=image->getData();

			int j=0,k=0,j3=0,k3=0;
			if(format==Texture::Format_RGB_8){
				for(j=0;j<hwidth*hheight;j++){
					k=*(src+j);

					j3=j*3;
					k3=k*3;
					*(data+j3+0)=*(pal+k3+0);
					*(data+j3+1)=*(pal+k3+1);
					*(data+j3+2)=*(pal+k3+2);
				}
			}
			else{
				for(j=0;j<hwidth*hheight;j++){
					k=*(src+j);

					j3=j*4;
					k3=k*3;
					*(data+j3+0)=*(pal+k3+0);
					*(data+j3+1)=*(pal+k3+1);
					*(data+j3+2)=*(pal+k3+2);
					*(data+j3+3)=(*(data+j3+0)==0 && *(data+j3+1)==0 && *(data+j3+2)==255)?0:255;
				}
			}

			texture->load(format,hwidth,hheight,0,mipLevel,image->getData());
			
			if(hwidth>=2) hwidth/=2;
			if(hheight>=2) hheight/=2;
		}
	}

	return texture;
}

}
}
}

