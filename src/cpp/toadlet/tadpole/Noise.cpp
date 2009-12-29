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

/* Ken Perlin's noise function, based on some code by John Ratcliff on Flipcode */

#include <toadlet/tadpole/Noise.h>
#include <toadlet/egg/math/Math.h>

using namespace toadlet::egg::math;

namespace toadlet{
namespace tadpole{

Noise::Noise(int octaves,float frequency,float amplitude,int seed,int sampleSize){
	mOctaves=octaves;
	mFrequency=frequency;
	mAmplitude=amplitude;
	mSampleSize=sampleSize;
	mRandom.setSeed(seed);

	p=new int[2*mSampleSize + 2];
	g1=new float[(2*mSampleSize + 2) * 1];
	g2=new float[(2*mSampleSize + 2) * 2];
	g3=new float[(2*mSampleSize + 2) * 3];

	init();
}

Noise::~Noise(){
	delete[] p;
	delete[] g1;
	delete[] g2;
	delete[] g3;
}

float Noise::noise1(float x){
	int bx0, bx1;
	float rx0, rx1, sx, u, v;

	x*=mFrequency;
	x+=mSampleSize;
	bx0=((int)x) & (mSampleSize-1);
	bx1=(bx0+1) & (mSampleSize-1);
	rx0=x-(int)x;
	rx1=rx0-1.0f;

	sx=scurve(rx0);

	u=rx0 * g1[p[bx0]];
	v=rx1 * g1[p[bx1]];

	return Math::lerp(u,v,sx) * mAmplitude;
}

float Noise::noise2(float x,float y){
	int bx0, bx1, by0, by1, b00, b10, b01, b11;
	float rx0, rx1, ry0, ry1, sx, sy, a, b, u, v;
	int i,j;

	x*=mFrequency;
	x+=mSampleSize;
	bx0=((int)x) & (mSampleSize-1);
	bx1=(bx0+1) & (mSampleSize-1);
	rx0=x-(int)x;
	rx1=rx0-1.0f;

	y*=mFrequency;
	y+=mSampleSize;
	by0=((int)y) & (mSampleSize-1);
	by1=(by0+1) & (mSampleSize-1);
	ry0=y-(int)y;
	ry1=ry0-1.0f;

	i=p[bx0];
	j=p[bx1];

	b00=p[i + by0];
	b10=p[j + by0];
	b01=p[i + by1];
	b11=p[j + by1];

	sx=scurve(rx0);
	sy=scurve(ry0);

	u=rx0*g2[2*b00] + ry0*g2[2*b00+1];
	v=rx1*g2[2*b10] + ry0*g2[2*b10+1];
	a=Math::lerp(u,v,sx);

	u=rx0*g2[2*b01] + ry1*g2[2*b01+1];
	v=rx1*g2[2*b11] + ry1*g2[2*b11+1];
	b=Math::lerp(u,v,sx);

	return Math::lerp(a,b,sy) * mAmplitude;
}

float Noise::noise3(float x,float y,float z){
	int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11, qoff;
	float rx0, rx1, ry0, ry1, rz0, rz1, sy, sz, a, b, c, d, t, u, v;
	int i,j;

	x*=mFrequency;
	x+=mSampleSize;
	bx0=((int)x) & (mSampleSize-1);
	bx1=(bx0+1) & (mSampleSize-1);
	rx0=x-(int)x;
	rx1=rx0-1.0f;

	y*=mFrequency;
	y+=mSampleSize;
	by0=((int)y) & (mSampleSize-1);
	by1=(by0+1) & (mSampleSize-1);
	ry0=y-(int)y;
	ry1=ry0-1.0f;

	z*=mFrequency;
	z+=mSampleSize;
	bz0=((int)z) & (mSampleSize-1);
	bz1=(bz0+1) & (mSampleSize-1);
	rz0=z-(int)z;
	rz1=rz0-1.0f;

	i=p[bx0];
	j=p[bx1];

	b00=p[i + by0];
	b10=p[j + by0];
	b01=p[i + by1];
	b11=p[j + by1];

	t=scurve(rx0);
	sy=scurve(ry0);
	sz=scurve(rz0);

	qoff=3*(b00+bz0); u=rx0*g3[qoff+0]+ry0*g3[qoff+1]+rz0*g3[qoff+2];
	qoff=3*(b10+bz0); v=rx1*g3[qoff+0]+ry0*g3[qoff+1]+rz0*g3[qoff+2];
	a=Math::lerp(u,v,t);

	qoff=3*(b01+bz0); u=rx0*g3[qoff+0]+ry1*g3[qoff+1]+rz0*g3[qoff+2];
	qoff=3*(b11+bz0); v=rx1*g3[qoff+0]+ry1*g3[qoff+1]+rz0*g3[qoff+2];
	b=Math::lerp(u,v,t);

	c=Math::lerp(a,b,sy);

	qoff=3*(b00+bz1); u=rx0*g3[qoff+0]+ry0*g3[qoff+1]+rz1*g3[qoff+2];
	qoff=3*(b10+bz1); v=rx1*g3[qoff+0]+ry0*g3[qoff+1]+rz1*g3[qoff+2];
	a=Math::lerp(u,v,t);

	qoff=3*(b01+bz1); u=rx0*g3[qoff+0]+ry1*g3[qoff+1]+rz1*g3[qoff+2];
	qoff=3*(b11+bz1); v=rx1*g3[qoff+0]+ry1*g3[qoff+1]+rz1*g3[qoff+2];
	b=Math::lerp(u,v,t);

	d=Math::lerp(a,b,sy);

	return Math::lerp(c,d,sz) * mAmplitude;
}

float Noise::perlin1(float x){
	float result=0.0f;
	float scale=1.0f;

	int i;
	for(i=0;i<mOctaves;++i){
		result+=noise1(x)*scale;
		x*=2;
		scale*=0.5f;
	}

	return result;
}

float Noise::perlin2(float x,float y){
	float result=0.0f;
	float scale=1.0f;

	int i;
	for(i=0;i<mOctaves;++i){
		result+=noise2(x,y)*scale;
		x*=2;
		y*=2;
		scale*=0.5f;
	}

	return result;
}

float Noise::perlin3(float x,float y,float z){
	float result=0.0f;
	float scale=1.0f;

	int i;
	for(i=0;i<mOctaves;++i){
		result+=noise3(x,y,z)*scale;
		x*=2;
		y*=2;
		z*=2;
		scale*=0.5f;
	}

	return result;
}

float Noise::tileableNoise1(float x,float w){
	float n1=noise1(x);
	float n2=noise1(x-w);
	float f=(n1*(w-x) +
			n2*x) / w;
	return f;
}

float Noise::tileableNoise2(float x,float y,float w,float h){
	return (noise2(x,y)*(w-x)*(h-y) +
			noise2(x-w,y)*x*(h-y) +
			noise2(x,y-h)*(w-x)*y +
			noise2(x-w,y-h)*x*y) / (w*h);
}

float Noise::tileableNoise3(float x,float y,float z,float w,float h,float d){
	return (noise3(x,y,z)*(w-x)*(h-y)*(d-z) +
			noise3(x-w,y,z)*x*(h-y)*(d-z) +
			noise3(x,y-h,z)*(w-x)*y*(d-z) +
			noise3(x-w,y-h,z)*x*y*(d-z) +
			noise3(x,y,z-d)*(w-x)*(h-y)*z +
			noise3(x-w,y,z-d)*x*(h-y)*z +
			noise3(x,y-h,z-d)*(w-x)*y*z +
			noise3(x-w,y-h,z-d)*x*y*z) / (w*h*d);
}

float Noise::tileablePerlin1(float x,float w){
	float result=0.0f;
	float scale=1.0f;

	int i;
	for(i=0;i<mOctaves;++i){
		result+=tileableNoise1(x,w)*scale;
		x*=2;w*=2;
		scale*=0.5f;
	}

	return result;
}

float Noise::tileablePerlin2(float x,float y,float w,float h){
	float result=0.0f;
	float scale=1.0f;

	int i;
	for(i=0;i<mOctaves;++i){
		result+=tileableNoise2(x,y,w,h)*scale;
		x*=2;w*=2;
		y*=2;h*=2;
		scale*=0.5f;
	}

	return result;
}

float Noise::tileablePerlin3(float x,float y,float z,float w,float h,float d){
	float result=0.0f;
	float scale=1.0f;

	int i;
	for(i=0;i<mOctaves;++i){
		result+=tileableNoise3(x,y,z,w,h,d)*scale;
		x*=2;w*=2;
		y*=2;h*=2;
		z*=2;d*=2;
		scale*=0.5f;
	}

	return result;
}

void Noise::init(){
	float len,x,y,z;
	int i,j,k;
	for(i=0;i<mSampleSize;i++){
		p[i]=i;

		g1[i]=mRandom.nextFloat(-1,1);

		x=mRandom.nextFloat(-1,1);
		y=mRandom.nextFloat(-1,1);
		len=Math::sqrt(x*x+y*y);
		if(len>0){ len=1.0/len; x*=len; y*=len; }
		g2[i*2+0]=x;
		g2[i*2+1]=y;

		x=mRandom.nextFloat(-1,1);
		y=mRandom.nextFloat(-1,1);
		z=mRandom.nextFloat(-1,1);
		len=Math::sqrt(x*x+y*y+z*z);
		if(len>0){ len=1.0/len; x*=len; y*=len; z*=len;}
		g3[i*3+0]=x;
		g3[i*3+1]=y;
		g3[i*3+2]=z;
	}

	while(--i>0){
		k=p[i];
		p[i]=p[j=mRandom.nextInt(0,mSampleSize)];
		p[j]=k;
	}

	for(i=0;i<mSampleSize+2;i++){
		p[mSampleSize+i]=p[i];

		g1[mSampleSize+i]=g1[i];

		g2[2*(mSampleSize+i)+0]=g2[2*i+0];
		g2[2*(mSampleSize+i)+1]=g2[2*i+1];

		g3[3*(mSampleSize+i)+0]=g3[3*i+0];
		g3[3*(mSampleSize+i)+1]=g3[3*i+1];
		g3[3*(mSampleSize+i)+2]=g3[3*i+2];
	}
}

}
}
