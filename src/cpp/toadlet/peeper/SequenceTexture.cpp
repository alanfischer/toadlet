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
#include <toadlet/peeper/SequenceTexture.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

SequenceTexture::SequenceTexture(Dimension dimension,int frames){
	mDimension=dimension;
	mTextures.resize(frames);
}

SequenceTexture::~SequenceTexture(){
}

Texture *SequenceTexture::getRootTexture(int frame){
	int i;
	for(i=0;i<mTextures.size();++i){
		if(mTextures[i]!=NULL){
			int frames=0;
			switch(mDimension){
				case(Dimension_D1):
					frames=mTextures[i]->getHeight();
				break;
				case(Dimension_D2):
					frames=mTextures[i]->getDepth();
				break;
			}
			if(frame>frames){
				frame-=frames;
			}
			else{
				return mTextures[i]->getRootTexture(frame);
			}
		}
	}
	return NULL;
}

bool SequenceTexture::getRootTransform(int frame,Matrix4x4 &transform){
	int i;
	for(i=0;i<mTextures.size();++i){
		if(mTextures[i]!=NULL){
			int frames=0;
			switch(mDimension){
				case(Dimension_D1):
					frames=mTextures[i]->getHeight();
				break;
				case(Dimension_D2):
					frames=mTextures[i]->getDepth();
				break;
			}
			if(frame>frames){
				frame-=frames;
			}
			else{
				return mTextures[i]->getRootTransform(frame,transform);
			}
		}
	}
	return true;
}

bool SequenceTexture::create(int usageFlags,Dimension dimension,int format,int width,int height,int depth,int mipLevels){
	Error::unimplemented("create not implemented for SequenceTexture");
	return false;
}

void SequenceTexture::destroy(){
	int i;
	for(i=0;i<mTextures.size();++i){
		if(mTextures[i]!=NULL){
			mTextures[i]->destroy();
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
	if(mDimension==Dimension_D1){
		return getTotalNumTextures();
	}
	else{
		return (mTextures.size()>0 && mTextures[0]!=NULL)?mTextures[0]->getHeight():0;
	}
}

int SequenceTexture::getDepth() const{
	if(mDimension==Dimension_D2){
		return getTotalNumTextures();
	}
	else{
		return (mTextures.size()>0 && mTextures[0]!=NULL)?mTextures[0]->getDepth():0;
	}
}

int SequenceTexture::getTotalNumTextures() const{
	int frames=0;
	int i;
	for(i=0;i<mTextures.size();++i){
		if(mTextures[i]!=NULL){
			switch(mDimension){
				case(Dimension_D1):
					frames+=mTextures[i]->getHeight();
				break;
				case(Dimension_D2):
					frames+=mTextures[i]->getDepth();
				break;
			}
		}
	}
	return frames;
}

int SequenceTexture::getNumTextures() const{
	return mTextures.size();
}

Texture::ptr SequenceTexture::getTexture(int frame){
	return mTextures[frame];
}

void SequenceTexture::setTexture(int frame,peeper::Texture::ptr texture){
	mTextures[frame]=texture;
}

}
}
