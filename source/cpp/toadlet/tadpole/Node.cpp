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
	ComponentCollection::iterator c;
	while((c=mComponents.begin())!=mComponents.end()){
		c->destroy();
	}
	NodeCollection::iterator n;
	while((n=mNodes.begin())!=mNodes.end()){
		n->destroy();
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
	mComponents.add(component);
}

void Node::componentRemoved(Component *component){
	mComponents.remove(component);
}

void Node::nodeAttached(Node *node){
	mNodes.add(node);

	node->transformChanged(node->mTransform); // Trigger bound updating
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

Component *Node::getChild(const String &name) const{
	tforeach(ComponentCollection::const_iterator,c,mComponents){
		if(c->getName()==name){
			return (Component*)(const Component*)c;
		}
	}
	tforeach(NodeCollection::const_iterator,n,mNodes){
		if(n->getName()==name){
			return (Node*)(const Node*)n;
		}
	}
	return NULL;
}

Component *Node::getChild(const Type<Component> *type) const{
	if(type->getFullName()==(char*)NULL){
		return NULL;
	}

	tforeach(ComponentCollection::const_iterator,c,mComponents){
		if(c->getType()->getFullName()==type->getFullName()){ // Compare names to avoid the issue of multiple types being built into different libraries
			return (Component*)(const Component*)c;
		}
	}
	tforeach(NodeCollection::const_iterator,n,mNodes){
		if(n->getType()->getFullName()==type->getFullName()){  // Compare names to avoid the issue of multiple types being built into different libraries
			return (Node*)(const Node*)n;
		}
	}
	return NULL;
}

Node *Node::getNode(const String &name) const{
	tforeach(NodeCollection::const_iterator,n,mNodes){
		if(n->getName()==name){
			return (Node*)(const Node*)n;
		}
	}
	return NULL;
}

Node *Node::getNode(const Type<Component> *type) const{
	if(type->getFullName()==(char*)NULL){
		return NULL;
	}

	tforeach(NodeCollection::const_iterator,n,mNodes){
		if(n->getType()->getFullName()==type->getFullName()){ // Compare names to avoid the issue of multiple types being built into different libraries
			return (Node*)(const Node*)n;
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
	mSpacials.add(spacial);

	spacial->transformChanged(spacial->getTransform());
}

void Node::spacialRemoved(Spacial *spacial){
	mSpacials.remove(spacial);
}

void Node::visibleAttached(Visible *visible){
	mVisibles.add(visible);
}

void Node::visibleRemoved(Visible *visible){
	mVisibles.remove(visible);
}

void Node::animatableAttached(Animatable *animatable){
	mAnimatables.add(animatable);
}

void Node::animatableRemoved(Animatable *animatable){
	mAnimatables.remove(animatable);
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

void Node::rootChanged(Node *root){
	BaseComponent::rootChanged(root);

	tforeach(ComponentCollection::iterator,c,mComponents){
		c->rootChanged(root);
	}
	tforeach(NodeCollection::iterator,n,mNodes){
		n->rootChanged(root);
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
		tforeach(NodeCollection::iterator,n,mNodes){
			n->activate();
		}
		mActivateChildren=false;
	}

	mChildrenActive=false;
	tforeach(ComponentCollection::iterator,c,mComponents){
		c->logicUpdate(dt,scope);
		mChildrenActive|=c->getActive();
	}

	tforeach(NodeCollection::iterator,n,mNodes){
		n->logicUpdate(dt,scope);
		mChildrenActive|=n->getActive();
		if(n->getActive() && (n->getScope()&scope)!=0){
			n->tryDeactivate();
		}
	}
}

void Node::frameUpdate(int dt,int scope){
	TOADLET_PROFILE_AUTOSCOPE();

	tforeach(ComponentCollection::iterator,c,mComponents){
		c->frameUpdate(dt,scope);
	}

	tforeach(NodeCollection::iterator,n,mNodes){
		n->frameUpdate(dt,scope);
	}
}

bool Node::handleEvent(Event *event){
	bool result=false;
	tforeach(ComponentCollection::iterator,c,mComponents){
		result|=c->handleEvent(event);
	}
	tforeach(NodeCollection::iterator,n,mNodes){
		result|=n->handleEvent(event);
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

	tforeach(NodeCollection::iterator,n,mNodes){
		n->deactivate();
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

	tforeach(NodeCollection::iterator,n,mNodes){
		n->transformChanged(mWorldTransform);
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

	tforeach(NodeCollection::iterator,n,mNodes){
		if((camera->getScope()&n->getScope())!=0 && camera->culled(n->getWorldBound())==false){
			n->gatherRenderables(camera,set);
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

		tforeach(NodeCollection::const_iterator,n,mNodes){
			mWorldBound->merge(n->getWorldBound(),epsilon);
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
