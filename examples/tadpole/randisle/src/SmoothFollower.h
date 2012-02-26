#ifndef SMOOTHFOLLOWER_H
#define SMOOTHFOLLOWER_H

#include <toadlet/toadlet.h>

class SmoothFollower:public BaseComponent{
public:
	TOADLET_IPTR(SmoothFollower);

	SmoothFollower(int bufferSize):BaseComponent(){
		mPositions.resize(bufferSize);
	}

	void destroy(){}

	bool parentChanged(Node *node){
		mNode=node;
		if(mNode!=NULL){
			mScene=mNode->getScene();
		}
		return true;
	}

	void setTarget(Node *parent,Node *target){
		parent->setDependsUpon(target);
		mTarget=target;

		Vector3 position;
		Math::mul(position,mTarget->getWorldRotate(),mOffset);
		Math::add(position,mTarget->getWorldTranslate());
		int i;
		for(i=0;i<mPositions.size();++i){
			mPositions[i]=position;
		}
	}

	void setOffset(const Vector3 &offset){mOffset.set(offset);}
	void setTargetOffset(const Vector3 &offset){mTargetOffset.set(offset);}

	void logicUpdate(int dt,int scope){
		if(mTarget!=NULL){
			Vector3 position;
			Math::mul(position,mTarget->getWorldRotate(),mOffset);
			Math::add(position,mTarget->getWorldTranslate());
			tadpole::Collision result;
			Segment segment;
			segment.origin.set(mTarget->getWorldTranslate());
			Math::sub(segment.direction,position,segment.origin);
			mTarget->getScene()->traceSegment(result,segment,-1,mTarget);
			result.time-=0.1;
			Math::madd(result.point,segment.direction,result.time,segment.origin);
			logicUpdate(result.point,dt);
		}
	}

	void logicUpdate(const toadlet::tadpole::Vector3 &position,int dt){
		mPositions.insert(mPositions.begin(),position);
		mPositions.removeAt(mPositions.size()-1);
	}

	void frameUpdate(int dt,int scope){
		if(dt==0){
			return;
		}

		float fraction=mScene->getLogicFraction();
		Vector3 position;
		int numPositions=mPositions.size();
		int i;
		for(i=0;i<numPositions;++i){
			float amount=2.0*((float)i+fraction)/(float)numPositions;
			if(amount>1.0){
				amount=1.0-(amount-1.0);
			}
			position+=mPositions[i]*amount;
		}
		position/=(float)numPositions/2;

		mNode->setTranslate(position);

		if(mTarget!=NULL){
			mNode->updateWorldTransform();

			Quaternion rotate,invrot;
			Matrix4x4 transform;
			Math::setMatrix4x4FromLookAt(transform,mNode->getWorldTranslate(),mTarget->getWorldTranslate()+mTargetOffset,toadlet::tadpole::Math::Z_UNIT_VECTOR3,true);
			Math::setQuaternionFromMatrix4x4(rotate,transform);
			if(mNode->getParent()!=NULL){
				Math::invert(invrot,mNode->getParent()->getWorldRotate());
				Math::preMul(rotate,invrot);
			}
			mNode->setRotate(rotate);
		}

		mNode->updateWorldTransform();
	}

	Scene *mScene;
	Node::ptr mNode,mTarget;
	Vector3 mOffset;
	Vector3 mTargetOffset;
	egg::Collection<Vector3> mPositions;
};

#endif
