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

#ifndef BSP_POLYGON_H
#define BSP_POLYGON_H

#include <toadlet/egg/Collection.h>
#include <toadlet/egg/math/Math.h>

namespace bsp{

class TOADLET_API Polygon{
public:
	enum Side{
		SIDE_ON,
		SIDE_FRONT,
		SIDE_BACK,
		SIDE_BOTH,
	};

	Polygon();

	void updatePlane(const toadlet::egg::Collection<toadlet::egg::math::Vector3> &vertexes);
	Side classify(const toadlet::egg::math::Plane &plane,const toadlet::egg::Collection<toadlet::egg::math::Vector3> &vertexes,float epsilon) const;
	void splitPolygon(toadlet::egg::Collection<toadlet::egg::math::Vector3> &vertexes,const toadlet::egg::math::Plane &plane,Polygon &front,Polygon &back,float epsilon) const;
	void cleanDoubleVerts(toadlet::egg::Collection<toadlet::egg::math::Vector3> &vertexes);

	toadlet::egg::math::Plane plane;
	toadlet::egg::Collection<int> indexes;
	int flags;
	bool isSplitter;
};

}

#endif
