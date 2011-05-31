#ifndef BRANCHSYSTEM_H
#define BRANCHSYSTEM_H

#include <toadlet/egg.h>
#include <toadlet/tadpole.h>

class BranchSystem{
public:
	TOADLET_SHARED_POINTERS(BranchSystem);

	class Branch{
	public:
		TOADLET_SHARED_POINTERS(Branch);
	
		Branch():scale(0),life(0),length(0),buildTime(0),barkTime(0),speed(0){}

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
	
	BranchSystem(int seed):
		mBranchListener(NULL)
	{
		mRandom=Random::ptr(new Random());

		initSeed=seed;
		initLife=5;
		initVelocity.set(0,0,40);
		initVelocityNoise.set(2,2,2);//4,4,4);
		initVisibilityAmount=0.75;
		branchLifeAmount=0.55f;
		branchVelNoise.set(30,30,15);
		branchScale=0.75f;
		minBranchLife=0.0f;
		maxBranchLife=12;
		branchAdjust=0.075;
		minLeaves=4;maxLeaves=8;//10;
		leafOffsetNoise.set(7,7,7);
		leafScale=4;
	}

	void setBranchListener(BranchListener *listener){
		mBranchListener=listener;
	}
	
	void start(){
		mRandom->setSeed(initSeed);

		Branch::ptr branch;
		if(mBranchListener!=NULL){
			branch=mBranchListener->branchCreated(NULL);
		}
		else{
			branch=Branch::ptr(new Branch());
		}

		branch->life=initLife;
		branch->speed=Math::length(initVelocity);
		branch->velocity.set(initVelocity);
		branch->velocityNoise.set(initVelocityNoise);
		branch->scale=branch->life*branchScale;
		branch->visibilityAmount=initVisibilityAmount;
		mBranches.add(branch);

		if(mBranchListener!=NULL){
			mBranchListener->branchBuild(branch);
		}
	}
	
	bool update(int dt){
		if(mBranches.size()>0){
			Branch::ptr branch=mBranches[0];
			updateBranch(branch,dt);
			return true;
		}
		else{
			return false;
		}
	}
	
	void updateBranch(Branch::ptr branch,int dt){
		scalar fdt=Math::fromMilli(dt);

		branch->life-=fdt;
		if(branch->life<0){
			branch->life=0;
		}

		branch->velocity.x+=mRandom->nextFloat(-branch->velocityNoise.x,branch->velocityNoise.x);
		branch->velocity.y+=mRandom->nextFloat(-branch->velocityNoise.y,branch->velocityNoise.y);
		branch->velocity.z+=mRandom->nextFloat(-branch->velocityNoise.z,branch->velocityNoise.z);
		Math::normalizeCarefully(branch->velocity,0);
		Math::mul(branch->velocity,branch->speed);
		branch->length+=Math::mul(branch->speed,fdt);

		Math::madd(branch->position,branch->velocity,fdt,branch->position);

		branch->scale=branch->life*branchScale;

		branch->barkTime+=fdt;

		branch->buildTime+=fdt;
		if(branch->buildTime>=Math::ONE-branch->visibilityAmount){
			branch->buildTime=0;
		}

		bool branchChild=
			(branch->life>minBranchLife) && (branch->life<maxBranchLife) &&
			(mRandom->nextFloat(0,1) < (Math::ONE-branch->life/maxBranchLife)*branchAdjust);

		if(branchChild){
			if(mBranchListener!=NULL){
				mBranchListener->branchBuild(branch);
			}

			Branch::ptr childBranch;
			if(mBranchListener!=NULL){
				childBranch=mBranchListener->branchCreated(branch);
			}
			else{
				childBranch=Branch::ptr(new Branch());
			}

			childBranch->life=branch->life*branchLifeAmount;

			childBranch->position.set(branch->position);

			childBranch->speed=branch->speed;

			childBranch->velocity.x=branch->velocity.x+mRandom->nextFloat(-branchVelNoise.x,branchVelNoise.x);
			childBranch->velocity.y=branch->velocity.y+mRandom->nextFloat(-branchVelNoise.y,branchVelNoise.y);
			childBranch->velocity.z=branch->velocity.z+mRandom->nextFloat(-branchVelNoise.z,branchVelNoise.z);
			Math::normalizeCarefully(childBranch->velocity,0);
			Math::mul(childBranch->velocity,childBranch->speed);

			childBranch->velocityNoise.set(branch->velocityNoise * 2.0);

			childBranch->visibilityAmount=branch->visibilityAmount * 1.05;

			// I'm not sure why the childBranch has to be forced to build next frame in order to not skip a section
			// and end up with a doublely long branch split from the parent.
			childBranch->buildTime=Math::ONE;

			childBranch->barkTime=branch->barkTime;

			mBranches.add(childBranch);

			if(mBranchListener!=NULL){
				mBranchListener->branchBuild(childBranch);
			}
		}
		else if(branch->life<=0){
			if(mBranchListener!=NULL){
				mBranchListener->branchBuild(branch);
			}

			int numLeaves=mRandom->nextInt(minLeaves,maxLeaves);
			int i;
			for(i=0;i<numLeaves;++i){
				Vector3 offset(mRandom->nextFloat(-leafOffsetNoise.x,leafOffsetNoise.x),mRandom->nextFloat(-leafOffsetNoise.y,leafOffsetNoise.y),mRandom->nextFloat(-leafOffsetNoise.z,leafOffsetNoise.z));
				if(mBranchListener!=NULL){
					mBranchListener->branchLeaf(branch,offset,leafScale);
				}
			}
			
			if(mBranchListener!=NULL){
				mBranchListener->branchDestroyed(branch);
			}
			
			mBranches.remove(branch);
		}
		else if(branch->buildTime==0){
			if(mBranchListener!=NULL){
				mBranchListener->branchBuild(branch);
			}
		}
	}

	inline Random::ptr getRandom(){return mRandom;}

	static void getBranchVectors(Branch *branch,Vector3 &branchUp,Vector3 &branchNormal,Vector3 &branchBinormal){
		branchUp.set(branch->velocity);
		Math::normalizeCarefully(branchUp,0);
		Math::cross(branchNormal,branchUp,Math::X_UNIT_VECTOR3);
		if(Math::normalizeCarefully(branchNormal,0)==false){
			branchNormal.set(Math::Y_UNIT_VECTOR3);
		}
		Math::cross(branchBinormal,branchUp,branchNormal);
		Math::normalizeCarefully(branchBinormal,0);
	}

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

#endif
