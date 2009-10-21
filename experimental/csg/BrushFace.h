#ifndef CSG_BRUSHFACE_H
#define CSG_BRUSHFACE_H

#include <toadlet/egg/math/Math.h>
#include <toadlet/egg/Collection.h>
#include <toadlet/egg/String.h>

namespace csg{

class Brush;

class BrushFace{
public:
	BrushFace(const toadlet::egg::math::Vector3 &pos1,const toadlet::egg::math::Vector2 &tx1,const toadlet::egg::math::Vector3 &pos2,const toadlet::egg::math::Vector2 &tx2,const toadlet::egg::math::Vector3 &pos3,const toadlet::egg::math::Vector2 &tx3,int textureWidth,int textureHeight,int materialIndex);
	~BrushFace();

	const toadlet::egg::math::Plane &getPlane() const{return mPlane;}
	int getNumPolygonPoints() const{return mPolygonPoints.size();}
	const toadlet::egg::math::Vector3 &getPolygonPoint(int i){return mPolygonPoints[i];}

	void setMaterialIndex(int index){mMaterialIndex=index;}
	int getMaterialIndex() const{return mMaterialIndex;}

	const toadlet::egg::math::Vector3 &getUAxis(){return mUAxis;}
	const toadlet::egg::math::Vector3 &getVAxis(){return mVAxis;}
	const toadlet::egg::math::Vector2 &getUVOffset(){return mUVOffset;}

protected:
	toadlet::egg::math::Plane mPlane;

	toadlet::egg::Collection<toadlet::egg::math::Vector3> mPolygonPoints; // TODO: Replace with a bsp::Polygon?

	int mMaterialIndex;
	toadlet::egg::math::Vector3 mUAxis,mVAxis;
	toadlet::egg::math::Vector2 mUVOffset;
	
	friend Brush;
};

}

#endif
