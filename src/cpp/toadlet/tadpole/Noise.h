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

#ifndef TOADLET_TADPOLE_NOISE_H
#define TOADLET_TADPOLE_NOISE_H

#include <toadlet/egg/Random.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API Noise{
public:
	Noise(int octaves,float freq,float amp,int seed,int sampleSize=1024);
	~Noise();

	float noise1(float x);
	float noise2(float x,float y);
	float noise3(float x,float y,float z);

	float perlin1(float x);
	float perlin2(float x,float y);
	float perlin3(float x,float y,float z);

	float tileableNoise1(float x,float w);
	float tileableNoise2(float x,float y,float w,float h);
	float tileableNoise3(float x,float y,float z,float w,float h,float d);

	float tileablePerlin1(float x,float w);
	float tileablePerlin2(float x,float y,float w,float h);
	float tileablePerlin3(float x,float y,float z,float w,float h,float d);

protected:
	void init();
	inline float scurve(float t){ return t*t*(3.0 - 2.0*t);}

	egg::Random mRandom;
	int mOctaves;
	float mFrequency;
	float mAmplitude;
	int mSampleSize;

	int *p;
	float *g1;
	float *g2;
	float *g3;
};

}
}

#endif

