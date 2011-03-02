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

#include <toadlet/toadlet.h>

class BACVertex{
public:
	BACVertex():normal(0,0,1){}

	Vector3 position;
	Vector3 normal;
	int id;
	int bone;
};

class BACTexCoord{
public:
	Vector2 texture;
	int id;
};

class BACTriangle{
public:
	Vector3 normal();

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
	String name;
	bool hasChild;
	bool hasBrother;
	Vector3 translate;
	Vector3 rotate;
	Vector3 handle;
	Collection<int> vertexIndexes;
};

class BACAnimationBone{
public:
	String name;
	Collection<Pair<int,float> > translatex;
	Collection<Pair<int,float> > translatey;
	Collection<Pair<int,float> > translatez;
	Collection<Pair<int,float> > scalex;
	Collection<Pair<int,float> > scaley;
	Collection<Pair<int,float> > scalez;
	Collection<Pair<int,float> > rotatex;
	Collection<Pair<int,float> > rotatey;
	Collection<Pair<int,float> > rotatez;
	Collection<Pair<int,float> > roll;
};

class BACMaterial{
public:
	String name;
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
	BACConverter(Engine *engine);
	virtual ~BACConverter();

	bool convertMesh(Mesh::ptr mesh,Stream *stream,bool submeshes=true,bool quads=true,float adjust=0,int version=6);
	bool convertAnimation(Mesh::ptr mesh,TransformSequence *animation,Stream *stream,int version=4);

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

	bool extractMeshData(Mesh::ptr mesh,bool submeshes=true);
	void buildBones(Mesh *mesh,MeshNodeSkeleton *nodeSkeleton,int bone);
	void constructQuads();
	void rewindTriangles();
	void adjustVertexes(float amount);
	void writeOutModelVersion6(Stream *stream);
	void writeOutModelVersion5(Stream *stream);

	void extractAnimationData(Mesh *mesh,TransformSequence *animation);
	void writeOutAnimationVersion4(Stream *stream);
	void writeOutAnimationVersion3(Stream *stream);
	void clean();
	
	float mPositionEpsilon;
	float mNormalEpsilon;
	float mTexCoordEpsilon;
	float mScaleEpsilon;
	float mRotationEpsilon;
	int mTotalFrame;
	bool mHasNormal;
	bool mHasBone;

	Engine *mEngine;
	String mName;
	Collection<BACVertex *> mVertices;
	Collection<BACTexCoord *> mTexCoords;
	Collection<BACTriangle *> mTriangles;
	Collection<BACQuad *> mQuads;
	Collection<Color> mColors;
	Collection<Vector2> mTextures;
	Collection<BACMaterial *> mMaterials;
	Collection<BACBone *> mBones;
	Collection<BACAnimationBone *> mAnimationBones;
};

#endif

