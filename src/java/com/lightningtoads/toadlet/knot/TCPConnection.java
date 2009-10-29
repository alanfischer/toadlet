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
import java.net.*;
import java.util.*;

public class TCPConnection implements Connection,Runnable{
	public TCPConnection(Socket socket){mSocket=socket;init();}
	public TCPConnection(ServerSocket socket){mSSocket=socket;init();}
	protected void init(){
		int maxSize=1024;
		mOutPacket=new ByteArrayOutputStream(maxSize);
		mDataOutPacket=new DataOutputStream(mOutPacket);
		mInPacketBytes=new byte[maxSize];
		mInPacket=new ByteArrayInputStream(mInPacketBytes);
		mDataInPacket=new DataInputStream(mInPacket);

		mMutex=new Object();
		mThread=new Thread(this);
	}

	public boolean connect(String address,int port){
		InetAddress remoteIP=null;
		try{
			remoteIP=InetAddress.getByName(address);
		}catch(Exception ex){}
		int remotePort=port;

		Logger.log(Categories.TOADLET_KNOT,Logger.Level.DEBUG,
			("connect: protocol ")+new String(CONNECTION_PACKET));

		boolean result=false;
		int amount=0;
		try{
			mSocket.connect(new InetSocketAddress(remoteIP,remotePort)); result=true;
			if(result){
				result=false;
				int size=buildConnectionPacket(mDataOutPacket);

				byte[] outPacketBytes=mOutPacket.toByteArray();
				mSocket.getOutputStream().write(outPacketBytes,0,outPacketBytes.length); amount=outPacketBytes.length;
				if(amount>0){
					Logger.log(Categories.TOADLET_KNOT,Logger.Level.DEBUG,
						"connect: sent connection packet");

					amount=mSocket.getInputStream().read(mInPacketBytes,0,mInPacketBytes.length);
					if(amount>0){
						Logger.log(Categories.TOADLET_KNOT,Logger.Level.DEBUG,
							"connect: received connection packet");

						if(verifyConnectionPacket(mDataInPacket)){
							Logger.log(Categories.TOADLET_KNOT,Logger.Level.DEBUG,
								"connect: verified connection packet");

							result=true;
						}
						else{
							Logger.log(Categories.TOADLET_KNOT,Logger.Level.DEBUG,
								"connect: error verifying connection packet");
						}
					}
				}
			}
		}
		catch(IOException ex){result=false;}

		mInPacket.reset();
		mOutPacket.reset();

		if(result){
			skipStart();
		}

		return result;
	}

	public boolean accept(){
		Logger.log(Categories.TOADLET_KNOT,Logger.Level.DEBUG,
			("accept: protocol ")+new String(CONNECTION_PACKET));

		boolean result=false;
		try{
			Socket socket=mSSocket.accept();
			if(socket!=null){
				mSocket=socket;
				int amount=0;

				int size=buildConnectionPacket(mDataOutPacket);

				amount=mSocket.getInputStream().read(mInPacketBytes,0,mInPacketBytes.length);
				if(amount>0){
					Logger.log(Categories.TOADLET_KNOT,Logger.Level.DEBUG,
						"accept: received connection packet");

					if(verifyConnectionPacket(mDataInPacket)){
						Logger.log(Categories.TOADLET_KNOT,Logger.Level.DEBUG,
							"accept: verified connection packet");

						byte[] outPacketBytes=mOutPacket.toByteArray();
						mSocket.getOutputStream().write(outPacketBytes,0,outPacketBytes.length);amount=outPacketBytes.length;
						if(amount>0){
							Logger.log(Categories.TOADLET_KNOT,Logger.Level.DEBUG,
								"accept: sent connection packet");

							result=true;
						}
					}
					else{
						Logger.log(Categories.TOADLET_KNOT,Logger.Level.DEBUG,
							"connect: error verifying connection packet");
					}
				}
			}
		}
		catch(IOException ex){result=false;}

		mInPacket.reset();
		mOutPacket.reset();

		if(result){
			mThread.start();
		}

		return result;
	}

	public void skipStart(){
		mThread.start();
	}

	public boolean disconnect(){
		try{
			if(mSocket!=null){
				mSocket.close();
			}
			if(mSSocket!=null){
				mSSocket.close();
			}
		}
		catch(IOException ex){}

		mRun=false;
		while(mThread.isAlive()){
			try{
				Thread.sleep(10);
			}catch(InterruptedException ex){}
		}

		return true;
	}

	public int send(byte[] data,int offset,int length){
		synchronized(mMutex){
			try{
				mDataOutPacket.writeShort(length);
				mDataOutPacket.write(data,offset,length);

				byte[] outPacketBytes=mOutPacket.toByteArray();
				mSocket.getOutputStream().write(outPacketBytes,0,outPacketBytes.length);length=outPacketBytes.length;
			}
			catch(IOException ex){length=-1;}

			mOutPacket.reset();
		}

		return length;
	}

	public int receive(byte[] data,int offset,int length){
		int amount=0;
		synchronized(mMutex){
			if(mPackets.size()>0){
				Packet packet=mPackets.get(0);
				if(packet.debugDeliverTime<=System.currentTimeMillis()){
					System.arraycopy(packet.data,0,data,offset,packet.length);
					amount=packet.length;
					mPackets.remove(0);
					mFreePackets.add(packet);
				}
			}
		}

		return amount;
	}

	public int getPing(){return -1;}

	public void run(){
		while(mRun){
			if(updatePacketReceive()==false){
				break;
			}
		}
	}

	/// Debug methods
	public void debugSetPacketDelayTime(int minTime,int maxTime){
		mDebugPacketDelayMinTime=minTime;
		mDebugPacketDelayMaxTime=maxTime;
	}

	protected class Packet{
		public byte[] data=new byte[1024];
		public int length=0;
		public long debugDeliverTime=0;
		
		public void reset(){
			length=0;
			debugDeliverTime=0;
		}
	};

	protected final static int CONNECTION_FRAME=-1;
	protected final static byte[] CONNECTION_PACKET=new byte[]{'t','o','a','d','l','e','t',':',':','k','n','o','t',':',':','t','c','p'};
	protected final static int CONNECTION_PACKET_LENGTH=18;
	protected final static int CONNECTION_VERSION=1;

	protected int buildConnectionPacket(DataOutputStream out) throws IOException{
		int size=0;

		out.writeInt(CONNECTION_FRAME);size+=4;
		out.write(CONNECTION_PACKET,0,CONNECTION_PACKET_LENGTH);size+=CONNECTION_PACKET_LENGTH;
		out.writeInt(CONNECTION_VERSION);size+=4;

		return size; 
	}
	
	protected boolean verifyConnectionPacket(DataInputStream in) throws IOException{
		int header=in.readInt();
		if(header!=CONNECTION_FRAME){
			return false;
		}

		byte[] packet=new byte[CONNECTION_PACKET_LENGTH];
		in.read(packet,0,CONNECTION_PACKET_LENGTH);
		int i=0;for(;i<CONNECTION_PACKET_LENGTH;++i){if(packet[i]!=CONNECTION_PACKET[i])break;}
		if(i!=CONNECTION_PACKET_LENGTH){
			return false;
		}

		int version=in.readInt();
		if(version!=CONNECTION_VERSION){
			return false;
		}

		return true;
	}

	boolean updatePacketReceive(){
		int amount=0;

		try{
			amount=mSocket.getInputStream().read(mInPacketBytes,0,2);
			if(amount>0){
				Packet packet;
				if(mFreePackets.size()>0){
					packet=mFreePackets.get(0);
					packet.reset();
					mFreePackets.remove(0);
				}
				else{
					packet=new Packet();
				}

				packet.length=mDataInPacket.readShort();
				mInPacket.reset();

				amount=mSocket.getInputStream().read(packet.data,0,packet.length);
				if(amount>0){
					synchronized(mMutex){
						if(mDebugPacketDelayMaxTime>0){
							packet.debugDeliverTime=System.currentTimeMillis() + mDebugRandom.nextInt(mDebugPacketDelayMaxTime-mDebugPacketDelayMinTime)+mDebugPacketDelayMinTime;
						}
						mPackets.add(packet);
					}
				}
				else{
					mFreePackets.add(packet);
				}
			}
		}
		catch(IOException ex){amount=0;}

		return amount>0;
	}

	protected Socket mSocket=null;
	protected ServerSocket mSSocket=null;
	protected ByteArrayOutputStream mOutPacket=null;
	protected DataOutputStream mDataOutPacket=null;
	protected byte[] mInPacketBytes;
	protected ByteArrayInputStream mInPacket=null;
	protected DataInputStream mDataInPacket=null;

	protected Object mMutex=null;
	protected Thread mThread=null;
	protected boolean mRun=true;

	protected Vector<Packet> mPackets=new Vector<Packet>();
	protected Vector<Packet> mFreePackets=new Vector<Packet>();
	
	protected Random mDebugRandom=new Random();
	protected int mDebugPacketDelayMinTime=0;
	protected int mDebugPacketDelayMaxTime=0;
}