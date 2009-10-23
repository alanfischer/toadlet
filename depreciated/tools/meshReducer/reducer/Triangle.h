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

#ifndef REDUCER_TRIANGLE_H
#define TOADLET_UTIL_REDUCER_TRIANGLE_H

#include <toadlet/egg/Collection.h>
#include <toadlet/egg/math/Vector3.h>
#include "Vertex.h"
#include "ReducerList.h"

namespace reducer{

class TOADLET_API Triangle{
public:
	Triangle(Vertex *v0,Vertex *v1,Vertex *v2,int subMeshId,int indexIds[3],ReducerList<Triangle *> *triangles);
	virtual ~Triangle();

	bool hasVertex(Vertex *v);
	void replaceVertex(Vertex *vo,Vertex *vn);
	void normal();
	void updateIndexIds(toadlet::egg::Collection<VertexData> vdOld,toadlet::egg::Collection<VertexData> vdNew);

	Vertex *mVertex[3];
	toadlet::egg::math::Vector3 mNormal;
	int mSubMeshId;
	int mIndexIds[3];
	int mPlace;

	ReducerList<Triangle *> *mTriangles;
};

}

#endif
