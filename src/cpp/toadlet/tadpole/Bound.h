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
		type=Type_AABOX;
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

	void merge(const Bound &b){
		if(type==Type_AABOX && b.type==Type_AABOX){
			box.merge(b.box);
		}
		else{
			// Just switch to a sphere
			type=Type_SPHERE;
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
		update();
	}


	bool testIntersection(const Vector3 &v) const{
		if(type==Type_AABOX){
			return Math::testInside(v,box);
		}
		else{
			if(sphere.radius<0){
				return true;
			}
			return Math::testInside(v,sphere);
		}
	}

	bool testIntersection(const Bound &b) const{
		if(type==Type_AABOX && b.type==Type_AABOX){
			return testIntersection(b.box);
		}
		/// @todo: Add checks for AABox to Sphere, which take into account infinite.  Or just move Inifinite to its own proper type
		else{
			return testIntersection(b.sphere);
		}
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
