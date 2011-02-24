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

#ifndef TOADLET_TADPOLE_TRANSFORM_H
#define TOADLET_TADPOLE_TRANSFORM_H

#include <toadlet/tadpole/Types.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API Transform{
public:
	TOADLET_SHARED_POINTERS(Transform);

	Transform():
		mScale(Math::ONE,Math::ONE,Math::ONE)
	{}

	Transform(const Vector3 &translate,const Quaternion &rotate,const Vector3 &scale){
		mTranslate.set(translate);
		mRotate.set(rotate);
		mScale.set(scale);
	}
	
	void reset(){
		mTranslate.reset();
		mRotate.reset();
		mScale.set(Math::ONE,Math::ONE,Math::ONE);
	}

	void set(Transform *transform){
		mTranslate.set(transform->mTranslate);
		mRotate.set(transform->mRotate);
		mScale.set(transform->mScale);
	}

	void set(const Vector3 &translate,const Quaternion &rotate,const Vector3 &scale){
		mTranslate.set(translate);
		mRotate.set(rotate);
		mScale.set(scale);
	}

	const Vector3 &getTranslate() const{return mTranslate;}
	void setTranslate(const Vector3 &t){mTranslate.set(t);}
	void setTranslate(scalar x,scalar y,scalar z){mTranslate.set(x,y,z);}
	const Quaternion &getRotate() const{return mRotate;}
	void setRotate(const Quaternion &r){mRotate.set(r);}
	void setRotate(const Matrix3x3 &r){Math::setQuaternionFromMatrix3x3(mRotate,r);}
	void setRotate(const Vector3 &axis,scalar angle){Math::setQuaternionFromAxisAngle(mRotate,axis,angle);}
	const Vector3 &getScale() const{return mScale;}
	void setScale(const Vector3 &s){mScale.set(s);}
	void setScale(scalar x,scalar y,scalar z){mScale.set(x,y,z);}
	
	void toMatrix(Matrix4x4 &r){
		Math::setMatrix4x4FromTranslateRotateScale(r,mTranslate,mRotate,mScale);
	}

	void transform(Transform *parentTransform,Transform *transform){
		Math::mul(mScale,parentTransform->mScale,transform->mScale);
		Math::mul(mRotate,parentTransform->mRotate,transform->mRotate);
		Math::mul(mTranslate,parentTransform->mRotate,transform->mTranslate);
		Math::mul(mTranslate,parentTransform->mScale);
		Math::add(mTranslate,parentTransform->mTranslate);
	}
	
	void transform(Vector3 &r,const Vector3 &t){transform(r,t,mTranslate,mScale,mRotate);}
	void transform(Vector3 &r){transform(r,mTranslate,mScale,mRotate);}
	void inverseTransform(Vector3 &r,const Vector3 &t){inverseTransform(r,t,mTranslate,mScale,mRotate);}
	void inverseTransform(Vector3 &r){inverseTransform(r,mTranslate,mScale,mRotate);}
	void transformNormal(Vector3 &r,const Vector3 &n){transformNormal(r,n,mRotate);}
	void transformNormal(Vector3 &r){transformNormal(r,mRotate);}
	void inverseTransformNormal(Vector3 &r,const Vector3 &n){inverseTransformNormal(r,n,mRotate);}
	void inverseTransformNormal(Vector3 &r){inverseTransformNormal(r,mRotate);}
	void inverseTransform(Segment &r,const Segment &s){inverseTransform(r,s,mTranslate,mScale,mRotate);}
	void inverseTransform(Segment &r){inverseTransform(r,mTranslate,mScale,mRotate);}
	void transform(Quaternion &r,const Quaternion &q){Math::mul(r,mRotate,q);}
	void transform(Quaternion &r){Math::preMul(r,mRotate);}
	/// @todo: I should cache the inverseRotation and only recompute it when necessary
	void inverseTransform(Quaternion &r,const Quaternion &q){Quaternion invRotate;Math::invert(invRotate,mRotate);Math::mul(r,invRotate,q);}
	void inverseTransform(Quaternion &r){Quaternion invRotate;Math::invert(invRotate,mRotate);Math::preMul(r,invRotate);}

	static void transform(Vector3 &r,const Vector3 &t,const Vector3 &translate,const Vector3 &scale,const Quaternion &rotate){
		Math::mul(r,rotate,t);
		Math::mul(r,scale);
		Math::add(r,translate);
	}

	static void transform(Vector3 &r,const Vector3 &translate,const Vector3 &scale,const Quaternion &rotate){
		Math::mul(r,rotate);
		Math::mul(r,scale);
		Math::add(r,translate);
	}

	static void inverseTransform(Vector3 &r,const Vector3 &t,const Vector3 &translate,const Vector3 &scale,const Quaternion &rotate){
		Quaternion invrot; Math::invert(invrot,rotate);
		Math::sub(r,t,translate);
		Math::div(r,scale);
		Math::mul(r,invrot);
	}

	static void inverseTransform(Vector3 &r,const Vector3 &translate,const Vector3 &scale,const Quaternion &rotate){
		Quaternion invrot; Math::invert(invrot,rotate);
		Math::sub(r,translate);
		Math::div(r,scale);
		Math::mul(r,invrot);
	}

	static void transformNormal(Vector3 &r,const Vector3 &n,const Quaternion &rotate){
		Math::mul(r,rotate,n);
		Math::normalize(r);
	}

	static void transformNormal(Vector3 &r,const Quaternion &rotate){
		Math::mul(r,rotate);
		Math::normalize(r);
	}

	static void inverseTransformNormal(Vector3 &r,const Vector3 &n,const Quaternion &rotate){
		Quaternion invrot; Math::invert(invrot,rotate);
		Math::mul(r,invrot,n);
	}

	static void inverseTransformNormal(Vector3 &r,const Quaternion &rotate){
		Quaternion invrot; Math::invert(invrot,rotate);
		Math::mul(r,invrot);
	}

	static void inverseTransform(Segment &r,const Segment &s,const Vector3 &translate,const Vector3 &scale,const Quaternion &rotate){
		Quaternion invrot; Math::invert(invrot,rotate);
		Math::sub(r.origin,s.origin,translate);
		Math::div(r.origin,scale);
		Math::mul(r.origin,invrot);
		Math::mul(r.direction,invrot,s.direction);
		Math::div(r.direction,scale);
	}

	static void inverseTransform(Segment &r,const Vector3 &translate,const Vector3 &scale,const Quaternion &rotate){
		Quaternion invrot; Math::invert(invrot,rotate);
		Math::sub(r.origin,translate);
		Math::div(r.origin,scale);
		Math::mul(r.origin,invrot);
		Math::mul(r.direction,invrot);
		Math::div(r.direction,scale);
	}

protected:
	Vector3 mTranslate;
	Quaternion mRotate;
	Vector3 mScale;
};

}
}

#endif
