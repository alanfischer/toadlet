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

#include <toadlet/tadpole/sensor/SensorResults.h>
#include <toadlet/tadpole/Node.h>

namespace toadlet{
namespace tadpole{
namespace sensor{

SensorResults::SensorResults():
	mScope(-1)
	//mResults
{}

void SensorResults::sensingBeginning(){
	mNodes.clear();
}
	
bool SensorResults::resultFound(Node *result,scalar distance){
	if((result->getScope()&mScope)==0){
		return true;
	}

	int handle=result->getUniqueHandle();
	Collection<Node::ptr>::iterator it,end;
	for(it=mNodes.begin(),end=mNodes.end();it!=end;++it){
		Node *node=*it;
		if(node->getUniqueHandle()>handle){
			break;
		}
	}
	mNodes.insert(it,result);

	return true;
}

void SensorResults::sensingEnding(){
	mRange=NodeRange::wrapCollection(mNodes);
}

}
}
}
