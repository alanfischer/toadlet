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

#include <toadlet/egg/Profile.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/System.h>
#include <toadlet/egg/Map.h>
#include <toadlet/egg/Collection.h>

namespace toadlet{
namespace egg{

class ProfileData{
public:
	Map<String,Profile::Timing::ptr> timings;
	Collection<Profile::Timing::ptr> timingStack;
	Collection<Map<String,Profile::Timing::ptr> > timingHistory;
	Map<String,String> timingNames;
};

Profile *Profile::getInstance(){
	if(mTheProfile==NULL){
		mTheProfile=new Profile();
	}
	return mTheProfile;
}

Profile::Profile(){
	mData=new ProfileData();
}

Profile::~Profile(){
	delete mData;
}

void Profile::beginSection(const String &name){
	Timing::ptr timing;
	Map<String,Timing::ptr>::iterator it=mData->timings.find(name);
	if(it!=mData->timings.end()){
		timing=it->second;
	}
	else{
		timing=Timing::ptr(new Timing(name));
		mData->timings[name]=timing;

		if(mData->timingNames.find(name)==mData->timingNames.end()){
			mData->timingNames[name]=name;
		}
	}

	timing->depth++;
	if(timing->depth==1){
		timing->current=System::utime();
	}
	else if(timing->depth>STACK_MAX_DEPTH){
		Error::unknown(Categories::TOADLET_EGG,
			"timing stack depth > STACK_MAX_DEPTH");
		return;
	}

	mData->timingStack.add(timing);
}

void Profile::endSection(const String &name){
	uint64 time=System::utime();

	if(mData->timingStack.size()==0){
		Error::unknown(Categories::TOADLET_EGG,
			"empty timing stack");
	}

	Timing::ptr timing=mData->timingStack.back();
	mData->timingStack.removeAt(mData->timingStack.size()-1);

	if(timing->name.equals(name)==false){
		Error::unknown(Categories::TOADLET_EGG,
			"mismatched begin/endSection");
		return;
	}

	timing->depth--;
	if(timing->depth==0){
		timing->total+=(time-timing->current);
	}
	else if(timing->depth<0){
		Error::unknown(Categories::TOADLET_EGG,
			"timing stack depth < 0");
		return;
	}
}

void Profile::addTimings(){
	if(mData->timingStack.size()!=0){
		Error::unknown(Categories::TOADLET_EGG,
			"non empty timing stack");
	}

	mData->timingHistory.add(mData->timings);
	mData->timings.clear();
}

void Profile::clearTimings(){
	mData->timings.clear();
	mData->timingStack.clear();
	mData->timingHistory.clear();
	mData->timingNames.clear();
}

int Profile::getTimingAverage(const String &name) const{
	uint64 time=0,count=0;
	int i;
	for(i=mData->timingHistory.size()-1;i>=0;--i){
		const Map<String,Timing::ptr> &item=mData->timingHistory.at(i);
		Map<String,Timing::ptr>::const_iterator it=item.find(name);
		if(it!=item.end()){
			time+=it->second->total;
		}
		count++;
	}

	if(count==0){
		return -1;
	}
	else{
		return time/count;
	}
}

int Profile::getNumTimings() const{
	return mData->timingNames.size();
}

String Profile::getTimingName(int i) const{
	return mData->timingNames[i].first;
}

Profile *Profile::mTheProfile;

}
}

