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

#include "Polygon.h"

#include <stdexcept>

using namespace toadlet::egg;
using namespace toadlet::egg::math;
using namespace toadlet::egg::math::Math;

namespace bsp{

Polygon::Polygon(){
	flags=0;
	isSplitter=false;
}

void Polygon::updatePlane(const Collection<Vector3> &vertexes){
	Vector3 normal=cross(vertexes[indexes[2]] - vertexes[indexes[0]],vertexes[indexes[1]] - vertexes[indexes[0]]);

	normalize(normal);

	plane=Plane(normal,dot(normal,vertexes[indexes[0]]));
}

Polygon::Side Polygon::classify(const Plane &pln,const Collection<Vector3> &vertexes,float epsilon) const{
	int a;
	bool front = true;
	bool back = true;
	bool on = true;
	float dis;

	for(a=0;a<indexes.size();++a){
		dis=length(pln,vertexes[indexes[a]]);
		if(dis<-epsilon) front = on = false;
		if(dis>epsilon) back = on = false;
	}

	if(on) return SIDE_ON;
	if(front) return SIDE_FRONT;
	if(back) return SIDE_BACK;
	return SIDE_BOTH;
}

void Polygon::splitPolygon(Collection<Vector3> &vertexes,const Plane &splitPlane,Polygon &front,Polygon &back,float epsilon) const{
	int a,fc=0,bc=0,v1,v2; // bc=Back count, fc=Front count
	bool allfront=true,allback=true;

	float dis[40];
	// vf = vertices front
	int vf[40];
	// vb = vertices back
	int vb[40];

	if(indexes.size()>=40){
		throw std::runtime_error("Polygon::splitPolygon: Too many indexes");
	}

	// Calc distances:
	for(a=indexes.size()-1;a>=0;a--){
		dis[a] = length(splitPlane,vertexes[indexes[a]]);
		if(dis[a]<-epsilon) allfront=false;
		if(dis[a]>epsilon) allback=false;
	}

	// this poly lies ON the splitPlane and should not be split
	if(allback && allfront){
		return;
	}

	if(!allback && !allfront){
		for(a=0;a<indexes.size();a++){
			v1=a;
			v2=a+1;
			if(a==indexes.size()-1){
				v2=0;
			}

			// Do NOT hold a reference to these, because it will become invalidated as the collection grows
			// pv1 = current edge startpoint
			Vector3 pv1=vertexes[indexes[v1]];
			// pv2 = current edge endpoint
			Vector3 pv2=vertexes[indexes[v2]];
			if(dis[v1] >= 0 && dis[v2] >= 0) { // Both on front, add end point(v2)
				vf[fc] = indexes[v2];
				fc++;
			}
			if(dis[v1] < 0 && dis[v2] < 0) { // Both on back, add end point to back
				vb[bc] = indexes[v2];
				bc++;
			}
			if(dis[v1] >= 0 && dis[v2] < 0) { // 1 on front, 2 on back, 2 to back, intersection to both
				float port = dis[v1] / (dis[v1] - dis[v2]);
				vertexes.push_back(Vector3());
				vf[fc]=vertexes.size()-1;
				vertexes[vf[fc]] = pv1 + (pv2 - pv1) * port;
				vb[bc] = vf[fc];
				fc++; bc++;
				vb[bc] = indexes[v2]; // Add ending point
				bc++;
			} 
			if(dis[v1] < 0 && dis[v2] >= 0) { // 1 on back
				float port = dis[v1] / (dis[v1] - dis[v2]);
				vertexes.push_back(Vector3());
				vf[fc]=vertexes.size()-1;
				vertexes[vf[fc]] = pv1 + (pv2 - pv1) * port;
				vb[bc] = vf[fc];
				fc++; bc++;
				vf[fc] = indexes[v2]; // Add ending point
				fc++;
			}
		}
	}
	else if(allback && !allfront) {
		bc = indexes.size();
		for(a=0;a<bc;a++){
			vb[a] = indexes[a];
		}
	}
	else{
		fc = indexes.size();
		for(a=0;a<fc;a++){
			vf[a] = indexes[a];
		}
	}

	// Now create the 2 new poly's
	back.indexes.resize(bc);
	back.plane=plane;
	back.flags=flags;
	memcpy(&back.indexes[0],vb,sizeof(int)*bc);
	back.cleanDoubleVerts(vertexes);

	front.indexes.resize(fc);
	front.plane=plane;
	front.flags=flags;
	memcpy(&front.indexes[0],vf,sizeof(int)*fc);
	front.cleanDoubleVerts(vertexes);
}

void Polygon::cleanDoubleVerts(Collection<Vector3> &vertexes){
	int a,b;

	for(a=0,b=indexes.size()-1;a<indexes.size();b=a,a++){
		if(length(vertexes[indexes[a]],vertexes[indexes[b]])<0.0001){
			indexes.erase(indexes.begin()+a);
		}
	}
}

}
