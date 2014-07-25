#ifndef BRANCHSYSTEM_H
#define BRANCHSYSTEM_H

#include <toadlet/toadlet.h>

namespace randisle{

using namespace toadlet;

class BranchSystem:public Object{
public:
	TOADLET_OBJECT(BranchSystem);

	class Branch:public Object{
	public:
		TOADLET_OBJECT(Branch);
	
		Branch():life(0),length(0),speed(0),scale(0),visibilityAmount(0),buildTime(0),barkTime(0){}

		virtual ~Branch(){}

		scalar life;
		scalar length;
		Vector3 position;
		scalar speed;
		Vector3 velocity;
		Vector3 velocityNoise;
		scalar scale;
		scalar visibilityAmount;
		scalar buildTime;
		scalar barkTime;
	};

	class BranchListener{
	public:
		virtual Branch::ptr branchCreated(Branch *parent)=0;
		virtual void branchBuild(Branch *branch)=0;
		virtual void branchLeaf(Branch *branch,const Vector3 &offset,scalar scale)=0;
		virtual void branchDestroyed(Branch *branch)=0;		
	};
	
	BranchSystem(int seed);

	inline Random::ptr getRandom(){return mRandom;}
	
	void setBranchListener(BranchListener *listener){mBranchListener=listener;}
	
	void start();
	
	bool update(int dt);
	
	void updateBranch(Branch *branch,int dt);

	static void getBranchVectors(Branch *branch,Vector3 &branchUp,Vector3 &branchNormal,Vector3 &branchBinormal);

	int initSeed;
	scalar initLife;
	Vector3 initVelocity;
	Vector3 initVelocityNoise;
	scalar initVisibilityAmount;
	scalar branchLifeAmount;
	Vector3 branchVelNoise;
	scalar branchSpeed;
	scalar branchScale;
	scalar minBranchLife;
	scalar maxBranchLife;
	scalar branchAdjust;
	int minLeaves,maxLeaves;
	Vector3 leafOffsetNoise;
	scalar leafScale;

protected:
	Random::ptr mRandom;
	BranchListener *mBranchListener;
	Collection<Branch::ptr> mBranches;
};

}

#endif
