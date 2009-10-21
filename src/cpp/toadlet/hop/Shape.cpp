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

#include <toadlet/hop/Shape.h>
#include <toadlet/hop/Solid.h>
#include <toadlet/hop/Simulator.h>

namespace toadlet{
namespace hop{

Shape::Shape():
	mType(Type_AABOX),
	//mAABox,
	//mSphere,
	//mCapsule,
	//mConvexSolid,
	mSolid(NULL)
{
}

Shape::Shape(const AABox &box):
	mType(Type_AABOX),
	//mAABox,
	//mSphere,
	//mCapsule,
	//mConvexSolid,
	mSolid(NULL)
{
	mAABox.set(box);
}

Shape::Shape(const Sphere &sphere):
	mType(Type_SPHERE),
	//mAABox,
	//mSphere,
	//mCapsule,
	//mConvexSolid,
	mSolid(NULL)
{
	mSphere.set(sphere);
}

Shape::Shape(const Capsule &capsule):
	mType(Type_CAPSULE),
	//mAABox,
	//mSphere,
	//mCapsule,
	//mConvexSolid,
	mSolid(NULL)
{
	mCapsule.set(capsule);
}

Shape::Shape(const ConvexSolid &convexSolid):
	mType(Type_CONVEXSOLID),
	//mAABox,
	//mSphere,
	//mCapsule,
	//mConvexSolid,
	mSolid(NULL)
{
	mConvexSolid.set(convexSolid);
}

void Shape::reset(){
	if(mSolid!=NULL){
		mSolid->removeShape(this);
		mSolid=NULL;
	}

	mAABox.reset();
	mType=Type_AABOX;

	mSphere.reset();
	mCapsule.reset();
}

void Shape::setAABox(const AABox &box){
	mType=Type_AABOX;
	mAABox.set(box);

	if(mSolid!=NULL){
		internal_clamp();
		mSolid->updateLocalBound();
	}
}

void Shape::setSphere(const Sphere &sphere){
	mType=Type_SPHERE;
	mSphere.set(sphere);

	if(mSolid!=NULL){
		mSolid->updateLocalBound();
	}
}

void Shape::setCapsule(const Capsule &capsule){
	mType=Type_CAPSULE;
	mCapsule.set(capsule);

	if(mSolid!=NULL){
		mSolid->updateLocalBound();
	}
}

void Shape::setConvexSolid(const ConvexSolid &convexSolid){
	mType=Type_CONVEXSOLID;
	mConvexSolid.set(convexSolid);

	if(mSolid!=NULL){
		mSolid->updateLocalBound();
	}
}

void Shape::getBound(AABox &box) const{
	switch(mType){
		case Type_AABOX:
			box.set(mAABox);
		break;
		case Type_SPHERE:
		{
			scalar r=mSphere.radius;
			box.mins.x=-r;
			box.mins.y=-r;
			box.mins.z=-r;
			box.maxs.x=r;
			box.maxs.y=r;
			box.maxs.z=r;
			Math::add(box,mSphere.origin);
		}
		break;
		case Type_CAPSULE:
		{
			scalar r=mCapsule.radius;
			const Vector3 &d=mCapsule.direction;

			if(d.x<0){
				box.mins.x=d.x-r;	
				box.maxs.x=r;
			}
			else{
				box.mins.x=-r;	
				box.maxs.x=d.x+r;
			}
			if(d.y<0){
				box.mins.y=d.y-r;	
				box.maxs.y=r;
			}
			else{
				box.mins.y=-r;	
				box.maxs.y=d.y+r;
			}
			if(d.z<0){
				box.mins.z=d.z-r;	
				box.maxs.z=r;
			}
			else{
				box.mins.z=-r;	
				box.maxs.z=d.z+r;
			}

			Math::add(box,mCapsule.origin);
		}
		break;
		case Type_CONVEXSOLID:
		{
			Vector3 t;
			for(int i=mConvexSolid.planes.size()-1;i>=0;--i){
				t.set(mConvexSolid.planes[i].normal);
				Math::mul(t,mConvexSolid.planes[i].d);
				if(i==0){
					box.mins.x=t.x;
					box.maxs.x=t.x;
					box.mins.y=t.y;
					box.maxs.y=t.y;
					box.mins.z=t.z;
					box.maxs.z=t.z;
				}
				else{
					if(t.x<box.mins.x) box.mins.x=t.x;
					if(t.x>box.maxs.x) box.maxs.x=t.x;
					if(t.y<box.mins.y) box.mins.y=t.y;
					if(t.y>box.maxs.y) box.maxs.y=t.y;
					if(t.z<box.mins.z) box.mins.z=t.z;
					if(t.z>box.maxs.z) box.maxs.z=t.z;
				}
			}
		}
		break;
	}
}

void Shape::internal_clamp(){
	if(mSolid!=NULL && mSolid->mSimulator!=NULL){
		switch(mType){
			case Type_AABOX:
				mSolid->mSimulator->clampPosition(mAABox.mins);
				mSolid->mSimulator->clampPosition(mAABox.maxs);
			break;
			default:
			break;
		}
	}
}

}
}

