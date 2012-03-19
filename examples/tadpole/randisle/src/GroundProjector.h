#ifndef GROUNDPROJECTOR_H
#define GROUNDPROJECTOR_H

#include <toadlet/toadlet.h>

class GroundProjector:public BaseComponent{
public:
	GroundProjector(HopEntity *node,MeshNode *mesh,scalar distance,scalar offset):BaseComponent(){
		mNode=node;
		mMesh=mesh;
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
			mMesh->getParent()->getWorldTransform().inverseTransform(result.point);
			mMesh->setTranslate(result.point);
		
			Vector3 right,forward,up;
			Math::mul(forward,mNode->getWorldRotate(),Math::Y_UNIT_VECTOR3);
			up.set(result.normal);
			Math::cross(right,forward,up);
			Math::normalize(right);
			Math::cross(forward,up,right);
			Quaternion rotate,invRotate;
			Math::setQuaternionFromAxes(rotate,right,forward,up);
			Math::invert(invRotate,mMesh->getParent()->getWorldTransform().getRotate());
			Math::postMul(rotate,invRotate);
			mMesh->setRotate(rotate);
		}
		mMesh->setScale(Math::ONE-result.time);
	}

protected:
	HopEntity::ptr mNode;
	MeshNode::ptr mMesh;
	scalar mDistance;
	scalar mOffset;
};

#endif
