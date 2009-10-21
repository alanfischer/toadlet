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

#ifndef TOADLET_EGG_RANDOM_H
#define TOADLET_EGG_RANDOM_H

#include <toadlet/Types.h>
#include <toadlet/egg/WeakPointer.h>

namespace toadlet{
namespace egg{

class TOADLET_API Random{
public:
	TOADLET_SHARED_POINTERS(Random,Random);

	Random();

	Random(int64 seed);

	void setSeed(int64 seed);

	// Returns a uniformly distributed random int
	int nextInt();

	// Returns a uniformly distributed random int between 0 (inclusive) and n (exclusive)
	int nextInt(int n);

	// Returns a uniformly distributed random int between lower (inclusive) and upper (exclusive)
	int nextInt(int lower,int upper);

	// Returns a uniformly distributed random float between 0.0 and 1.0
	float nextFloat();

	float nextFloat(float upper);

	// Returns a uniformly distributed random float between lower and upper
	float nextFloat(float lower,float upper);

	inline scalar nextScalar(scalar lower,scalar upper){
		#if defined(TOADLET_FIXED_POINT)
			return nextInt(lower,upper);
		#else
			return nextFloat(lower,upper);
		#endif
	}

protected:
	int next(int bits);

	int64 mSeed;
};

}
}

#endif

