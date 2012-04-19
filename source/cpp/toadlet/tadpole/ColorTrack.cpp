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

#include <toadlet/tadpole/ColorTrack.h>

namespace toadlet{
namespace tadpole{

ColorTrack::ColorTrack():
	index(0),
	length(0)
{}

scalar ColorTrack::getKeyFramesAtTime(scalar time,const ColorKeyFrame *&f1,const ColorKeyFrame *&f2,int &trackHint) const{
	int numKeyFrames=keyFrames.size();
	if(numKeyFrames<2){
		if(numKeyFrames==1){
			f1=&keyFrames.at(0);
			f2=&keyFrames.at(0);
		}
		return 0;
	}

	if(trackHint<0 || trackHint>=numKeyFrames){
		trackHint=0;
	}

	int it=trackHint;
	int it2=0;
	if(keyFrames.at(it).time>=time){
		// Search backwards
		while(it!=0 && keyFrames.at(it).time>time){
			--it;
		}
	}
	else{
		// Search forwards
		while(it!=numKeyFrames && keyFrames.at(it).time<time){
			++it;
		}

		--it;
	}

	trackHint=it;

	if(it+1==numKeyFrames){
		it2=numKeyFrames-1;
	}
	else{
		it2=it+1;
	}

	f1=&keyFrames.at(it);
	f2=&keyFrames.at(it2);

	if(f1->time<f2->time){
		time=Math::div(time-f1->time,f2->time-f1->time);
	}
	// This code is removed, I'm not sure what purpose its supposed to serve
	//  and it introduced a dependency on mParent, which I'd like to remove
	//  -back in for now, cause i added length to here
	else if(f1->time>f2->time){
		time=Math::div(time-f1->time,length-f1->time);
	}
	else{
		time=0;
	}

	return time;
}

void ColorTrack::compile(){
	length=0;
	int i;
	for(i=0;i<keyFrames.size();++i){
		if(length<keyFrames[i].time){
			length=keyFrames[i].time;
		}
	}
}

}
}
