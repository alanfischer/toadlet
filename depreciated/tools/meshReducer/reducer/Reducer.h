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

#ifndef REDUCER_REDUCER_H
#define REDUCER_REDUCER_H

#include <toadlet/egg/Collection.h>
#include <toadlet/egg/List.h>
#include <toadlet/peeper/IndexBuffer.h>
#include <toadlet/peeper/VertexBuffer.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/mesh/MeshData.h>
#include "Vertex.h"
#include "Triangle.h"
#include "ReducerList.h"

namespace reducer{

class TOADLET_API Reducer{
public:
	Reducer(toadlet::tadpole::Engine *engine);
	virtual ~Reducer();

	void computeProgressiveMesh(toadlet::tadpole::mesh::MeshData::Ptr mesh,bool useEdgeLength=true);
	void doProgressiveMesh(float ratio);

	void dumpDataToLog();

protected:
	void extractBufferData();
	void constructMeshData();
	
	float edgeCollapseCost(Vertex *u, Vertex *v);
	void edgeCollapseCostAtVertex(Vertex *v);
	void allEdgeCollapseCosts();

	void collapse(Vertex *u,Vertex *v,bool recompute=true);
	toadlet::egg::Collection<Triangle*> getTrianglesWithSubMeshId(int id);

	float heapVal(int i);
	void heapSortUp(int i);
	void heapSortDown(int i);
	void heapAdd(Vertex *v);
	Vertex *heapPop();

	toadlet::tadpole::Engine *mEngine;
	toadlet::tadpole::mesh::MeshData::Ptr mMeshData;
	
	ReducerList<int> mOrder;
	ReducerList<int> mMap;
	ReducerList<Vertex *> mHeap;
	ReducerList<Vertex *> mVertices;
	ReducerList<Triangle *> mTriangles;

	toadlet::egg::Collection<int> mIdMap;
	
	toadlet::peeper::VertexBuffer::Ptr mStoredVertexBuffer;
	toadlet::egg::Collection<toadlet::peeper::IndexBuffer::Ptr> mStoredIndexBuffers;

	bool mUseCurvature;
	bool mLockBorder;
	bool mUseEdgeLength;
};

}

#endif
