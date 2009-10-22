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

#include "Vertex.h"
#include "Triangle.h"
#include <assert.h>

using namespace toadlet::egg;
using namespace toadlet::egg::math;

namespace reducer{

Vertex::Vertex(Vector3 position,Vector3 normal,Vector2 texture,Vector4 color,int id,ReducerList<Vertex *> *vertices){
	mId=id;
	mCost=10000.0f;
	mCollapse=NULL;
	
	VertexData vd;
	vd.position=position;
	vd.normal=normal;
	vd.texture=texture;
	vd.color=color;
	vd.id=id;
	mVertexData.push_back(vd);

	mVertices=vertices;
	mPlace=mVertices->size();
	mVertices->add(this);
}

Vertex::Vertex(VertexData vd,int id,ReducerList<Vertex *> *vertices){
	mId=id;
	mCost=10000.0f;
	mCollapse=NULL;

	mVertexData.push_back(vd);

	mVertices=vertices;
	mPlace=mVertices->size();
	mVertices->add(this);
}

Vertex::~Vertex(){
	assert(mFaces.size()==0);

	while(mNeighbors.size()!=0){
		mNeighbors[0]->mNeighbors.del(this);
		mNeighbors.del(mNeighbors[0]);
	}

	(*mVertices)[mPlace]=(*mVertices)[mVertices->size()-1];
	(*mVertices)[mPlace]->mPlace=mPlace;
	mVertices->pop();
}

Vector3 Vertex::position(){
	Vector3 p;
	if(mVertexData.size()>0){
		p=mVertexData[0].position;
	}
	return p;
}

void Vertex::deleteNonNeighbor(Vertex *v){
	int i;
	if(!mNeighbors.contains(v)){
		return;
	}
	for(i=0;i<mFaces.size();++i){
		if(mFaces[i]->hasVertex(v)){
			return;
		}
	}
	mNeighbors.del(v);
}

bool Vertex::onBorder(){
	int i,j,count;
	for(i=0;i<mNeighbors.size();++i){
		count=0;
		for(j=0;j<mFaces.size();++j){
		    if(mFaces[j]->hasVertex(mNeighbors[i])){
			++count;
		    }
		}
		assert(count>0);
		if(count==1){
			return true;
		}
	}
	return false;
}

}
