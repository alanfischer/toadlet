#ifndef SMDCONVERTER_H
#define SMDCONVERTER_H

#include <toadlet/toadlet.h>

class Vertex;

class SMDConverter{
public:
	enum{
		Block_NONE,
		Block_NODES,
		Block_SKELETON,
		Block_TRIANGLES,
	};

	SMDConverter();

	void load(Engine *engine,Stream *in,const String &fileName);

	Mesh::ptr getMesh(){return mMesh;}

	void setPositionEpsilon(scalar epsilon){mPositionEpsilon=epsilon;}
	scalar getPositionEpsilon() const{return mPositionEpsilon;}

	void setNormalEpsilon(scalar epsilon){mNormalEpsilon=epsilon;}
	scalar getNormalEpsilon() const{return mNormalEpsilon;}

	void setTexCoordEpsilon(scalar epsilon){mTexCoordEpsilon=epsilon;}
	scalar getTexCoordEpsilon() const{return mTexCoordEpsilon;}

	void setFPS(scalar fps){mFPS=fps;}
	scalar getFPS() const{return mFPS;}

	void setRemoveSkeleton(bool removeSkeleton){mRemoveSkeleton=removeSkeleton;}
	bool getRemoveSkeleton() const{return mRemoveSkeleton;}

	void setInvertFaces(bool invertFaces){mInvertFaces=invertFaces;}
	bool getInvertFaces() const{return mInvertFaces;}

	void setScale(scalar scale){mScale=scale;}
	scalar getScale() const{return mScale;}

protected:
	String readLine(Stream *in);
	void setQuaternionFromXYZ(Quaternion &r,scalar x,scalar y,scalar z);
	bool vertsEqual(const Vertex &v1,const Vertex &v2);

	Engine *mEngine;

	Mesh::ptr mMesh;
	Skeleton::ptr mSkeleton;
	Sequence::ptr mSequence;

	scalar mPositionEpsilon;
	scalar mNormalEpsilon;
	scalar mTexCoordEpsilon;
	scalar mFPS;
	bool mRemoveSkeleton;
	bool mInvertFaces;
	scalar mScale;
};

#endif
