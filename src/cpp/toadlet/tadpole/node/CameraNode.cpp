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
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/node/CameraNode.h>
#include <toadlet/tadpole/node/ParentNode.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{
namespace node{

TOADLET_NODE_IMPLEMENT(CameraNode,Categories::TOADLET_TADPOLE_NODE+".CameraNode");

CameraNode::CameraNode():super(),
	mProjectionType(ProjectionType_FOVX),
	mFov(0),mAspect(0),
	mLeftDist(0),mRightDist(0),
	mBottomDist(0),mTopDist(0),
	mNearDist(0),mFarDist(0),
	//mProjectionTransform
	mViewportSet(false),
	//mViewport,
	mClearFlags(0),
	//mClearColor,
	mSkipFirstClear(false),
	mAlignmentCalculationsUseOrigin(false),
	//mMidNode,
	//mTarget,

	//mWorldTranslate,
	//mViewTransform,

	mFPSLastTime(0),
	mFPSFrameCount(0),
	mFPS(0)
{}

Node *CameraNode::create(Scene *scene){
	super::create(scene);

	setProjectionFovX(Math::HALF_PI,Math::fromInt(1),Math::fromInt(1),Math::fromInt(1000));
	mViewportSet=false;
	mViewport.reset();
	mClearFlags=Renderer::ClearFlag_COLOR|Renderer::ClearFlag_DEPTH;
	mClearColor.reset();
	mSkipFirstClear=false;
	mMidNode=NULL;

	mFPSLastTime=0;
	mFPSFrameCount=0;
	mFPS=0;

	return this;
}

void CameraNode::setProjectionFovX(scalar fovx,scalar aspect,scalar nearDist,scalar farDist){
	mProjectionType=ProjectionType_FOVX;
	mFov=fovx;mAspect=aspect;
	mLeftDist=0;mRightDist=0;
	mBottomDist=0;mTopDist=0;
	mNearDist=nearDist;mFarDist=farDist;

	Math::setMatrix4x4FromPerspectiveX(mProjectionTransform,fovx,aspect,nearDist,farDist);

	projectionUpdated();
}

void CameraNode::setProjectionFovY(scalar fovy,scalar aspect,scalar nearDist,scalar farDist){
	mProjectionType=ProjectionType_FOVY;
	mFov=fovy;mAspect=aspect;
	mLeftDist=0;mRightDist=0;
	mBottomDist=0;mTopDist=0;
	mNearDist=nearDist;mFarDist=farDist;

	Math::setMatrix4x4FromPerspectiveY(mProjectionTransform,fovy,aspect,nearDist,farDist);

	projectionUpdated();
}

void CameraNode::setProjectionOrtho(scalar leftDist,scalar rightDist,scalar bottomDist,scalar topDist,scalar nearDist,scalar farDist){
	mProjectionType=ProjectionType_ORTHO;
	mFov=0;mAspect=0;
	mLeftDist=leftDist;mRightDist=rightDist;
	mBottomDist=bottomDist;mTopDist=topDist;
	mNearDist=nearDist;mFarDist=farDist;

	Math::setMatrix4x4FromOrtho(mProjectionTransform,leftDist,rightDist,bottomDist,topDist,nearDist,farDist);

	projectionUpdated();
}

void CameraNode::setProjectionFrustum(scalar leftDist,scalar rightDist,scalar bottomDist,scalar topDist,scalar nearDist,scalar farDist){
	mProjectionType=ProjectionType_FRUSTUM;
	mFov=0;mAspect=0;
	mLeftDist=leftDist;mRightDist=rightDist;
	mBottomDist=bottomDist;mTopDist=topDist;
	mNearDist=nearDist;mFarDist=farDist;

	Math::setMatrix4x4FromFrustum(mProjectionTransform,leftDist,rightDist,bottomDist,topDist,nearDist,farDist);

	projectionUpdated();
}

void CameraNode::setProjectionTransform(const Matrix4x4 &transform){
	mProjectionType=ProjectionType_MATRIX;
	mFov=0;mAspect=0;
	mLeftDist=0;mRightDist=0;
	mBottomDist=0;mTopDist=0;
	mNearDist=0;mFarDist=0;

	mProjectionTransform.set(transform);

	projectionUpdated();
}

void CameraNode::setProjectionRotation(scalar rotate){
	Matrix4x4 projection=cache_setProjectionRotation_projection.reset();

	scalar x=mViewport.x+mViewport.width/2;
	scalar y=mViewport.y+mViewport.height/2;

	Math::setMatrix4x4FromTranslate(projection,x,y,0);
	Math::preMul(projection,mProjectionTransform);

	projection.reset();
	Math::setMatrix4x4FromZ(projection,rotate);
	Math::postMul(mProjectionTransform,projection);

	projection.reset();
	Math::setMatrix4x4FromTranslate(projection,-x,-y,0);
	Math::postMul(mProjectionTransform,projection);

	projectionUpdated();
}

void CameraNode::setNearAndFarDist(scalar nearDist,scalar farDist){
	mNearDist=nearDist;
	mFarDist=farDist;

	switch(mProjectionType){
		case(ProjectionType_FOVX):
			setProjectionFovX(mFov,mAspect,mNearDist,mFarDist);
		break;
		case(ProjectionType_FOVY):
			setProjectionFovY(mFov,mAspect,mNearDist,mFarDist);
		break;
		case(ProjectionType_ORTHO):
			setProjectionOrtho(mLeftDist,mRightDist,mBottomDist,mTopDist,mNearDist,mFarDist);
		break;
		case(ProjectionType_FRUSTUM):
			setProjectionFrustum(mLeftDist,mRightDist,mBottomDist,mTopDist,mNearDist,mFarDist);
		break;
		default:
			Error::unimplemented("projection type does not support adjusting near and far");
		break;
	}
}

void CameraNode::setLookAt(const Vector3 &eye,const Vector3 &point,const Vector3 &up){
	Matrix4x4 &transform=cache_setProjectionRotation_projection;
	Math::setMatrix4x4FromLookAt(transform,eye,point,up,true);

	Math::setTranslateFromMatrix4x4(mTranslate,transform);
	Math::setQuaternionFromMatrix4x4(mRotate,transform);
	transformUpdated();
}

void CameraNode::setWorldLookAt(const Vector3 &eye,const Vector3 &point,const Vector3 &up){
	Matrix4x4 &transform=cache_setProjectionRotation_projection;
	Math::setMatrix4x4FromLookAt(transform,eye,point,up,true);

	Math::setTranslateFromMatrix4x4(mWorldTranslate,transform);
	Math::setQuaternionFromMatrix4x4(mWorldRotate,transform);
	if(mParent!=NULL){
		Quaternion invrot;
		Math::invert(invrot,mParent->getWorldRotate());
		Math::sub(mWorldTranslate,mParent->getWorldTranslate());
		Math::div(mWorldTranslate,mParent->getWorldScale());
		Math::mul(mTranslate,invrot,mWorldTranslate);
		Math::mul(mRotate,invrot,mWorldRotate);
	}
	transformUpdated();
}

void CameraNode::setLookDir(const Vector3 &eye,const Vector3 &dir,const Vector3 &up){
	Matrix4x4 &transform=cache_setProjectionRotation_projection;
	Math::setMatrix4x4FromLookDir(transform,eye,dir,up,true);

	Math::setTranslateFromMatrix4x4(mTranslate,transform);
	Math::setQuaternionFromMatrix4x4(mRotate,transform);
	transformUpdated();
}

void CameraNode::setWorldLookDir(const Vector3 &eye,const Vector3 &dir,const Vector3 &up){
	Matrix4x4 &transform=cache_setProjectionRotation_projection;
	Math::setMatrix4x4FromLookDir(transform,eye,dir,up,true);

	Math::setTranslateFromMatrix4x4(mWorldTranslate,transform);
	Math::setQuaternionFromMatrix4x4(mWorldRotate,transform);
	if(mParent!=NULL){
		Quaternion invrot;
		Math::invert(invrot,mParent->getWorldRotate());
		Math::sub(mWorldTranslate,mParent->getWorldTranslate());
		Math::div(mWorldTranslate,mParent->getWorldScale());
		Math::mul(mTranslate,invrot,mWorldTranslate);
		Math::mul(mRotate,invrot,mWorldRotate);
	}
	transformUpdated();
}

void CameraNode::setTarget(Node *node){
	mTarget=node;
	setDependsUpon(mTarget);
}

void CameraNode::setViewport(const Viewport &viewport){
	mViewportSet=true;
	mViewport.set(viewport);
}

void CameraNode::setViewport(int x,int y,int width,int height){
	mViewportSet=true;
	mViewport.set(x,y,width,height);
}

ParentNode::ptr CameraNode::getMidNode(){
	if(mMidNode==NULL){
		mMidNode=mEngine->createNodeType(ParentNode::type(),getScene());
		attach(mMidNode);
		updateMidNode();
	}
	return mMidNode;
}

void CameraNode::projectionUpdated(){
	updateViewTransform();
	updateMidNode();
}

void CameraNode::updateWorldTransform(){
	if(mTarget!=NULL){
		setLookAt(getTranslate(),mTarget->getWorldTranslate(),Math::Z_UNIT_VECTOR3);
	}

	super::updateWorldTransform();

	updateViewTransform();
}

bool CameraNode::culled(const Sphere &sphere) const{
	if(sphere.radius<0) return false;
	scalar distance=0;
	int i;
	for(i=0;i<6;++i){
		distance=Math::dot(mClipPlanes[i].normal,sphere.origin)+mClipPlanes[i].distance;
		if(distance<-sphere.radius){
			return true;
		}
	}
	return false;
}

bool CameraNode::culled(const AABox &box){
	int i;
	for(i=0;i<6;i++){
		if(box.findPVertexLength(mClipPlanes[i])<0){
			return true;
		}
	}
	return false;
}

void CameraNode::updateFramesPerSecond(){
	mFPSFrameCount++;
	int fpsTime=mScene->getTime();
	if(mFPSLastTime==0 || fpsTime-mFPSLastTime>5000){
		scalar fps=0;
		if(mFPSLastTime>0){
			fps=Math::div(Math::fromInt(mFPSFrameCount),Math::fromMilli(fpsTime-mFPSLastTime));
		}
		mFPSLastTime=fpsTime;
		mFPSFrameCount=0;
		mFPS=fps;
	}
}

void CameraNode::updateViewTransform(){
	scalar wt00=mWorldTransform.at(0,0),wt01=mWorldTransform.at(0,1),wt02=mWorldTransform.at(0,2);
	scalar wt10=mWorldTransform.at(1,0),wt11=mWorldTransform.at(1,1),wt12=mWorldTransform.at(1,2);
	scalar wt20=mWorldTransform.at(2,0),wt21=mWorldTransform.at(2,1),wt22=mWorldTransform.at(2,2);

	mViewTransform.setAt(0,0,wt00);
	mViewTransform.setAt(0,1,wt10);
	mViewTransform.setAt(0,2,wt20);
	mViewTransform.setAt(1,0,wt01);
	mViewTransform.setAt(1,1,wt11);
	mViewTransform.setAt(1,2,wt21);
	mViewTransform.setAt(2,0,wt02);
	mViewTransform.setAt(2,1,wt12);
	mViewTransform.setAt(2,2,wt22);

	mViewTransform.setAt(0,3,-(Math::mul(wt00,mWorldTranslate.x) + Math::mul(wt10,mWorldTranslate.y) + Math::mul(wt20,mWorldTranslate.z)));
	mViewTransform.setAt(1,3,-(Math::mul(wt01,mWorldTranslate.x) + Math::mul(wt11,mWorldTranslate.y) + Math::mul(wt21,mWorldTranslate.z)));
	mViewTransform.setAt(2,3,-(Math::mul(wt02,mWorldTranslate.x) + Math::mul(wt12,mWorldTranslate.y) + Math::mul(wt22,mWorldTranslate.z)));

	// Update frustum planes
	Math::mul(mViewProjectionTransform,mProjectionTransform,mViewTransform);
	scalar *vpt=mViewProjectionTransform.data;
	// Right clipping plane.
	Math::normalize(mClipPlanes[0].set(vpt[3]-vpt[0], vpt[7]-vpt[4], vpt[11]-vpt[8], vpt[15]-vpt[12]));
	// Left clipping plane.
	Math::normalize(mClipPlanes[1].set(vpt[3]+vpt[0], vpt[7]+vpt[4], vpt[11]+vpt[8], vpt[15]+vpt[12]));
	// Bottom clipping plane.
	Math::normalize(mClipPlanes[2].set(vpt[3]+vpt[1], vpt[7]+vpt[5], vpt[11]+vpt[9], vpt[15]+vpt[13]));
	// Top clipping plane.
	Math::normalize(mClipPlanes[3].set(vpt[3]-vpt[1], vpt[7]-vpt[5], vpt[11]-vpt[9], vpt[15]-vpt[13]));
	// Far clipping plane.
	Math::normalize(mClipPlanes[4].set(vpt[3]-vpt[2], vpt[7]-vpt[6], vpt[11]-vpt[10], vpt[15]-vpt[14]));
	// Near clipping plane.
	Math::normalize(mClipPlanes[5].set(vpt[3]+vpt[2], vpt[7]+vpt[6], vpt[11]+vpt[10], vpt[15]+vpt[14]));
}

void CameraNode::updateMidNode(){
	if(mMidNode!=NULL){
		mMidNode->setTranslate(0,0,-(mFarDist-mNearDist)/2);
	}
}

}
}
}
