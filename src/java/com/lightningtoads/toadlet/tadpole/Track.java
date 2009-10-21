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

package com.lightningtoads.toadlet.tadpole;

#include <com/lightningtoads/toadlet/tadpole/Types.h>

import java.util.Vector;

public class Track{
	public Track(){}

	public scalar getKeyFramesAtTime(scalar time,KeyFrame[] f1,KeyFrame[] f2,int[] trackHint,int trackHintOffset){
		int numKeyFrames=keyFrames.size();
		if(numKeyFrames<2){
			if(numKeyFrames==1){
				f1[0]=keyFrames.get(0);
				f2[0]=keyFrames.get(0);
			}
			return 0;
		}

		if(trackHint[0]<0 || trackHint[0]>=numKeyFrames){
			trackHint[0]=0;
		}

		int it=trackHint[trackHintOffset];
		int it2=0;
		if(keyFrames.get(it).time>=time){
			// Search backwards
			while(it!=0 && keyFrames.get(it).time>time){
				--it;
			}
		}
		else{
			// Search forwards
			while(it!=numKeyFrames && keyFrames.get(it).time<time){
				++it;
			}

			--it;
		}

		trackHint[trackHintOffset]=it;

		if(it+1==numKeyFrames){
			it2=numKeyFrames-1;
		}
		else{
			it2=it+1;
		}

		f1[0]=keyFrames.get(it);
		f2[0]=keyFrames.get(it2);

		if(f1[0].time<f2[0].time){
			time=Math.div(time-f1[0].time,f2[0].time-f1[0].time);
		}
		// This code is removed, I'm not sure what purpose its supposed to serve
		//  and it introduced a dependency on mParent, which I'd like to remove
		//  -back in for now, cause i added length to here
		else if(f1[0].time>f2[0].time){
			time=Math.div(time-f1[0].time,length-f1[0].time);
		}
		else{
			time=0;
		}

		return time;
	}

	public void compile(){
		length=0;
		int i;
		for(i=0;i<keyFrames.size();++i){
			if(length<keyFrames.get(i).time){
				length=keyFrames.get(i).time;
			}
		}
	}

	public int index=0;
	public scalar length=0; // calculated by compile
	public Vector<KeyFrame> keyFrames=new Vector<KeyFrame>();
}
