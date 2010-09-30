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

namespace toadlet{
namespace tadpole{

class Bound{
public:
	enum Type{
		Type_AABOX=			1<<0,
		Type_SPHERE=		1<<1,
		Type_INFINITE=		1<<2,
	};

	Bound(){}

	Bound(Type type){
		TOADLET_ASSERT(type==Type_INFINITE);
		set(-Math::ONE);
	}

	explicit Bound(const Sphere &s){
		set(s);
	}
	
	explicit Bound(const AABox &a){
		set(a);
	}
	
	void reset(){
		box.reset();
		sphere.reset();
	}

	void setInfinite(){
		type=Type_SPHERE;
		sphere.radius=-Math::ONE;
		update();
	}

	void set(const Bound &b){
		type=b.Type_AABOX;
		sphere.set(b.sphere);
		box.set(b.box);
	}

	void set(const Sphere &s){
		type=Type_SPHERE;
		sphere.set(s);
		update();
	}

	void set(const Vector3 &origin,scalar radius){
		type=Type_SPHERE;
		sphere.origin.set(origin);
		sphere.radius=radius;
		update();
	}

	void set(scalar radius){
		type=Type_SPHERE;
		sphere.origin.reset();
		sphere.radius=radius;
		update();
	}

	void set(const AABox &a){
		type=Type_AABOX;
		box.set(a);
		update();
	}

	void set(const Vector3 &mins,const Vector3 &maxs){
		type=Type_AABOX;
		box.set(mins,maxs);
		update();
	}

	// Merge two spheres, passing along -1 radius, and ignoring 0 radius
	void merge(const Bound &b){
		Vector3 origin=(sphere.origin+b.sphere.origin)/2;
		scalar radius=Math::maxVal(Math::length(sphere.origin,origin)+Math::maxVal(sphere.radius,0),Math::length(b.sphere.origin,origin)+Math::maxVal(b.sphere.radius,0));
		sphere.origin.set(origin);
		if(sphere.radius>=0 && b.sphere.radius>=0){
			sphere.radius=radius;
		}
		else{
			sphere.radius=-Math::ONE;
		}
	}

	void transform(const Vector3 &translate,const Quaternion &rotate,const Vector3 &scale,const Bound &b){
		Math::mul(sphere.origin,scale,b.sphere.radius);
		sphere.radius=Math::maxVal(Math::maxVal(sphere.origin.x,sphere.origin.y),sphere.origin.z);
		Math::mul(sphere.origin,rotate,b.sphere.origin);
		Math::mul(sphere.origin,scale);
		Math::add(sphere.origin,translate);
	}

	bool testIntersection(const Bound &b) const{
		return testIntersection(b.sphere);
	}

	bool testIntersection(const AABox &a) const{
		if(sphere.radius<0){
			return true;
		}

		return Math::testIntersection(sphere,a);
	}

	bool testIntersection(const Sphere &s) const{
		if(sphere.radius<0 || s.radius<0){
			return true;
		}

		return Math::testIntersection(sphere,s);
	}

	const Sphere &getSphere() const{return sphere;}
	const AABox &getAABox() const{return box;}

protected:
	void update(){
		if(type==Type_AABOX){
			Math::findBoundingSphere(sphere,box);
		}
		else if(type==Type_SPHERE){
			Math::findBoundingBox(box,sphere);
		}
	}

	Type type;
	Sphere sphere;
	AABox box;
};

}
}

#endif
