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

#ifndef M3GCONVERTER_H
#define M3GCONVERTER_H

#include <toadlet/tadpole/mesh/Mesh.h>
#include <toadlet/egg/io/Stream.h>

#define USE_ACTC
#define DONT_USE_NVTRISTRIP

#ifdef USE_ACTC
extern "C"{
	#include <stdio.h>
	#include "../shared/actc/tc.h"
}
#endif

#ifdef USE_NVTRISTRIP
#include "../shared/NVTriStrip/NvTriStrip.h"
#endif

class M3GObject3D;

class M3GConverter{
public:
	M3GConverter();
	~M3GConverter();

	bool convertMesh(toadlet::tadpole::mesh::Mesh *mesh,toadlet::egg::io::Stream *out,float scale,int forceBytes,bool invertYTexCoord,bool viewable,int animation);

protected:
	M3GObject3D *buildSceneGraph(toadlet::tadpole::mesh::Mesh *mesh,float scale,int forceBytes,bool invertYTexCoord,bool viewable,int animation);

#ifdef USE_ACTC
	ACTCData *mTC;
#endif
};

#endif

