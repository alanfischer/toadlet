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

using namespace toadlet::egg;

namespace toadlet{
namespace tadpole{
namespace node{

TOADLET_NODE_IMPLEMENT(CameraAlignedNode,Categories::TOADLET_TADPOLE_NODE+".CameraAlignedNode");

CameraAlignedNode::CameraAlignedNode():super(),
	TOADLET_GIB_IMPLEMENT()

	mCameraAligned(false),
	mPerspective(false)
{
	setCameraAligned(true);
	setPerspective(true);
}

void CameraAlignedNode::frameUpdate(int dt){
	super::frameUpdate(dt);

	if(mLocalBound.radius<0 || mPerspective==false) mWorldBound.radius=-Math::ONE;
}

void CameraAlignedNode::queueRenderables(CameraNode *camera,RenderQueue *queue){
	super::queueRenderables(camera,queue);

	if(mCameraAligned){
		Matrix3x3 rotate;
		if(camera->getAlignmentCalculationsUseOrigin()){
			Vector3 nodeWorldTranslate; Math::setTranslateFromMatrix4x4(nodeWorldTranslate,mWorldRenderTransform);
			Vector3 cameraWorldTranslate; Math::setTranslateFromMatrix4x4(cameraWorldTranslate,camera->getWorldRenderTransform());
			Matrix4x4 lookAtCamera; Math::setMatrix4x4FromLookAt(lookAtCamera,cameraWorldTranslate,nodeWorldTranslate,Math::Z_UNIT_VECTOR3,false);
			Math::setMatrix3x3FromMatrix4x4Transpose(rotate,lookAtCamera);
		}
		else{
			Math::setMatrix3x3FromMatrix4x4Transpose(rotate,camera->getViewTransform());
		}
		Math::setMatrix4x4FromRotateScale(mWorldTransform,rotate,mWorldScale);
	}

	if(!mPerspective){
		Matrix4x4 scale;
		Vector4 point;

		point.set(mWorldTransform.at(0,3),mWorldTransform.at(1,3),mWorldTransform.at(2,3),Math::ONE);
		Math::mul(point,camera->getViewTransform());
		Math::mul(point,camera->getProjectionTransform());
		scale.setAt(0,0,point.w);
		scale.setAt(1,1,point.w);
		scale.setAt(2,2,point.w);

		Math::postMul(mWorldTransform,scale);
	}
}

}
}
}
