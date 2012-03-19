#ifndef PATHCLIMBER_H
#define PATHCLIMBER_H

#include <toadlet/toadlet.h>
#include <toadlet/tadpole/plugins/hop/HopEntity.h>
#include "PathSystem.h"
#include "PathClimberListener.h"

class PathClimber:public BaseComponent{
public:
	TOADLET_OBJECT(PathClimber);

	PathClimber();

	bool parentChanged(Node *node);

	void mount(Node *system,PathSystem::Path *path,const Vector3 &point);
	void dismount();

	Node *getMounted(){return mMounted;}
	PathSystem::Path *getPath(){return mPath;}
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

	int findPassedNeighbor(PathSystem::Path *path,int direction,scalar time);
	void findRotation(Quaternion &r,const Vector3 &tangent,const Vector3 &normal);
	bool passedJunction(int direction,scalar oldTime,scalar newTime,scalar junctionTime);

	void logicUpdate(int dt,int scope);

protected:
	HopScene *mScene;
	HopEntity *mNode;
	PathClimberListener *mPathClimberListener;
	Node::ptr mMounted;
	PathSystem::Path *mPreviousPath;
	PathSystem::Path *mPath;
	float mPathTime;
	int mPathDirection;
	int mPassedNeighbor;
	int mNoClimbTime;
	float mSpeed;
	Quaternion mIdealRotation;
	Collection<Vector3> mPreviousNormals;
	int mNormalIndex;
};

#endif
