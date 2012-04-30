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

#ifndef TOADLET_TADPOLE_STUDIO_STUDIOMODELANIMATION_H
#define TOADLET_TADPOLE_STUDIO_STUDIOMODELANIMATION_H

#include <toadlet/tadpole/animation/Animation.h>
#include <toadlet/tadpole/studio/StudioModelNode.h>

namespace toadlet{
namespace tadpole{
namespace studio{

class StudioModelAnimation:public Animation{
public:
	StudioModelAnimation(StudioModelNode::ptr node,bool gait){
		mNode=node;
		mGait=gait;
	}

	void set(scalar value){
		studioseqdesc *sseqdesc=seqdesc();
		if(sseqdesc==NULL) return;
		value=value*sseqdesc->fps;
		if(mGait==false){mNode->setSequenceTime(value);}
		else{mNode->setGaitSequenceTime(value);}
	}
	scalar getMin() const{return 0;}
	scalar getMax() const{
		studioseqdesc *sseqdesc=seqdesc();
		if(sseqdesc==NULL || sseqdesc->numframes<=1) return 0;
		return (sseqdesc->numframes-1)/sseqdesc->fps;
	}

protected:
	studioseqdesc *seqdesc() const{
		if(mNode->getModel()==NULL) return NULL;
		return mNode->getModel()->seqdesc(mGait==false?mNode->getSequence():mNode->getGaitSequence());
	}

	StudioModelNode::ptr mNode;
	bool mGait;
};

}
}
}

#endif
