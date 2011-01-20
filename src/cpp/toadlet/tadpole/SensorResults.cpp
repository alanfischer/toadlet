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

#include <toadlet/tadpole/SensorResults.h>
#include <toadlet/tadpole/node/Node.h>

using namespace toadlet::tadpole::node;

namespace toadlet{
namespace tadpole{

SensorResults::SensorResults():
	mMaxResults(-1),
	mSortByHandle(false),
	mScope(-1),
	//mResults,
	mMaxDistance(0)
{}

SensorResults::~SensorResults(){
}

void SensorResults::setMaxNumResults(int num){
	mMaxResults=num;
	if(num>=0){
		mResults.reserve(num);
	}
}

// This could be optimized by storing some 'sensor data' into the node, which would be an index into our array
//  It would avoid the initial lookup, but I'm not sure I want to do that
Node *SensorResults::getNextResult(Node *result){
	if(result==NULL){
		if(mResults.size()==0){
			return NULL;
		}
		else{
			return mResults[0];
		}
	}

	int i;
	for(i=0;i<mResults.size();++i){
		if(mResults[i]==result){
			++i;
			break;
		}
	}

	if(i>=mResults.size()){
		return NULL;
	}
	else{
		return mResults[i];
	}
}

void SensorResults::sensingBeginning(){
	mResults.clear();
	mMaxDistance=0;
}
	
bool SensorResults::resultFound(Node *result,scalar distance){
	if((result->getScope()&mScope)==0){
		return true;
	}

	if(mMaxResults>0 && mResults.size()>=mMaxResults && distance>mMaxDistance){
		return mMaxDistance>0; // If 0, then we can't find anything closer, so stop searching
	}

	/// @todo: Implement SortByHandle and closest results
	// if(results full, but result is closer){
	//   remove farthest result (stored as member variable)
	// }
	// if(sorted){
	//   insert at sorted place
	// }
	// else{
	//   insert at end
	// }
	// if(had farthest result){
	//   remember farthest result
	// }

	if(mSortByHandle){
		int handle=result->getUniqueHandle();
		int i;
		for(i=0;i<mResults.size();++i){
			if(mResults[i]->getUniqueHandle()>=handle){
				mResults.insert(mResults.begin()+i,result);
				break;
			}
		}
		if(i==mResults.size()){
			mResults.add(result);
		}
	}
	else{
		mResults.add(result);
	}

	return true;
}

void SensorResults::sensingEnding(){
}

}
}

