#ifndef SMOOTHFOLLOWER_H
#define SMOOTHFOLLOWER_H

#include <toadlet/toadlet.h>

class SmoothFollower:public NodeListener{
public:
	TOADLET_SHARED_POINTERS(SmoothFollower);

	SmoothFollower(int bufferSize){
		mPositions.resize(bufferSize);
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

	void nodeDestroyed(Node *node){}

	void transformUpdated(Node *node,int tu){}

	void logicUpdated(Node *node,int dt){
		if(mTarget!=NULL){
			Vector3 position;
			Math::mul(position,mTarget->getWorldRotate(),mOffset);
/*Quaternion rot;Vector3 x,y,z;
Math::setAxesFromQuaternion(mTarget->getWorldRotate(),x,y,z);
z.z*=6;
Math::normalize(z);
Math::cross(x,y,z);
Math::normalize(x);
Math::cross(y,z,x);
Math::setQuaternionFromAxes(rot,x,y,z);
Math::mul(position,rot,mOffset);
*/
			Math::add(position,mTarget->getWorldTranslate());
			tadpole::Collision result;
			Segment segment;
			segment.origin.set(mTarget->getWorldTranslate());
			Math::sub(segment.direction,position,segment.origin);
			mTarget->getScene()->traceSegment(result,segment,-1,mTarget);
			result.time-=0.1;
			Math::madd(result.point,segment.direction,result.time,segment.origin);
			logicUpdated(result.point,dt);
		}
	}

	void logicUpdated(const toadlet::tadpole::Vector3 &position,int dt){
		mPositions.insert(mPositions.begin(),position);
		mPositions.removeAt(mPositions.size()-1);
	}

	void frameUpdated(toadlet::tadpole::node::Node *node,int dt){
		if(dt==0){
			return;
		}

		float fraction=node->getScene()->getLogicFraction();
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

		node->setTranslate(position);

		if(mTarget!=NULL){
			node->updateWorldTransform();

			Quaternion rotate,invrot;
			Matrix4x4 transform;
			Math::setMatrix4x4FromLookAt(transform,node->getWorldTranslate(),mTarget->getWorldTranslate()+mTargetOffset,toadlet::tadpole::Math::Z_UNIT_VECTOR3,true);
			Math::setQuaternionFromMatrix4x4(rotate,transform);
			if(node->getParent()!=NULL){
				Math::invert(invrot,node->getParent()->getWorldRotate());
				Math::preMul(rotate,invrot);
			}
			node->setRotate(rotate);
		}

		node->updateWorldTransform();
	}
	
	Node::ptr mTarget;
	Vector3 mOffset;
	Vector3 mTargetOffset;
	egg::Collection<Vector3> mPositions;
};

#endif
