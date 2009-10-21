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

#include "InputData.h"
#include <toadlet/tadpole/mesh/MeshData.h>

#ifndef BSP_MSHCONVERTER_H
#define BSP_MSHCONVERTER_H

namespace bsp{

class TOADLET_API MeshDataConverter{
public:
	static void convertMeshDataToInputData(toadlet::tadpole::mesh::MeshData *mesh,InputData *in,int submesh=-1);
};

}

#endif

