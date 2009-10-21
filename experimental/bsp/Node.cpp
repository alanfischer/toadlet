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

#include "Node.h"

using namespace toadlet::egg;
using namespace toadlet::egg::math;

namespace bsp{

Node::Node(){
	child[0]=-1;
	child[1]=-1;
	parent=-1;
	leaf=-1;
}

Node::~Node(){
}

List<Polygon>::iterator Node::findBestSplitter(List<Polygon> &list,const Collection<Vector3> &vertexes,bool optimizeSplits,bool balanceTree,float epsilon) const{
	List<Polygon>::iterator bestPoly=list.end(),it=list.end(),cur=list.end();
	int bestScore=0;
	Polygon::Side r=(Polygon::Side)0;
	int front=0, back=0, split=0, on=0, score=0;
	int splitmul=optimizeSplits ? 3 : 0;
	int balancemul=balanceTree ? 1 : 0;

	for(it=list.begin();it!=list.end();++it){
		front = back = split = on = 0;
		if(it->isSplitter){
			continue;
		}

		for(cur=list.begin();cur!=list.end();++cur){
			if(cur==it){
				continue;
			}

			r=cur->classify(it->plane,vertexes,epsilon);
			if(r == Polygon::SIDE_ON) on++;
			if(r == Polygon::SIDE_BACK) back++;
			if(r == Polygon::SIDE_FRONT) front++;
			if(r == Polygon::SIDE_BOTH) split++;
		}

		score=(abs(front-back)-on)*balancemul+split*splitmul-on;

		if(score<bestScore || bestPoly==list.end()){
			bestScore = score;
			bestPoly = it;
		}
	}

	return bestPoly;
}

}
