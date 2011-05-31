#ifndef GROUNDPROJECTOR_H
#define GROUNDPROJECTOR_H

#include <toadlet/toadlet.h>

class GroundProjector:public NodeListener{
public:
	TOADLET_SHARED_POINTERS(GroundProjector);

	GroundProjector(Node::ptr target,scalar distance,scalar offset){
		mTarget=target;
		mDistance=distance;
		mOffset=offset;
	}

	void nodeDestroyed(Node *node){}

	void transformUpdated(Node *node,int tu){}

	void logicUpdated(Node *node,int dt){}
	
	void frameUpdated(Node *node,int dt){
		Segment segment;
		segment.origin.set(mTarget->getWorldTranslate());
		Math::mul(segment.direction,Math::NEG_Z_UNIT_VECTOR3,mDistance);
		tadpole::Collision result;
		mTarget->getScene()->traceSegment(result,segment,-1,mTarget);

		if(result.time<Math::ONE){
			Math::madd(result.point,result.normal,mOffset,result.point);
			node->getParent()->getWorldTransform().inverseTransform(result.point);
			node->setTranslate(result.point);
		
			Vector3 right,forward,up;
			Math::mul(forward,mTarget->getWorldRotate(),Math::Y_UNIT_VECTOR3);
			up.set(result.normal);
			Math::cross(right,forward,up);
			Math::normalize(right);
			Math::cross(forward,up,right);
			Quaternion rotate,invRotate;
			Math::setQuaternionFromAxes(rotate,right,forward,up);
			Math::invert(invRotate,node->getParent()->getWorldTransform().getRotate());
			Math::postMul(rotate,invRotate);
			node->setRotate(rotate);
		}
		node->setScale(Math::ONE-result.time);
	}
	
protected:
	Node::ptr mTarget;
	scalar mDistance;
	scalar mOffset;
};

#endif
