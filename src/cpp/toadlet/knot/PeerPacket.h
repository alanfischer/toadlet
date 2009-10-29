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

#ifndef TOADLET_KNOT_PEERPACKET_H
#define TOADLET_KNOT_PEERPACKET_H

#include <toadlet/egg/WeakPointer.h>
#include <string.h>

namespace toadlet{
namespace knot{

class PeerPacket{
public:
	TOADLET_SHARED_POINTERS(PeerPacket,PeerPacket);

	PeerPacket():
		mData(NULL),
		mDataLength(0),
		mDataMaxSize(0),

		mFrame(0),
		mFrameBits(0),
		mFrameBitsReferenceFrame(0),

		mTimeHandled(0)
	{}

	~PeerPacket(){
		if(mData!=NULL){
			delete[] mData;
			mData=NULL;
		}
	}

	void set(PeerPacket *packet){
		setData(packet->getData(),packet->getDataLength());
		setFrame(packet->getFrame());
		setFrameBits(packet->getFrameBits(),packet->getFrameBitsReferenceFrame());
	}
	
	inline char *getData() const{return mData;}
	inline int getDataLength() const{return mDataLength;}

	int setData(const char *data,int length){
		if(mDataMaxSize<length){
			if(mData!=NULL){
				delete[] mData;
				mData=NULL;
			}
			mData=new char[length];
			mDataMaxSize=length;
		}
		memcpy(mData,data,length);
		mDataLength=length;
		return length;
	}

	inline int getFrame() const{return mFrame;}
	void setFrame(int frame){
		mFrame=frame;
	}

	inline int getFrameBits() const{return mFrameBits;}
	inline int getFrameBitsReferenceFrame() const{return mFrameBitsReferenceFrame;}
	void setFrameBits(int bits,int referenceFrame){
		mFrameBits=bits;
		mFrameBitsReferenceFrame=referenceFrame;
	}

	inline uint64 getTimeHandled() const{return mTimeHandled;}
	void setTimeHandled(uint64 time){
		mTimeHandled=time;
	}

protected:
	char *mData;
	int mDataLength;
	int mDataMaxSize;

	int mFrame;
	int mFrameBits;
	int mFrameBitsReferenceFrame;

	uint64 mTimeHandled;
};

}
}

#endif
