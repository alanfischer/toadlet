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

#include <toadlet/egg/io/DataStream.h>
#include <toadlet/tadpole/Types.h>

namespace toadlet{
namespace tadpole{

/// @todo: Decide if this is a State style object, or a full Object
class TOADLET_API Transform{
public:
	Transform():
		mScale(Math::ONE,Math::ONE,Math::ONE)
	{}

	Transform(const Vector3 &t,const Vector3 &s,const Quaternion &r){
		mTranslate.set(t);
		mScale.set(s);
		mRotate.set(r);
	}
	
	void reset(){
		mTranslate.reset();
		mScale.set(Math::ONE,Math::ONE,Math::ONE);
		mRotate.reset();
	}

	void set(const Transform &transform){
		mTranslate.set(transform.mTranslate);
		mScale.set(transform.mScale);
		mRotate.set(transform.mRotate);
	}

	void set(const Vector3 &t,const Vector3 &s,const Quaternion &r){
		mTranslate.set(t);
		mScale.set(s);
		mRotate.set(r);
	}

	void setTransform(const Transform &parentTransform,const Transform &transform){
		Math::mul(mRotate,parentTransform.mRotate,transform.mRotate);
		Math::mul(mScale,parentTransform.mScale,transform.mScale);
		Math::mul(mTranslate,parentTransform.mRotate,transform.mTranslate);
		Math::mul(mTranslate,parentTransform.mScale);
		Math::add(mTranslate,parentTransform.mTranslate);
	}
	
	const Vector3 &getTranslate() const{return mTranslate;}
	void setTranslate(const Vector3 &t){mTranslate.set(t);}
	void setTranslate(scalar x,scalar y,scalar z){mTranslate.set(x,y,z);}

	const Vector3 &getScale() const{return mScale;}
	void setScale(const Vector3 &s){mScale.set(s);}
	void setScale(scalar x,scalar y,scalar z){mScale.set(x,y,z);}

	const Quaternion &getRotate() const{return mRotate;}
	void setRotate(const Quaternion &r){mRotate.set(r);}
	void setRotate(const Matrix3x3 &r){Math::setQuaternionFromMatrix3x3(mRotate,r);}
	void setRotate(const Vector3 &axis,scalar angle){Math::setQuaternionFromAxisAngle(mRotate,axis,angle);}

	void getMatrix(Matrix4x4 &r) const{
		Math::setMatrix4x4FromTranslateRotateScale(r,mTranslate,mRotate,mScale);
	}
	void setMatrix(const Matrix4x4 &m){
		Math::setScaleFromMatrix4x4(mScale,m);
		Math::setQuaternionFromMatrix4x4(mRotate,m);
		Math::setTranslateFromMatrix4x4(mTranslate,m);
	}

	void transform(Vector3 &r,const Vector3 &t) const{
		Math::mul(r,mRotate,t);
		Math::mul(r,mScale);
		Math::add(r,mTranslate);
	}

	void transform(Vector3 &r) const{
		Math::mul(r,mRotate);
		Math::mul(r,mScale);
		Math::add(r,mTranslate);
	}

	void transformNormal(Vector3 &r,const Vector3 &n) const{
		Math::mul(r,mRotate,n);
		Math::div(r,mScale);
		Math::normalizeCarefully(r,0);
	}

	void transformNormal(Vector3 &r) const{
		Math::mul(r,mRotate);
		Math::div(r,mScale);
		Math::normalizeCarefully(r,0);
	}

	void inverseTransform(Vector3 &r,const Vector3 &v) const{
		Quaternion invrot; Math::invert(invrot,mRotate);
		Math::sub(r,v,mTranslate);
		Math::div(r,mScale);
		Math::mul(r,invrot);
	}

	void inverseTransform(Vector3 &r) const{
		Quaternion invrot; Math::invert(invrot,mRotate);
		Math::sub(r,mTranslate);
		Math::div(r,mScale);
		Math::mul(r,invrot);
	}

	void transform(Segment &r,const Segment &s) const{
		Math::mul(r.origin,mRotate,s.origin);
		Math::mul(r.origin,mScale);
		Math::add(r.origin,mTranslate);
		Math::mul(r.direction,mRotate,s.direction);
		Math::mul(r.direction,mScale);
	}

	void transform(Segment &r) const{
		Math::mul(r.origin,mRotate);
		Math::mul(r.origin,mScale);
		Math::add(r.origin,mTranslate);
		Math::mul(r.direction,mRotate);
		Math::mul(r.direction,mScale);
	}

	void inverseTransform(Segment &r,const Segment &s) const{
		Quaternion invrot; Math::invert(invrot,mRotate);
		Math::sub(r.origin,s.origin,mTranslate);
		Math::div(r.origin,mScale);
		Math::mul(r.origin,invrot);
		Math::mul(r.direction,invrot,s.direction);
		Math::div(r.direction,mScale);
	}

	void inverseTransform(Segment &r) const{
		Quaternion invrot; Math::invert(invrot,mRotate);
		Math::sub(r.origin,mTranslate);
		Math::div(r.origin,mScale);
		Math::mul(r.origin,invrot);
		Math::mul(r.direction,invrot);
		Math::div(r.direction,mScale);
	}

	void inverseTransform(Matrix4x4 &r,const Matrix4x4 &m) const{
		Matrix4x4 matrix; getMatrix(matrix);
		Matrix4x4 invxform;	Math::invert(invxform,matrix);
		Math::mul(r,invxform,m);
	}

protected:
	Vector3 mTranslate;
	Vector3 mScale;
	Quaternion mRotate;
};

}
}

#endif
