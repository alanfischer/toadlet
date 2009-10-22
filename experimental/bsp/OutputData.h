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

#ifndef BSP_OUTPUTDATA_H
#define BSP_OUTPUTDATA_H

#include <toadlet/egg/List.h>
#include <toadlet/egg/Collection.h>
#include <toadlet/egg/math/Vector3.h>
#include "Polygon.h"
#include "Leaf.h"
#include "Node.h"

namespace bsp{

class OutputData{
public:
	toadlet::egg::List<Polygon> polygons;
	toadlet::egg::Collection<toadlet::egg::math::Vector3> vertexes;
	toadlet::egg::Collection<Leaf> leafs;
	toadlet::egg::Collection<Node> nodes;
	int numSplits;
};

}

#endif
