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
#include <toadlet/tadpole/node/CameraNode.h>
#include <toadlet/tadpole/node/ParentNode.h>
#include <toadlet/tadpole/Engine.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{
namespace node{

TOADLET_NODE_IMPLEMENT(CameraNode,"toadlet::tadpole::node::CameraNode");

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
	mMidNode(NULL),

	//mRenderBoundingOrigin,
	//mWorldTranslate,
	//mViewTransform,

	mNumCulledEntities(0),

	mFPSLastTime(0),
	mFPSFrameCount(0),
	mFPS(0)
{}

Node *CameraNode::create(Engine *engine){
	super::create(engine);

	setProjectionFovX(Math::HALF_PI,Math::fromInt(1),Math::fromInt(1),Math::fromInt(100));
	mViewportSet=false;
	mViewport.reset();
	mClearFlags=Renderer::ClearFlag_COLOR|Renderer::ClearFlag_DEPTH;
	mClearColor.reset();
	mSkipFirstClear=false;
	mMidNode=NULL;

	updateViewTransform();

	mNumCulledEntities=0;

	mFPSLastTime=0;
	mFPSFrameCount=0;
	mFPS=0;

	return this;
}

void CameraNode::setProjectionFovX(scalar fovx,scalar aspect,scalar nearDist,scalar farDist){
	mProjectionType=ProjectionType_FOVX;
	mFov=fovx;mAspect=aspect;
	mNearDist=nearDist;mFarDist=farDist;

	Math::setMatrix4x4FromPerspectiveX(mProjectionTransform,fovx,aspect,nearDist,farDist);

	update();
}

void CameraNode::setProjectionFovY(scalar fovy,scalar aspect,scalar nearDist,scalar farDist){
	mProjectionType=ProjectionType_FOVY;
	mFov=fovy;mAspect=aspect;
	mNearDist=nearDist;mFarDist=farDist;

	Math::setMatrix4x4FromPerspectiveY(mProjectionTransform,fovy,aspect,nearDist,farDist);

	update();
}

void CameraNode::setProjectionOrtho(scalar leftDist,scalar rightDist,scalar bottomDist,scalar topDist,scalar nearDist,scalar farDist){
	mProjectionType=ProjectionType_ORTHO;
	mLeftDist=leftDist;mRightDist=rightDist;
	mBottomDist=bottomDist;mTopDist=topDist;
	mNearDist=nearDist;mFarDist=farDist;

	Math::setMatrix4x4FromOrtho(mProjectionTransform,leftDist,rightDist,bottomDist,topDist,nearDist,farDist);

	update();
}

void CameraNode::setProjectionFrustum(scalar leftDist,scalar rightDist,scalar bottomDist,scalar topDist,scalar nearDist,scalar farDist){
	mProjectionType=ProjectionType_FRUSTUM;
	mLeftDist=leftDist;mRightDist=rightDist;
	mBottomDist=bottomDist;mTopDist=topDist;
	mNearDist=nearDist;mFarDist=farDist;

	Math::setMatrix4x4FromFrustum(mProjectionTransform,leftDist,rightDist,bottomDist,topDist,nearDist,farDist);

	update();
}

void CameraNode::setProjectionTransform(const Matrix4x4 &transform){
	mProjectionType=ProjectionType_MATRIX;

	mProjectionTransform.set(transform);

	// TODO: Set mNearDist and mFarDist

	update();
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
	Math::setMatrix4x4FromLookAt(mRenderTransform,eye,point,up,true);

	Math::setTranslateFromMatrix4x4(mTranslate,mRenderTransform);
	Math::setMatrix3x3FromMatrix4x4(mRotate,mRenderTransform);

	mIdentityTransform=false;
	modified();
}

void CameraNode::setLookDir(const Vector3 &eye,const Vector3 &dir,const Vector3 &up){
	Math::setMatrix4x4FromLookDir(mRenderTransform,eye,dir,up,true);

	Math::setTranslateFromMatrix4x4(mTranslate,mRenderTransform);
	Math::setMatrix3x3FromMatrix4x4(mRotate,mRenderTransform);

	mIdentityTransform=false;
	modified();
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
		mMidNode=mEngine->createNodeType(ParentNode::type());
		attach(mMidNode);
		update();
	}
	return mMidNode;
}

void CameraNode::updateViewTransform(){
	scalar wt00=mWorldRenderTransform.at(0,0),wt01=mWorldRenderTransform.at(0,1),wt02=mWorldRenderTransform.at(0,2);
	scalar wt10=mWorldRenderTransform.at(1,0),wt11=mWorldRenderTransform.at(1,1),wt12=mWorldRenderTransform.at(1,2);
	scalar wt20=mWorldRenderTransform.at(2,0),wt21=mWorldRenderTransform.at(2,1),wt22=mWorldRenderTransform.at(2,2);
	mWorldRenderTranslate.x=mWorldRenderTransform.at(0,3);mWorldRenderTranslate.y=mWorldRenderTransform.at(1,3);mWorldRenderTranslate.z=mWorldRenderTransform.at(2,3);

	mRenderBoundingOrigin.set(mWorldRenderTranslate.x-Math::mul(wt02,mBoundingRadius),mWorldRenderTranslate.y-Math::mul(wt12,mBoundingRadius),mWorldRenderTranslate.z-Math::mul(wt22,mBoundingRadius));

	mViewTransform.setAt(0,0,wt00);
	mViewTransform.setAt(0,1,wt10);
	mViewTransform.setAt(0,2,wt20);
	mViewTransform.setAt(1,0,wt01);
	mViewTransform.setAt(1,1,wt11);
	mViewTransform.setAt(1,2,wt21);
	mViewTransform.setAt(2,0,wt02);
	mViewTransform.setAt(2,1,wt12);
	mViewTransform.setAt(2,2,wt22);

	mViewTransform.setAt(0,3,-(Math::mul(wt00,mWorldRenderTranslate.x) + Math::mul(wt10,mWorldRenderTranslate.y) + Math::mul(wt20,mWorldRenderTranslate.z)));
	mViewTransform.setAt(1,3,-(Math::mul(wt01,mWorldRenderTranslate.x) + Math::mul(wt11,mWorldRenderTranslate.y) + Math::mul(wt21,mWorldRenderTranslate.z)));
	mViewTransform.setAt(2,3,-(Math::mul(wt02,mWorldRenderTranslate.x) + Math::mul(wt12,mWorldRenderTranslate.y) + Math::mul(wt22,mWorldRenderTranslate.z)));

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

bool CameraNode::culled(const Sphere &sphere) const{
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

// TODO: Test this culling algorithm, remove the use of the temporary to make something this important threadsafe
bool CameraNode::culled(const AABox &box){
	Vector3 &vertex=cache_culled_vertex;
	int i;
	for(i=0;i<6;i++){
		box.findPVertex(vertex,mClipPlanes[i].normal);
		if(Math::dot(mClipPlanes[i].normal,vertex)+mClipPlanes[i].distance<0){
			return true;
		}
	}
	return false;
}

void CameraNode::updateFramesPerSecond(){
	mFPSFrameCount++;
	int fpsTime=mEngine->getScene()->getRenderTime();
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

void CameraNode::update(){
	mBoundingRadius=mFarDist/2;
	if(mMidNode!=NULL){
		mMidNode->setTranslate(0,0,-(mFarDist-mNearDist)/2);
	}
}

}
}
}
