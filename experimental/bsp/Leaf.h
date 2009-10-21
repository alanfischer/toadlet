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

#ifndef BSP_LEAF_H
#define BSP_LEAF_H

#include <toadlet/egg/Collection.h>

namespace bsp{

class TOADLET_API Leaf{
public:
	Leaf();

	int contents;
	toadlet::egg::Collection<int> polygonIndexes;
	toadlet::egg::Collection<int> portalIndexes;
	int sector;
};

}

#endif
