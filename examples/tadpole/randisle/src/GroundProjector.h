#ifndef GROUNDPROJECTOR_H
#define GROUNDPROJECTOR_H

#include <toadlet/toadlet.h>

class GroundProjector:public BaseComponent{
public:
	GroundProjector(Node *node,scalar distance,scalar offset):BaseComponent(){
		mNode=node;
		mDistance=distance;
		mOffset=offset;
	}

	void frameUpdate(int dt,int scope){
		Segment segment;
		segment.origin.set(mNode->getWorldTranslate());
		Math::mul(segment.direction,Math::NEG_Z_UNIT_VECTOR3,mDistance);
		tadpole::Collision result;
		mNode->getScene()->traceSegment(result,segment,-1,mNode);

		if(result.time<Math::ONE){
			Math::madd(result.point,result.normal,mOffset,result.point);
			mParent->getParent()->getWorldTransform().inverseTransform(result.point);
			mParent->setTranslate(result.point);
	
			Vector3 right,forward,up;
			Math::mul(forward,mNode->getWorldRotate(),Math::Y_UNIT_VECTOR3);
			up.set(result.normal);
			Math::cross(right,forward,up);
			Math::normalize(right);
			Math::cross(forward,up,right);
			Quaternion rotate,invRotate;
			Math::setQuaternionFromAxes(rotate,right,forward,up);
			Math::invert(invRotate,mParent->getParent()->getWorldTransform().getRotate());
			Math::postMul(rotate,invRotate);
			mParent->setRotate(rotate);
		}
		mParent->setScale(Math::ONE-result.time);
	}

protected:
	Node::ptr mNode;
	scalar mDistance;
	scalar mOffset;
};

#endif
