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
import android.media.AudioFormat;
import android.media.AudioTrack;

public class ATAudioBuffer extends BaseAudioBuffer{
	public ATAudioBuffer(){
		mAudioFormat=new us.toadlet.ribbit.AudioFormat(0,0,0);
	}

	public AudioBuffer getRootAudioBuffer(){return this;}

	public boolean create(AudioStream stream){
		System.out.println("ATAudioBuffer.create");

		if(stream==null){
			return false;
		}
		us.toadlet.ribbit.AudioFormat format=stream.getAudioFormat();
		mAudioFormat.setBitsPerSample(format.getBitsPerSample());
		mAudioFormat.setChannels(format.getChannels());
		mAudioFormat.setSamplesPerSecond(format.getSamplesPerSecond());

		try{
			int available=stream.length();
			// Adjust available to by a multiple of MinBuffer
			int sps=format.getSamplesPerSecond();
			int chan=(format.getChannels()==2?AudioFormat.CHANNEL_OUT_STEREO:AudioFormat.CHANNEL_OUT_MONO);
			int bps=(format.getBitsPerSample()==8?AudioFormat.ENCODING_PCM_8BIT:AudioFormat.ENCODING_PCM_16BIT);
			int min=AudioTrack.getMinBufferSize(sps,chan,bps);
			available=((int)(available/min)+1)*(min);

			mData=new byte[available];
			stream.read(mData,available);
		}
		catch(Exception ex){
			return false;
		}
		catch(OutOfMemoryError er){
			return false;
		}
		
		return true;
	}
	
	public void destroy(){
		System.out.println("ATAudioBuffer.destroy");

		mData=null;
	}
	
	us.toadlet.ribbit.AudioFormat getAudioFormat(){return mAudioFormat;}

	byte[] mData;
	us.toadlet.ribbit.AudioFormat mAudioFormat;
}
