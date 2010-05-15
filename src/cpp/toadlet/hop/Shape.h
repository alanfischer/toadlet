/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright 2009, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer, Andrew Fischer
 *
 * This file is part of The Toadlet Engine.
 *
 * The Toadlet Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * The Toadlet Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with The Toadlet Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 ********** Copyright header - do not remove **********/

#ifndef TOADLET_HOP_SHAPE_H
#define TOADLET_HOP_SHAPE_H

#include <toadlet/hop/Types.h>
#include <toadlet/hop/TraceCallback.h>

namespace toadlet{
namespace hop{

class Solid;
class Simulator;
	
class TOADLET_API Shape{
public:
	TOADLET_SHARED_POINTERS(Shape);

	enum Type{
		Type_AABOX=			1<<0,
		Type_SPHERE=		1<<1,
		Type_CAPSULE=		1<<2,
		Type_CONVEXSOLID=	1<<3,
		Type_CALLBACK=		1<<4,
	};

	Shape();
	Shape(const AABox &box);
	Shape(const Sphere &sphere);
	Shape(const Capsule &capsule);
	Shape(const ConvexSolid &convexSolid);
	Shape(TraceCallback *callback);

	void reset();

	void setAABox(const AABox &box);
	inline const AABox &getAABox() const{return mAABox;}

	void setSphere(const Sphere &sphere);
	inline const Sphere &getSphere() const{return mSphere;}
	
	void setCapsule(const Capsule &capsule);
	inline const Capsule &getCapsule() const{return mCapsule;}

	void setConvexSolid(const ConvexSolid &convexSolid);
	inline const ConvexSolid &getConvexSolid() const{return mConvexSolid;}

	void setCallback(TraceCallback *callback);
	inline TraceCallback *getCallback() const{return mCallback;}

	inline Type getType() const{return mType;}

	void getBound(AABox &box) const;

protected:
	Type mType;
	AABox mAABox;
	Sphere mSphere;
	Capsule mCapsule;
	ConvexSolid mConvexSolid;
	TraceCallback *mCallback;
	Solid *mSolid;

	Vector3 cache_getBound_r;
	
	friend class Solid;
	friend class Simulator;
};

}
}

#endif

