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

#ifndef TOADLET_KNOT_SYNCHRONIZEDEVENTPEER_H
#define TOADLET_KNOT_SYNCHRONIZEDEVENTPEER_H

#include <toadlet/egg/Collection.h>
#include <toadlet/egg/EventFactory.h>
#include <toadlet/egg/io/DataStream.h>
#include <toadlet/egg/io/MemoryStream.h>
#include <toadlet/knot/Connection.h>
#include <toadlet/knot/EventConnection.h>

namespace toadlet{
namespace knot{

class TOADLET_API SynchronizedPeerEventConnection:public EventConnection{
public:
	TOADLET_SPTR(SynchronizedPeerEventConnection);

	enum{
		PeerStatus_FRAME_OK=0,
		PeerStatus_FRAME_MISSING=-1,
		PeerStatus_FRAME_BAD=-2,
	};

	const static int CONTROL_EVENT_FLAG=0x4000; // Switch to 0x4000 from 0x8000 since setting the sign bit for a short ended up confusing the bit operations in java
	const static int CONTROL_EVENT_FRAMEBUFFER=1;
	const static int MAX_FRAME_DIFFERENCE=1000;

	SynchronizedPeerEventConnection(Connection::ptr connection=NULL,EventFactory *factory=NULL);
	virtual ~SynchronizedPeerEventConnection();

	bool opened();
	void close();

	void reset(int frameBuffer,int frameGroupSize);

	void requestFrameBuffer(int frameBuffer,int frameGroupSize);
	inline int getFrameBuffer() const{return mFrameBuffer;}
	inline int getFrameGroupSize() const{return mFrameGroupSize;}

	bool send(Event::ptr event);
	Event::ptr receive();

	int update();

protected:
	class EventGroup:public Collection<Event::ptr>{
	public:
		TOADLET_SPTR(EventGroup);

		EventGroup():Collection<Event::ptr>(),
			mFrameBuffer(0),
			mFrameGroup(0)
		{}

		EventGroup(EventGroup::ptr eventGroup):Collection<Event::ptr>(eventGroup),
			mFrameBuffer(eventGroup->getFrameBuffer()),
			mFrameGroup(eventGroup->getFrameGroup())
		{}

		void set(EventGroup::ptr eventGroup){
			Collection<Event::ptr>::clear();
			Collection<Event::ptr>::addAll(eventGroup);
			mFrameBuffer=eventGroup->getFrameBuffer();
			mFrameGroup=eventGroup->getFrameGroup();
		}

		void clear(){
			Collection<Event::ptr>::clear();
			mFrameBuffer=0;
			mFrameGroup=0;
		}

		void setFrameBuffer(int buffer,int group){mFrameBuffer=buffer;mFrameGroup=group;}
		inline bool hasFrameBufferInfo() const{return mFrameBuffer!=0 || mFrameGroup!=0;}
		inline int getFrameBuffer() const{return mFrameBuffer;}
		inline int getFrameGroup() const{return mFrameGroup;}

	protected:
		int mFrameBuffer;
		int mFrameGroup;
	};

	bool adjustFrameBuffer(int frameBuffer,int frameGroupSize,bool force);

	Connection::ptr mConnection;
	EventFactory *mEventFactory;

	MemoryStream::ptr mPacketIn;
	DataStream::ptr mDataPacketIn;
	MemoryStream::ptr mPacketOut;
	DataStream::ptr mDataPacketOut;

	int mFrame;
	int mFrameBuffer;
	int mSkipAtFrame;
	int mSkipReceivingFrames;
	int mSkipSendingFrames;
	int mFrameGroupSize;
	int mFrameGroupCount;
	EventGroup::ptr mIncomingEvents;
	EventGroup::ptr mOutgoingEvents;
	Collection<EventGroup::ptr> mLocalEventGroups;
	Collection<EventGroup::ptr> mRemoteEventGroups;
	Collection<EventGroup::ptr> mFreeEventGroups;
};

}
}

#endif
