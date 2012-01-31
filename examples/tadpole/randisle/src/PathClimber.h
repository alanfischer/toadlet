#ifndef PATHCLIMBER_H
#define PATHCLIMBER_H

#include "RandIsle.h"

class PathClimber:public HopEntity{
public:
	TOADLET_NODE(PathClimber,HopEntity);

	PathClimber();

	Node *create(Scene *scene);

	void logicUpdate(int dt,int scope);

	void mount(ParentNode *system,PathSystem::Path *path,const Vector3 &point);
	void dismount();

	ParentNode *getMounted(){return mMounted;}
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

	// Player class
	MeshNode::ptr getPlayerMeshNode(){return mPlayerMeshNode;}
	MeshNode::ptr getShadowMeshNode(){return mShadowMeshNode;}
	
	int getAcornCount(){return mAcornCount;}

	int getGroundTime(){return mGroundTime;}

	void setDanger(scalar danger){mDanger=danger;}
	scalar getDanger(){return mDanger;}

	void setHealth(int health){mHealth=health;}
	int getHealth(){return mHealth;}

protected:
	PathClimberListener *mPathClimberListener;
	ParentNode::ptr mMounted;
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

	// Ideally this would be in a Player class
	MeshNode::ptr mPlayerMeshNode;
	MeshNode::ptr mShadowMeshNode;
	int mAcornCount;
	int mGroundTime;
	scalar mDanger;
	int mHealth;
	BoundingVolumeSensor::ptr mSensor;
};

#endif
