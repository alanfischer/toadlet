/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine 
 *
 * Copyright (C) 2005, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer
 * 
 * All source code for the Toadlet Engine, including 
 * this file, is the sole property of Lightning Toads 
 * Productions, LLC. It has been developed on our own 
 * time, with our own tools, on our own hardware. None
 * of this code may be observed, used or changed 
 * without our explicit permission. 
 *
 *  - Lightning Toads Productions, LLC
 *
 ********** Copyright header - do not remove **********/

#ifndef BACCONVERTER_H
#define BACCONVERTER_H

#include <toadlet/egg/Collection.h>
#include <toadlet/egg/Map.h> // For Pair
#include <toadlet/egg/math/Vector2.h>
#include <toadlet/egg/math/Vector3.h>
#include <toadlet/egg/math/Vector4.h>
#include <toadlet/egg/io/Stream.h>
#include <toadlet/tadpole/mesh/Mesh.h>
#include <toadlet/tadpole/node/MeshNode.h>

class BACVertex{
public:
	BACVertex():normal(0,0,1){}

	toadlet::egg::math::Vector3 position;
	toadlet::egg::math::Vector3 normal;
	int id;
	int bone;
};

class BACTexCoord{
public:
	toadlet::egg::math::Vector2 texture;
	int id;
};

class BACTriangle{
public:
	toadlet::egg::math::Vector3 normal();

	BACVertex *vertex[3];
	BACTexCoord *texture[3];
	int subMeshId;
	bool dead;
	bool textured;
};

class BACQuad{
public:
	float computeSP();

	BACVertex *vertex[4];
	BACTexCoord *texture[4];
	int subMeshId;
	int tris[2];
	bool textured;
};

class BACBone{
public:
	toadlet::egg::String name;
	bool hasChild;
	bool hasBrother;
	toadlet::egg::math::Vector3 translate;
	toadlet::egg::math::Vector3 rotate;
	toadlet::egg::math::Vector3 handle;
	toadlet::egg::Collection<int> vertexIndexes;
};

class BACAnimationBone{
public:
	toadlet::egg::String name;
	toadlet::egg::Collection<toadlet::egg::Pair<int,float> > translatex;
	toadlet::egg::Collection<toadlet::egg::Pair<int,float> > translatey;
	toadlet::egg::Collection<toadlet::egg::Pair<int,float> > translatez;
	toadlet::egg::Collection<toadlet::egg::Pair<int,float> > scalex;
	toadlet::egg::Collection<toadlet::egg::Pair<int,float> > scaley;
	toadlet::egg::Collection<toadlet::egg::Pair<int,float> > scalez;
	toadlet::egg::Collection<toadlet::egg::Pair<int,float> > rotatex;
	toadlet::egg::Collection<toadlet::egg::Pair<int,float> > rotatey;
	toadlet::egg::Collection<toadlet::egg::Pair<int,float> > rotatez;
	toadlet::egg::Collection<toadlet::egg::Pair<int,float> > roll;
};

class BACMaterial{
public:
	toadlet::egg::String name;
	int colorIndex;
	int textureIndex;
	bool doubleSided;
	bool lighting;
};

class BACSharedEdge{
public:
	BACQuad *quad;
	BACTriangle *tri;
	int edge[2];
};

class BACConverter{
public:
	BACConverter(toadlet::tadpole::Engine *engine);
	virtual ~BACConverter();

	bool convertMesh(toadlet::tadpole::mesh::Mesh::ptr mesh,toadlet::egg::io::Stream *stream,bool submeshes=true,bool quads=true,float adjust=0,int version=6);
	bool convertAnimation(toadlet::tadpole::mesh::Mesh::ptr mesh,toadlet::tadpole::mesh::Sequence *animation,toadlet::egg::io::Stream *stream,int version=4);

	void setPositionEpsilon(float epsilon){mPositionEpsilon=epsilon;}
	float getPositionEpsilon() const{return mPositionEpsilon;}

	void setNormalEpsilon(float epsilon){mNormalEpsilon=epsilon;}
	float getNormalEpsilon() const{return mNormalEpsilon;}

	void setTexCoordEpsilon(float epsilon){mTexCoordEpsilon=epsilon;}
	float getTexCoordEpsilon() const{return mTexCoordEpsilon;}

	void setRotationEpsilon(float epsilon){mRotationEpsilon=epsilon;}
	float getRotationEpsilon() const{return mRotationEpsilon;}

protected:
	enum{
		BAC_ATTR_VID=0x00000001,
		BAC_ATTR_NORMAL=0x00000002,
	};

	enum{
		POLY_ATTR_2S=0x00000001,
		POLY_ATTR_TP=0x00000002,
		POLY_ATTR_ALPHA=0x00000ff0,
		POLY_ATTR_H=0x00001000,
		POLY_ATTR_KH=0x00002000,
		POLY_ATTR_LIGHT=0x00008000,
		POLY_ATTR_SPEC=0x00ff0000,
		POLY_ATTR_SHIN=0xff000000,
	};

	enum{
		TRA_MAGIC=0x12345678,
	};

	bool extractMeshData(toadlet::tadpole::mesh::Mesh::ptr mesh,bool submeshes=true);
	void buildBones(toadlet::tadpole::mesh::Mesh *mesh,toadlet::tadpole::node::MeshNodeSkeleton *nodeSkeleton,int bone);
	void constructQuads();
	void rewindTriangles();
	void adjustVertexes(float amount);
	void writeOutModelVersion6(toadlet::egg::io::Stream *stream);
	void writeOutModelVersion5(toadlet::egg::io::Stream *stream);

	void extractAnimationData(toadlet::tadpole::mesh::Mesh *mesh,toadlet::tadpole::mesh::Sequence *animation);
	void writeOutAnimationVersion4(toadlet::egg::io::Stream *stream);
	void writeOutAnimationVersion3(toadlet::egg::io::Stream *stream);
	void clean();
	
	float mPositionEpsilon;
	float mNormalEpsilon;
	float mTexCoordEpsilon;
	float mScaleEpsilon;
	float mRotationEpsilon;
	int mTotalFrame;
	bool mHasNormal;
	bool mHasBone;

	toadlet::tadpole::Engine *mEngine;
	toadlet::egg::String mName;
	toadlet::egg::Collection<BACVertex *> mVertices;
	toadlet::egg::Collection<BACTexCoord *> mTexCoords;
	toadlet::egg::Collection<BACTriangle *> mTriangles;
	toadlet::egg::Collection<BACQuad *> mQuads;
	toadlet::egg::Collection<toadlet::peeper::Color> mColors;
	toadlet::egg::Collection<toadlet::egg::math::Vector2> mTextures;
	toadlet::egg::Collection<BACMaterial *> mMaterials;
	toadlet::egg::Collection<BACBone *> mBones;
	toadlet::egg::Collection<BACAnimationBone *> mAnimationBones;
};

#endif

