#ifndef GROUNDPROJECTOR_H
#define GROUNDPROJECTOR_H

#include <toadlet/toadlet.h>

class GroundProjector:public BaseComponent{
public:
	GroundProjector(Node::ptr target,scalar distance,scalar offset):BaseComponent(){
		mTarget=target;
		mDistance=distance;
		mOffset=offset;
	}

	void destroy(){}

	bool parentChanged(Node *node){
		mNode=node;
		return true;
	}

	void logicUpdate(int dt,int scope){}
	
	void frameUpdate(int dt,int scope){
		Segment segment;
		segment.origin.set(mTarget->getWorldTranslate());
		Math::mul(segment.direction,Math::NEG_Z_UNIT_VECTOR3,mDistance);
		tadpole::Collision result;
		mTarget->getScene()->traceSegment(result,segment,-1,mTarget);

		if(result.time<Math::ONE){
			Math::madd(result.point,result.normal,mOffset,result.point);
			mNode->getParent()->getWorldTransform().inverseTransform(result.point);
			mNode->setTranslate(result.point);
		
			Vector3 right,forward,up;
			Math::mul(forward,mTarget->getWorldRotate(),Math::Y_UNIT_VECTOR3);
			up.set(result.normal);
			Math::cross(right,forward,up);
			Math::normalize(right);
			Math::cross(forward,up,right);
			Quaternion rotate,invRotate;
			Math::setQuaternionFromAxes(rotate,right,forward,up);
			Math::invert(invRotate,mNode->getParent()->getWorldTransform().getRotate());
			Math::postMul(rotate,invRotate);
			mNode->setRotate(rotate);
		}
		mNode->setScale(Math::ONE-result.time);
	}
	
protected:
	Node::ptr mNode,mTarget;
	scalar mDistance;
	scalar mOffset;
};

#endif
