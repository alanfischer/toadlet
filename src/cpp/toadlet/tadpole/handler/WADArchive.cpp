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
#include <toadlet/peeper/CapabilitySet.h>
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
	mStream->read((tbyte*)mHeader.identification,sizeof(mHeader.identification));
	mHeader.numlumps=mStream->readLittleInt32();
	mHeader.infotableofs=mStream->readLittleInt32();

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
	mStream->read((tbyte*)mInBuffer,info->size);

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
	int swidth=littleInt32(miptex->width),sheight=littleInt32(miptex->height);
	int dwidth=swidth,dheight=sheight;
	int size=swidth*sheight;
	bool hasNonPowerOf2=textureManager->getRenderer()==NULL?false:textureManager->getRenderer()->getCapabilitySet().textureNonPowerOf2;

	if(size<=0 || littleInt32(miptex->offsets[0])==0){
		return NULL;
	}

	if(hasNonPowerOf2==false && (Math::isPowerOf2(swidth)==false || Math::isPowerOf2(sheight)==false)){
		dwidth=Math::nextPowerOf2(swidth)>>1;
		dheight=Math::nextPowerOf2(sheight)>>1;
	}

	int datasize=size + (size/4) + (size/16) + (size/64);
	tbyte *pal=(tbyte*)miptex + littleInt32(miptex->offsets[0]) + datasize + 2;

	int format=Texture::Format_RGB_8;
	if(miptex->name[0]=='{'){
		format=Texture::Format_RGBA_8;
	}

	Image::ptr images[4];
	int hswidth=swidth,hsheight=sheight;
	int hdwidth=dwidth,hdheight=dheight;
	int mipLevel;
	for(mipLevel=0;mipLevel<4;++mipLevel){
		images[mipLevel]=Image::ptr(new Image(Image::Dimension_D2,format,dwidth,dheight));

		tbyte *src=(tbyte*)miptex + littleInt32(miptex->offsets[mipLevel]);
		tbyte *data=images[mipLevel]->getData();

		if(hswidth==hdwidth && hsheight==hdheight){
			int j=0,k=0,j3=0,k3=0;
			if(format==Texture::Format_RGB_8){
				for(j=0;j<hswidth*hsheight;j++){
					k=*(src+j);

					j3=j*3;
					k3=k*3;
					*(data+j3+0)=*(pal+k3+0);
					*(data+j3+1)=*(pal+k3+1);
					*(data+j3+2)=*(pal+k3+2);
				}
			}
			else{
				for(j=0;j<hswidth*hsheight;j++){
					k=*(src+j);

					j3=j*4;
					k3=k*3;
					*(data+j3+0)=*(pal+k3+0);
					*(data+j3+1)=*(pal+k3+1);
					*(data+j3+2)=*(pal+k3+2);
					*(data+j3+3)=(*(data+j3+0)==0 && *(data+j3+1)==0 && *(data+j3+2)==255)?0:255;
				}
			}
		}
		else{
			int i=0,j=0,k=0,j3=0,k3=0;
			if(format==Texture::Format_RGB_8){
				for(j=0;j<hdheight;j++){
					for(i=0;i<hdwidth;i++){
						k=*(src+((j*hsheight/hdheight)*hswidth+(i*hswidth/hdwidth)));

						j3=((j*hdwidth)+i)*3;
						k3=k*3;
						*(data+j3+0)=*(pal+k3+0);
						*(data+j3+1)=*(pal+k3+1);
						*(data+j3+2)=*(pal+k3+2);
					}
				}
			}
			else{
				for(j=0;j<hdheight;j++){
					for(i=0;i<hdwidth;i++){
						k=*(src+((j*hsheight/hdheight)*hswidth+(i*hswidth/hdwidth)));

						j3=((j*hdwidth)+i)*4;
						k3=k*3;
						*(data+j3+0)=*(pal+k3+0);
						*(data+j3+1)=*(pal+k3+1);
						*(data+j3+2)=*(pal+k3+2);
						*(data+j3+3)=(*(data+j3+0)==0 && *(data+j3+1)==0 && *(data+j3+2)==255)?0:255;
					}
				}
			}
		}

		if(hswidth>=2) hswidth/=2; if(hsheight>=2) hsheight/=2;
		if(hdwidth>=2) hdwidth/=2; if(hdheight>=2) hdheight/=2;
	}

	Texture::ptr texture=textureManager->createTexture(images,0,4);
	if(texture!=NULL){
		textureManager->manage(texture,miptex->name);
	}

	return texture;
}

}
}
}

