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

#ifndef REDUCER_VERTEX_H
#define REDUCER_VERTEX_H

#include <toadlet/egg/Collection.h>
#include <toadlet/egg/math/Vector2.h>
#include <toadlet/egg/math/Vector3.h>
#include <toadlet/egg/math/Vector4.h>
#include <toadlet/peeper/VertexBuffer.h>
#include "ReducerList.h"

namespace reducer{

class Triangle;

class VertexData{
public:
	toadlet::egg::math::Vector3 position;
	toadlet::egg::math::Vector3 normal;
	toadlet::egg::math::Vector2 texture;
	toadlet::egg::math::Vector4 color;
	toadlet::peeper::BoneAssignmentCollection bone;
	int id;
};

class TOADLET_API Vertex{
public:
	Vertex(toadlet::egg::math::Vector3 position,toadlet::egg::math::Vector3 normal,toadlet::egg::math::Vector2 texture,toadlet::egg::math::Vector4 color,int id,ReducerList<Vertex *> *vertices);
	Vertex(VertexData vd,int id,ReducerList<Vertex *> *vertices);
	virtual ~Vertex();

	toadlet::egg::math::Vector3 position();
	void deleteNonNeighbor(Vertex *v);
	bool onBorder();

	int mId;
	toadlet::egg::Collection<VertexData> mVertexData;

	int mPlace;
	int mHeapSpot;
	float mCost;
	Vertex *mCollapse;

	ReducerList<Vertex *> *mVertices;
	ReducerList<Vertex *> mNeighbors;
	ReducerList<Triangle *> mFaces;
};

}

#endif
