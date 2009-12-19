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

#ifndef TOADLET_TADPOLE_NODE_CAMERANODE_H
#define TOADLET_TADPOLE_NODE_CAMERANODE_H

#include <toadlet/tadpole/node/ParentNode.h>
#include <toadlet/peeper/Color.h>
#include <toadlet/peeper/Viewport.h>

namespace toadlet{
namespace tadpole{
namespace node{

class TOADLET_API CameraNode:public ParentNode{
public:
	TOADLET_NODE(CameraNode,ParentNode);

	enum ProjectionType{
		ProjectionType_FOVX=0,
		ProjectionType_FOVY,
		ProjectionType_ORTHO,
		ProjectionType_FRUSTUM,
		ProjectionType_MATRIX
	};

	CameraNode();
	virtual Node *create(Engine *engine);

	void setProjectionFovX(scalar fovx,scalar aspect,scalar nearDist,scalar farDist);
	void setProjectionFovY(scalar fovy,scalar aspect,scalar nearDist,scalar farDist);
	void setProjectionOrtho(scalar leftDist,scalar rightDist,scalar bottomDist,scalar topDist,scalar nearDist,scalar farDist);
	void setProjectionFrustum(scalar leftDist,scalar rightDist,scalar bottomDist,scalar topDist,scalar nearDist,scalar farDist);
	void setProjectionTransform(const Matrix4x4 &transform);
	void setProjectionRotation(scalar rotate);
	void setNearAndFarDist(scalar nearDist,scalar farDist);

	void setAlignmentCalculationsUseOrigin(bool origin){mAlignmentCalculationsUseOrigin=origin;}
	bool getAlignmentCalculationsUseOrigin() const{return mAlignmentCalculationsUseOrigin;}

	void setLookAt(const Vector3 &eye,const Vector3 &point,const Vector3 &up);
	void setLookDir(const Vector3 &eye,const Vector3 &dir,const Vector3 &up);

	void setViewport(const peeper::Viewport &viewport);
	void setViewport(int x,int y,int width,int height);

	void setClearFlags(int clearFlags){mClearFlags=clearFlags;}
	void setClearColor(peeper::Color clearColor){mClearColor.set(clearColor);}
	void setSkipFirstClear(bool skip){mSkipFirstClear=skip;}

	ParentNode::ptr getMidNode();

	inline ProjectionType getProjectionType() const{return mProjectionType;}
	inline const Matrix4x4 &getProjectionTransform() const{return mProjectionTransform;}
	inline scalar getFov() const{return mFov;}
	inline scalar getAspect() const{return mAspect;}
	inline scalar getLeftDist() const{return mLeftDist;}
	inline scalar getRightDist() const{return mRightDist;}
	inline scalar getBottomDist() const{return mBottomDist;}
	inline scalar getTopDist() const{return mTopDist;}
	inline scalar getNearDist() const{return mNearDist;}
	inline scalar getFarDist() const{return mFarDist;}

	inline bool getViewportSet() const{return mViewportSet;}
	inline const peeper::Viewport &getViewport() const{return mViewport;}
	inline int getViewportX() const{return mViewport.x;}
	inline int getViewportY() const{return mViewport.y;}
	inline int getViewportWidth() const{return mViewport.width;}
	inline int getViewportHeight() const{return mViewport.height;}

	inline int getClearFlags() const{return mClearFlags;}
	inline peeper::Color getClearColor() const{return mClearColor;}
	inline bool getSkipFirstClear() const{return mSkipFirstClear;}

	void updateViewTransform();
	inline const Matrix4x4 &getViewTransform() const{return mViewTransform;}  /// Only valid during rendering
	inline const Vector3 &getRenderWorldTranslate() const{return mRenderWorldTranslate;}  /// Only valid during rendering

	// TODO: These should probably be moved into the Math library, and passing in a list of planes
	bool culled(const Sphere &sphere) const;
	bool culled(const AABox &box);
	inline int getNumCulledEntities() const{return mNumCulledEntities;}

	void updateFramesPerSecond();
	inline scalar getFramesPerSecond() const{return mFPS;}

protected:
	void update();

	ProjectionType mProjectionType;
	scalar mFov,mAspect;
	scalar mLeftDist,mRightDist;
	scalar mBottomDist,mTopDist;
	scalar mNearDist,mFarDist;
	Matrix4x4 mProjectionTransform;
	bool mViewportSet;
	peeper::Viewport mViewport;
	int mClearFlags;
	peeper::Color mClearColor;
	bool mSkipFirstClear;
	bool mAlignmentCalculationsUseOrigin;
	ParentNode::ptr mMidNode;

	Vector3 mRenderBoundingOrigin;
	Vector3 mRenderWorldTranslate;
	Matrix4x4 mViewTransform;
	Matrix4x4 mViewProjectionTransform;
	Plane mClipPlanes[6];

	int mNumCulledEntities;

	int mFPSLastTime;
	int mFPSFrameCount;
	scalar mFPS;

	Matrix4x4 cache_setProjectionRotation_projection;
	Vector3 cache_culled_vertex;

	friend class SceneNode;
};

}
}
}

#endif

