#ifndef SMOOTHFOLLOWER_H
#define SMOOTHFOLLOWER_H

#include <toadlet/toadlet.h>

class SmoothFollower:public BaseComponent{
public:
	TOADLET_OBJECT(SmoothFollower);

	SmoothFollower(int bufferSize):BaseComponent(){
		mPositions.resize(bufferSize);
	}

	void setTarget(Node *target){
		mTarget=target;

		// Attach target to last as a temporary hack until update layers are added
		Node::ptr parent=mParent;
		Node::ptr parentParent=parent->getParent();
		parentParent->remove(parent);
		parentParent->attach(parent);

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
			Segment segment;
			segment.origin.set(mTarget->getTranslate());
			Math::mul(segment.direction,mTarget->getWorldRotate(),mOffset);
			PhysicsCollision result;
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

		float fraction=mTarget->getScene()->getLogicFraction();
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

		mParent->setTranslate(position);

		if(mTarget!=NULL){
			Quaternion rotate,invrot;
			Matrix4x4 transform;
			Math::setMatrix4x4FromLookAt(transform,mParent->getTranslate(),mTarget->getWorldTranslate()+mTargetOffset,toadlet::tadpole::Math::Z_UNIT_VECTOR3,true);
			Math::setQuaternionFromMatrix4x4(rotate,transform);
			if(mParent->getParent()!=NULL){
				Math::invert(invrot,mParent->getParent()->getWorldRotate());
				Math::preMul(rotate,invrot);
			}
			mParent->setRotate(rotate);
		}
	}

	Node::ptr mTarget;
	Vector3 mOffset;
	Vector3 mTargetOffset;
	egg::Collection<Vector3> mPositions;
};

#endif
