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

package us.toadlet.ribbit;

// AudioTrack STATIC mode apparently leaks memory, so we just buffer the data here
public class ATAudioBuffer implements AudioBuffer{
	public ATAudioBuffer(){
		mAudioFormat=new AudioFormat(0,0,0);
	}

	public AudioBuffer getRootAudioBuffer(){return this;}

	public boolean create(AudioStream stream){
		System.out.println("ATAudioBuffer.create");

		AudioFormat format=stream.getAudioFormat();
		mAudioFormat.set(format);
		
		/*
		int sps=format.samplesPerSecond;
		int chan=(format.channels==2?AudioFormat.CHANNEL_OUT_STEREO:AudioFormat.CHANNEL_OUT_MONO);
		int bps=(format.bitsPerSample==8?AudioFormat.ENCODING_PCM_8BIT:AudioFormat.ENCODING_PCM_16BIT);

		int available=0;
		try{
			available=stream.available();
		}
		catch(java.io.IOException ex){
			return false;
		}

		mAudioTrack=new AudioTrack(AudioManager.STREAM_ALARM,sps,chan,bps,available,AudioTrack.MODE_STATIC);
		
		byte[] data=new byte[available];
		try{
			available=stream.read(data,0,available);
		}
		catch(java.io.IOException ex){
			return false;
		}
		if(available>0){
			mAudioTrack.write(data,0,available);
			mEndPosition=available/format.frameSize();
			mPlayTime=mEndPosition*1000/format.samplesPerSecond;
		}
		*/
		
		int available=0;
		try{
			available=stream.available();
			mData=new byte[available];
			stream.read(mData,0,available);
		}
		catch(java.io.IOException ex){
			return false;
		}
		
		return true;
	}
	
	public void destroy(){
		System.out.println("ATAudioBuffer.destroy");

		/*
		if(mAudioTrack!=null){
			mAudioTrack.release();
			mAudioTrack=null;
		}
		*/
		mData=null;
	}
	
	AudioFormat getAudioFormat(){return mAudioFormat;}
	
	//AudioTrack mAudioTrack;
	//int mPlayTime;
	byte[] mData;
	AudioFormat mAudioFormat;
}
