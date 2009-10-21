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

namespace toadlet{
namespace egg{

Profile *Profile::getInstance(){
	if(mTheProfile==NULL){
		mTheProfile=new Profile();
	}
	return mTheProfile;
}

Profile::Profile(){
}

void Profile::beginSection(const String &name){
	Timing::ptr timing;
	Map<String,Timing::ptr>::iterator it=mTimings.find(name);
	if(it!=mTimings.end()){
		timing=it->second;
	}
	else{
		timing=Timing::ptr(new Timing(name));
		mTimings[name]=timing;
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

	mTimingStack.add(timing);
}

void Profile::endSection(const String &name){
	uint64 time=System::utime();

	if(mTimingStack.size()==0){
		Error::unknown(Categories::TOADLET_EGG,
			"empty timing stack");
	}

	Timing::ptr timing=mTimingStack.back();
	mTimingStack.removeAt(mTimingStack.size()-1);

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
	if(mTimingStack.size()!=0){
		Error::unknown(Categories::TOADLET_EGG,
			"non empty timing stack");
	}

	mTimingHistory.add(mTimings);
	mTimings.clear();
}

void Profile::clearTimings(){
	mTimings.clear();
	mTimingStack.clear();
	mTimingHistory.clear();
}

int Profile::getTimingAverage(const String &name) const{
	uint64 time=0,count=0;
	int i;
	for(i=mTimingHistory.size()-1;i>=0;--i){
		const Map<String,Timing::ptr> &item=mTimingHistory.at(i);
		Map<String,Timing::ptr>::const_iterator it=item.find(name);
		if(it!=item.end()){
			time+=it->second->total;
			count++;
		}
	}

	if(count==0){
		return -1;
	}
	else{
		return time/count;
	}
}

Profile *Profile::mTheProfile;

}
}

