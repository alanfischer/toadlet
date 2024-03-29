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
	mTraceable(NULL),
	mSolid(NULL)
{
}

Shape::Shape(const AABox &box):
	mType(Type_AABOX),
	//mAABox,
	//mSphere,
	//mCapsule,
	//mConvexSolid,
	mTraceable(NULL),
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
	mTraceable(NULL),
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
	mTraceable(NULL),
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
	mTraceable(NULL),
	mSolid(NULL)
{
	mConvexSolid.set(convexSolid);
}

Shape::Shape(Traceable *traceable):
	mType(Type_TRACEABLE),
	//mAABox,
	//mSphere,
	//mCapsule,
	//mConvexSolid,
	mTraceable(NULL),
	mSolid(NULL)
{
	mTraceable=traceable;
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

void Shape::setTraceable(Traceable *traceable){
	mType=Type_TRACEABLE;
	mTraceable=traceable;

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
			Math::findBoundingBox(box,mSphere);
		break;
		case Type_CAPSULE:
			Math::findBoundingBox(box,mCapsule);
		break;
		case Type_CONVEXSOLID:
		{
			Vector3 &r=const_cast<Vector3&>(cache_getBound_r);
			int i,j,k,l;
			scalar epsilon;
			#if defined(TOADLET_FIXED_POINT)
				epsilon=1<<4;
			#else
				epsilon=0.0001;
			#endif
			box.reset();
			for(i=0;i<mConvexSolid.planes.size()-2;++i){
				for(j=i+1;j<mConvexSolid.planes.size()-1;++j){
					for(k=j+1;k<mConvexSolid.planes.size();++k){
						if(Math::getIntersectionOfThreePlanes(r,mConvexSolid.planes[i],mConvexSolid.planes[j],mConvexSolid.planes[k],epsilon)){
							bool legal=true;
							for(l=0;l<mConvexSolid.planes.size();++l){
								if(l!=i && l!=j && l!=k){
									const Plane &plane=mConvexSolid.planes[l];
									if((Math::dot(plane.normal,r)-plane.distance) > epsilon){
										legal=false;
										break;
									}
								}
							}

							if(legal){
								if(i==0 && j==1 && k==2){
									box.mins.x=r.x;
									box.maxs.x=r.x;
									box.mins.y=r.y;
									box.maxs.y=r.y;
									box.mins.z=r.z;
									box.maxs.z=r.z;
								}
								else{
									if(r.x<box.mins.x) box.mins.x=r.x;
									if(r.x>box.maxs.x) box.maxs.x=r.x;
									if(r.y<box.mins.y) box.mins.y=r.y;
									if(r.y>box.maxs.y) box.maxs.y=r.y;
									if(r.z<box.mins.z) box.mins.z=r.z;
									if(r.z>box.maxs.z) box.maxs.z=r.z;
								}
							}
						}	
					}
				}
			}
		}
		break;
		case(Type_TRACEABLE):
			mTraceable->getBound(box);
		break;
	}
}

}
}

