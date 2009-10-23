/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine 
 *
 * Copyright (C) 2005, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer
 * 
 * All source code for the Toadlet Engine, including 
 * this file, is the sole property of Lightning Toads 
 * Productions, LLC. It has been developed on our own 
 * time, with our own tools, on our own hardware. None
 * of this code may be observed, used or changed 
 * without our explicit permission. 
 *
 *  - Lightning Toads Productions, LLC
 *
 ********** Copyright header - do not remove **********/

#ifndef BSP_NODE_H
#define BSP_NODE_H

#include "Polygon.h"
#include <toadlet/egg/List.h>
#include <toadlet/egg/math/Plane.h>

namespace bsp{

class TOADLET_API Node{
public:
	Node();
	~Node();

	toadlet::egg::List<Polygon>::iterator findBestSplitter(toadlet::egg::List<Polygon> &list,const toadlet::egg::Collection<toadlet::egg::math::Vector3> &vertexes,bool optimizeSplits,bool balanceTree,float epsilon) const;

	toadlet::egg::math::Plane plane;
	int child[2];
	int parent;
	int leaf; // One leaf, since they are always on front side
};

}

#endif
