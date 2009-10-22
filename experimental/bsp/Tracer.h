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

#ifndef BSP_TRACER_H
#define BSP_TRACER_H

#include "Node.h"

namespace bsp{

class TOADLET_API Tracer{
public:
	Tracer();
	void setNodes(const Node *nodes);
	void setEpsilon(float epsilon);

	float traceLine(const toadlet::egg::math::Vector3 &start,const toadlet::egg::math::Vector3 &end,float offset,toadlet::egg::math::Vector3 &normal);

protected:
	void checkNode(int nodeIndex,float startFraction,float endFraction,const toadlet::egg::math::Vector3 &start,const toadlet::egg::math::Vector3 &end,const toadlet::egg::math::Vector3 &currentNormal);

	const Node *mNodes;
	toadlet::egg::math::Vector3 mNormal;
	float mEpsilon;
	float mFraction;
	float mOffset;
};

}

#endif
