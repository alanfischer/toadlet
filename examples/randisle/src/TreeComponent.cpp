#include "TreeComponent.h"
#include "Resources.h"

TreeComponent::TreeComponent(Scene *scene,int seed,Material::ptr branchMaterial,Material::ptr leafMaterial):
	mScene(NULL)
{
	mScene=scene;

	mSystem=new TreeSystem(mScene->getEngine(),seed,branchMaterial,leafMaterial,this);

	mBound=new Bound();
	mWorldBound=new Bound();
}

void TreeComponent::destroy(){
	mSystem->destroy();

	BaseComponent::destroy();
}

void TreeComponent::grow(){
	mSystem->grow();

	mBound->set(mSystem->getBound());

	MeshComponent::ptr mesh=new MeshComponent(mScene->getEngine());
	mesh->setMesh(mSystem->getMesh());
	mParent->attach(mesh);
}

Path *TreeComponent::getClosestPath(Vector3 &closestPoint,const Vector3 &point){
	Sphere bound(point,50);
	Math::sub(bound.origin,point,mParent->getWorldTranslate());
	bound.radius=50;
	Path *path=mSystem->getClosestPath(closestPoint,bound);
	Math::add(closestPoint,mParent->getWorldTranslate());
	return path;
}

void TreeComponent::parentChanged(Node *node){
	if(mParent!=NULL){
		mParent->spacialRemoved(this);
	}

	BaseComponent::parentChanged(node);

	if(mParent!=NULL){
		mParent->spacialAttached(this);
	}
}

void TreeComponent::branchBuild(BranchSystem::Branch *branch){
	TreeSystem::TreeBranch *treeBranch=(TreeSystem::TreeBranch*)branch;

	if(treeBranch->points.size()>0){
		Segment segment;
		segment.origin=branch->position + mParent->getWorldTranslate();
		segment.direction=branch->position - treeBranch->points[treeBranch->points.size()-1];
		PhysicsCollision result;
		mScene->traceSegment(result,segment,-1,NULL);
		if(result.time<Math::ONE){
			branch->life=0;
		}
	}

	mSystem->branchBuild(branch);
}

void TreeComponent::transformChanged(Transform *transform){
	if(mParent==NULL){
		return;
	}

	mWorldBound->transform(getBound(),mParent->getWorldTransform());
}

void TreeComponent::traceSegment(PhysicsCollision &result,const Vector3 &position,const Segment &segment,const Vector3 &size){
	mSystem->traceSegment(result,position,segment,size);
}
