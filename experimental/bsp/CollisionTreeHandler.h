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

#ifndef BSP_COLLISIONTREEHANDLER_H
#define BSP_COLLISIONTREEHANDLER_H

#include <toadlet/egg/io/InputStream.h>
#include <toadlet/egg/io/OutputStream.h>
#include "Node.h"
#include "OutputData.h"

namespace bsp{

// The CollisionTree format is stored in big endian, for ease of loading on mobile devices
class TOADLET_API CollisionTreeHandler{
public:
	static Node *loadCollisionTree(toadlet::egg::io::InputStream *in);
	static void saveCollisionTree(OutputData *data,toadlet::egg::io::OutputStream *out);

protected:

	static int floatToFixed(float f);
	static float fixedToFloat(int f);
};

}

#endif
