#ifndef SMDCONVERTER_H
#define SMDCONVERTER_H

#include <toadlet/egg/io/Stream.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/mesh/Mesh.h>

class Vertex;
using toadlet::scalar;

class SMDConverter{
public:
	enum{
		Block_NONE,
		Block_NODES,
		Block_SKELETON,
		Block_TRIANGLES,
	};

	SMDConverter();

	void load(toadlet::tadpole::Engine *engine,toadlet::egg::io::Stream *in,const toadlet::egg::String &fileName);

	toadlet::tadpole::mesh::Mesh::ptr getMesh(){return mMesh;}

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

protected:
	toadlet::egg::String readLine(toadlet::egg::io::Stream *in);
	void setQuaternionFromXYZ(toadlet::tadpole::Quaternion &r,scalar x,scalar y,scalar z);
	bool vertsEqual(const Vertex &v1,const Vertex &v2);

	toadlet::tadpole::Engine *mEngine;

	toadlet::tadpole::mesh::Mesh::ptr mMesh;
	toadlet::tadpole::mesh::Skeleton::ptr mSkeleton;
	toadlet::tadpole::mesh::Sequence::ptr mSequence;

	scalar mPositionEpsilon;
	scalar mNormalEpsilon;
	scalar mTexCoordEpsilon;
	scalar mFPS;
	bool mRemoveSkeleton;
	bool mInvertFaces;
};

#endif
