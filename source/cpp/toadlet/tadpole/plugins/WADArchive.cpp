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

#include <toadlet/egg/EndianConversion.h>
#include <toadlet/egg/io/DataStream.h>
#include "WADArchive.h"

namespace toadlet{
namespace tadpole{

WADArchive::WADArchive(TextureManager *textureManager){
	mTextureManager=textureManager;
};

void WADArchive::destroy(){
	if(mStream!=NULL){
		mStream->close();
		mStream=NULL;
	}
}

bool WADArchive::open(Stream::ptr stream){
	destroy();

	mStream=new DataStream(stream);
	mStream->read((tbyte*)mHeader.identification,sizeof(mHeader.identification));
	mHeader.numlumps=mStream->readLInt32();
	mHeader.infotableofs=mStream->readLInt32();

	if (strncmp(mHeader.identification,"WAD2",4) &&
		strncmp(mHeader.identification,"WAD3",4)){
		Error::unknown("invalid wad file");
		return false;
	}

	mLumpinfos.resize(mHeader.numlumps);
	mStream->seek(mHeader.infotableofs);

	mStream->read((tbyte*)&mLumpinfos[0],sizeof(wlumpinfo)*mHeader.numlumps);
	int i;
	for(i=0;i<mHeader.numlumps;i++){
		mLumpinfos[i].filepos=littleInt32(mLumpinfos[i].filepos);
		mLumpinfos[i].size=littleInt32(mLumpinfos[i].size);
		mNameMap.add(String(mLumpinfos[i].name).toLower(),i);
		mEntries.add(mLumpinfos[i].name);
	}

	return true;
}

Resource::ptr WADArchive::openResource(const String &name){
	Map<String,int>::iterator texindex=mNameMap.find(name.toLower());
	if(texindex==mNameMap.end()){
		//Error::fileNotFound(Categories::TOADLET_TADPOLE,"resource not found");
		return false;
	}
	
	wlumpinfo *info=&mLumpinfos[texindex->second];
	mStream->seek(info->filepos);
	mStream->read((tbyte*)mInBuffer,info->size);

	Texture::ptr texture=createTexture(mTextureManager,(wmiptex*)mInBuffer);
	return texture;
}

Texture::ptr WADArchive::createTexture(toadlet::tadpole::TextureManager *textureManager,wmiptex *miptex,tbyte *pal){
	int width=littleInt32(miptex->width),height=littleInt32(miptex->height);
	int size=width*height;

	if(size<=0 || littleInt32(miptex->offsets[0])==0){
		return NULL;
	}

	int datasize=size + (size/4) + (size/16) + (size/64);
	if(pal==NULL){
		pal=(tbyte*)miptex + littleInt32(miptex->offsets[0]) + datasize + 2;
	}

	int pixelFormat=TextureFormat::Format_RGB_8;
	if(miptex->name[0]=='{'){
		pixelFormat=TextureFormat::Format_RGBA_8;
	}

	TextureFormat::ptr textureFormat=new TextureFormat(TextureFormat::Dimension_D2,pixelFormat,width,height,1,4);
	tbyte *mipDatas[4];
	int mipLevel;
	for(mipLevel=0;mipLevel<4;++mipLevel){
		TextureFormat::ptr mipFormat=new TextureFormat(textureFormat,mipLevel);
		mipDatas[mipLevel]=new tbyte[mipFormat->getDataSize()];

		tbyte *src=(tbyte*)miptex + littleInt32(miptex->offsets[mipLevel]);
		tbyte *data=mipDatas[mipLevel];
		int widthheight=mipFormat->getWidth()*mipFormat->getHeight();

		int j=0,k=0,j3=0,k3=0;
		if(pixelFormat==TextureFormat::Format_RGB_8){
			for(j=0;j<widthheight;j++){
				k=*(src+j);

				j3=j*3;
				k3=k*3;
				*(data+j3+0)=*(pal+k3+0);
				*(data+j3+1)=*(pal+k3+1);
				*(data+j3+2)=*(pal+k3+2);
			}
		}
		else{
			for(j=0;j<widthheight;j++){
				k=*(src+j);

				j3=j*4;
				k3=k*3;
				if(*(pal+k3+0)==0 && *(pal+k3+1)==0 && *(pal+k3+2)==255){
					*(data+j3+0)=0;
					*(data+j3+1)=0;
					*(data+j3+2)=0;
					*(data+j3+3)=0;
				}
				else{
					*(data+j3+0)=*(pal+k3+0);
					*(data+j3+1)=*(pal+k3+1);
					*(data+j3+2)=*(pal+k3+2);
					*(data+j3+3)=255;
				}
			}
		}
	}

	Texture::ptr texture=textureManager->createTexture(textureFormat,mipDatas);
	if(texture!=NULL){
		textureManager->manage(texture,miptex->name);
	}

	return texture;
}

}
}

