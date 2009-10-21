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

package com.lightningtoads.toadlet.knot;

#include <com/lightningtoads/toadlet/Types.h>

import com.lightningtoads.toadlet.egg.*;
import com.lightningtoads.toadlet.egg.Error;
import java.io.*;
import java.util.Vector;

public class PeerEventSynchronizer{
	public enum PeerStatus{
		FRAME_OK,
		FRAME_MISSING,
		FRAME_BAD,
	}

	public final static int CONTROL_EVENT_FLAG=0x4000; // Switch to 0x4000 from 0x8000 since setting the sign bit for a short ended up confusing the bit operations in java
	public final static int CONTROL_EVENT_FRAMEBUFFER=1;
	public final static int MAX_FRAME_DIFFERENCE=1000;
	
	public PeerEventSynchronizer(){
		init(null,null);
	}
	public PeerEventSynchronizer(Connection connection,EventFactory factory){
		init(connection,factory);
	}
	protected void init(Connection connection,EventFactory factory){
		mPacketInBytes=new byte[1024];
		mPacketIn=new ByteArrayInputStream(mPacketInBytes);
		mDataPacketIn=new DataInputStream(mPacketIn);
		mPacketOut=new ByteArrayOutputStream();
		mDataPacketOut=new DataOutputStream(mPacketOut);

		mIncomingEvents=new EventGroup();
		mOutgoingEvents=new EventGroup();

		mConnection=connection;
		mEventFactory=factory;

		adjustFrameBuffer(1,1,true);
	}

	public void reset(int frameBuffer,int frameGroupSize){
		mFrame=0;
		mFrameBuffer=0;
		mSkipAtFrame=0;
		mSkipReceivingFrames=0;
		mSkipSendingFrames=0;
		mFrameGroupSize=0;
		mFrameGroupCount=0;
		mIncomingEvents.clear();
		mOutgoingEvents.clear();
		mLocalEventGroups.clear();
		mRemoteEventGroups.clear();
		mFreeEventGroups.clear();

		adjustFrameBuffer(frameBuffer,frameGroupSize,true);
	}
	
	public void setConnection(Connection connection){mConnection=connection;}
	public void setEventFactory(EventFactory factory){mEventFactory=factory;}

	public void requestFrameBuffer(int frameBuffer,int frameGroupSize){
		mOutgoingEvents.setFrameBuffer(frameBuffer,frameGroupSize);
	}

	public int getFrameBuffer(){return mFrameBuffer;}
	public int getFrameGroupSize(){return mFrameGroupSize;}

	public void pushEvent(Event event){mOutgoingEvents.add(event);}
	public Event popEvent(){
		Event event=null;
		if(mIncomingEvents.size()>0){
			event=mIncomingEvents.get(0);
			mIncomingEvents.remove(0);
		}
		return event;
	}

	public PeerStatus update(){
		int i,j;
		int amount=0;
		Event event=null;
		int frameAdvance=0;

		try{
			while((amount=mConnection.receive(mPacketInBytes,0,mPacketInBytes.length))>0){
				int remoteFrame=mDataPacketIn.readInt();
				if(remoteFrame==0 || remoteFrame<mFrame || remoteFrame>mFrame+MAX_FRAME_DIFFERENCE){
					Logger.log(Categories.TOADLET_KNOT,Logger.Level.WARNING,
						("invalid frame:")+remoteFrame+" current frame:"+mFrame);

					return PeerStatus.FRAME_BAD;
				}

				int numFrames=mDataPacketIn.readByte();
				if(numFrames<=0){
					Logger.log(Categories.TOADLET_KNOT,Logger.Level.WARNING,
						("invalid numFrames:")+numFrames);

					return PeerStatus.FRAME_BAD;
				}

				if(Logger.getInstance().getMasterCategoryReportingLevel(Categories.TOADLET_KNOT)>=Logger.Level.EXCESSIVE){
					Logger.log(Categories.TOADLET_KNOT,Logger.Level.EXCESSIVE,
						("Received events for frames:")+remoteFrame+"-"+(remoteFrame+numFrames-1)+" current frame:"+mFrame);
				}

				for(j=0;j<numFrames;++j){
					EventGroup events;
					if(mFreeEventGroups.size()>0){
						events=mFreeEventGroups.get(0);
						events.clear();
						mFreeEventGroups.remove(0);
					}
					else{
						events=new EventGroup();
					}
					int numEvents=mDataPacketIn.readShort();
					if((numEvents&CONTROL_EVENT_FLAG)==CONTROL_EVENT_FLAG){
						numEvents=numEvents&~CONTROL_EVENT_FLAG;
						int type=mDataPacketIn.readByte();
						if(type==CONTROL_EVENT_FRAMEBUFFER){
							int frameBuffer=mDataPacketIn.readByte();
							int frameGroup=mDataPacketIn.readByte();
							events.setFrameBuffer(frameBuffer,frameGroup);
						}
					}
					for(i=0;i<numEvents;++i){
						int type=mDataPacketIn.readByte();
						if(mEventFactory!=null){
							event=mEventFactory.makeEventFromType(type);
						}
						if(event!=null){
							event.read(mDataPacketIn);
							events.add(event);
						}
						else{
							Logger.log(Categories.TOADLET_KNOT,Logger.Level.WARNING,
								("Received unknown event type:")+type);
						}
					}
					frameAdvance=remoteFrame+j-mFrame;
					if(mRemoteEventGroups.size()<frameAdvance+1){
						mRemoteEventGroups.setSize(frameAdvance+1);
					}
					mRemoteEventGroups.set(frameAdvance,events);
				}
				mPacketIn.reset();
			}
		}
		catch(IOException ex){
			return PeerStatus.FRAME_BAD;
		}

		if(mSkipReceivingFrames>0 && mSkipAtFrame<=mFrame){
			if(Logger.getInstance().getMasterCategoryReportingLevel(Categories.TOADLET_KNOT)>=Logger.Level.EXCESSIVE){
				Logger.log(Categories.TOADLET_KNOT,Logger.Level.EXCESSIVE,
					("Skipping receiving frame:")+mFrame);
			}

			mSkipReceivingFrames--;
		}
		else if(mRemoteEventGroups.size()==0){
			Logger.log(Categories.TOADLET_KNOT,Logger.Level.ALERT,
				("Missing events for frame:")+mFrame);

			return PeerStatus.FRAME_MISSING;
		}

		if(mSkipSendingFrames>0 && mSkipAtFrame<=mFrame){
			if(Logger.getInstance().getMasterCategoryReportingLevel(Categories.TOADLET_KNOT)>=Logger.Level.EXCESSIVE){
				Logger.log(Categories.TOADLET_KNOT,Logger.Level.EXCESSIVE,
					("Skipping sending frame:")+mFrame);
			}

			mSkipSendingFrames--;
		}
		else{
			try{
				// Save current outgoing events
				int frame=mFrameBuffer+mFrame;
				frameAdvance=frame-mFrame;
				if(mLocalEventGroups.size()<frameAdvance+1){
					mLocalEventGroups.setSize(frameAdvance+1);
				}
				EventGroup events;
				if(mFreeEventGroups.size()>0){
					events=mFreeEventGroups.get(0);
					events.clear();
					mFreeEventGroups.remove(0);
				}
				else{
					events=new EventGroup();
				}
				events.set(mOutgoingEvents);
				mLocalEventGroups.set(frameAdvance,events);
				mOutgoingEvents.clear();

				mFrameGroupCount++;
				if(mFrameGroupCount>=mFrameGroupSize){
					mFrameGroupCount=0;

					if(Logger.getInstance().getMasterCategoryReportingLevel(Categories.TOADLET_KNOT)>=Logger.Level.EXCESSIVE){
						Logger.log(Categories.TOADLET_KNOT,Logger.Level.EXCESSIVE,
							("Sending events for frames:")+(frame-mFrameGroupSize+1)+"-"+frame+" current frame:"+mFrame);
					}

					// Package up and send pending event groups
					mDataPacketOut.writeInt(frame-mFrameGroupSize+1); // TODO: extra, can be calculated & removed?
					mDataPacketOut.writeByte(mFrameGroupSize);
					for(j=0;j<mFrameGroupSize;++j){
						events=mLocalEventGroups.get(frameAdvance-mFrameGroupSize+1+j);
						int numEvents=events.size();
						if(events.hasFrameBufferInfo()){
							numEvents|=CONTROL_EVENT_FLAG;
						}
						mDataPacketOut.writeShort(numEvents);
						if(events.hasFrameBufferInfo()){
							mDataPacketOut.writeByte(CONTROL_EVENT_FRAMEBUFFER);
							mDataPacketOut.writeByte(events.getFrameBuffer());
							mDataPacketOut.writeByte(events.getFrameGroup());
						}
						for(i=0;i<events.size();++i){
							event=events.get(i);
							mDataPacketOut.writeByte(event.getType());
							event.write(mDataPacketOut);
						}
					}
					// TODO: Perhaps we could use a custom Stream class that allows the flexability we need to not reallocate bytes in toByteArray here
					byte[] packetOutBytes=mPacketOut.toByteArray();
					amount=mConnection.send(packetOutBytes,0,packetOutBytes.length);

					mPacketOut.reset();
				}
			}
			catch(IOException ex){
				return PeerStatus.FRAME_BAD;
			}
		}

		// Put both the pending local events and the incoming remote events into the group for access
		int frameBuffer=0;
		int frameGroup=0;
		int frameBufferCount=0;
		mIncomingEvents.clear();
		if(mRemoteEventGroups.size()>0){
			EventGroup events=mRemoteEventGroups.get(0);
			if(events!=null){
				mIncomingEvents.addAll(events);
				if(events.hasFrameBufferInfo()){
					frameBuffer+=events.getFrameBuffer();
					frameGroup+=events.getFrameGroup();
					frameBufferCount++;
				}
				mFreeEventGroups.add(events);
			}
			mRemoteEventGroups.remove(0);
		}
		if(mLocalEventGroups.size()>0){
			EventGroup events=mLocalEventGroups.get(0);
			if(events!=null){
				mIncomingEvents.addAll(events);
				if(events.hasFrameBufferInfo()){
					frameBuffer+=events.getFrameBuffer();
					frameGroup+=events.getFrameGroup();
					frameBufferCount++;
				}
				mFreeEventGroups.add(events);
			}
			mLocalEventGroups.remove(0);
		}

		if(frameBufferCount>0){
			adjustFrameBuffer(frameBuffer/frameBufferCount,frameGroup/frameBufferCount,false);
		}

		mFrame++;

		if(amount<0){
			return PeerStatus.FRAME_BAD;
		}
		else{
			return PeerStatus.FRAME_OK;
		}
	}

	protected class EventGroup extends Vector<Event>{
		public EventGroup(){
			super();
		}

		public EventGroup(EventGroup eventGroup){
			super(eventGroup);
			mFrameBuffer=eventGroup.getFrameBuffer();
			mFrameGroup=eventGroup.getFrameGroup();
		}

		public void set(EventGroup eventGroup){
			super.clear();
			super.addAll(eventGroup);
			mFrameBuffer=eventGroup.getFrameBuffer();
			mFrameGroup=eventGroup.getFrameGroup();
		}

		public void clear(){
			super.clear();
			mFrameBuffer=0;
			mFrameGroup=0;
		}

		public void setFrameBuffer(int buffer,int group){mFrameBuffer=buffer;mFrameGroup=group;}
		public boolean hasFrameBufferInfo(){return mFrameBuffer!=0 || mFrameGroup!=0;}
		public int getFrameBuffer(){return mFrameBuffer;}
		public int getFrameGroup(){return mFrameGroup;}

		protected int mFrameBuffer;
		protected int mFrameGroup;
	}

	boolean adjustFrameBuffer(int frameBuffer,int frameGroupSize,boolean force){
		if(force==false && (mSkipReceivingFrames>0 || mSkipSendingFrames>0)){
			// For now, dont let us adjust the frame buffer if we're already adjusting it
			Logger.log(Categories.TOADLET_KNOT,Logger.Level.WARNING,
				"Cannot adjust FrameBuffer, currently adjusting");

			return false;
		}
		if(frameBuffer==0 || frameGroupSize>frameBuffer){
			Error.invalidParameters(Categories.TOADLET_KNOT,
				"invalid frameBuffer or frameGroupSize");

			return false;
		}

		Logger.log(Categories.TOADLET_TADPOLE,Logger.Level.ALERT,
			("Adjusting FrameBuffer from:")+mFrameBuffer+" to:"+frameBuffer);

		Logger.log(Categories.TOADLET_TADPOLE,Logger.Level.ALERT,
			("Adjusting FrameGroupSize from:")+mFrameGroupSize+" to:"+frameGroupSize);

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

	protected Connection mConnection=null;
	protected EventFactory mEventFactory=null;

	protected byte[] mPacketInBytes=null;
	protected ByteArrayInputStream mPacketIn=null;
	protected DataInputStream mDataPacketIn=null;
	protected ByteArrayOutputStream mPacketOut=null;
	protected DataOutputStream mDataPacketOut=null;

	protected int mFrame=0;
	protected int mFrameBuffer=0;
	protected int mSkipAtFrame=0;
	protected int mSkipReceivingFrames=0;
	protected int mSkipSendingFrames=0;
	protected int mFrameGroupSize=0;
	protected int mFrameGroupCount=0;
	protected EventGroup mIncomingEvents=null;
	protected EventGroup mOutgoingEvents=null;
	protected Vector<EventGroup> mLocalEventGroups=new Vector<EventGroup>();
	protected Vector<EventGroup> mRemoteEventGroups=new Vector<EventGroup>();
	protected Vector<EventGroup> mFreeEventGroups=new Vector<EventGroup>();
}
