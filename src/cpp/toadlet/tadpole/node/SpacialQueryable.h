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

#ifndef TOADLET_TADPOLE_NODE_SPACIALQUERYABLE_H
#define TOADLET_TADPOLE_NODE_SPACIALQUERYABLE_H

#include <toadlet/tadpole/Types.h>

namespace toadlet{
namespace tadpole{
namespace node{

// This is a broader interface, to expose some functionality for querying states of the scene.
//  Maybe this should be just rolled into Scene?
class SceneQueryable{
public:
	virtual ~SceneQueryable(){}

	
	getRayTrace() -> actually reimplemented by HopScene to do a physical trace
	getNodesInAABox()
	getNodesInSphere()
	getNodesTouchingNode(Node)

	
	tracing	(segment, sphere, box)
	nodes in area	(sphere, box)
	nodes touching node?  Something here for telling what we are "in".  basically 2 direcitons this can work:  
		Triggers = node IS IN other node, or Passive = node IS IN other nodes.  wait, same thing.
		Triggers = node is NOTIFIED BY other nodes when it touches it?  or node ASKS IF he is in other nodes
	The key is some of these can be CACHED in a way.  Basically, if some nodes coorespond to stationary parts in a scene, then
		asking what nodes am in IN for a node in the scene will be CACHED (just check what leaf im in)
		
	Then, I can say:
		result=Scene->getNodesTouchingNode(player);
		int flags=0;
		for in(result)
			flags|=result.node->type;
		// Or something like that
		if(flags&water){
			// Swim!
		}
	
	virtual void nodeContents
	virtual void traceSegment(TraceResult &result,const Segment &segment)=0;
	virtual void traceSphere(TraceResult &result,const Segment &segment,const Sphere &sphere)=0;
	virtual void traceAABox(TraceResult &result,const Segment &segment,const AABox &box)=0;
};

}
}
}

#endif
