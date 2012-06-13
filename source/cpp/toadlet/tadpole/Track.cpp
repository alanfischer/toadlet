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

#include <toadlet/tadpole/Track.h>

namespace toadlet{
namespace tadpole{

Track::Track(VertexFormat *format):
	//mFormat,
	mIndex(0),
	mData(NULL),
	//mTimes,
	mLength(0)
{

	mFormat=format;
	int i;
	for(i=0;i<mFormat->getNumElements();++i){
		int semantic=mFormat->getElementSemantic(i);
		if(semantic>=mSemantics.size()){
			mSemantics.resize(semantic+1,-1);
		}
		mSemantics[semantic]=i;
	}
}

int Track::addKeyFrame(scalar time,void *frame){
	int size=mFormat->getVertexSize();
	int index=mTimes.size();
Logger::alert("ADDING FRAME");
	mTimes.resize(mTimes.size()+1);
	mTimes[index]=time;
	mData.resize(mData.size()+size);
Logger::alert("ADDING FRAME2");
	if(frame!=NULL){
Logger::alert("ADDING FRAME3");
		memcpy(&mData[size*index],frame,size);
Logger::alert("ADDING FRAME4");
	}

Logger::alert("ADDING FRAME5");
	mVBA.unlock();
	mVBA.lock(this,mFormat);

	return index;
}

bool Track::getKeyFrame(void *frame,int index){
	if(index<0 || index>=mTimes.size()){
		return false;
	}

	int size=mFormat->getVertexSize();
	memcpy(frame,&mData[size*index],size);
	return true;
}

scalar Track::getKeyFramesAtTime(scalar time,int &f1,int &f2,int &trackHint) const{
	int numKeyFrames=mTimes.size();
	if(numKeyFrames<2){
		if(numKeyFrames==1){
			f1=0;
			f2=0;
		}
		return 0;
	}

	if(trackHint<0 || trackHint>=numKeyFrames){
		trackHint=0;
	}

	int it=trackHint;
	int it2=0;
	if(mTimes.at(it)>=time){
		// Search backwards
		while(it!=0 && mTimes.at(it)>time){
			--it;
		}
	}
	else{
		// Search forwards
		while(it!=numKeyFrames && mTimes.at(it)<time){
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

	f1=it;
	f2=it2;

	if(mTimes[f1]<mTimes[f2]){
		time=Math::div(time-mTimes[f1],mTimes[f2]-mTimes[f1]);
	}
	// This code is removed, I'm not sure what purpose its supposed to serve
	//  and it introduced a dependency on mParent, which I'd like to remove
	//  -back in for now, cause i added length to here
	else if(mTimes[f1]>mTimes[f2]){
		time=Math::div(time-mTimes[f1],mLength-mTimes[f1]);
	}
	else{
		time=0;
	}

	return time;
}

void Track::compile(){
	mLength=0;
	int i;
	for(i=0;i<mTimes.size();++i){
		if(mLength<mTimes[i]){
			mLength=mTimes[i];
		}
	}
}

}
}
