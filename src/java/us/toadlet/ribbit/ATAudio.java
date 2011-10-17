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
		mAudioBuffer=(buffer!=null)?(ATAudioBuffer)buffer.getRootAudioBuffer():null;

		if(mAudioBuffer==null){
			return false;
		}
		
		createAudioTrack();
		
		return true;
	}
	
	public boolean create(AudioStream stream){
		mAudioBuffer=null;
		mAudioStream=stream;

		if(mAudioStream==null){
			return false;
		}

		createAudioTrack();

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
				mAudioTrack.stop();
				mAudioTrack.setPlaybackHeadPosition(0);
			
				if(mAudioBuffer!=null){
					mAudioTrack.write(mAudioBuffer.mData,0,mAudioBuffer.mData.length);
				}
				else if(mAudioStream!=null){
					onPeriodicNotification(mAudioTrack);
				}
				mAudioTrack.play();
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
				if(mAudioTrack.getState()!=AudioTrack.STATE_UNINITIALIZED){
					mAudioTrack.stop();
					mAudioTrack.setPlaybackHeadPosition(0);
//					mAudioTrack.release();
				}
//				mAudioTrack=null;
			}
			catch(Exception ex){
				return false;
			}
		}

		return true;
	}
	
	// If the AudioTrack is alive, it is playing since we can't properly stop and reuse them
	public boolean getPlaying(){
		int playState=AudioTrack.PLAYSTATE_STOPPED;
		if(mAudioTrack!=null){
			playState=mAudioTrack.getPlayState();
		}
		return mPlayTime<mEndTime;//playState==AudioTrack.PLAYSTATE_PLAYING;
	}
	
	public boolean getFinished(){
		int playState=AudioTrack.PLAYSTATE_STOPPED;
		if(mAudioTrack!=null){
			playState=mAudioTrack.getPlayState();
		}
		return mPlayTime>=mEndTime;//playState==AudioTrack.PLAYSTATE_STOPPED;
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
		stop();
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
				stop();
			}
			
			if(amount>0){
				mAudioTrack.write(mStreamData,0,amount);
			}
		}
		catch(Exception ex){
			ex.printStackTrace();
		}
	}
	
	void update(int dt){
		// Hack to work around MarkerReached not being called
		if(mAudioBuffer!=null && getPlaying()){
			mPlayTime+=dt;
			if(mPlayTime>mEndTime+500){
				stop();
			}
		}
	}
	
	void createAudioTrack(){
		if(mAudioBuffer!=null){
			us.toadlet.ribbit.AudioFormat format=mAudioBuffer.getAudioFormat();
			int sps=format.samplesPerSecond;
			int chan=(format.channels==2?AudioFormat.CHANNEL_OUT_STEREO:AudioFormat.CHANNEL_OUT_MONO);
			int bps=(format.bitsPerSample==8?AudioFormat.ENCODING_PCM_8BIT:AudioFormat.ENCODING_PCM_16BIT);
			int available=mAudioBuffer.mData.length;

			// We have to allocate twice the available amount to avoid a crash on some 2.3 systems.
			// This may be causing the audio to play twice sometimes.  Maybe we can allocate only whats needed on 2.2, but twice on 2.3,
			// trusting that OpenSL will be used on 2.3 systems instead.
			mAudioTrack=new AudioTrack(AudioManager.STREAM_ALARM,sps,chan,bps,available*2,AudioTrack.MODE_STREAM);
			mAudioTrack.setPlaybackPositionUpdateListener(this);
			mAudioTrack.setNotificationMarkerPosition(available/format.frameSize());
			mEndTime=(mAudioBuffer.mData.length/format.frameSize()) * 1000 / format.samplesPerSecond;
		}
		else if(mAudioStream!=null){
			us.toadlet.ribbit.AudioFormat format=mAudioStream.getAudioFormat();
			int sps=format.samplesPerSecond;
			int chan=(format.channels==2?AudioFormat.CHANNEL_OUT_STEREO:AudioFormat.CHANNEL_OUT_MONO);
			int bps=(format.bitsPerSample==8?AudioFormat.ENCODING_PCM_8BIT:AudioFormat.ENCODING_PCM_16BIT);
			int available=AudioTrack.getMinBufferSize(sps,chan,bps);

			mStreamData=new byte[available];
			mAudioTrack=new AudioTrack(AudioManager.STREAM_MUSIC,sps,chan,bps,available,AudioTrack.MODE_STREAM);
			mAudioTrack.setPlaybackPositionUpdateListener(this);
			mAudioTrack.setPositionNotificationPeriod(available/format.frameSize());
		}
	}
	
	ATAudioRegister mDevice;
	ATAudioBuffer mAudioBuffer;
	AudioStream mAudioStream;
	AudioTrack mAudioTrack;
	float mGain;
	byte[] mStreamData;
	int mPlayTime;
	int mEndTime;
}
