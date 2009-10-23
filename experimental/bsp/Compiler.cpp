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

#include "Compiler.h"
#include <toadlet/egg/math/MathTextSTDStream.h>

#include <iostream>

using namespace toadlet::egg;
using namespace toadlet::egg::math;
using namespace toadlet::egg::math::Math;

namespace bsp{

Compiler::Compiler(){
	mIn=NULL;
	mOut=NULL;

	mCleanDoubleVertexes=true;
	mCleanZeroAreaPolygons=true;
	mOptimizeSplits=true;
	mBalanceTree=true;
	mEpsilon=0.001f;
}

Compiler::~Compiler(){
}

void Compiler::compile(InputData *in,OutputData *out){
	mIn=in;
	mOut=out;

	// Clean up input data
	validate();

	// Copy over vertex data
	mOut->vertexes=mIn->vertexes;

	// Build the tree
	createBSP();
}

void Compiler::createBSP(){
	List<Polygon>::iterator it;
	for(it=mIn->polygons.begin();it!=mIn->polygons.end();++it){
		it->updatePlane(mIn->vertexes);
	}

	mOut->nodes.push_back(Node());
	createNode(0,mIn->polygons);
}

void Compiler::createNode(int node,List<Polygon> &list){
	List<Polygon>::iterator c,splitter;
	List<Polygon> frontList,backList;
	Polygon::Side r=(Polygon::Side)0;

	splitter=list.end();
	if(mOptimizeSplits || mBalanceTree){
		splitter=mOut->nodes[node].findBestSplitter(list,mOut->vertexes,mOptimizeSplits,mBalanceTree,mEpsilon);
	}
	else{
		for(c=list.begin();c!=list.end();++c){
			if(c->isSplitter==false){
				splitter=c;
				break;
			}
		}
	}

	if(splitter==list.end()){
		// All poly have been used as splitter, so create a leaf
		mOut->nodes[node].leaf=createLeaf(list);
		return;
	}

	c=list.begin();
	mOut->nodes[node].plane=splitter->plane;
	mOut->nodes[node].leaf=-1;
	splitter->isSplitter=true;

	while(c!=list.end()){
		if(c==splitter){
			c++;
			continue;
		}

		r=c->classify(splitter->plane,mOut->vertexes,mEpsilon);
		if(r==Polygon::SIDE_ON) {
			c->isSplitter=true;

			// Test front facing or back facing
			if(dot(c->plane.normal,mOut->nodes[node].plane.normal)>mEpsilon){
				frontList.push_back(*c);
			}
			else{
				backList.push_back(*c);
			}
		}
		else if(r==Polygon::SIDE_FRONT){
			frontList.push_back(*c);
		}
		else if(r==Polygon::SIDE_BACK){
			backList.push_back(*c);
		}
		else if(r==Polygon::SIDE_BOTH){
			Polygon cf,cb;

			c->splitPolygon(mOut->vertexes,mOut->nodes[node].plane,cf,cb,mEpsilon);

			if(cf.indexes.size()>=3){
				frontList.push_back(cf);
			}
			if(cb.indexes.size()>=3){
				backList.push_back(cb);
			}
			
			mOut->numSplits++;
		}

		c++;
	}

	// Add splitter to front list
	frontList.push_back(*splitter);

	if(frontList.size()){
		mOut->nodes.push_back(Node());
		int index=mOut->nodes.size()-1;
		mOut->nodes[node].child[0]=index;
		mOut->nodes[index].parent=node;
		createNode(index,frontList);
	}

	if(backList.size()){
		mOut->nodes.push_back(Node());
		int index=mOut->nodes.size()-1;
		mOut->nodes[node].child[1]=index;
		mOut->nodes[index].parent=node;
		createNode(index,backList);
	}
}

int Compiler::createLeaf(List<Polygon> &list){
	int leafIndex;
	int a=0;
	Leaf *leaf;
	List<Polygon>::iterator it;

	mOut->leafs.push_back(Leaf());
	leafIndex=mOut->leafs.size()-1;
	leaf=&mOut->leafs[leafIndex];
	leaf->polygonIndexes.resize(list.size());

	// Store the polygons in the array
	for(it=list.begin();it!=list.end();++it,++a){
		mOut->polygons.push_back(Polygon(*it));
		leaf->polygonIndexes[a]=mOut->polygons.size()-1;
	}

	return leafIndex;
}

void Compiler::validate(){
	int a, b;
	List<Polygon> &polygons=mIn->polygons;
	Collection<Vector3> &vertexes=mIn->vertexes;
	Collection<Vector3> newVertexes;
	List<Polygon>::iterator it;

	for(it=polygons.begin();it!=polygons.end();){
		if(mCleanDoubleVertexes){
			for(a=0;a<it->indexes.size();++a){
				for(b=0;b<newVertexes.size();++b){
					if(newVertexes[b]==vertexes[it->indexes[a]]){
						break;
					}
				}

				if(b<newVertexes.size()){
					it->indexes[a]=b;
				}
				else{
					newVertexes.push_back(vertexes[it->indexes[a]]);
					it->indexes[a]=newVertexes.size()-1;
				}
			}
		}


		for(a=0,b=it->indexes.size()-1;a<it->indexes.size();b=a,a++){
			if(it->indexes[b]==it->indexes[a]){
				it->indexes.erase(it->indexes.begin()+a);
			}
		}

		if(it->indexes.size()<3){
			it=polygons.erase(it);
			continue;
		}

		if(mCleanZeroAreaPolygons){
			float area, areaXY, areaYZ, areaZX;
			areaXY = 0;
			areaYZ = 0;
			areaZX = 0;

			for(a=0,b=it->indexes.size()-1;a<it->indexes.size();b=a,a++){
				const Vector3 &edge0=newVertexes[it->indexes[a]];
				const Vector3 &edge1=newVertexes[it->indexes[b]];
				areaXY+=(edge1.x+edge0.x)*(edge0.y-edge1.y)/2;
				areaYZ+=(edge1.y+edge0.y)*(edge0.z-edge1.z)/2;
				areaZX+=(edge0.x+edge1.x)*(edge1.z-edge0.z)/2;
			}

			area=sqrt(areaXY*areaXY+areaYZ*areaYZ+areaZX*areaZX);

			if(area<mEpsilon){
				it=polygons.erase(it);
				continue;
			}
		}

		++it;
	}

	mIn->vertexes=newVertexes;
}

void Compiler::pushPolygon(List<Polygon> &polygons,Collection<Vector3> &vertexes,const Polygon &polygon,int node){
	if(node==-1){
		return; // Inside, throw out polygon
	}

	Node &n=mOut->nodes[node];

	if(n.leaf!=-1){
		polygons.push_back(polygon); // Outside, add polygon to list
		return;
	}

	Polygon::Side r=polygon.classify(n.plane,vertexes,mEpsilon);
	if(r==Polygon::SIDE_ON) {
		if(dot(polygon.plane.normal,n.plane.normal)>mEpsilon){
			pushPolygon(polygons,vertexes,polygon,n.child[0]);
		}
		else{
			pushPolygon(polygons,vertexes,polygon,n.child[1]);
		}
	}
	else if(r==Polygon::SIDE_FRONT){
		pushPolygon(polygons,vertexes,polygon,n.child[0]);
	}
	else if(r==Polygon::SIDE_BACK){
		pushPolygon(polygons,vertexes,polygon,n.child[1]);
	}
	else if(r==Polygon::SIDE_BOTH){
		Polygon cf,cb;

		polygon.splitPolygon(vertexes,n.plane,cf,cb,mEpsilon);

		if(cf.indexes.size()>=3){
			pushPolygon(polygons,vertexes,cf,n.child[0]);
		}
		if(cb.indexes.size()>=3){
			pushPolygon(polygons,vertexes,cb,n.child[1]);
		}
	}
}

void Compiler::outputTree(int node,int depth){
	int i;

	std::ostream &os=std::cout;

	Node *n=&mOut->nodes[node];

	for(i=0;i<depth;++i){os << "\t";}
	os << node << ":Node" << std::endl;

	for(i=0;i<depth;++i){os << "\t";}
	os << node << ":Plane:" << n->plane.normal << " - " << n->plane.d << std::endl;
	for(i=0;i<depth;++i){os << "\t";}
	os << node << ":FrontChild:" << n->child[0] << std::endl;
	if(n->child[0]!=-1){
		outputTree(n->child[0],depth+1);
	}
	for(i=0;i<depth;++i){os << "\t";}
	os << node << ":BackChild:" << n->child[1] << std::endl;
	if(n->child[1]!=-1){
		outputTree(n->child[1],depth+1);
	}
}

}
