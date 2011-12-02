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

#ifndef TOADLET_HOP_CONSTRAINT_H
#define TOADLET_HOP_CONSTRAINT_H

#include <toadlet/hop/Types.h>
#include <toadlet/hop/Solid.h>

namespace toadlet{
namespace hop{

class TOADLET_API Constraint{
public:
	TOADLET_SHARED_POINTERS(Constraint);

	Constraint();
	Constraint(Solid::ptr startSolid,Solid::ptr endSolid);
	Constraint(Solid::ptr startSolid,const Vector3 &endPoint);
	virtual ~Constraint();
	void destroy();

	void reset();

	void setStartSolid(Solid::ptr startSolid);
	Solid::ptr getStartSolid() const{return mStartSolid;}

	void setEndSolid(Solid::ptr endSolid);
	Solid::ptr getEndSolid() const{return mEndSolid;}

	void setEndPoint(const Vector3 &endPoint);
	const Vector3 &getEndPoint() const{return mEndPoint;}

	void setSpringConstant(scalar constant);
	scalar getSpringConstant() const{return mSpringConstant;}

	void setDampingConstant(scalar constant);
	scalar getDampingConstant() const{return mDampingConstant;}

	void setDistanceThreshold(scalar threshold);
	scalar getDistanceThreshold() const{return mDistanceThreshold;}

	void activate(){}
	bool active() const{return (mSimulator!=NULL);}

	inline Simulator *getSimulator() const{return mSimulator;}

protected:
	void internal_setSimulator(Simulator *s);

	Solid::ptr mStartSolid;
	Solid::ptr mEndSolid;
	Vector3 mEndPoint;

	scalar mSpringConstant;
	scalar mDampingConstant;
	scalar mDistanceThreshold;

	Simulator *mSimulator;

	friend class Solid;
	friend class Simulator;
};

}
}

#endif

