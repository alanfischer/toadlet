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
#include <toadlet/tadpole/node/CameraAlignedNode.h>
#include <toadlet/tadpole/node/CameraNode.h>
#include <toadlet/tadpole/node/ParentNode.h>
#include <toadlet/tadpole/Engine.h>

namespace toadlet{
namespace tadpole{
namespace node{

TOADLET_NODE_IMPLEMENT(CameraAlignedNode,Categories::TOADLET_TADPOLE_NODE+".CameraAlignedNode");

CameraAlignedNode::CameraAlignedNode():super(),
	mCameraAligned(false),
	mPerspective(false)
{
	setCameraAligned(true);
	setPerspective(true);
}

Node *CameraAlignedNode::set(Node *node){
	super::set(node);

	CameraAlignedNode *alignedNode=(CameraAlignedNode*)node;
	setCameraAligned(alignedNode->getCameraAligned());
	setPerspective(alignedNode->getPerspective());

	return this;
}

void CameraAlignedNode::frameUpdate(int dt,int scope){
	super::frameUpdate(dt,scope);

	if(mPerspective==false) mWorldBound.setInfinite();
}

void CameraAlignedNode::gatherRenderables(CameraNode *camera,RenderableSet *set){
	super::gatherRenderables(camera,set);

	if(mCameraAligned){
		Quaternion alignRotate;
		if(camera->getAlignmentCalculationsUseOrigin()){
			Matrix4x4 lookAtCamera; Math::setMatrix4x4FromLookAt(lookAtCamera,camera->getWorldTranslate(),getWorldTranslate(),Math::Z_UNIT_VECTOR3,false);
			Math::setQuaternionFromMatrix4x4(alignRotate,lookAtCamera);
		}
		else{
			Math::setQuaternionFromMatrix4x4(alignRotate,camera->getViewMatrix());
		}
		Math::invert(alignRotate);
		Math::postMul(alignRotate,getRotate());
		mWorldTransform.setRotate(alignRotate);
	}

	if(!mPerspective){
		Vector4 point;
		Vector3 scale;

		point.set(getWorldTranslate(),Math::ONE);
		Math::mul(point,camera->getViewMatrix());
		Math::mul(point,camera->getProjectionMatrix());

		Math::mul(scale,getWorldScale(),point.w);
		mWorldTransform.setScale(scale);
	}
}

}
}
}
