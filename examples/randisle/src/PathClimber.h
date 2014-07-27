#ifndef PATHCLIMBER_H
#define PATHCLIMBER_H

#include <toadlet/toadlet.h>
#include "Path.h"
#include "PathClimberListener.h"

namespace randisle{

using namespace toadlet;

class PathClimber:public BaseComponent{
public:
	TOADLET_COMPONENT(PathClimber);

	PathClimber();

	void mount(Node *system,PathVertex *path,const Vector3 &point);
	void dismount();

	Node *getMounted(){return mMounted;}
	PathVertex *getPath(){return mPath;}
	int getPathDirection(){return mPathDirection;}
	void setPathDirection(int direction);
	float getPathTime(){return mPathTime;}

	void setPathClimberListener(PathClimberListener *listener){mPathClimberListener=listener;}
	PathClimberListener *getPathClimberListener(){return mPathClimberListener;}

	int getNoClimbTime(){return mNoClimbTime;}
	
	float getSpeed(){return mSpeed;}
	void setSpeed(float speed){mSpeed=speed;}

	void setIdealRotation(const Quaternion &idealRotation);
	const Quaternion &getIdealRotation(){return mIdealRotation;}

	PathEdge *findPassedEdge(PathVertex *vertex,int direction,scalar time);
	void findRotation(Quaternion &r,const Vector3 &tangent,const Vector3 &normal);
	bool passedJunction(int direction,scalar oldTime,scalar newTime,scalar junctionTime);

	void logicUpdate(int dt,int scope);

protected:
	PathClimberListener *mPathClimberListener;
	Node::ptr mMounted;
	PathVertex *mPreviousPath;
	PathVertex *mPath;
	float mPathTime;
	int mPathDirection;
	PathEdge *mPassedEdge;
	int mNoClimbTime;
	float mSpeed;
	Quaternion mIdealRotation;
	Collection<Vector3> mPreviousNormals;
	int mNormalIndex;
};

}

#endif
