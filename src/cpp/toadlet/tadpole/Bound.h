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

	Bound():type(Type_AABOX){}

	/// @todo: Eliminiate the -1 for inifinite, and just replace it with the infinits type checks
	Bound(Type t){
		TOADLET_ASSERT(t==Type_INFINITE);
		type=t;
	}

	explicit Bound(const Sphere &s){
		set(s);
	}
	
	explicit Bound(const AABox &a){
		set(a);
	}
	
	void reset(){
		type=Type_AABOX;
		box.reset();
		sphere.reset();
	}

	void setInfinite(){
		type=Type_INFINITE; 
		box.reset();
		sphere.reset();
	}

	void set(const Bound &b){
		type=b.type;
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

	void merge(const Bound &b){
		if(type==Type_INFINITE || b.type==Type_INFINITE){
			type=Type_INFINITE;
		}
		else if(type==Type_AABOX && b.type==Type_AABOX){
			box.merge(b.box);
		}
		else{
			// Just switch to a sphere
			type=Type_SPHERE;
			sphere.merge(b.sphere);
		}
		update();
	}


	bool testIntersection(const Vector3 &v) const{
		if(type==Type_INFINITE){
			return true;
		}
		else if(type==Type_AABOX){
			return Math::testInside(v,box);
		}
		else{
			return Math::testInside(v,sphere);
		}
	}

	bool testIntersection(const Bound &b) const{
		if(type==Type_INFINITE || b.type==Type_INFINITE){
			return true;
		}
		else if(type==Type_AABOX && b.type==Type_AABOX){
			return testIntersection(b.box);
		}
		else{
			return testIntersection(b.sphere);
		}
	}

	bool testIntersection(const AABox &a) const{
		if(type==Type_INFINITE){
			return true;
		}
		else{
			return Math::testIntersection(box,a);
		}
	}

	bool testIntersection(const Sphere &s) const{
		if(type==Type_INFINITE){
			return true;
		}
		else{
			return Math::testIntersection(sphere,s);
		}
	}

	Type getType() const{return type;}
	const Sphere &getSphere() const{return sphere;}
	const AABox &getAABox() const{return box;}

	static void transform(Bound &r,const Bound &b,const Vector3 &translate,const Quaternion &rotate,const Vector3 &scale){
		r.type=b.type;
		if(b.type==Type_SPHERE){
			Math::mul(r.sphere.origin,scale,b.sphere.radius);
			r.sphere.radius=Math::maxVal(Math::maxVal(r.sphere.origin.x,r.sphere.origin.y),r.sphere.origin.z);
			Math::mul(r.sphere.origin,rotate,b.sphere.origin);
			Math::mul(r.sphere.origin,scale);
			Math::add(r.sphere.origin,translate);
		}
		else if(b.type==Type_AABOX){
			Math::mul(r.box.mins,b.box.mins,scale);
			Math::mul(r.box.maxs,b.box.maxs,scale);
			Math::add(r.box,translate);
		}
		r.update();
	}

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
