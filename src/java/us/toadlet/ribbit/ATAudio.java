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
		mAudioGain=1.0f;
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
		mDesiredState=AudioTrack.PLAYSTATE_PLAYING;
		return true;
	}
	
	public boolean stop(){
		mDesiredState=AudioTrack.PLAYSTATE_STOPPED;
		return true;
	}
	
	public synchronized boolean internal_play(){
		mBufferPosition=0;
		if(mAudioTrack!=null){
			try{
				mAudioTrack.play();
				onPeriodicNotification(mAudioTrack);
			}
			catch(Exception ex){
				return false;
			}
		}
		
		return true;
	}
	
	public synchronized boolean internal_stop(){
		if(mAudioTrack!=null){
			try{
				if(mAudioTrack.getState()!=AudioTrack.STATE_UNINITIALIZED){
					mAudioTrack.stop();
				}
			}
			catch(Exception ex){
				return false;
			}
		}

		return true;
	}
	
	// If the AudioTrack is alive, it is playing since we can't properly stop and reuse them
	public boolean getPlaying(){
		return mDesiredState==AudioTrack.PLAYSTATE_PLAYING || mCurrentState==AudioTrack.PLAYSTATE_PLAYING;
	}
	
	public boolean getFinished(){
		return mDesiredState==AudioTrack.PLAYSTATE_STOPPED && mCurrentState==AudioTrack.PLAYSTATE_STOPPED;
	}

	/// @todo: enable looping
	public void setLooping(boolean looping){}
	public boolean getLooping(){return false;}

	public void setGain(float gain){
		mGain=gain;
	}
	
	public void fadeToGain(float gain,int time){
		/// @todo: enable fading to gain
		mGain=gain;
	}
	
	public float getGain(){return mGain;}

	/// @todo: enable pitch
	public void setPitch(float pitch){}
	public float getPitch(){return 1.0f;}

	public synchronized void onMarkerReached(AudioTrack track){}

	public synchronized void onPeriodicNotification(AudioTrack track){
		try{
			int amount=0;
			if(mAudioBuffer!=null){
				int left=mAudioBuffer.mData.length-mBufferPosition;
				amount=(left>mStreamData.length)?mStreamData.length:left;
				System.arraycopy(mAudioBuffer.mData,mBufferPosition,mStreamData,0,amount);
				mBufferPosition+=amount;
			}
			if(mAudioStream!=null){
				amount=mAudioStream.read(mStreamData,0,mStreamData.length);
			}

			if(amount>=0 && amount<mStreamData.length){
				if(mAudioBuffer!=null){
					mAudioTrack.stop();
				}
				if(mAudioStream!=null){
					java.util.Arrays.fill(mStreamData,amount,mStreamData.length-amount,(byte)0);
					amount=mStreamData.length;
				}
			}
			if(amount<0){
				mAudioTrack.stop();
			}
			
			if(amount>0){
				mAudioTrack.write(mStreamData,0,amount);
			}
		}
		catch(Exception ex){
			ex.printStackTrace();
		}
	}
	
	synchronized void update(int dt){
		if(mAudioTrack!=null){
			mCurrentState=mAudioTrack.getPlayState();
			if(mDesiredState==AudioTrack.PLAYSTATE_PLAYING && mCurrentState==AudioTrack.PLAYSTATE_STOPPED){
				mDesiredState=0;
				System.out.println("internal_play");
				internal_play();
				System.out.println("internal_play end");
			}
			if(mDesiredState==AudioTrack.PLAYSTATE_STOPPED && mCurrentState==AudioTrack.PLAYSTATE_PLAYING){
				mDesiredState=0;
				internal_stop();
			}
			if(mGain!=mAudioGain){
				mAudioGain=mGain;
				mAudioTrack.setStereoVolume(mGain,mGain);
			}
			mCurrentState=mAudioTrack.getPlayState();
		}
	}

	void createAudioTrack(){
		us.toadlet.ribbit.AudioFormat format=null;
		if(mAudioBuffer!=null){
			format=mAudioBuffer.getAudioFormat();
		}
		else{
			format=mAudioStream.getAudioFormat();
		}

		int sps=format.samplesPerSecond;
		int chan=(format.channels==2?AudioFormat.CHANNEL_OUT_STEREO:AudioFormat.CHANNEL_OUT_MONO);
		int bps=(format.bitsPerSample==8?AudioFormat.ENCODING_PCM_8BIT:AudioFormat.ENCODING_PCM_16BIT);
		int available=AudioTrack.getMinBufferSize(sps,chan,bps);

		mStreamData=new byte[available];
		mAudioTrack=new AudioTrack(AudioManager.STREAM_MUSIC,sps,chan,bps,available,AudioTrack.MODE_STREAM);
		mAudioTrack.setPlaybackPositionUpdateListener(this);
		mAudioTrack.setPositionNotificationPeriod(available/format.frameSize());
	}
	
	ATAudioRegister mDevice;
	ATAudioBuffer mAudioBuffer;
	AudioStream mAudioStream;
	AudioTrack mAudioTrack;
	float mGain,mAudioGain;
	byte[] mStreamData;
	int mBufferPosition;
	int mDesiredState,mCurrentState;
}
