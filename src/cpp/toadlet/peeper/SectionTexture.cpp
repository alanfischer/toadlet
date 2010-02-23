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
#include <toadlet/peeper/SectionTexture.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

SectionTexture::SectionTexture(int widthFrames,int frameOffset,int numFrames,Texture::ptr texture){
	mDimension=Dimension_D1;
	mWidthFrames=widthFrames;
	mHeightFrames=0;
	mDepthFrames=0;
	mFrameOffset=frameOffset;
	mNumFrames=numFrames;
	mTexture=texture;
}

SectionTexture::SectionTexture(int widthFrames,int heightFrames,int frameOffset,int numFrames,Texture::ptr texture){
	mDimension=Dimension_D2;
	mWidthFrames=widthFrames;
	mHeightFrames=heightFrames;
	mDepthFrames=0;
	mFrameOffset=frameOffset;
	mNumFrames=numFrames;
	mTexture=texture;
}

// TODO: We don't really support framed 3d textures yet, since the framing is the dimension above the texture dimension
SectionTexture::SectionTexture(int widthFrames,int heightFrames,int depthFrames,int frameOffset,int numFrames,Texture::ptr texture){
	mDimension=Dimension_D3;
	mWidthFrames=widthFrames;
	mHeightFrames=heightFrames;
	mDepthFrames=depthFrames;
	mFrameOffset=frameOffset;
	mNumFrames=numFrames;
	mTexture=texture;
}

SectionTexture::~SectionTexture(){
}

Texture *SectionTexture::getRootTexture(int frame){
	return mTexture->getRootTexture(frame);
}

bool SectionTexture::getRootTransform(int frame,Matrix4x4 &transform){
	if(mWidthFrames>0){
		int v=(frame+mFrameOffset)%mWidthFrames;
		scalar scale=Math::div(Math::ONE,Math::fromInt(mWidthFrames));
		scalar offset=Math::mul(Math::fromInt(v),scale);
		transform.setAt(0,0,scale);
		transform.setAt(0,3,offset);
	}

	if(mHeightFrames>0){
		int v=((frame+mFrameOffset)/mWidthFrames)%mHeightFrames;
		scalar scale=Math::div(Math::ONE,Math::fromInt(mHeightFrames));
		scalar offset=Math::mul(Math::fromInt(v),scale);
		transform.setAt(1,1,scale);
		transform.setAt(1,3,offset);
	}

	if(mDepthFrames>0){
		int v=((frame+mFrameOffset)/(mWidthFrames*mHeightFrames))%mDepthFrames;
		scalar scale=Math::div(Math::ONE,Math::fromInt(mDepthFrames));
		scalar offset=Math::mul(Math::fromInt(v),scale);
		transform.setAt(2,2,scale);
		transform.setAt(2,3,offset);
	}

	return false;
}

bool SectionTexture::create(int usageFlags,Dimension dimension,int format,int width,int height,int depth,int mipLevels){
	Error::unimplemented("create not implemented for SectionTexture");
	return false;
}

void SectionTexture::destroy(){
	if(mTexture!=NULL){
		mTexture->destroy();
		mTexture=NULL;
	}
}

bool SectionTexture::createContext(){
	return mTexture!=NULL?mTexture->createContext():false;
}

void SectionTexture::destroyContext(bool backData){
	if(mTexture!=NULL) mTexture->destroyContext(backData);
}

bool SectionTexture::contextNeedsReset(){
	return mTexture!=NULL?mTexture->contextNeedsReset():false;
}

int SectionTexture::getWidth() const{
	return mTexture!=NULL?mTexture->getWidth():0;
}

int SectionTexture::getHeight() const{
	if(mDimension==Dimension_D1){
		return getTotalNumTextures();
	}
	else{
		return mTexture!=NULL?mTexture->getHeight():0;
	}
}

int SectionTexture::getDepth() const{
	if(mDimension==Dimension_D2){
		return getTotalNumTextures();
	}
	else{
		return mTexture!=NULL?mTexture->getDepth():0;
	}
}

int SectionTexture::getTotalNumTextures() const{
	return mNumFrames;
}

int SectionTexture::getNumTextures() const{
	return mNumFrames;
}

}
}
