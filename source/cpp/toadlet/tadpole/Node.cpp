/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright 2009, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer, Andrew Fischer
 *
 * This file is part of The Toadlet Engine.
 *
 * The Toadlet Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * The Toadlet Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with The Toadlet Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 ********** Copyright header - do not remove **********/

#include <toadlet/egg/Error.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/ActionComponent.h>
#include <toadlet/tadpole/LightComponent.h>
#include <toadlet/tadpole/PhysicsComponent.h>
#include <toadlet/tadpole/Visible.h>
#include <toadlet/tadpole/Node.h>

namespace toadlet{
namespace tadpole{

Node::Node(Scene *scene):BaseComponent(),
	mUniqueHandle(0),

	//mParent,
	mParentData(NULL),

	mChildrenActive(false),
	mActivateChildren(false),

	//mActions,
	mPhysics(NULL),

	mActive(false),
	mDeactivateCount(0),
	mTransformUpdatedFrame(0),

	//mTransform,
	//mBound,
	//mWorldTransform,
	//mWorldBound,
	mScope(0)
{
	mParent=NULL;
	mParentData=NULL;

	mChildrenActive=false;
	mActivateChildren=true;

	mActive=true;
	mDeactivateCount=0;
	mTransformUpdatedFrame=-1;

	mTransform.reset();
	mBound=new Bound();
	mWorldTransform.reset();
	mWorldBound=new Bound();
	mComponentBound=new Bound();
	mComponentWorldBound=new Bound();
	mScope=-1;

	if(scene!=NULL){
		create(scene);
	}
}

Node::~Node(){
}

void Node::destroy(){
	destroyAllChildren();
	mEngine->nodeDestroyed(this);

	BaseComponent::destroy();
}

void Node::create(Scene *scene){
	mScene=scene;
	mEngine=mScene->getEngine();
	mEngine->nodeCreated(this);
}

void Node::destroyAllChildren(){
	while(mComponents.size()>0){
		mComponents[0]->destroy();
	}
}

bool Node::attach(Component *component){
	Component::ptr reference=component; // To make sure that the object is not released early

	mComponents.add(component);

	component->parentChanged(this);

	if(mRoot!=NULL){
		component->rootChanged(mRoot);
	}

	if(getActive()==false){
		activate();
	}

	return true;
}

bool Node::remove(Component *component){
	Component::ptr reference=component; // To make sure that the object is not released early

	component->parentChanged(NULL);

	mComponents.remove(component);

	if(mRoot!=NULL){
		component->rootChanged(NULL);
	}

	if(getActive()==false){
		activate();
	}

	return true;
}

void Node::nodeAttached(Node *node){
	mNodes.add(node);
}

void Node::nodeRemoved(Node *node){
	mNodes.remove(node);
}

void Node::actionAttached(ActionComponent *action){
	mActions.add(action);
}

void Node::actionRemoved(ActionComponent *action){
	mActions.remove(action);
}

Component *Node::getChild(const String &name){
	int i;
	for(i=0;i<mComponents.size();++i){
		if(mComponents[i]->getName()==name){
			return mComponents[i];
		}
	}
	return NULL;
}

ActionComponent *Node::getAction(const String &name){
	int i;
	for(i=0;i<mActions.size();++i){
		if(mActions[i]->getName()==name){
			return mActions[i];
		}
	}
	return NULL;
}

void Node::startAction(const String &action){
	int i;
	for(i=0;i<mActions.size();++i){
		if(mActions[i]->getName()==action){
			mActions[i]->start();
		}
	}
}

void Node::stopAction(const String &action){
	int i;
	for(i=0;i<mActions.size();++i){
		if(mActions[i]->getName()==action){
			mActions[i]->stop();
		}
	}
}

void Node::stopAllActions(){
	int i;
	for(i=0;i<mActions.size();++i){
		if(mActions[i]->getActive()){
			mActions[i]->stop();
		}
	}
}

bool Node::getActionActive(const String &action){
	int i;
	for(i=0;i<mActions.size();++i){
		if(mActions[i]->getName()==action){
			return mActions[i]->getActive();
		}
	}
	return false;
}

void Node::visibleAttached(Visible *visible){
	mVisibles.add(visible);
}

void Node::visibleRemoved(Visible *visible){
	mVisibles.remove(visible);
}

void Node::lightAttached(LightComponent *light){
	mLights.add(light);
}

void Node::lightRemoved(LightComponent *light){
	mLights.remove(light);
}

void Node::physicsAttached(PhysicsComponent *physics){
	mPhysics=physics;
}

void Node::physicsRemoved(PhysicsComponent *physics){
	mPhysics=NULL;
}

void Node::parentChanged(Node *node){
	Node *parent=mParent;
	
	BaseComponent::parentChanged(node);

	if(parent!=NULL){
		parent->nodeRemoved(this);
	}
	if(mParent!=NULL){
		mParent->nodeAttached(this);
	}
}

void Node::rootChanged(Node *root){
	BaseComponent::rootChanged(root);

	int i;
	for(i=0;i<mComponents.size();++i){
		Component *component=mComponents[i];
		component->rootChanged(root);
	}
}

void Node::setTranslate(const Vector3 &translate){
	mTransform.setTranslate(translate);
	spacialUpdated();
}

void Node::setTranslate(scalar x,scalar y,scalar z){
	mTransform.setTranslate(x,y,z);
	spacialUpdated();
}

void Node::setRotate(const Quaternion &rotate){
	mTransform.setRotate(rotate);
	spacialUpdated();
}

void Node::setRotate(const Matrix3x3 &rotate){
	mTransform.setRotate(rotate);
	spacialUpdated();
}

void Node::setRotate(const Vector3 &axis,scalar angle){
	mTransform.setRotate(axis,angle);
	spacialUpdated();
}

void Node::setScale(const Vector3 &scale){
	mTransform.setScale(scale);
	spacialUpdated();
}

void Node::setScale(scalar x,scalar y,scalar z){
	mTransform.setScale(x,y,z);
	spacialUpdated();
}

void Node::setMatrix4x4(const Matrix4x4 &matrix){
	mTransform.setMatrix(matrix);
	spacialUpdated();
}

void Node::setTransform(const Transform &transform){
	mTransform.set(transform);
	spacialUpdated();
}

void Node::setBound(Bound *bound){
	mBound->set(bound);
	spacialUpdated();
}

void Node::logicUpdate(int dt,int scope){
	int i;

	updateWorldTransform();

	if(mActivateChildren){
		for(i=0;i<mNodes.size();++i){
			Node *node=mNodes[i];
			node->activate();
		}
		mActivateChildren=false;
	}

	mChildrenActive=false;
	for(i=0;i<mComponents.size();++i){
		Component *component=mComponents[i];
		component->logicUpdate(dt,scope);
		mChildrenActive|=component->getActive();
	}

	for(i=0;i<mNodes.size();++i){
		Node *node=mNodes[i];
		if(node->getActive() && (node->getScope()&scope)!=0){
			node->tryDeactivate();
		}
		else{
			mergeWorldBound(node,false);
		}
	}
}

void Node::frameUpdate(int dt,int scope){
	int i;

	updateWorldTransform();

	for(i=0;i<mComponents.size();++i){
		Component *component=mComponents[i];
		component->frameUpdate(dt,scope);
	}

	for(i=0;i<mNodes.size();++i){
		Node *node=mNodes[i];
		if(node->getActive() && (node->getScope()&scope)!=0){
		}
		else{
			mergeWorldBound(node,false);
		}
	}
}

bool Node::handleEvent(Event *event){
	int i=0;
	bool result=false;
	for(i=0;i<mComponents.size();++i){
		Component *component=mComponents[i];
		result|=component->handleEvent(event);
	}
	return result;
}

void Node::mergeWorldBound(Node *child,bool justAttached){
	mWorldBound->merge(child->getWorldBound(),mScene->getEpsilon());
}

void Node::setStayActive(bool active){
	if(active){
		activate();
		mDeactivateCount=-1;
	}
	else{
		mDeactivateCount=0;
	}
}

void Node::activate(){
	mActivateChildren=true;

	if(mDeactivateCount>0){
		mDeactivateCount=0;
	}
	if(mActive==false){
		mActive=true;
		if(mParent!=NULL && mParent->getActive()==false){
			mParent->activate();
		}
	}
}

void Node::deactivate(){
	if(mActive==false){
		return;
	}

	mActive=false;
	mDeactivateCount=0;

	int i;
	for(i=0;i<mNodes.size();++i){
		Node *node=mNodes[i];
		node->deactivate();
	}
}

void Node::tryDeactivate(){
	if(mDeactivateCount>=0){
		if(mChildrenActive==false){
			mDeactivateCount++;
			if(mDeactivateCount>4){
				deactivate();
			}
		}
		else{
			mDeactivateCount=0;
		}
	}
}

bool Node::getTransformUpdated(){return mScene->getFrame()==mTransformUpdatedFrame;}

void Node::updateWorldTransform(){
	if(mParent==NULL){
		mWorldTransform.set(mTransform);
	}
	else if(mTransformUpdatedFrame==-1){
		mWorldTransform.set(mParent->mWorldTransform);
	}
	else{
		mWorldTransform.setTransform(mParent->mWorldTransform,mTransform);
	}
	mWorldBound->transform(mBound,mWorldTransform);

	mComponentBound->reset();
	int i;
	for(i=0;i<mComponents.size();++i){
		Component *component=mComponents[i];
		Bound *bound=component->getBound();
		if(bound!=NULL){
			mComponentBound->merge(bound,mScene->getEpsilon());
		}
	}
	mComponentWorldBound->transform(mComponentBound,mWorldTransform);
	mWorldBound->merge(mComponentWorldBound,mScene->getEpsilon());

	if(mParent!=NULL){
		mParent->mergeWorldBound(this,false);
	}
}

void Node::spacialUpdated(){
	if(mScene!=NULL){
		mTransformUpdatedFrame=mScene->getFrame();
	}

	if(mPhysics!=NULL){
		mPhysics->transformChanged();
	}

	activate();
}

void Node::gatherRenderables(Camera *camera,RenderableSet *set){
	int i;

	set->queueNode(this);

	for(i=0;i<mNodes.size();++i){
		Node *node=mNodes[i];
		if((camera->getScope()&node->getScope())!=0 && camera->culled(node->getWorldBound())==false){
			node->gatherRenderables(camera,set);
		}
	}
	
	for(i=0;i<mLights.size();++i){
		if(mLights[i]->getEnabled()){
			set->queueLight(mLights[i]);
		}
	}

	for(i=0;i<mVisibles.size();++i){
		mVisibles[i]->gatherRenderables(camera,set);
	}
}

}
}
