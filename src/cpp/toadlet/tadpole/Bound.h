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

#ifndef TOADLET_TADPOLE_BOUND_H
#define TOADLET_TADPOLE_BOUND_H

#include <toadlet/tadpole/Types.h>
#include <toadlet/tadpole/Transform.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API Bound{
public:
	TOADLET_SHARED_POINTERS(Bound);

	enum Type{
		Type_AABOX=			1<<0,
		Type_SPHERE=		1<<1,
		Type_INFINITE=		1<<2,
	};

	Bound():
		mType(Type_AABOX),
		mEpsilon(Math::fromMilli(1))
	{}

	Bound(Type type):
		mType(type),
		mEpsilon(Math::fromMilli(1))
	{}

	Bound(Bound *bound):
		mType(bound->mType),
		mEpsilon(Math::fromMilli(1)),
		mBox(bound->mBox),
		mSphere(bound->mSphere)
	{}

	Bound(const AABox &box):
		mType(Type_AABOX),
		mEpsilon(Math::fromMilli(1)),
		mBox(box)
	{}

	Bound(const Sphere &sphere):
		mType(Type_SPHERE),
		mEpsilon(Math::fromMilli(1)),
		mSphere(sphere)
	{}
	
	void reset(){
		mType=Type_AABOX;
		mBox.reset();
		mSphere.reset();
	}

	void set(Bound *b){
		mType=b->mType;
		mSphere.set(b->mSphere);
		mBox.set(b->mBox);
	}

	void setInfinite(){
		mType=Type_INFINITE; 
		mBox.reset();
		mSphere.reset();
	}

	void set(const Sphere &s){
		mType=Type_SPHERE;
		mSphere.set(s);
		update();
	}

	void set(const Vector3 &origin,scalar radius){
		mType=Type_SPHERE;
		mSphere.origin.set(origin);
		mSphere.radius=radius;
		update();
	}

	void set(scalar radius){
		mType=Type_SPHERE;
		mSphere.origin.reset();
		mSphere.radius=radius;
		update();
	}

	void set(const AABox &a){
		mType=Type_AABOX;
		mBox.set(a);
		update();
	}

	void set(const Vector3 &mins,const Vector3 &maxs){
		mType=Type_AABOX;
		mBox.set(mins,maxs);
		update();
	}

	void merge(Bound *b){
		if(mType==Type_INFINITE || b->mType==Type_INFINITE){
			mType=Type_INFINITE;
		}
		else if(mType==Type_AABOX && b->mType==Type_AABOX){
			mBox.merge(b->mBox);
		}
		else{
			// Just switch to a sphere
			mType=Type_SPHERE;
			mSphere.merge(b->mSphere,mEpsilon);
		}
		update();
	}

	bool testIntersection(Bound *b) const{
		if(mType==Type_INFINITE || b->mType==Type_INFINITE){
			return true;
		}
		else if(mType==Type_AABOX && b->mType==Type_AABOX){
			return testIntersection(b->mBox);
		}
		else{
			return testIntersection(b->mSphere);
		}
	}

	bool testIntersection(const Vector3 &v) const{
		switch(mType){
			case Type_INFINITE:
				return true;
			case Type_AABOX:
				return Math::testInside(mBox,v);
			default:
				return Math::testInside(mSphere,v);
		}
	}

	bool testIntersection(const AABox &a) const{
		if(mType==Type_INFINITE){
			return true;
		}
		else{
			return Math::testIntersection(mBox,a);
		}
	}

	bool testIntersection(const Sphere &s) const{
		if(mType==Type_INFINITE){
			return true;
		}
		else{
			return Math::testIntersection(mSphere,s);
		}
	}

	Type getType() const{return mType;}
	const Sphere &getSphere() const{return mSphere;}
	const AABox &getAABox() const{return mBox;}

	void transform(Bound *source,Transform *transform){
		mType=source->mType;

		const Vector3 &translate=transform->getTranslate();
		const Quaternion &rotate=transform->getRotate();
		const Vector3 &scale=transform->getScale();

		if(source->mType==Type_SPHERE){
			Math::mul(mSphere.origin,scale,source->mSphere.radius);
			mSphere.radius=Math::maxVal(Math::maxVal(mSphere.origin.x,mSphere.origin.y),mSphere.origin.z);
			Math::mul(mSphere.origin,rotate,source->mSphere.origin);
			Math::mul(mSphere.origin,scale);
			Math::add(mSphere.origin,translate);
		}
		else if(source->mType==Type_AABOX){
			Math::mul(mBox.mins,source->mBox.mins,scale);
			Math::mul(mBox.maxs,source->mBox.maxs,scale);
			Math::add(mBox,translate);
		}
		update();
	}

protected:
	void update(){
		switch(mType){
			case Type_AABOX:
				Math::findBoundingSphere(mSphere,mBox);
			case Type_SPHERE:
				Math::findBoundingBox(mBox,mSphere);
		}
	}

	Type mType;
	scalar mEpsilon;
	Sphere mSphere;
	AABox mBox;
};

}
}

#endif
