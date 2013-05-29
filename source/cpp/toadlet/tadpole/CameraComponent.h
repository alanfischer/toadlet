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

#ifndef TOADLET_TADPOLE_CAMERACOMPONENT_H
#define TOADLET_TADPOLE_CAMERACOMPONENT_H

#include <toadlet/tadpole/BaseComponent.h>
#include <toadlet/tadpole/Camera.h>
#include <toadlet/tadpole/Spacial.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API CameraComponent:public BaseComponent,public Spacial{
public:
	TOADLET_COMPONENT(CameraComponent);

	CameraComponent(Camera::ptr camera);

	void parentChanged(Node *node);

	void setCamera(Camera::ptr camera){mCamera=camera;}
	Camera *getCamera(){return mCamera;}

	void setAutoProjectionFov(scalar fov,bool yheight,scalar nearDist,scalar farDist){mCamera->setAutoProjectionFov(fov,yheight,nearDist,farDist);}

	void setProjectionOrtho(scalar leftDist,scalar rightDist,scalar bottomDist,scalar topDist,scalar nearDist,scalar farDist){mCamera->setProjectionOrtho(leftDist,rightDist,bottomDist,topDist,nearDist,farDist);}\
	void setProjectionFrustum(scalar leftDist,scalar rightDist,scalar bottomDist,scalar topDist,scalar nearDist,scalar farDist){mCamera->setProjectionFrustum(leftDist,rightDist,bottomDist,topDist,nearDist,farDist);}\

	void setClearColor(const Vector4 &clearColor){mCamera->setClearColor(clearColor);}
	const Vector4 &getClearColor() const{return mCamera->getClearColor();}

	void setLookAt(const Vector3 &eye,const Vector3 &point,const Vector3 &up){mCamera->setLookAt(eye,point,up);setTransformFromCamera();}
	void setLookDir(const Vector3 &eye,const Vector3 &dir,const Vector3 &up){mCamera->setLookDir(eye,dir,up);setTransformFromCamera();}

	// Spacial
	Transform *getTransform() const{return NULL;}
	Bound *getBound() const{return NULL;}
	Transform *getWorldTransform() const{return NULL;}
	Bound *getWorldBound() const{return NULL;}
	void transformChanged(Transform *transform);

protected:
	void setTransformFromCamera();
	void setCameraFromTransform();

	Camera::ptr mCamera;
};

}
}

#endif

