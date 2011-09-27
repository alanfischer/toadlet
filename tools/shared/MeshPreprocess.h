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

#ifndef MESHPREPROCESSES_H
#define MESHPREPROCESSES_H

#include <toadlet/toadlet.h>

class MeshPreprocess{
public:
	static void adjustTexCoords(Mesh *mesh,float texCoordAdjust);
	static void adjustPositions(Mesh *mesh,float positionAdjust);
};

#endif
