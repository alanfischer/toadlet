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

#include <toadlet/egg/math/Vector3.h>
#include <toadlet/egg/math/Math.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/System.h>
#include "Reducer.h"

#include <iostream>

using namespace toadlet::egg;
using namespace toadlet::egg::math;
using namespace toadlet::egg::math::Math;
using namespace toadlet::peeper;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::mesh;

namespace reducer{

Reducer::Reducer(Engine *engine){
	mEngine=engine;
	mUseCurvature=true;
	mLockBorder=true;
	mUseEdgeLength=true;
}

Reducer::~Reducer(){
}

void Reducer::computeProgressiveMesh(MeshData::Ptr mesh,bool useEdgeLength){
	int i;
	mMeshData=mesh;
	mUseEdgeLength=useEdgeLength;

	// Store vertex buffer
	mStoredVertexBuffer=mEngine->getVertexBufferManager()->load(mMeshData->getVertexBuffer()->clone());

	// Store index buffers
	mStoredIndexBuffers.clear();
	for(i=0;i<mMeshData->getNumSubMeshDatas();++i){
		mStoredIndexBuffers.push_back(mEngine->getIndexBufferManager()->load(mMeshData->getSubMeshData(i)->getIndexBuffer()->clone()));
	}
	
	extractBufferData();

	mHeap.resize(0);
	
	allEdgeCollapseCosts();

	// Construct map and order while collapsing vertices
	// Continue the collapse all the way down to nothing
	mOrder.resize(mVertices.size());
	mMap.resize(mVertices.size());
	while(mVertices.size()!=0){
		Vertex *nv=heapPop();
		mOrder[mVertices.size()-1]=nv->mId;
		if(nv->mCollapse!=NULL){
			mMap[nv->mId]=nv->mCollapse->mId;
		}
		else{
			mMap[nv->mId]=-1;
		}
		collapse(nv,nv->mCollapse,true);
	}

	// Double check the triangles for emptiness
	assert(mTriangles.size()==0);
	mHeap.resize(0);
}

void Reducer::doProgressiveMesh(float ratio){
	ReducerList<Vertex*> collapseList;
	int i,target;

	extractBufferData();

	target=(int)(ratio*mVertices.size());
	if(target>mVertices.size()) target=mVertices.size();
	else if(target<0) target=0;
	
	// Build vertex collapse list
	collapseList.resize(mVertices.size());
	for(i=0;i<mVertices.size();++i){
		Vertex *v=mVertices[i];
		if(mMap[i]==-1){
			v->mCollapse=NULL;
		}
		else{
			v->mCollapse=mVertices[mMap[i]];
		}
		collapseList[i]=mVertices[mOrder[i]];
	}

	// Collapse vertices
	while(mVertices.size()>target){
		Vertex *nv=collapseList[collapseList.size()-1];
		collapseList.pop();
		collapse(nv,nv->mCollapse,false);
	}
	
	constructMeshData();

	// Clean remaining storage out
	while(mTriangles.size()!=0){
		delete mTriangles[mTriangles.size()-1];
	}
	while(mVertices.size()!=0){
		delete mVertices[mVertices.size()-1];
	}
}

void Reducer::extractBufferData(){
	int h,i,j,k,l;
	List<VertexData> vertexData;
	List<VertexData>::iterator it1,it2,itmp;
	
	// Collect vertex data
	for(i=0;i<mStoredVertexBuffer->getNumVertexes();++i){
		VertexData vd;
		if(mStoredVertexBuffer->getType() & VertexBuffer::VT_POSITION){
			vd.position=mStoredVertexBuffer->position(i);
		}
		if(mStoredVertexBuffer->getType() & VertexBuffer::VT_NORMAL){
			vd.normal=mStoredVertexBuffer->normal(i);
		}
		if(mStoredVertexBuffer->getType() & VertexBuffer::VT_TEXCOORD1_DIM2){
			vd.texture=mStoredVertexBuffer->texCoord2d(i);
		}
		if(mStoredVertexBuffer->getType() & VertexBuffer::VT_COLOR){
			vd.color=mStoredVertexBuffer->color(i);
		}
		if(mStoredVertexBuffer->getType() & VertexBuffer::VT_BONE){
			vd.bone=mStoredVertexBuffer->bone(i);
		}
		vd.id=i;	
		vertexData.push_back(vd);
	}

	// Pack vertex data that shares positions into a single vertex
	mIdMap.resize(vertexData.size());
	for(i=0;i<mIdMap.size();++i){
		mIdMap[i]=-1;
	}
	i=0;
	for(it1=vertexData.begin();it1!=vertexData.end();++it1){
		VertexData vd1=(*it1);
		Vertex *v=new Vertex(vd1,i,&mVertices);

		// Store the mapping of vertexData id to mVertices index
		mIdMap[vd1.id]=i;

		it2=it1;
		itmp=it2;
		++it2;
		while(it2!=vertexData.end()){
			VertexData vd2=(*it2);
			if(vd1.position==vd2.position){
				v->mVertexData.push_back(vd2);
				vertexData.erase(it2);
				it2=itmp;

				// Store the mapping of vertexData id to mVertices index
				mIdMap[vd2.id]=i;
			}
			itmp=it2;
			++it2;
		}
		++i;
	}

	// Collect triangles
	int ids[3];
	for(i=0;i<mStoredIndexBuffers.size();++i){
		IndexBuffer::Ptr ib=mStoredIndexBuffers[i];
		for(h=0;h<ib->getNumIndexes();h+=3){
			j=ib->index(h);
			k=ib->index(h+1);
			l=ib->index(h+2);

			// Store the vertexData id obtained above to reference an mVertexData id
			ids[0]=j;
			ids[1]=k;
			ids[2]=l;
			
			// Ignore repeat index triangles - it does happen
			if(j!=k && k!=l && l!=j){
				Triangle *t=new Triangle(mVertices[mIdMap[j]],mVertices[mIdMap[k]],mVertices[mIdMap[l]],i,ids,&mTriangles);
			}
		}
	}
}

void Reducer::constructMeshData(){
	int i,j,k,count;
	Vertex *v;
	
	// Compute number of vertices needed
	count=0;
	for(i=0;i<mVertices.size();++i){
		count+=mVertices[i]->mVertexData.size();
	}

	// Rebuild vertex buffer in the mesh
	VertexBuffer *vertexBuffer=mMeshData->getVertexBuffer();
	vertexBuffer->resize((VertexBuffer::VertexType)vertexBuffer->getType(),count);
	for(i=0;i<mIdMap.size();++i){
		mIdMap[i]=-1;
	}
	count=0;
	for(i=0;i<mVertices.size();++i){
		v=mVertices[i];
		for(j=0;j<v->mVertexData.size();++j){
			mIdMap[v->mVertexData[j].id]=count;
			if(vertexBuffer->getType() & VertexBuffer::VT_POSITION){
				// Use vertex position to be safe; all vertex data for a vertex should have the same position
				vertexBuffer->position(count)=v->position();
			}
			if(vertexBuffer->getType() & VertexBuffer::VT_NORMAL){
				vertexBuffer->normal(count)=v->mVertexData[j].normal;
			}
			if(vertexBuffer->getType() & VertexBuffer::VT_TEXCOORD1_DIM2){
				vertexBuffer->texCoord2d(count)=v->mVertexData[j].texture; 
			}
			if(vertexBuffer->getType() & VertexBuffer::VT_COLOR){
				vertexBuffer->color(count)=v->mVertexData[j].color;  
			}
			if(vertexBuffer->getType() & VertexBuffer::VT_BONE){
				vertexBuffer->bone(count)=v->mVertexData[j].bone;  
			}
			++count;
		}
	}

	// Rebuild index buffers on a per-submesh basis
	for(i=0;i<mMeshData->getNumSubMeshDatas();++i){
		SubMeshData *subMeshData=mMeshData->getSubMeshData(i);
		Collection<Triangle *> tris=getTrianglesWithSubMeshId(i);

		// Compute number of triangles needed
		count=0;
		count=3*tris.size();
		subMeshData->getIndexBuffer()->resize(count);
		count=0;
		for(j=0;j<tris.size();++j){
			for(k=0;k<3;++k){
				subMeshData->getIndexBuffer()->index(count)=mIdMap[tris[j]->mIndexIds[k]];

				if(mIdMap[tris[j]->mIndexIds[k]]>=mMeshData->getVertexBuffer()->getNumVertexes() || mIdMap[tris[j]->mIndexIds[k]]<0){
					TOADLET_LOG(NULL,Logger::LEVEL_ERROR) <<
						"Index buffer " << count << " of Triangle " << j << " in submesh " << i << " has id " << 
						tris[j]->mIndexIds[k] << "pointing to vertex index " << mIdMap[tris[j]->mIndexIds[k]] << 
						" with VertexBuffer size " << mMeshData->getVertexBuffer()->getNumVertexes();
				}
				count++;
			}
		}
	}
}

float Reducer::edgeCollapseCost(Vertex *u,Vertex *v){
	int i,j;
	float curvature=0.0f;
	float edgeLength=1.0f;

	if(mUseEdgeLength){
		edgeLength=length(v->position()-u->position());
	}

	// Find the triangles sharing edge u,v
	ReducerList<Triangle *> sides;
	for(i=0;i<u->mFaces.size();++i){
		if(u->mFaces[i]->hasVertex(v)){
			sides.add(u->mFaces[i]);
		}
	}

	// Compute curvature with triangle facing most away from sides
	// This uses the cost function developed by Stan Melax 
	// TODO: Permit the option to use other algorithms, such as
	// those found in qslim
	if(mUseCurvature){
		for(i=0;i<u->mFaces.size();++i){
			float curve=1.0f;
			for(j=0;j<sides.size();++j){
				float dp=dot(u->mFaces[i]->mNormal,sides[j]->mNormal);
				curve=minVal(curve,(1.0f-dp)*0.5f);
			}
			curvature=maxVal(curvature,curve);
		}
	}
	else if(0){
		// Other algorithm
	}

	// Check for collapses
	if(u->onBorder() && sides.size()>1){
		curvature=1.0f;
	}

	// Check for material ripping
	bool nomatch=false;
	for(i=0;i<u->mFaces.size();++i){
		for(j=0;j<sides.size();++j){
			// TODO: Check to see if this is really what we want,
			// or if we also need to look at texture coordinates
			if(u->mFaces[i]->mSubMeshId==sides[j]->mSubMeshId) break;
		}
		if(j==sides.size()){
			nomatch=true;
		}
	}
	if(nomatch==true){
		curvature=1.0f;
	}

	// Lock down border edges by locking any border vertices
	if(u->onBorder() && mLockBorder) {
		curvature = 9999.9f;
	}
	
	return edgeLength*curvature;
}

void Reducer::edgeCollapseCostAtVertex(Vertex *v){
	int i;
	
	if(v->mNeighbors.size()==0){
		// No neighbors so collapse is "free"
		v->mCollapse=NULL;
		v->mCost=-0.01f;
		return;
	}
	v->mCost=1000000.0f;
	v->mCollapse=NULL;

	// Find the least cost edge among all neighbors
	for(i=0;i<v->mNeighbors.size();++i){
		float cost;
		cost=edgeCollapseCost(v,v->mNeighbors[i]);
		if(v->mCollapse==NULL || cost<v->mCost){
			v->mCollapse=v->mNeighbors[i];
			v->mCost=cost;
		}
	}
}

void Reducer::allEdgeCollapseCosts(){
	int i;
	for(i=0;i<mVertices.size();++i){
		edgeCollapseCostAtVertex(mVertices[i]);
		heapAdd(mVertices[i]);
	}
}

void Reducer::collapse(Vertex *u,Vertex *v,bool recompute){
	ReducerList<Vertex *> uNeighbors;
	ReducerList<Triangle *> sides;
	int i,j;

	// Delete a lone vertex
	if(v==NULL){
		delete u;
		return;
	}

	// List u's vertex neighbors
	for(i=0;i<u->mNeighbors.size();++i){
		uNeighbors.add(u->mNeighbors[i]);
	}

	// List faces shared between u and v as sides
	for(i=0;i<u->mFaces.size();++i){
		if(u->mFaces[i]->hasVertex(v)){
			sides.add(u->mFaces[i]);
		}
	}

	// Copy and store old vertex data
	Collection<VertexData> du=u->mVertexData;
	Collection<VertexData> dv=v->mVertexData;
	
	// Collapse vertex data from v to u
	// For extra vertex data in u, search for
	// the data in v that has the closest normal
	// and copy it; trying to preserve appearance
	int closest=0;
	float dot1,dot2;
	for(i=dv.size();i<du.size();++i){
		for(j=1;j<dv.size();++j){
			dot1=dot(du[i].normal,dv[j].normal);
			dot2=dot(du[i].normal,dv[closest].normal);
			if(dot2>dot1){
				closest=j;
			}
		}
		v->mVertexData.push_back(dv[closest]);
		
		/*
		u->mVertexData[i].position=v->position();
		v->mVertexData.push_back(u->mVertexData[i]);
		*/
	}
		
	// Update triangle vertex mapping ids
	for(i=0;i<u->mFaces.size();++i){
		u->mFaces[i]->updateIndexIds(du,v->mVertexData);
	}

	// Delete triangles
	for(i=u->mFaces.size()-1;i>=0;--i){
		Triangle *f=u->mFaces[i];
		if(f->hasVertex(v)){
			delete f;
		}
	}

	// Update remaining triangles
	for(i=u->mFaces.size()-1;i>=0;--i){
		u->mFaces[i]->replaceVertex(u,v);
	}

	delete u;

	// Recompute edge collapse costs
	if(recompute==true){
		for(i=0;i<uNeighbors.size();++i){
			edgeCollapseCostAtVertex(uNeighbors[i]);
			heapSortUp(uNeighbors[i]->mHeapSpot);
			heapSortDown(uNeighbors[i]->mHeapSpot);
		}
	}
}

Collection<Triangle*> Reducer::getTrianglesWithSubMeshId(int id){
	Collection<Triangle*> tris;
	int i;

	for(i=0;i<mTriangles.size();++i){
		Triangle *t=mTriangles[i];
		if(t->mSubMeshId==id){
			tris.push_back(t);
		}
	}

	return tris;
}

float Reducer::heapVal(int i){
	if(i>=mHeap.size() || i<0 || mHeap[i]==NULL){
		return 9999999999999.9f;
	}
    return mHeap[i]->mCost;
}

void Reducer::heapSortUp(int i){
	int j;
	while(heapVal(i)<heapVal((j=(i-1)/2))){
		if(i<0){
			return;
		}
		Vertex *v=mHeap[i];
		mHeap[i]=mHeap[j];
		mHeap[i]->mHeapSpot=i;
		mHeap[j]=v;
		mHeap[j]->mHeapSpot=j;
		i=j;
	}
}

void Reducer::heapSortDown(int i){
	int j;
	while(heapVal(i)>heapVal((j=(i+1)*2)) || heapVal(i)>heapVal(j-1)){
		j=(heapVal(j)<heapVal(j-1))? j:j-1;
		if(i<0){
			return;
		}
		Vertex *v=mHeap[i];
		mHeap[i]=mHeap[j];
		mHeap[i]->mHeapSpot=i;
		mHeap[j]=v;
		if(v){
			mHeap[j]->mHeapSpot=j;
		}
		i=j;
	}
}

void Reducer::heapAdd(Vertex *v){
	int i=mHeap.size();
	mHeap.add(v);
	v->mHeapSpot=i;
	heapSortUp(i);
}

Vertex *Reducer::heapPop(){
	Vertex *v=mHeap[0];
	assert(v);
	v->mHeapSpot=-1;
	mHeap[0]=NULL;
	heapSortDown(0);
	return v;
}

void Reducer::dumpDataToLog(){
	int i,j;
	Triangle *t;
	Vertex *v;

	StringStream o;
	for(i=0;i<mVertices.size();++i){
		v=mVertices[i];
		o << "Vertex #=" << v->mId 
		<< "\nVertexPtr=" << v
		<< "\nPosition=" << v->position()
		<< "\nCost=" << v->mCost
		<< "\nCollapse=" << v->mCollapse
		<< "\nVertices size=" << v->mVertices->size()
		<< "\nNeighbors size=" << v->mNeighbors.size()
		<< "\nFaces size=" << v->mFaces.size()
		<< "\nOnBorder?=" << v->onBorder()
		<< "\nNeighbors size =" << v->mNeighbors.size()
		<< "\nNeighbors=\n";
		for(j=0;j<v->mNeighbors.size();j++){
			o << &(v->mNeighbors[j]) << std::endl;
		}
		o << "\n"<< std::endl;

	}
	
	for(i=0;i<mTriangles.size();++i){
		t=mTriangles[i];
		o << "TrianglePtr=" << t
		<< "\nVertices=" << t->mVertex[0] << "," << t->mVertex[1] << "," << t->mVertex[2]
		<< "\nTriangles size=" << t->mTriangles->size()
		<< "\nNormal=" << t->mNormal
		<< "\n"
		<< std::endl;
	}

	Logger::getInstance()->addLogString(NULL,Logger::LEVEL_ALERT,o.str());
}

}

