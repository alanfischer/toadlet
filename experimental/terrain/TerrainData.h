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

#ifndef TOADLET_UTIL_TERRAIN_TERRAINDATA_H
#define TOADLET_UTIL_TERRAIN_TERRAINDATA_H

namespace toadlet{
namespace util{
namespace terrain{

class TerrainData{
public:
	TerrainData(){
		heightData=NULL;
		width=0;
		height=0;
	}

	float *heightData;
	int width;
	int height;
};

}
}
}

#endif
