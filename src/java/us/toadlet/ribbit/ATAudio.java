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

class ATAudio implements Audio{
	public ATAudio(){
		mGain=1.0f;
	}

	public boolean create(AudioBuffer buffer){
		mAudioBuffer=buffer;
		mAudioStream=null;

		ATAudioBuffer atbuffer=(ATAudioBuffer)buffer.getRootAudioBuffer();
		mAudioTrack=atbuffer.mAudioTrack;
		
		return true;
	}
	
	public boolean create(AudioStream stream){
		us.toadlet.ribbit.AudioFormat format=stream.getAudioFormat();
		int sps=format.samplesPerSecond;
		int chan=(format.channels==2?AudioFormat.CHANNEL_OUT_STEREO:AudioFormat.CHANNEL_OUT_MONO);
		int bps=(format.bitsPerSample==8?AudioFormat.ENCODING_PCM_8BIT:AudioFormat.ENCODING_PCM_16BIT);
		int available=AudioTrack.getMinBufferSize(sps,chan,bps);

		mAudioBuffer=null;
		mAudioStream=stream;
		
		mAudioTrack=new AudioTrack(AudioManager.STREAM_MUSIC,sps,chan,bps,available,AudioTrack.MODE_STREAM);
		
		return true;
	}
	
	public void destroy(){
		if(mAudioBuffer!=null){
			//AudioBuffer destroyed by resource management
			mAudioBuffer=null;

			//AudiOTrack released by AudioBuffer
			mAudioTrack=null;
		}
		if(mAudioStream!=null){
			try{
				mAudioStream.close();
			}
			catch(java.io.IOException ex){}
			mAudioStream=null;

			mAudioTrack.release();
			mAudioTrack=null;
		}
	}

	public AudioBuffer getAudioBuffer(){return mAudioBuffer;}
	public AudioStream getAudioStream(){return mAudioStream;}

	public boolean play(){
		if(mAudioTrack!=null){
			mAudioTrack.play();
		}
		
		return true;
	}
	
	public boolean stop(){
		if(mAudioTrack!=null){
			mAudioTrack.stop();
		}

		return true;
	}
	
	public boolean getPlaying(){
		int playState=AudioTrack.PLAYSTATE_STOPPED;
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
	
	AudioBuffer mAudioBuffer;
	AudioStream mAudioStream;
	AudioTrack mAudioTrack;
	float mGain;
}
