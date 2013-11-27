#ifndef TREECOMPONENT_H
#define TREECOMPONENT_H

#include <toadlet/toadlet.h>
#include "TreeSystem.h"

class TreeComponent:public BaseComponent,public BranchSystem::BranchListener,public Spacial,public DetailTraceable{
public:
	TOADLET_COMPONENT(TreeComponent);

	TreeComponent(Scene *scene,int seed,Material::ptr branchMaterial,Material::ptr leafMaterial);
	void destroy();

	void grow();

	TreeSystem::TreeBranch *getClosestBranch(Vector3 &closestPoint,const Vector3 &point);

	void parentChanged(Node *node);

	BranchSystem::Branch::ptr branchCreated(BranchSystem::Branch *parent){return mSystem->branchCreated(parent);}
	void branchBuild(BranchSystem::Branch *branch);
	void branchLeaf(BranchSystem::Branch *branch,const Vector3 &offset,scalar scale){mSystem->branchLeaf(branch,offset,scale);}
	void branchDestroyed(BranchSystem::Branch *branch){mSystem->branchDestroyed(branch);}

	// Spacial
	Transform *getTransform() const{return NULL;}
	Bound *getBound() const{return mBound;}
	Transform *getWorldTransform() const{return NULL;}
	Bound *getWorldBound() const{return mWorldBound;}
	void transformChanged(Transform *transform);

	// DetailTraceable
	Bound *getTraceableBound() const{return getBound();}
	void traceSegment(PhysicsCollision &result,const Vector3 &position,const Segment &segment,const Vector3 &size);

protected:
	Scene *mScene;

	Bound::ptr mBound;
	Bound::ptr mWorldBound;

	TreeSystem::ptr mSystem;
};

#endif