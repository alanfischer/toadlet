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

public class ATAudioBuffer implements AudioBuffer{
	public ATAudioBuffer(){
		mAudioFormat=new AudioFormat(0,0,0);
	}

	public AudioBuffer getRootAudioBuffer(){return this;}

	public boolean create(AudioStream stream){
		System.out.println("ATAudioBuffer.create");

		if(stream==null){
			return false;
		}
		AudioFormat format=stream.getAudioFormat();
		mAudioFormat.set(format);

		try{
			int available=stream.available();
			mData=new byte[available];
			stream.read(mData,0,available);
		}
		catch(java.io.IOException ex){
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
	
	AudioFormat getAudioFormat(){return mAudioFormat;}

	byte[] mData;
	AudioFormat mAudioFormat;
}
