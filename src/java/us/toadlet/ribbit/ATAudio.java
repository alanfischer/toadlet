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

import android.media.AudioTrack;
import android.media.AudioFormat;
import android.media.AudioManager;

public class ATAudio implements Audio,AudioTrack.OnPlaybackPositionUpdateListener{
	public ATAudio(ATAudioRegister device){
		mGain=1.0f;
		mDevice=device;
		mDevice.registerAudio(this);
	}

	public boolean create(AudioBuffer buffer){
		mAudioStream=null;
		mAudioBuffer=(ATAudioBuffer)buffer.getRootAudioBuffer();

		us.toadlet.ribbit.AudioFormat format=mAudioBuffer.getAudioFormat();
		int sps=format.samplesPerSecond;
		int chan=(format.channels==2?AudioFormat.CHANNEL_OUT_STEREO:AudioFormat.CHANNEL_OUT_MONO);
		int bps=(format.bitsPerSample==8?AudioFormat.ENCODING_PCM_8BIT:AudioFormat.ENCODING_PCM_16BIT);
		int available=mAudioBuffer.mData.length;
		
		mAudioTrack=new AudioTrack(AudioManager.STREAM_ALARM,sps,chan,bps,available,AudioTrack.MODE_STREAM);
		mAudioTrack.setPlaybackPositionUpdateListener(this);
		mAudioTrack.setNotificationMarkerPosition(available/format.frameSize());

		return true;
	}
	
	public boolean create(AudioStream stream){
		mAudioBuffer=null;
		mAudioStream=stream;

		us.toadlet.ribbit.AudioFormat format=mAudioStream.getAudioFormat();
		int sps=format.samplesPerSecond;
		int chan=(format.channels==2?AudioFormat.CHANNEL_OUT_STEREO:AudioFormat.CHANNEL_OUT_MONO);
		int bps=(format.bitsPerSample==8?AudioFormat.ENCODING_PCM_8BIT:AudioFormat.ENCODING_PCM_16BIT);
		int available=AudioTrack.getMinBufferSize(sps,chan,bps);

		mStreamData=new byte[available];
		mAudioTrack=new AudioTrack(AudioManager.STREAM_MUSIC,sps,chan,bps,available,AudioTrack.MODE_STREAM);
		mAudioTrack.setPlaybackPositionUpdateListener(this);
		mAudioTrack.setPositionNotificationPeriod(available/format.frameSize());
		
		return true;
	}
	
	public void destroy(){
		if(mAudioTrack!=null){
			if(mAudioTrack.getState()!=AudioTrack.STATE_UNINITIALIZED){
				mAudioTrack.stop();
				mAudioTrack.release();
			}
			mAudioTrack=null;
		}

		if(mAudioBuffer!=null){
			//AudioBuffer destroyed by resource management
			mAudioBuffer=null;
		}
		if(mAudioStream!=null){
			try{
				mAudioStream.close();
			}
			catch(java.io.IOException ex){}
			mAudioStream=null;
		}
		
		if(mDevice!=null){
			mDevice.unregisterAudio(this);
			mDevice=null;
		}
	}

	public AudioBuffer getAudioBuffer(){return mAudioBuffer;}
	public AudioStream getAudioStream(){return mAudioStream;}

	public boolean play(){
		mPlayTime=0;

		if(mAudioTrack!=null){
			try{
				mAudioTrack.play();

				if(mAudioBuffer!=null){
					mAudioTrack.write(mAudioBuffer.mData,0,mAudioBuffer.mData.length);
				}
				else if(mAudioStream!=null){
					onPeriodicNotification(mAudioTrack);
				}
			}
			catch(Exception ex){
				return false;
			}
		}
		
		return true;
	}
	
	public boolean stop(){
		if(mAudioTrack!=null){
			try{
				mAudioTrack.stop();
			}
			catch(Exception ex){
				return false;
			}
		}

		return true;
	}
	
	public boolean getPlaying(){
		int playState=AudioTrack.PLAYSTATE_PLAYING;
		if(mAudioTrack!=null){
			playState=mAudioTrack.getPlayState();
		}
		return playState==AudioTrack.PLAYSTATE_PLAYING;
	}
	
	public boolean getFinished(){
		int playState=AudioTrack.PLAYSTATE_STOPPED;
		if(mAudioTrack!=null){
			playState=mAudioTrack.getPlayState();
		}
		return playState==AudioTrack.PLAYSTATE_STOPPED;
	}

	/// @todo: enable looping
	public void setLooping(boolean looping){}
	public boolean getLooping(){return false;}

	public void setGain(float gain){
		if(mAudioTrack!=null){
			mAudioTrack.setStereoVolume(gain,gain);
		}
		mGain=gain;
	}
	
	public void fadeToGain(float gain,int time){
		/// @todo: enable fading to gain
		setGain(gain);
	}
	
	public float getGain(){return mGain;}

	/// @todo: enable pitch
	public void setPitch(float pitch){}
	public float getPitch(){return 1.0f;}

	public void onMarkerReached(AudioTrack track){
		try{
			track.stop();
		}
		catch(Exception ex){
			ex.printStackTrace();
		}
	}

	public void onPeriodicNotification(AudioTrack track){
		if(mAudioStream==null){
			return;
		}
	
		try{
			int amount=mAudioStream.read(mStreamData,0,mStreamData.length);
			if(amount>=0 && amount<mStreamData.length){
				java.util.Arrays.fill(mStreamData,amount,mStreamData.length-amount,(byte)0);
				amount=mStreamData.length;
			}
			else if(amount<0){
				track.stop();
			}
			
			if(amount>0){
				track.write(mStreamData,0,amount);
			}
		}
		catch(Exception ex){
			ex.printStackTrace();
		}
	}
	
	void update(int dt){
		// Hack to work around MarkerReached not being called
		if(mAudioBuffer!=null && getPlaying()){
			us.toadlet.ribbit.AudioFormat format=mAudioBuffer.getAudioFormat();
			mPlayTime+=dt;
			int endTime=(mAudioBuffer.mData.length/format.frameSize()) * 1000 / format.samplesPerSecond;
			if(mPlayTime>endTime+500){
				stop();
			}
		}
	}
	
	ATAudioRegister mDevice;
	ATAudioBuffer mAudioBuffer;
	AudioStream mAudioStream;
	AudioTrack mAudioTrack;
	float mGain;
	byte[] mStreamData;
	int mPlayTime;
}
