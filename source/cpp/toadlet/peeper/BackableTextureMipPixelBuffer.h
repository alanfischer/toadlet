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

#ifndef TOADLET_PEEPER_BACKABLETEXTUREMIPPIXELBUFFER_H
#define TOADLET_PEEPER_BACKABLETEXTUREMIPPIXELBUFFER_H

#include <toadlet/peeper/PixelBuffer.h>
#include <toadlet/peeper/BackableTexture.h>

namespace toadlet{
namespace peeper{

/// @todo:Clean this up so the getWidth & other functions work without mBack
///  I should calculate most of these parameters, then update them when the back is set
///  and move these into a .cpp file
class TOADLET_API BackableTextureMipPixelBuffer:public PixelBuffer{
public:
	TOADLET_SHARED_POINTERS(BackableTextureMipPixelBuffer);

	BackableTextureMipPixelBuffer(BackableTexture *texture,int level,int cubeSide){
		mTexture=texture;
		mLevel=level;
		mCubeSide=cubeSide;
		
		resetCreate();
	}
	
	virtual ~BackableTextureMipPixelBuffer(){}

	virtual PixelBuffer *getRootPixelBuffer(){return mBack;}

	virtual void setBufferDestroyedListener(BufferDestroyedListener *listener){}

	virtual bool create(int usage,int access,TextureFormat::ptr format){return false;}
	virtual void destroy(){
		resetDestroy();

		mTexture=NULL;
	}

	virtual void resetCreate(){
		mBack=mTexture->getBack()->getMipPixelBuffer(mLevel,mCubeSide);
	}
	
	virtual void resetDestroy(){
		mBack=NULL;
	}

	virtual int getUsage() const{return mBack!=NULL?mBack->getUsage():0;}
	virtual int getAccess() const{return mBack!=NULL?mBack->getAccess():0;}
	virtual int getDataSize() const{return mBack!=NULL?mBack->getDataSize():0;}
	virtual TextureFormat::ptr getTextureFormat() const{return mBack!=NULL?mBack->getTextureFormat():NULL;}

	virtual tbyte *lock(int lockAccess){return mBack!=NULL?mBack->lock(lockAccess):NULL;}
	virtual bool unlock(){return mBack!=NULL?mBack->unlock():false;}

	virtual bool update(tbyte *data,int start,int size){return mBack!=NULL?mBack->update(data,start,size):false;}

	virtual void setBack(PixelBuffer::ptr back){mBack=back;}
	virtual Buffer::ptr getBack(){return mBack;}

protected:
	BackableTexture *mTexture;
	int mLevel;
	int mCubeSide;
	PixelBuffer::ptr mBack;
};

}
}

#endif