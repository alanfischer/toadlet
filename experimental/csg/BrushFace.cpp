#include "BrushFace.h"

using namespace toadlet::egg;
using namespace toadlet::egg::math;
using namespace toadlet::egg::math::Math;

namespace csg{

BrushFace::BrushFace(const Vector3 &pos1,const Vector2 &tx1,const Vector3 &pos2,const Vector2 &tx2,const Vector3 &pos3,const Vector2 &tx3,int textureWidth,int textureHeight,int materialIndex){
	mPlane=Plane(pos1,pos2,pos3);
	mMaterialIndex=materialIndex;

	Vector3 WorldCoordinate1=pos1;
	Vector3 WorldCoordinate2=pos2;
	Vector3 WorldCoordinate3=pos3;

	Vector3 Normal=mPlane.normal;

	Vector2 TextureCoordinate1UV=tx1;
	Vector2 TextureCoordinate2UV=tx2;
	Vector2 TextureCoordinate3UV=tx3;

	Vector2 TextureWidthHeight(textureWidth,textureHeight);

	// Unknowns - you will be solving for these
	Vector3 Uaxis;
	Vector3 Vaxis;
	Vector2 OffsetUV;

	// Storage for solution process
	// The A, x, and b to form Ax=b, plus IA for the inverse of A
	Matrix4x4 A;
	Vector4 b;
	Vector4 x;
	Matrix4x4 IA;

	// Solve for Uaxis and OffsetU
	// Create A and b
	A.set(
		WorldCoordinate1.x, WorldCoordinate1.y, WorldCoordinate1.z,1,
		WorldCoordinate2.x, WorldCoordinate2.y, WorldCoordinate2.z,1,
		WorldCoordinate3.x, WorldCoordinate3.y, WorldCoordinate3.z,1,
		Normal.x, Normal.y, Normal.z,0
	);
	b.set(
		TextureCoordinate1UV.x*TextureWidthHeight.x,
		TextureCoordinate2UV.x*TextureWidthHeight.x,
		TextureCoordinate3UV.x*TextureWidthHeight.x,
		0
	);

	// Solve for x	
	setInverse(IA,A);
	mul(x,IA,b);

	// Reassign into results
	Uaxis.x=x.x;
	Uaxis.y=x.y;
	Uaxis.z=x.z;
	OffsetUV.x=x.w;

	// Solve for Vaxis and OffsetV
	// Matrix A stays as is, so just remake b
	b.set(
		TextureCoordinate1UV.y*TextureWidthHeight.y,
		TextureCoordinate2UV.y*TextureWidthHeight.y,
		TextureCoordinate3UV.y*TextureWidthHeight.y,
		0
	);

	// Solve for x
	// Inverse of A also the same
	mul(x,IA,b);

	// Reassign into results
	Vaxis.x=x.x;
	Vaxis.y=x.y;
	Vaxis.z=x.z;
	OffsetUV.y=x.w;

	mUAxis=Uaxis;
	mVAxis=Vaxis;
	mUVOffset=OffsetUV;
}

BrushFace::~BrushFace(){
}

}
