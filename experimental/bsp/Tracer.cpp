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

#include "Tracer.h"

using namespace toadlet::egg::math;
using namespace toadlet::egg::math::Math;

namespace bsp{

Tracer::Tracer(){
	mNodes=NULL;
	mEpsilon=0.0001f;
}

void Tracer::setNodes(const Node *nodes){
	mNodes=nodes;
}

void Tracer::setEpsilon(float epsilon){
	mEpsilon=epsilon;
}

float Tracer::traceLine(const Vector3 &start,const Vector3 &end,float offset,Vector3 &normal){
	mOffset=offset;
	mFraction=-1;
	checkNode(0,0,1,start,end,ZERO_VECTOR3);
	normal=mNormal;
	return mFraction;
}

void Tracer::checkNode(int nodeIndex,float startFraction,float endFraction,const Vector3 &start,const Vector3 &end,const Vector3 &currentNormal){
	const Node *node=&mNodes[nodeIndex];
	const Plane *plane=&node->plane;

	if(node->leaf!=-1){
		if(mFraction==-1 || startFraction<mFraction){
			mFraction=startFraction;
			mNormal=currentNormal;
		}
		return;
	}

	float d=dot(node->plane.normal,end-start);

	float startDistance=dot(start,plane->normal)-plane->d;
	float endDistance=dot(end,plane->normal)-plane->d;

	// Both in front
	if(startDistance>=mOffset && endDistance>=mOffset){
		if(node->child[0]>=0){
			checkNode(node->child[0],startFraction,endFraction,start,end,currentNormal);
		}
		else{
			return;
		}
	}
	// Both in back
	else if(startDistance<-mOffset && endDistance<-mOffset){
		if(node->child[1]>=0){
			checkNode(node->child[1],startFraction,endFraction,start,end,currentNormal);
		}
		else{
			return;
		}
	}
	// Cross the splitter
	else{
		int side;
		float fraction0, fraction1, middleFraction;
		Vector3 middle;

		// Get back piece
		if(startDistance < endDistance){
			side=1;
			float inverseDistance=1.0f/(startDistance-endDistance);
			fraction0=(startDistance-mOffset+mEpsilon)*inverseDistance;
			fraction1=(startDistance+mOffset+mEpsilon)*inverseDistance;
		}
		// Get front piece
		else if(endDistance<startDistance){
			side=0;
			float inverseDistance=1.0f/(startDistance-endDistance);
			fraction0=(startDistance+mOffset+mEpsilon)*inverseDistance;
			fraction1=(startDistance-mOffset-mEpsilon)*inverseDistance;
		}
		else{
			side=0;
			fraction0=1.0f;
			fraction1=0.0f;
		}

		if(node->child[side]>=0){
			if(fraction0<0.0f){
				fraction0=0.0f;
			}
			else if(fraction0>1.0f){
				fraction0=1.0f;
			}

			middleFraction=startFraction+(endFraction-startFraction)*fraction0;
			middle=start+fraction0*(end-start);
			checkNode(node->child[side],startFraction,middleFraction,start,middle,currentNormal);
		}

		if(node->child[!side]>=0){
			Vector3 newNormal(currentNormal);

			if(fraction1<0.0f){
				fraction1=0.0f;
			}
			else if(fraction1>1.0f){
				fraction1=1.0f;
			}

			if(fraction1==0.0f){
				middleFraction=startFraction;
				middle=start;
			}
			else{
				middleFraction=startFraction+(endFraction-startFraction)*fraction1;
				middle=start+fraction1*(end-start);

				if(d>0){
					newNormal=-node->plane.normal;
				}
				else{
					newNormal=node->plane.normal;
				}
			}

			middleFraction=startFraction+(endFraction-startFraction)*fraction1;
			middle=start+fraction1*(end-start);
			checkNode(node->child[!side],middleFraction,endFraction,middle,end,newNormal);
		}
    }
}

}
