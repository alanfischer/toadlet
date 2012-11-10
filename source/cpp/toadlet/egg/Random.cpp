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

#include <toadlet/egg/Random.h>

namespace toadlet{
namespace egg{

Random::Random(){
	setSeed(0);
}

Random::Random(int64 seed){
	setSeed(seed);
}

void Random::setSeed(int64 seed){
	mSeed=(seed ^ TOADLET_MAKE_INT64(0x5DEECE66D)) & ((int64(1) << 48) - 1);
}

int Random::next(int bits){
	mSeed=(mSeed * TOADLET_MAKE_INT64(0x5DEECE66D) + int64(0xB)) & ((int64(1) << 48) - 1);
	return (int)(((uint64)mSeed)>>(48-bits));
}

int Random::nextInt(){
	return next(32);
}

int Random::nextInt(int n){
	if(n<=0){
		return 0;
	}

	if((n&-n)==n){
		return (int)((n*(int64)next(31))>>31);
	}

	int bits,val;
	do{
		bits=next(31);
		val=bits%n;
	}while(bits-val+(n-1)<0);

	return val;
}

int Random::nextInt(int lower,int upper){
	return nextInt(upper-lower)+lower;
}

float Random::nextFloat(){
	return next(24) / ((float)(1<<24));
}

float Random::nextFloat(float n){
	return (nextFloat()*n);
}

float Random::nextFloat(float lower,float upper){
	return (nextFloat()*(upper-lower))+lower;
}

}
}

