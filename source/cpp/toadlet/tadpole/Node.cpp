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
	mParentData(NULL),

	//mComponents,
	//mNodes,
	mChildrenActive(false),
	mActivateChildren(false),

	//mActions,
	mPhysics(NULL),

	mActive(false),
	mDeactivateCount(0),

	//mTransform,
	//mBound,
	//mWorldTransform,
	//mWorldBound,
	mScope(0),
	mWorldScope(0)
{
	mParent=NULL;
	mParentData=NULL;

	mChildrenActive=false;
	mActivateChildren=true;

	mActive=true;
	mDeactivateCount=0;

	mTransform=new Transform();
	mTransform->setTransformListener(this);
	mBound=new Bound();
	mWorldTransform=new Transform();
	mWorldBound=new Bound();
	mScope=-1;
	mWorldScope=-1;

	if(scene!=NULL){
		create(scene);
	}
}

Node::~Node(){
}

void Node::destroy(){
	destroyAllChildren();

	if(mScene!=NULL){
		mScene->getNodeManager()->nodeDestroyed(this);
	}

	BaseComponent::destroy();
}

void Node::create(Scene *scene){
	mScene=scene;
	mEngine=mScene->getEngine();
	mScene->getNodeManager()->nodeCreated(this);
}

void Node::destroyAllChildren(){
	ComponentCollection::iterator componentIt;
	while((componentIt=mComponents.begin())!=mComponents.end()){
		Component *component=*componentIt;
		component->destroy();
	}
	NodeCollection::iterator nodeIt;
	while((nodeIt=mNodes.begin())!=mNodes.end()){
		Node *node=*nodeIt;
		node->destroy();
	}
}

bool Node::attach(Component *component){
	Component::ptr reference=component;

	component->parentChanged(this);

	if(getActive()==false){
		activate();
	}

	return true;
}

bool Node::remove(Component *component){
	Component::ptr reference=component;

	component->parentChanged(NULL);

	if(getActive()==false){
		activate();
	}

	return true;
}

void Node::componentAttached(Component *component){
	mComponents.push_back(component);
}

void Node::componentRemoved(Component *component){
	mComponents.erase(stlit::remove(mComponents.begin(),mComponents.end(),component),mComponents.end());
}

void Node::nodeAttached(Node *node){
	mNodes.push_back(node);

	node->transformChanged(node->mTransform); // Trigger bound updating
}

void Node::nodeRemoved(Node *node){
	mNodes.erase(stlit::remove(mNodes.begin(),mNodes.end(),node),mNodes.end());
}

void Node::actionAttached(ActionComponent *action){
	mActions.push_back(action);
}

void Node::actionRemoved(ActionComponent *action){
	mActions.erase(stlit::remove(mActions.begin(),mActions.end(),action),mActions.end());
}

Component *Node::getChild(const String &name) const{
	tforeach(ComponentCollection::const_iterator,it,mComponents){
		Component *component=*it;
		if(component->getName()==name){
			return component;
		}
	}
	tforeach(NodeCollection::const_iterator,it,mNodes){
		Node *node=*it;
		if(node->getName()==name){
			return node;
		}
	}
	return NULL;
}

Component *Node::getChild(const Type<Component> *type) const{
	if(type->getFullName()==(char*)NULL){
		return NULL;
	}

	tforeach(ComponentCollection::const_iterator,it,mComponents){
		Component *component=*it;
		if(component->getType()->getFullName()==type->getFullName()){ // Compare names to avoid the issue of multiple types being built into different libraries
			return component;
		}
	}
	tforeach(NodeCollection::const_iterator,it,mNodes){
		Node *node=*it;
		if(node->getType()->getFullName()==type->getFullName()){  // Compare names to avoid the issue of multiple types being built into different libraries
			return node;
		}
	}
	return NULL;
}

Node *Node::getNode(const String &name) const{
	tforeach(NodeCollection::const_iterator,it,mNodes){
		Node *node=*it;
		if(node->getName()==name){
			return node;
		}
	}
	return NULL;
}

Node *Node::getNode(const Type<Component> *type) const{
	if(type->getFullName()==(char*)NULL){
		return NULL;
	}

	tforeach(NodeCollection::const_iterator,it,mNodes){
		Node *node=*it;
		if(node->getType()->getFullName()==type->getFullName()){ // Compare names to avoid the issue of multiple types being built into different libraries
			return node;
		}
	}
	return NULL;
}

Action *Node::getAction(const String &name) const{
	int i;
	for(i=0;i<mActions.size();++i){
		if(mActions[i]->getName()==name){
			return mActions[i]->getAction();
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

bool Node::getActionActive(const String &action) const{
	int i;
	for(i=0;i<mActions.size();++i){
		if(mActions[i]->getName()==action){
			return mActions[i]->getActive();
		}
	}
	return false;
}

void Node::spacialAttached(Spacial *spacial){
	mSpacials.push_back(spacial);

	spacial->transformChanged(spacial->getTransform());
}

void Node::spacialRemoved(Spacial *spacial){
	mSpacials.erase(stlit::remove(mSpacials.begin(),mSpacials.end(),spacial),mSpacials.end());
}

void Node::visibleAttached(Visible *visible){
	mVisibles.push_back(visible);
}

void Node::visibleRemoved(Visible *visible){
	mVisibles.erase(stlit::remove(mVisibles.begin(),mVisibles.end(),visible),mVisibles.end());
}

void Node::animatableAttached(Animatable *animatable){
	mAnimatables.push_back(animatable);
}

void Node::animatableRemoved(Animatable *animatable){
	mAnimatables.erase(stlit::remove(mAnimatables.begin(),mAnimatables.end(),animatable),mAnimatables.end());
}

void Node::lightAttached(LightComponent *light){
	mLights.push_back(light);
}

void Node::lightRemoved(LightComponent *light){
	mLights.erase(stlit::remove(mLights.begin(),mLights.end(),light),mLights.end());
}

void Node::physicsAttached(PhysicsComponent *physics){
	mPhysics=physics;
}

void Node::physicsRemoved(PhysicsComponent *physics){
	mPhysics=NULL;
}

void Node::rootChanged(Node *root){
	BaseComponent::rootChanged(root);

	tforeach(ComponentCollection::iterator,it,mComponents){
		Component *component=*it;
		component->rootChanged(root);
	}
	tforeach(NodeCollection::iterator,it,mNodes){
		Node *node=*it;
		node->rootChanged(root);
	}
}

void Node::notifyParentAttached(){
	mParent->nodeAttached(this);
}

void Node::notifyParentRemoved(){
	mParent->nodeRemoved(this);
}

void Node::setTranslate(const Vector3 &translate){
	mTransform->setTranslate(translate);
}

void Node::setTranslate(scalar x,scalar y,scalar z){
	mTransform->setTranslate(x,y,z);
}

void Node::setRotate(const Quaternion &rotate){
	mTransform->setRotate(rotate);
}

void Node::setRotate(const Matrix3x3 &rotate){
	mTransform->setRotate(rotate);
}

void Node::setRotate(const Vector3 &axis,scalar angle){
	mTransform->setRotate(axis,angle);
}

void Node::setScale(const Vector3 &scale){
	mTransform->setScale(scale);
}

void Node::setScale(scalar x,scalar y,scalar z){
	mTransform->setScale(x,y,z);
}

void Node::setMatrix4x4(const Matrix4x4 &matrix){
	mTransform->setMatrix(matrix);
}

void Node::setTransform(Transform::ptr transform){
	if(transform==NULL){
		mTransform->reset();
	}
	else{
		mTransform->set(transform);
	}
}

void Node::setBound(Bound::ptr bound){
	mBound->set(bound);

	boundChanged();
}

void Node::logicUpdate(int dt,int scope){
	TOADLET_PROFILE_AUTOSCOPE();

	if(mActivateChildren){
		tforeach(NodeCollection::iterator,it,mNodes){
			Node *node=*it;
			node->activate();
		}
		mActivateChildren=false;
	}

	mChildrenActive=false;
	tforeach(ComponentCollection::iterator,it,mComponents){
		Component *component=*it;
		component->logicUpdate(dt,scope);
		mChildrenActive|=component->getActive();
	}

	tforeach(NodeCollection::iterator,it,mNodes){
		Node *node=*it;
		node->logicUpdate(dt,scope);
		mChildrenActive|=node->getActive();
		if(node->getActive() && (node->getScope()&scope)!=0){
			node->tryDeactivate();
		}
	}
}

void Node::frameUpdate(int dt,int scope){
	TOADLET_PROFILE_AUTOSCOPE();

	tforeach(ComponentCollection::iterator,it,mComponents){
		Component *component=*it;
		component->frameUpdate(dt,scope);
	}

	tforeach(NodeCollection::iterator,it,mNodes){
		Node *node=*it;
		node->frameUpdate(dt,scope);
	}
}

bool Node::handleEvent(Event *event){
	bool result=false;
	tforeach(ComponentCollection::iterator,it,mComponents){
		Component *component=*it;
		result|=component->handleEvent(event);
	}
	tforeach(NodeCollection::iterator,it,mNodes){
		Node *node=*it;
		result|=node->handleEvent(event);
	}
	return result;
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

	tforeach(NodeCollection::iterator,it,mNodes){
		Node *node=*it;
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

void Node::transformChanged(Transform *transform){
	TOADLET_PROFILE_AUTOSCOPE();

	if(mParent==NULL){
		mWorldTransform->set(mTransform);
		mWorldScope=mScope;
	}
	else{
		mWorldTransform->setTransform(mParent->mWorldTransform,mTransform);
		mWorldScope=mParent->mWorldScope&mScope;
	}

	tforeach(NodeCollection::iterator,it,mNodes){
		Node *node=*it;
		node->transformChanged(mWorldTransform);
	}
	for(int i=0;i<mSpacials.size();++i){
		mSpacials[i]->transformChanged(mWorldTransform);
	}

	if(transform==mTransform){
		boundChanged();
	}
	else{
		calculateBound();
	}
}

void Node::boundChanged(){
	calculateBound();

	if(mParent!=NULL){
		mParent->nodeBoundChanged(this);
	}
}

void Node::nodeBoundChanged(Node *node){
	boundChanged();
}

void Node::gatherRenderables(Camera *camera,RenderableSet *set){
	TOADLET_PROFILE_AUTOSCOPE();

	set->queueNode(this);

	tforeach(NodeCollection::iterator,it,mNodes){
		Node *node=*it;
		if((camera->getScope()&node->getScope())!=0 && camera->culled(node->getWorldBound())==false){
			node->gatherRenderables(camera,set);
		}
	}
	
	for(int i=0;i<mLights.size();++i){
		if(mLights[i]->getEnabled()){
			set->queueLight(mLights[i]);
		}
	}

	for(int i=0;i<mVisibles.size();++i){
		mVisibles[i]->gatherRenderables(camera,set);
	}
}

void Node::calculateBound(){
	TOADLET_PROFILE_AUTOSCOPE();

	/// @todo: We need to rework this so it doesn't rebuild the bounds every time a child transform is changed.  For a root node of an active scene, that will get very (almost) pointlessly expensive
	{
		mWorldBound->transform(mBound,mWorldTransform);

		scalar epsilon=mScene!=NULL?mScene->getEpsilon():0;

		tforeach(NodeCollection::const_iterator,it,mNodes){
			Node *node=*it;
			mWorldBound->merge(node->getWorldBound(),epsilon);
		}

		for(int i=0;i<mSpacials.size();++i){
			Spacial *spacial=mSpacials[i];
			Bound *bound=spacial->getWorldBound();
			if(bound!=NULL){
				mWorldBound->merge(bound,epsilon);
			}
		}
	}
}

}
}
