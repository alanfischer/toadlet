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

#include "Triangle.h"
#include <toadlet/egg/math/Math.h>
#include <assert.h>

using namespace toadlet::egg;
using namespace toadlet::egg::math;
using namespace toadlet::egg::math::Math;

namespace reducer{

Triangle::Triangle(Vertex *v0,Vertex *v1,Vertex *v2,int subMeshId,int indexIds[3],ReducerList<Triangle *> *triangles){
	int i,j;
	assert(v0!=v1 && v1!=v2 && v2!=v0);
	
	mVertex[0]=v0;
	mVertex[1]=v1;
	mVertex[2]=v2;
	mSubMeshId=subMeshId;
	mIndexIds[0]=indexIds[0];
	mIndexIds[1]=indexIds[1];
	mIndexIds[2]=indexIds[2];
	
	normal();
	
	mTriangles=triangles;
	mPlace=mTriangles->size();
	mTriangles->add(this);
	
	for(i=0;i<3;++i){
		mVertex[i]->mFaces.add(this);
		for(j=0;j<3;++j){
			if(i!=j){
				mVertex[i]->mNeighbors.addUnique(mVertex[j]);
			}
		}
	}
}

Triangle::~Triangle(){
	int i,j;

	(*mTriangles)[mPlace]=(*mTriangles)[mTriangles->size()-1];
	(*mTriangles)[mPlace]->mPlace=mPlace;
	mTriangles->pop();
	
	for(i=0;i<3;++i){
		if(mVertex[i]!=NULL){
			mVertex[i]->mFaces.del(this);
		}
	}
	
	for(i=0;i<3;++i){
		j=(i+1)%3;
		if(mVertex[i]==NULL || mVertex[j]==NULL) continue;
		mVertex[i]->deleteNonNeighbor(mVertex[j]);
		mVertex[j]->deleteNonNeighbor(mVertex[i]);
	}
}

bool Triangle::hasVertex(Vertex *v){
	if(v==mVertex[0] || v==mVertex[1] || v==mVertex[2]){
		return true;
	}
	else{
		return false;
	}
}

void Triangle::replaceVertex(Vertex *vo, Vertex *vn){
	int i,j;
	assert(vo!=NULL || vn!=NULL);
	assert(vo==mVertex[0] || vo==mVertex[1] || vo==mVertex[2]);
	assert(vn!=mVertex[0] && vn!=mVertex[1] && vn!=mVertex[2]);

	if(vo==mVertex[0]){
		mVertex[0]=vn;
	}
	else if(vo==mVertex[1]){
		mVertex[1]=vn;
	}
	else{
		mVertex[2]=vn;
	}

	vo->mFaces.del(this);
	assert(!vn->mFaces.contains(this));
	vn->mFaces.add(this);

	for(i=0;i<3;++i){
		vo->deleteNonNeighbor(mVertex[i]);
		mVertex[i]->deleteNonNeighbor(vo);
	}
	for(i=0;i<3;++i){
		assert(mVertex[i]->mFaces.contains(this)>0);
		for(j=0;j<3;++j){
			if(i!=j){
				mVertex[i]->mNeighbors.addUnique(mVertex[j]);
			}
		}
	}
	normal();
}

void Triangle::normal(){
	Vector3 v0=mVertex[1]->position()-mVertex[0]->position();
	Vector3 v1=mVertex[2]->position()-mVertex[1]->position();
	mNormal=cross(v0,v1);
	if(lengthSquared(mNormal)!=0){
		normalize(mNormal);
	}
}

void Triangle::updateIndexIds(Collection<VertexData> vdOld,Collection<VertexData> vdNew){
	int i,j;
	assert(vdOld.size()<=vdNew.size());
	
	for(i=0;i<3;++i){
		for(j=0;j<vdOld.size();++j){
			if(mIndexIds[i]==vdOld[j].id){
				mIndexIds[i]=vdNew[j].id;
			}
		}
	}
}

}

