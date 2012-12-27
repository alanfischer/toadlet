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

#include <toadlet/tadpole/StereoscopicCamera.h>

namespace toadlet{
namespace tadpole{

StereoscopicCamera::StereoscopicCamera():
	mSeparation(0),
	mCrossEyed(false)
{
	mLeftCamera=new Camera();

	mRightCamera=new Camera();

	setSeparation(Math::ONE);
	setCrossEyed(false);
}

void StereoscopicCamera::destroy(){
}

void StereoscopicCamera::setScope(int scope){
	Camera::setScope(scope);

	mLeftCamera->setScope(scope);
	mRightCamera->setScope(scope);
}

void StereoscopicCamera::setClearFlags(int clearFlags){
	Camera::setClearFlags(clearFlags);

	mLeftCamera->setClearFlags(clearFlags);
	mRightCamera->setClearFlags(clearFlags);
}

void StereoscopicCamera::setClearColor(const Vector4 &clearColor){
	Camera::setClearColor(clearColor);

	mLeftCamera->setClearColor(clearColor);
	mRightCamera->setClearColor(clearColor);
}

void StereoscopicCamera::setSkipFirstClear(bool skip){
	Camera::setSkipFirstClear(skip);

	mLeftCamera->setSkipFirstClear(skip);
	mRightCamera->setSkipFirstClear(skip);
}

void StereoscopicCamera::setDefaultState(RenderState *renderState){
	Camera::setDefaultState(renderState);

	mLeftCamera->setDefaultState(renderState);
	mRightCamera->setDefaultState(renderState);
}

void StereoscopicCamera::setRenderTarget(RenderTarget *target){
	Camera::setRenderTarget(target);

	mLeftCamera->setRenderTarget(target);
	mRightCamera->setRenderTarget(target);
}

void StereoscopicCamera::setViewport(const Viewport &viewport){
	Camera::setViewport(viewport);

	int hw=viewport.width/2;
	mLeftCamera->setViewport(Viewport(viewport.x,viewport.y,hw,viewport.height));
	mRightCamera->setViewport(Viewport(viewport.x+hw,viewport.y,hw,viewport.height));
}

void StereoscopicCamera::setSeparation(scalar separation){
	mSeparation=separation;

	updateWorldTransform();
}

void StereoscopicCamera::setCrossEyed(bool crossEyed){
	mCrossEyed=crossEyed;

	updateWorldTransform();
}

void StereoscopicCamera::render(RenderDevice *device,Scene *scene,Node *node){
	if(mProjectionType==ProjectionType_FOV){
		autoUpdateProjection(mRenderTarget!=NULL?mRenderTarget.get():device->getPrimaryRenderTarget());
	}

	if(mViewport.empty){
		RenderTarget *target=mRenderTarget!=NULL?mRenderTarget.get():device->getPrimaryRenderTarget();
		setViewport(Viewport(0,0,target->getWidth(),target->getHeight()));
		mViewport.empty=true;
	}

	mLeftCamera->render(device,scene,node);
	mRightCamera->render(device,scene,node);
}

void StereoscopicCamera::projectionUpdated(){
	Camera::projectionUpdated();

	Matrix4x4 projectionMatrix;
	projectionMatrix.set(mProjectionMatrix);
	projectionMatrix.setAt(0,0,1/Math::atan(1/projectionMatrix.at(0,0))*2);

	mLeftCamera->setProjectionMatrix(projectionMatrix);
	mRightCamera->setProjectionMatrix(projectionMatrix);
}

void StereoscopicCamera::updateWorldTransform(){
	Camera::updateWorldTransform();

	scalar separation=mCrossEyed?-mSeparation:mSeparation;

	mLeftCamera->setWorldMatrix(mWorldMatrix);
	{
		Vector3 position=mLeftCamera->getPosition();
		Math::madd(position,mRight,-separation,position);
		mLeftCamera->setPosition(position);
	}

	mRightCamera->setWorldMatrix(mWorldMatrix);
	{
		Vector3 position=mRightCamera->getPosition();
		Math::madd(position,mRight,separation,position);
		mRightCamera->setPosition(position);
	}
}

}
}
