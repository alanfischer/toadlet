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
#include <toadlet/peeper/SequenceTexture.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

SequenceTexture::SequenceTexture(Dimension dimension,int frames){
	mDimension=dimension;
	mTextures.resize(frames);
	mDelays.resize(frames,0);
}

SequenceTexture::~SequenceTexture(){
}

Texture *SequenceTexture::getRootTexture(scalar time){
	int i;
	for(i=0;i<mTextures.size();++i){
		scalar length=mTextures[i]!=NULL?mTextures[i]->getLength():0;
		length=length>0?length:mDelays[i];
		if(time>=length) time-=length;
		else return mTextures[i]->getRootTexture(time);
	}

	// Otherwise just return the last one if available
	if(mTextures.size()>0){
		return mTextures[mTextures.size()-1]->getRootTexture(time);
	}
	else{
		return NULL;
	}
}

bool SequenceTexture::getRootTransform(scalar time,Matrix4x4 &transform){
	int i;
	for(i=0;i<mTextures.size();++i){
		scalar length=mTextures[i]!=NULL?mTextures[i]->getLength():0;
		length=length>0?length:mDelays[i];
		if(time>=length) time-=length;
		else return mTextures[i]->getRootTransform(time,transform);
	}

	// Otherwise just return the last one if available
	if(mTextures.size()>0){
		return mTextures[mTextures.size()-1]->getRootTransform(time,transform);
	}
	else{
		return NULL;
	}
}

bool SequenceTexture::create(int usageFlags,Dimension dimension,int format,int width,int height,int depth,int mipLevels){
	Error::unimplemented("create not implemented for SequenceTexture");
	return false;
}

void SequenceTexture::destroy(){
	int i;
	for(i=0;i<mTextures.size();++i){
		if(mTextures[i]!=NULL){
			mTextures[i]->release();
		}
	}
	mTextures.clear();
}

bool SequenceTexture::createContext(){
	bool result=true;
	int i;
	for(i=0;i<mTextures.size();++i){
		if(mTextures[i]!=NULL){
			result&=mTextures[i]->createContext();
		}
	}
	return result;
}

void SequenceTexture::destroyContext(bool backData){
	int i;
	for(i=0;i<mTextures.size();++i){
		if(mTextures[i]!=NULL){
			mTextures[i]->destroyContext(backData);
		}
	}
}

bool SequenceTexture::contextNeedsReset(){
	int i;
	for(i=0;i<mTextures.size();++i){
		if(mTextures[i]!=NULL){
			if(mTextures[i]->contextNeedsReset()){
				return true;
			}
		}
	}
	return false;
}

int SequenceTexture::getWidth() const{
	return (mTextures.size()>0 && mTextures[0]!=NULL)?mTextures[0]->getWidth():0;
}

int SequenceTexture::getHeight() const{
	return (mTextures.size()>0 && mTextures[0]!=NULL)?mTextures[0]->getHeight():0;
}

int SequenceTexture::getDepth() const{
	return (mTextures.size()>0 && mTextures[0]!=NULL)?mTextures[0]->getDepth():0;
}

scalar SequenceTexture::getLength() const{
	scalar length=0;
	int i;
	for(i=0;i<mTextures.size();++i){
		scalar sublength=mTextures[i]!=NULL?mTextures[i]->getLength():0;
		length+=sublength>0?sublength:mDelays[i];
	}
	return length;
}

int SequenceTexture::getNumTextures() const{
	return mTextures.size();
}

Texture::ptr SequenceTexture::getTexture(int frame){
	return mTextures[frame];
}

void SequenceTexture::setTexture(int frame,peeper::Texture::ptr texture,scalar delay){
	if(mTextures[frame]!=NULL){
		mTextures[frame]->release();
	}

	mTextures[frame]=texture;
	mDelays[frame]=delay;

	if(mTextures[frame]!=NULL){
		mTextures[frame]->retain();
	}
}

}
}
