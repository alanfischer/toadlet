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

#ifndef TOADLET_TADPOLE_TRACK_H
#define TOADLET_TADPOLE_TRACK_H

#include <toadlet/egg/BaseResource.h>
#include <toadlet/egg/Collection.h>
#include <toadlet/peeper/VertexFormat.h>
#include <toadlet/peeper/VertexBufferAccessor.h>
#include <toadlet/tadpole/Types.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API Track:public BaseResource,public Buffer{
public:
	TOADLET_RESOURCE(Track,Buffer);

	Track(VertexFormat *format);

	void destroy(){}

	VertexFormat::ptr getFormat() const{return mFormat;}

	int addKeyFrame(scalar time,void *frame=NULL);
	bool getKeyFrame(void *frame,int index);
	
	int getKeyFrameSize() const{return mFormat->getVertexSize();}
	int getNumKeyFrames() const{return mTimes.size();}

	scalar getTime(int i){return mTimes[i];}
	scalar getKeyFramesAtTime(scalar time,int &f1,int &f2,int &trackHint) const;

	void setIndex(int index){mIndex=index;}
	int getIndex() const{return mIndex;}

	void setLength(scalar length){mLength=length;}
	scalar getLength() const{return mLength;}

	VertexBufferAccessor &getAccessor(){return mVBA;}

	int getUsage() const{return Usage_BIT_DYNAMIC;}
	int getAccess() const{return Access_READ_WRITE;}
	int getDataSize() const{return mData.size();}
	int getSize() const{return mTimes.size();}

	tbyte *lock(int lockAccess){return &mData[0];}
	bool unlock(){return true;}

	bool update(tbyte *data,int start,int size){return false;}

	void compile();

protected:
	VertexFormat::ptr mFormat;
	int mIndex;
	Collection<tbyte> mData;
	Collection<scalar> mTimes;
	scalar mLength;
	VertexBufferAccessor mVBA;
};

}
}

#endif

