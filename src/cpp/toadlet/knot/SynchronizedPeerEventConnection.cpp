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
#include <toadlet/knot/SynchronizedPeerEventConnection.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace knot{

SynchronizedPeerEventConnection::SynchronizedPeerEventConnection(Connection::ptr connection,EventFactory *factory):
	//mConnection,
	mEventFactory(NULL),

	//mPacketIn,
	//mDataPacketIn,
	//mPacketOut,
	//mDataPacketOut,

	mFrame(0),
	mFrameBuffer(0),
	mSkipAtFrame(0),
	mSkipReceivingFrames(0),
	mSkipSendingFrames(0),
	mFrameGroupSize(0),
	mFrameGroupCount(0)
	//mIncomingEvents,
	//mOutgoingEvents,
	//mLocalEventGroups,
	//mRemoteEventGroups
{
	mPacketIn=MemoryStream::ptr(new MemoryStream(new byte[1024],1024,1024,true));
	mDataPacketIn=DataStream::ptr(new DataStream(Stream::ptr(mPacketIn)));
	mPacketOut=MemoryStream::ptr(new MemoryStream());
	mDataPacketOut=DataStream::ptr(new DataStream(Stream::ptr(mPacketOut)));

	mIncomingEvents=EventGroup::ptr(new EventGroup());
	mOutgoingEvents=EventGroup::ptr(new EventGroup());

	adjustFrameBuffer(1,1,true);

	mEventFactory=factory;

	if(connection->blocking()){
		Error::unknown(Categories::TOADLET_KNOT,
			"SynchronizedPeerEventConnection requires a non blocking Connection");
	}
	else{
		mConnection=connection;
		mEventFactory=factory;
	}
}

void SynchronizedPeerEventConnection::reset(int frameBuffer,int frameGroupSize){
	mFrame=0;
	mFrameBuffer=0;
	mSkipAtFrame=0;
	mSkipReceivingFrames=0;
	mSkipSendingFrames=0;
	mFrameGroupSize=0;
	mFrameGroupCount=0;
	mIncomingEvents->clear();
	mOutgoingEvents->clear();
	mLocalEventGroups.clear();
	mRemoteEventGroups.clear();

	adjustFrameBuffer(frameBuffer,frameGroupSize,true);
}

SynchronizedPeerEventConnection::~SynchronizedPeerEventConnection(){
	close();
}

bool SynchronizedPeerEventConnection::opened(){
	return mConnection!=NULL;
}

void SynchronizedPeerEventConnection::close(){
	if(mConnection!=NULL){
		mConnection->close();
		mConnection=NULL;
	}
}

void SynchronizedPeerEventConnection::requestFrameBuffer(int frameBuffer,int frameGroupSize){
	mOutgoingEvents->setFrameBuffer(frameBuffer,frameGroupSize);
}

bool SynchronizedPeerEventConnection::send(Event::ptr event){
	mOutgoingEvents->add(event);
	return true;
}

Event::ptr SynchronizedPeerEventConnection::receive(){
	Event::ptr event;
	if(mIncomingEvents->size()>0){
		event=mIncomingEvents->get(0);
		mIncomingEvents->removeAt(0);
	}
	return event;
}

int SynchronizedPeerEventConnection::update(){
	int i,j;
	int amount=0;
	Event::ptr event;
	int frameAdvance=0;

	while((amount=mConnection->receive(mPacketIn->getOriginalDataPointer(),mPacketIn->length()))>0){
		int remoteFrame=mDataPacketIn->readBigInt32();
		if(remoteFrame==0 || remoteFrame<mFrame || remoteFrame>mFrame+MAX_FRAME_DIFFERENCE){
			Logger::warning(Categories::TOADLET_KNOT,
				String("invalid frame:")+remoteFrame+" current frame:"+mFrame);

			return PeerStatus_FRAME_BAD;
		}

		int numFrames=mDataPacketIn->readUInt8();
		if(numFrames<=0){
			Logger::warning(Categories::TOADLET_KNOT,
				String("invalid numFrames:")+numFrames);

			return PeerStatus_FRAME_BAD;
		}

		if(Logger::getInstance()->getMasterCategoryReportingLevel(Categories::TOADLET_KNOT)>=Logger::Level_EXCESS){
			Logger::excess(Categories::TOADLET_KNOT,
				String("Received events for frames:")+remoteFrame+"-"+(remoteFrame+numFrames-1)+" current frame:"+mFrame);
		}

		for(j=0;j<numFrames;++j){
			EventGroup::ptr events;
			if(mFreeEventGroups.size()>0){
				events=mFreeEventGroups[0];
				events->clear();
				mFreeEventGroups.removeAt(0);
			}
			else{
				events=EventGroup::ptr(new EventGroup());
			}
			int numEvents=mDataPacketIn->readBigInt16();
			if((numEvents&CONTROL_EVENT_FLAG)==CONTROL_EVENT_FLAG){
				numEvents=numEvents&~CONTROL_EVENT_FLAG;
				int type=mDataPacketIn->readUInt8();
				if(type==CONTROL_EVENT_FRAMEBUFFER){
					int frameBuffer=mDataPacketIn->readUInt8();
					int frameGroup=mDataPacketIn->readUInt8();
					events->setFrameBuffer(frameBuffer,frameGroup);
				}
			}
			for(i=0;i<numEvents;++i){
				int type=mDataPacketIn->readUInt8();
				if(mEventFactory!=NULL){
					event=mEventFactory->createEventType(type);
				}
				if(event!=NULL){
					event->read(mDataPacketIn);
					events->add(event);
				}
				else{
					Logger::warning(Categories::TOADLET_KNOT,
						String("Received unknown event type:")+type);
				}
			}
			frameAdvance=remoteFrame+j-mFrame;
			if(mRemoteEventGroups.size()<frameAdvance+1){
				mRemoteEventGroups.resize(frameAdvance+1);
			}
			mRemoteEventGroups.setAt(frameAdvance,events);

		}
		mPacketIn->reset();
	}

	if(mSkipReceivingFrames>0 && mSkipAtFrame<=mFrame){
		if(Logger::getInstance()->getMasterCategoryReportingLevel(Categories::TOADLET_KNOT)>=Logger::Level_EXCESS){
			Logger::excess(Categories::TOADLET_KNOT,
				String("Skipping receiving frame:")+mFrame);
		}

		mSkipReceivingFrames--;
	}
	else if(mRemoteEventGroups.size()==0){
		Logger::alert(Categories::TOADLET_KNOT,
			String("Missing events for frame:")+mFrame);

		return PeerStatus_FRAME_MISSING;
	}

	if(mSkipSendingFrames>0 && mSkipAtFrame<=mFrame){
		if(Logger::getInstance()->getMasterCategoryReportingLevel(Categories::TOADLET_KNOT)>=Logger::Level_EXCESS){
			Logger::excess(Categories::TOADLET_KNOT,
				String("Skipping sending frame:")+mFrame);
		}

		mSkipSendingFrames--;
	}
	else{
		// Save current outgoing events
		int frame=mFrameBuffer+mFrame;
		frameAdvance=frame-mFrame;
		if(mLocalEventGroups.size()<frameAdvance+1){
			mLocalEventGroups.resize(frameAdvance+1);
		}
		EventGroup::ptr events;
		if(mFreeEventGroups.size()>0){
			events=mFreeEventGroups[0];
			events->clear();
			mFreeEventGroups.removeAt(0);
		}
		else{
			events=EventGroup::ptr(new EventGroup());
		}
		events->set(mOutgoingEvents);
		mLocalEventGroups.setAt(frameAdvance,events);
		mOutgoingEvents->clear();

		mFrameGroupCount++;
		if(mFrameGroupCount>=mFrameGroupSize){
			mFrameGroupCount=0;

			if(Logger::getInstance()->getMasterCategoryReportingLevel(Categories::TOADLET_KNOT)>=Logger::Level_EXCESS){
				Logger::excess(Categories::TOADLET_KNOT,
					String("Sending events for frames:")+(frame-mFrameGroupSize+1)+"-"+frame+" current frame:"+mFrame);
			}

			// Package up and send pending event groups
			mDataPacketOut->writeBigInt32(frame-mFrameGroupSize+1); /// @todo: extra, can be calculated & removed?
			mDataPacketOut->writeUInt8(mFrameGroupSize);
			for(j=0;j<mFrameGroupSize;++j){
				EventGroup::ptr events=mLocalEventGroups[frameAdvance-mFrameGroupSize+1+j];
				int numEvents=events->size();
				if(events->hasFrameBufferInfo()){
					numEvents|=CONTROL_EVENT_FLAG;
				}
				mDataPacketOut->writeBigInt16(numEvents);
				if(events->hasFrameBufferInfo()){
					mDataPacketOut->writeUInt8(CONTROL_EVENT_FRAMEBUFFER);
					mDataPacketOut->writeUInt8(events->getFrameBuffer());
					mDataPacketOut->writeUInt8(events->getFrameGroup());
				}
				for(i=0;i<events->size();++i){
					event=events->get(i);
					mDataPacketOut->writeUInt8(event->getType());
					event->write(mDataPacketOut);
				}
			}
			amount=mConnection->send(mPacketOut->getOriginalDataPointer(),mPacketOut->length());

			mPacketOut->reset();
		}
	}

	// Put both the pending local events and the incoming remote events into the group for access
	int frameBuffer=0;
	int frameGroup=0;
	int frameBufferCount=0;
	mIncomingEvents->clear();
	if(mRemoteEventGroups.size()>0){
		EventGroup::ptr events=mRemoteEventGroups[0];
		if(events!=NULL){
			mIncomingEvents->addAll(events);
			if(events->hasFrameBufferInfo()){
				frameBuffer+=events->getFrameBuffer();
				frameGroup+=events->getFrameGroup();
				frameBufferCount++;
			}
			mFreeEventGroups.add(events);
		}
		mRemoteEventGroups.removeAt(0);
	}
	if(mLocalEventGroups.size()>0){
		EventGroup::ptr events=mLocalEventGroups[0];
		if(events!=NULL){
			mIncomingEvents->addAll(events);
			if(events->hasFrameBufferInfo()){
				frameBuffer+=events->getFrameBuffer();
				frameGroup+=events->getFrameGroup();
				frameBufferCount++;
			}
			mFreeEventGroups.add(events);
		}
		mLocalEventGroups.removeAt(0);
	}

	if(frameBufferCount>0){
		adjustFrameBuffer(frameBuffer/frameBufferCount,frameGroup/frameBufferCount,false);
	}

	mFrame++;

	if(amount<0){
		return PeerStatus_FRAME_BAD;
	}
	else{
		return PeerStatus_FRAME_OK;
	}
}

bool SynchronizedPeerEventConnection::adjustFrameBuffer(int frameBuffer,int frameGroupSize,bool force){
	if(force==false && (mSkipReceivingFrames>0 || mSkipSendingFrames>0)){
		// For now, dont let us adjust the frame buffer if we're already adjusting it
		Logger::warning(Categories::TOADLET_KNOT,
			"Cannot adjust FrameBuffer, currently adjusting");

		return false;
	}
	if(frameBuffer==0 || frameGroupSize>frameBuffer){
		Error::invalidParameters(Categories::TOADLET_KNOT,
			"invalid frameBuffer or frameGroupSize");

		return false;
	}

	Logger::alert(Categories::TOADLET_TADPOLE,
		String("Adjusting FrameBuffer from:")+mFrameBuffer+" to:"+frameBuffer);

	Logger::alert(Categories::TOADLET_TADPOLE,
		String("Adjusting FrameGroupSize from:")+mFrameGroupSize+" to:"+frameGroupSize);

	if(frameBuffer>mFrameBuffer){
		mSkipAtFrame=mFrame+mFrameBuffer;
		mSkipReceivingFrames=frameBuffer-mFrameBuffer;
	}
	else{
		mSkipAtFrame=mFrame;
		mSkipSendingFrames=mFrameBuffer-frameBuffer;
	}
	mFrameBuffer=frameBuffer;
	mFrameGroupSize=frameGroupSize;

	return true;
}

}
}
