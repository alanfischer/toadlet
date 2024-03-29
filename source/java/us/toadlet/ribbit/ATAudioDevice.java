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

import android.os.Handler;
import android.os.Looper;
import android.content.Context;
import android.media.AudioManager;

public class ATAudioDevice extends BaseAudioDevice implements Runnable{
	public ATAudioDevice(Context context){
		super(0,false);
		
		mContext=context;
		
//		mCaps=new AudioCaps();
//		mCaps.maxSources=16;
//		mCaps.streaming=true;
//		mCaps.positional=false;

		// All AudioTrack commands need to be issued from the main thread, or else they may crash.
		// Since we can not post Messages or Runnables from code called from JNI, we start a handler that reposts itself every 50ms, and update our ATAudios that way
		mHandler=new Handler(Looper.getMainLooper());
	}

	public boolean create(){
		System.out.println("ATAudioDevice.create");
		
		mHandler.post(this);
		
		return true;
	}
	
	public void destroy(){
		System.out.println("ATAudioDevice.destroy");
		
		mHandler.removeCallbacks(this);
	}
	
	public void activate(){}
	public void deactivate(){}

	public AudioBuffer createAudioBuffer(){return new ATAudioBuffer();}
	public Audio createAudio(){return new ATAudio(this);}

	public void setListenerGain(float gain){
		AudioManager manager=(AudioManager)mContext.getSystemService(Context.AUDIO_SERVICE);
		manager.setStreamVolume(AudioManager.STREAM_MUSIC,(int)(gain * manager.getStreamMaxVolume(AudioManager.STREAM_MUSIC)), 0);
	}
	
	public void update(int dt){}
	
	public void run(){
		mHandler.removeCallbacks(this);
		internal_update(50);
		mHandler.postDelayed(this,50);
	}

	protected void internal_update(int dt){
		int i;
		for(i=0;i<mAudios.size();++i){
			mAudios.get(i).update(dt);
		}
	}

//	public boolean getAudioCaps(AudioCaps caps){caps.set(mCaps);return true;}

	public void registerAudio(Audio audio){
		mAudios.add((ATAudio)audio);
	}
	
	public void unregisterAudio(Audio audio){
		mAudios.remove((ATAudio)audio);
	}
	
	Context mContext;
	int mMaxBufferSize;
//	AudioCaps mCaps;
	Handler mHandler;
	java.util.Vector<ATAudio> mAudios=new java.util.Vector<ATAudio>();
}
