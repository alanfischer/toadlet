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

#include <toadlet/tadpole/CameraComponent.h>
#include <toadlet/tadpole/Node.h>

namespace toadlet{
namespace tadpole{

CameraComponent::CameraComponent(Camera::ptr camera){
	mCamera=camera;
}

void CameraComponent::parentChanged(Node *node){
	if(mParent!=NULL){
		mParent->spacialRemoved(this);
	}

	BaseComponent::parentChanged(node);

	if(mParent!=NULL){
		mParent->spacialAttached(this);
	}
}

void CameraComponent::transformChanged(Transform *transform){
	setCameraFromTransform();
}

void CameraComponent::setCameraFromTransform(){
	if(mParent!=NULL){
		Matrix4x4 matrix;
		mParent->getWorldTransform()->getMatrix(matrix);
		mCamera->setWorldMatrix(matrix);
	}
}

void CameraComponent::setTransformFromCamera(){
	if(mParent!=NULL){
		mParent->setMatrix4x4(mCamera->getWorldMatrix());
	}
}

}
}
